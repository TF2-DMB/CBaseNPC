#pragma once

#include "toolsnav.h"
#include "sourcesdk/nav_area.h"

#include <set>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

class CToolsNavMesh;
class CNavArea;
class CToolsNavArea
{
public:
	CNavArea* GetArea() const
	{
		return m_realArea;
	}

private:
	friend class CToolsNavMesh;
	CToolsNavArea(CNavArea* area);

	CNavArea* m_realArea;
};

class CToolsNavMesh
{
public:
	CToolsNavMesh();
	void Load();
	void Clear();

	void AddNavArea(CNavArea* area);

	CNavArea* GetNavArea(const Vector &pos, float beneathLimt = 120.0f) const;

	CNavArea* GetNavArea(CBaseEntity *pEntity, int nGetNavAreaFlags, float flBeneathLimit = 120.0f) const;

	CNavArea* GetNearestNavArea(const Vector &pos, bool anyZ, float maxDist, bool checkLOS, bool checkGround, int team) const;

	CNavArea* GetNavAreaByID(std::uint32_t id) const
	{
		return m_hashmap.at(id)->GetArea();
	}

	std::uint32_t GetNavAreaCount(void) const
	{
		return m_areas.size();
	}

	bool IsLoaded() const
	{
		return m_isLoaded;
	}

	template < typename Functor >
	bool ForAllAreasOverlappingExtent( Functor &func, const Extent &extent )
	{
		if (!m_grid.size())
		{
			return true;
		}

		static unsigned int searchMarker = RandomInt(0, 1024*1024);
		if (++searchMarker == 0)
		{
			++searchMarker;
		}

		Extent areaExtent;

		// get list in cell that contains position
		int startX = WorldToGridX( extent.lo.x );
		int endX = WorldToGridX( extent.hi.x );
		int startY = WorldToGridY( extent.lo.y );
		int endY = WorldToGridY( extent.hi.y );

		for( int x = startX; x <= endX; ++x )
		{
			for( int y = startY; y <= endY; ++y )
			{
				int iGrid = x + y * m_gridSizeX;
				if (iGrid >= (int)m_grid.size())
				{
					ExecuteNTimes(10, Warning( "** Walked off of the CNavMesh::m_grid in ForAllAreasOverlappingExtent()\n"));
					return true;
				}

				for (auto it = m_grid[iGrid].begin(); it != m_grid[iGrid].end(); it++)
				{
					CNavArea* area = (*it)->GetArea();
					// skip if we've already visited this area
					if (area->m_nearNavSearchMarker == searchMarker)
					{
						continue;
					}

					// mark as visited
					area->m_nearNavSearchMarker = searchMarker;
					area->GetExtent( &areaExtent );

					if (extent.IsOverlapping(areaExtent))
					{
						if (func( area ) == false)
						{
							return false;
						}
					}
				}
			}
		}
		return true;
	}

	template < typename Functor >
	bool ForAllAreasInRadius( Functor &func, const Vector &pos, float radius )
	{
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
		int shiftLimit = ceil( radius / m_gridCellSize );
		float radiusSq = radius * radius;
		if ( radius == 0.0f )
		{
			shiftLimit = MAX( m_gridSizeX, m_gridSizeY );	// range 0 means all areas
		}

		for( int x = originX - shiftLimit; x <= originX + shiftLimit; ++x )
		{
			if ( x < 0 || x >= m_gridSizeX )
				continue;

			for( int y = originY - shiftLimit; y <= originY + shiftLimit; ++y )
			{
				if ( y < 0 || y >= m_gridSizeY )
					continue;

				int iGrid = x + y * m_gridSizeX;

				// find closest area in this cell
				for (auto it = m_grid[iGrid].begin(); it != m_grid[iGrid].end(); it++)
				{
					CNavArea* area = (*it)->GetArea();

					// skip if we've already visited this area
					if ( area->m_nearNavSearchMarker == searchMarker )
						continue;

					// mark as visited
					area->m_nearNavSearchMarker = searchMarker;

					float distSq = ( area->GetCenter() - pos ).LengthSqr();

					if ( ( distSq <= radiusSq ) || ( radiusSq == 0 ) )
					{
						if ( func( area ) == false )
							return false;
					}
				}
			}
		}
		return true;
	}

