#include "toolsnav_mesh.h"
#include "toolsutil.h"
#include "sourcesdk/nav_mesh.h"
#include "tier0/vprof.h"

CToolsNavMesh* ToolsNavMesh = new CToolsNavMesh;
ConVar* nav_solid_props = nullptr;

CToolsNavArea::CToolsNavArea(CNavArea* area) :
	m_realArea(area)
{
}

CToolsNavMesh::CToolsNavMesh() :
	m_gridCellSize(300.0f),
	m_isLoaded(false)
{
}

void CToolsNavMesh::Load()
{
	this->Clear();

	if (pTheNavAreas->Count() <= 0)
	{
		return;
	}

	Extent extent;
	extent.lo.x = 9999999999.9f;
	extent.lo.y = 9999999999.9f;
	extent.hi.x = -9999999999.9f;
	extent.hi.y = -9999999999.9f;

	Extent areaExtent;
	for (std::uint32_t i = 0; i < pTheNavAreas->Count(); i++)
	{
		CNavArea* area = pTheNavAreas->Element(i);

		auto pSpots = area->GetHidingSpots();
		FOR_EACH_VEC((*pSpots), it)
		{
			HidingSpot* spot = (*pSpots)[ it ];
			TheHidingSpots.AddToTail(spot);
		}

		TheNavAreas.AddToTail(area);

		area->GetExtent(&areaExtent);

		if (areaExtent.lo.x < extent.lo.x)
			extent.lo.x = areaExtent.lo.x;
		if (areaExtent.lo.y < extent.lo.y)
			extent.lo.y = areaExtent.lo.y;
		if (areaExtent.hi.x > extent.hi.x)
			extent.hi.x = areaExtent.hi.x;
		if (areaExtent.hi.y > extent.hi.y)
			extent.hi.y = areaExtent.hi.y;
	}
	AllocateGrid(extent.lo.x, extent.hi.x, extent.lo.y, extent.hi.y);

	FOR_EACH_VEC(TheNavAreas, it)
	{
		AddNavArea(TheNavAreas[it]);
	}
	m_isLoaded = true;

	g_pSM->LogMessage(myself, "Parsed %d nav areas.", GetNavAreaCount());
}

void CToolsNavMesh::Clear()
{
	m_isLoaded = false;
	m_grid.clear();
	m_areas.clear();
	m_hashmap.clear();
	TheHidingSpots.RemoveAll();
	TheNavAreas.RemoveAll();
}

void CToolsNavMesh::AllocateGrid(float minX, float maxX, float minY, float maxY)
{
	m_minX = minX;
	m_minY = minY;

	m_gridSizeX = (int)((maxX - minX) / m_gridCellSize) + 1;
	m_gridSizeY = (int)((maxY - minY) / m_gridCellSize) + 1;

	m_grid.resize(m_gridSizeX * m_gridSizeY);
}

void CToolsNavMesh::AddNavArea(CNavArea* area)
{
	CToolsNavArea* toolsArea = new CToolsNavArea(area);
	m_areas.emplace(toolsArea);

	int loX = WorldToGridX( area->GetCorner( NORTH_WEST ).x );
	int loY = WorldToGridY( area->GetCorner( NORTH_WEST ).y );
	int hiX = WorldToGridX( area->GetCorner( SOUTH_EAST ).x );
	int hiY = WorldToGridY( area->GetCorner( SOUTH_EAST ).y );

	for( int y = loY; y <= hiY; ++y )
	{
		for( int x = loX; x <= hiX; ++x )
		{
			m_grid[x + y * m_gridSizeX].emplace_back(toolsArea);
		}
	}

	m_hashmap.insert({area->GetID(), toolsArea});
}

