#ifndef NAV_MESH_H
#define NAV_MESH_H

#include "sourcesdk/nav_area.h"
#include "sourcesdk/baseentity.h"
#include "sourcesdk/basecombatcharacter.h"
#include "sourcesdk/GameEventListener.h"

#pragma once

#ifdef _X360
#define PREFETCH360(address, offset) __dcbt(offset,address)
#else
#define PREFETCH360(x,y) // nothing
#endif

class CNavMesh;
class IPathCost
{
public:
	virtual float operator()( CNavArea *area, CNavArea *fromArea, const CNavLadder *ladder, const CFuncElevator *elevator, float length ) const = 0;
};


extern CNavMesh *TheNavMesh;
extern NavAreaVector* pTheNavAreas;

class CNavMesh
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);
	static void SDK_OnUnload();

	bool IsLoaded( void ) const		{ return *(bool*)((uint8_t*)this + offset_m_isLoaded); }
	bool IsAnalyzed( void ) const	{ return *(bool*)((uint8_t*)this + offset_m_isLoaded + 1); }
	bool IsOutOfDate( void ) const	{ return *(bool*)((uint8_t*)this + offset_m_isLoaded + 2); }

	/**
	 * Return true if nav mesh can be trusted for all climbing/jumping decisions because game environment is fairly simple.
	 * Authoritative meshes mean path followers can skip CPU intensive realtime scanning of unpredictable geometry.
	 */
	/*virtual*/ bool IsAuthoritative() { return true; }

	static MCall<bool, const Vector&, float*, Vector*> mGetGroundHeight;
	bool GetGroundHeight(const Vector&, float*, Vector*);

	static int offset_m_isLoaded;
};

inline void CollectSurroundingAreas( CUtlVector< CNavArea * > *nearbyAreaVector, CNavArea *startArea, float travelDistanceLimit = 1500.0f, float maxStepUpLimit = StepHeight, float maxDropDownLimit = 100.0f )
{
	nearbyAreaVector->RemoveAll();

	if ( startArea )
	{
		CNavArea::MakeNewMarker();
		CNavArea::ClearSearchLists();

		startArea->AddToOpenList();
		startArea->SetTotalCost( 0.0f );
		startArea->SetCostSoFar( 0.0f );
		startArea->SetParent( NULL );
		startArea->Mark();

		while( !CNavArea::IsOpenListEmpty() )
		{
			// get next area to check
			CNavArea *area = CNavArea::PopOpenList();

			if ( travelDistanceLimit > 0.0f && area->GetCostSoFar() > travelDistanceLimit )
				continue;

			if ( area->GetParent() )
			{
				float deltaZ = area->GetParent()->ComputeAdjacentConnectionHeightChange( area );

				if ( deltaZ > maxStepUpLimit )
					continue;

				if ( deltaZ < -maxDropDownLimit )
					continue;
			}

			nearbyAreaVector->AddToTail( area );

			// mark here to ensure all marked areas are also valid areas that are in the collection
			area->Mark();

			// search adjacent outgoing connections
			for( int dir=0; dir<NUM_DIRECTIONS; ++dir )
			{
				int count = area->GetAdjacentCount( (NavDirType)dir );
				for( int i=0; i<count; ++i )
				{
					CNavArea *adjArea = area->GetAdjacentArea( (NavDirType)dir, i );

					if ( adjArea->IsBlocked( TEAM_ANY ) )
					{
						continue;
					}

					if ( !adjArea->IsMarked() )
					{
						adjArea->SetTotalCost( 0.0f );
						adjArea->SetParent( area );

						// compute approximate travel distance from start area of search
						float distAlong = area->GetCostSoFar();
						distAlong += ( adjArea->GetCenter() - area->GetCenter() ).Length();
						adjArea->SetCostSoFar( distAlong );
						adjArea->AddToOpenList();
					}
				}
			}
		}
	}
}


#define IGNORE_NAV_BLOCKERS true
bool NavAreaBuildPath(CNavArea *startArea, CNavArea *goalArea, Vector *goalPos, IPathCost &costFunc, CNavArea **closestArea = NULL, float maxPathLength = 0.0f, int teamID = TEAM_ANY, bool ignoreNavBlockers = false);
#endif