	//---------------------------------------------------------------------------------------------------------------
	/*
	 * Step through nav mesh along line between startArea and endArea.
	 * Return true if enumeration reached endArea, false if doesn't reach it (no mesh between, bad connection, etc)
	 */
	template < typename Functor >
	bool ForAllAreasAlongLine( Functor &func, CNavArea *startArea, CNavArea *endArea )
	{
		if ( !startArea || !endArea )
			return false;

		if ( startArea == endArea )
		{
			func( startArea );
			return true;
		}

		Vector start = startArea->GetCenter();
		Vector end = endArea->GetCenter();

		Vector to = end - start;
		float range = to.NormalizeInPlace();

		const float epsilon = 0.00001f;

		if ( range < epsilon )
		{
			func( startArea );
			return true;
		}

		if ( abs( to.x ) < epsilon )
		{
			NavDirType dir = ( to.y < 0.0f ) ? NORTH : SOUTH;

			CNavArea *area = startArea;
			while( area )
			{
				func( area );

				if ( area == endArea )
					return true;

				const NavConnectVector *adjVector = area->GetAdjacentAreas( dir );

				area = NULL;

				for( int i=0; i<adjVector->Count(); ++i )
				{
					CNavArea *adjArea = adjVector->Element(i).area;

					const Vector &adjOrigin = adjArea->GetCorner( NORTH_WEST );

					if ( adjOrigin.x <= start.x && adjOrigin.x + adjArea->GetSizeX() >= start.x )
					{
						area = adjArea;
						break;
					}
				}
			}

			return false;
		}
		else if ( abs( to.y ) < epsilon )
		{
			NavDirType dir = ( to.x < 0.0f ) ? WEST : EAST;

			CNavArea *area = startArea;
			while( area )
			{
				func( area );

				if ( area == endArea )
					return true;

				const NavConnectVector *adjVector = area->GetAdjacentAreas( dir );

				area = NULL;

				for( int i=0; i<adjVector->Count(); ++i )
				{
					CNavArea *adjArea = adjVector->Element(i).area;

					const Vector &adjOrigin = adjArea->GetCorner( NORTH_WEST );

					if ( adjOrigin.y <= start.y && adjOrigin.y + adjArea->GetSizeY() >= start.y )
					{
						area = adjArea;
						break;
					}
				}
			}

			return false;
		}


		CNavArea *area = startArea;

		while( area )
		{
			func( area );

			if ( area == endArea )
				return true;

			const Vector &origin = area->GetCorner( NORTH_WEST );
			float xMin = origin.x;
			float xMax = xMin + area->GetSizeX();
			float yMin = origin.y;
			float yMax = yMin + area->GetSizeY();

			// clip ray to area
			Vector exit;
			NavDirType edge = NUM_DIRECTIONS;

			if ( to.x < 0.0f )
			{
				// find Y at west edge intersection
				float t = ( xMin - start.x ) / ( end.x - start.x );
				if ( t > 0.0f && t < 1.0f )
				{
					float y = start.y + t * ( end.y - start.y );
					if ( y >= yMin && y <= yMax )
					{
						// intersects this edge
						exit.x = xMin;
						exit.y = y;
						edge = WEST;
					}
				}
			}
			else
			{
				// find Y at east edge intersection
				float t = ( xMax - start.x ) / ( end.x - start.x );
				if ( t > 0.0f && t < 1.0f )
				{
					float y = start.y + t * ( end.y - start.y );
					if ( y >= yMin && y <= yMax )
					{
						// intersects this edge
						exit.x = xMax;
						exit.y = y;
						edge = EAST;
					}
				}
			}

			if ( edge == NUM_DIRECTIONS )
			{
				if ( to.y < 0.0f )
				{
					// find X at north edge intersection
					float t = ( yMin - start.y ) / ( end.y - start.y );
					if ( t > 0.0f && t < 1.0f )
					{
						float x = start.x + t * ( end.x - start.x );
						if ( x >= xMin && x <= xMax )
						{
							// intersects this edge
							exit.x = x;
							exit.y = yMin;
							edge = NORTH;
						}
					}
				}
				else
				{
					// find X at south edge intersection
					float t = ( yMax - start.y ) / ( end.y - start.y );
					if ( t > 0.0f && t < 1.0f )
					{
						float x = start.x + t * ( end.x - start.x );
						if ( x >= xMin && x <= xMax )
						{
							// intersects this edge
							exit.x = x;
							exit.y = yMax;
							edge = SOUTH;
						}
					}
				}
			}

			if ( edge == NUM_DIRECTIONS )
				break;

			const NavConnectVector *adjVector = area->GetAdjacentAreas( edge );

			area = NULL;

			for( int i=0; i<adjVector->Count(); ++i )
			{
				CNavArea *adjArea = adjVector->Element(i).area;

				const Vector &adjOrigin = adjArea->GetCorner( NORTH_WEST );

				if ( edge == NORTH || edge == SOUTH )
				{
					if ( adjOrigin.x <= exit.x && adjOrigin.x + adjArea->GetSizeX() >= exit.x )
					{
						area = adjArea;
						break;
					}
				}
				else
				{
					if ( adjOrigin.y <= exit.y && adjOrigin.y + adjArea->GetSizeY() >= exit.y )
					{
						area = adjArea;
						break;
					}
				}
			}
		}

		return false;
	}

private:
	void AllocateGrid(float minX, float maxX, float minY, float maxY);
	std::int32_t WorldToGridX(float wx) const;
	std::int32_t WorldToGridY(float wy) const;

	std::set<std::unique_ptr<CToolsNavArea>> m_areas;
	std::vector<std::vector<CToolsNavArea*>> m_grid;
	float m_gridCellSize;
	std::int32_t m_gridSizeX;
	std::int32_t m_gridSizeY;
	float m_minX;
	float m_minY;
	bool m_isLoaded;
	std::uint32_t m_areaCount;
	std::unordered_map<std::uint32_t, CToolsNavArea*> m_hashmap;
};

extern CToolsNavMesh* ToolsNavMesh;

inline std::int32_t CToolsNavMesh::WorldToGridX(float wx) const
{ 
	std::int32_t x = (wx - m_minX) / m_gridCellSize;

	if (x < 0)
	{
		x = 0;
	}
	else if (x >= m_gridSizeX)
	{
		x = m_gridSizeX - 1;
	}
	
	return x;
}

inline std::int32_t CToolsNavMesh::WorldToGridY(float wy) const
{ 
	std::int32_t y = (wy - m_minY) / m_gridCellSize;

	if (y < 0)
	{
		y = 0;
	}
	else if (y >= m_gridSizeY)
	{
		y = m_gridSizeY - 1;
	}
	
	return y;
}