CNavArea* CToolsNavMesh::GetNavArea(const Vector &pos, float beneathLimit) const
{
	VPROF_BUDGET("CToolsNavMesh::GetNavArea", "NextBot");

	if (!m_grid.size())
	{
		return nullptr;
	}

	// search cell list to find correct area
	CNavArea* use = NULL;
	float useZ = -99999999.9f;
	Vector testPos = pos + Vector( 0, 0, 5 );

	// get list in cell that contains position
	int x = WorldToGridX(pos.x);
	int y = WorldToGridY(pos.y);
	auto vecPos = x + y * m_gridSizeX;
	for (auto it = m_grid[vecPos].begin(); it != m_grid[vecPos].end(); it++)
	{
		CNavArea* area = (*it)->GetArea();

		// check if position is within 2D boundaries of this area
		if (area->IsOverlapping( testPos ))
		{
			// project position onto area to get Z
			float z = area->GetZ( testPos );

			// if area is above us, skip it
			if (z > testPos.z)
				continue;

			// if area is too far below us, skip it
			if (z < pos.z - beneathLimit)
				continue;

			// if area is higher than the one we have, use this instead
			if (z > useZ)
			{
				use = area;
				useZ = z;
			}
		}
	}

	return use;
}

CNavArea* CToolsNavMesh::GetNavArea(CBaseEntity* paramEntity, int nFlags, float flBeneathLimit) const
{
	VPROF("CToolsNavMesh::GetNavArea [ent]");

	if (!m_grid.size())
	{
		return nullptr;
	}

	CBaseEntityHack* pEntity = reinterpret_cast<CBaseEntityHack*>(paramEntity);

	Vector testPos = pEntity->GetAbsOrigin();

	float flStepHeight = 1e-3;
	CBaseCombatCharacterHack* pBCC = pEntity->MyCombatCharacterPointer();
	if ( pBCC )
	{
		// Check if we're still in the last area
		CNavArea *pLastNavArea = pBCC->GetLastKnownArea();
		if ( pLastNavArea && pLastNavArea->IsOverlapping( testPos ) )
		{
			float flZ = pLastNavArea->GetZ( testPos );
			if ( ( flZ <= testPos.z + StepHeight ) && ( flZ >= testPos.z - StepHeight ) )
				return pLastNavArea;
		}
		flStepHeight = StepHeight;
	}

	// get list in cell that contains position
	int x = WorldToGridX( testPos.x );
	int y = WorldToGridY( testPos.y );

	// search cell list to find correct area
	CNavArea *use = nullptr;
	float useZ = -99999999.9f;

	bool bSkipBlockedAreas = ( ( nFlags & GETNAVAREA_ALLOW_BLOCKED_AREAS ) == 0 );
	auto vecPos =  x + y * m_gridSizeX;
	for (auto it = m_grid[vecPos].begin(); it != m_grid[vecPos].end(); it++)
	{
		CNavArea* pArea = (*it)->GetArea();

		// check if position is within 2D boundaries of this area
		if ( !pArea->IsOverlapping( testPos ) )
			continue;

		// don't consider blocked areas
		if ( bSkipBlockedAreas && pArea->IsBlocked( pEntity->GetTeamNumber() ) )
			continue;

		// project position onto area to get Z
		float z = pArea->GetZ( testPos );

		// if area is above us, skip it
		if ( z > testPos.z + flStepHeight )
			continue;

		// if area is too far below us, skip it
		if ( z < testPos.z - flBeneathLimit )
			continue;

		// if area is lower than the one we have, skip it
		if ( z <= useZ )
			continue;

		use = pArea;
		useZ = z;
	}

	// Check LOS if necessary
	if ( use && (nFlags & GETNAVAREA_CHECK_LOS) && ( useZ < testPos.z - flStepHeight ) )
	{
		// trace directly down to see if it's below us and unobstructed
		trace_t result;
		UTILTools_TraceLine( testPos, Vector( testPos.x, testPos.y, useZ ), MASK_NPCSOLID_BRUSHONLY, nullptr, COLLISION_GROUP_NONE, &result );
		if ( ( result.fraction != 1.0f ) && ( fabs( result.endpos.z - useZ ) > flStepHeight ) )
			return nullptr;
	}
	return use;
}

CNavArea* CToolsNavMesh::GetNearestNavArea(const Vector &pos, bool anyZ, float maxDist, bool checkLOS, bool checkGround, int team) const
{
	VPROF_BUDGET("CToolsNavMesh::GetNearestNavArea", "NextBot");

	if (!m_grid.size())
	{
		return nullptr;
	}	

	CNavArea *close = nullptr;
	float closeDistSq = maxDist * maxDist;

	// quick check
	if (!checkLOS && !checkGround)
	{
		close = GetNavArea( pos );
		if ( close )
		{
			return close;
		}
	}

	// ensure source position is well behaved
	Vector source;
	source.x = pos.x;
	source.y = pos.y;
	if (TheNavMesh->GetGroundHeight(pos, &source.z, nullptr) == false)
	{
		if (!checkGround)
		{
			source.z = pos.z;
		}
		else
		{
			return NULL;
		}
	}

	source.z += HalfHumanHeight;

	// find closest nav area

	// use a unique marker for this method, so it can be used within a SearchSurroundingArea() call
	static unsigned int searchMarker = RandomInt(0, 1024*1024 );

	++searchMarker;

	if ( searchMarker == 0 )
	{
		++searchMarker;
	}


	// get list in cell that contains position
	int originX = WorldToGridX( pos.x );
	int originY = WorldToGridY( pos.y );

	int shiftLimit = ceil(maxDist / m_gridCellSize);

	//
	// Search in increasing rings out from origin, starting with cell
	// that contains the given position.
	// Once we find a close area, we must check one more step out in
	// case our position is just against the edge of the cell boundary
	// and an area in an adjacent cell is actually closer.
	// 
	for( int shift=0; shift <= shiftLimit; ++shift )
	{
		for( int x = originX - shift; x <= originX + shift; ++x )
		{
			if ( x < 0 || x >= m_gridSizeX )
				continue;

			for( int y = originY - shift; y <= originY + shift; ++y )
			{
				if ( y < 0 || y >= m_gridSizeY )
					continue;

				// only check these areas if we're on the outer edge of our spiral
				if ( x > originX - shift &&
					 x < originX + shift &&
					 y > originY - shift &&
					 y < originY + shift )
					continue;

				auto vecPos = x + y * m_gridSizeX;
				// find closest area in this cell
				for (auto it = m_grid[vecPos].begin(); it != m_grid[vecPos].end(); it++)
				{
					CNavArea *area = (*it)->GetArea();

					// skip if we've already visited this area
					if ( area->m_nearNavSearchMarker == searchMarker )
						continue;

					// don't consider blocked areas
					if ( area->IsBlocked( team ) )
						continue;

					// mark as visited
					area->m_nearNavSearchMarker = searchMarker;

					Vector areaPos;
					area->GetClosestPointOnArea( source, &areaPos );

					// TERROR: Using the original pos for distance calculations.  Since it's a pure 3D distance,
					// with no Z restrictions or LOS checks, this should work for passing in bot foot positions.
					// This needs to be ported back to CS:S.
					float distSq = ( areaPos - pos ).LengthSqr();

					// keep the closest area
					if ( distSq >= closeDistSq )
						continue;

					// check LOS to area
					// REMOVED: If we do this for !anyZ, it's likely we wont have LOS and will enumerate every area in the mesh
					// It is still good to do this in some isolated cases, however
					if ( checkLOS )
					{
						trace_t result;

						// make sure 'pos' is not embedded in the world
						Vector safePos;

						UTILTools_TraceLine(pos, pos + Vector( 0, 0, StepHeight ), MASK_NPCSOLID_BRUSHONLY, nullptr, COLLISION_GROUP_NONE, &result);
						if ( result.startsolid )
						{
							// it was embedded - move it out
							safePos = result.endpos + Vector( 0, 0, 1.0f );
						}
						else
						{
							safePos = pos;
						}

						// Don't bother tracing from the nav area up to safePos.z if it's within StepHeight of the area, since areas can be embedded in the ground a bit
						float heightDelta = fabs(areaPos.z - safePos.z);
						if ( heightDelta > StepHeight )
						{
							// trace to the height of the original point
							UTILTools_TraceLine( areaPos + Vector( 0, 0, StepHeight ), Vector( areaPos.x, areaPos.y, safePos.z ), MASK_NPCSOLID_BRUSHONLY, nullptr, COLLISION_GROUP_NONE, &result );
							
							if ( result.fraction != 1.0f )
							{
								continue;
							}
						}

						// trace to the original point's height above the area
						UTILTools_TraceLine( safePos, Vector( areaPos.x, areaPos.y, safePos.z + StepHeight ), MASK_NPCSOLID_BRUSHONLY, nullptr, COLLISION_GROUP_NONE, &result );

						if ( result.fraction != 1.0f )
						{
							continue;
						}
					}

					closeDistSq = distSq;
					close = area;

					// look one more step outwards
					shiftLimit = shift+1;
				}
			}
		}
	}

	return close;
}