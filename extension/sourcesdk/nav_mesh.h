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

class CNavMesh : public CGameEventListener
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);
	static void OnCoreMapEnd();
	static void SDK_OnUnload();

	bool IsLoaded( void ) const		{ return m_isLoaded; }
	bool IsAnalyzed( void ) const	{ return m_isAnalyzed; }

	/**
	 * Return true if nav mesh can be trusted for all climbing/jumping decisions because game environment is fairly simple.
	 * Authoritative meshes mean path followers can skip CPU intensive realtime scanning of unpredictable geometry.
	 */
	/*virtual*/ bool IsAuthoritative() { return true; }

	bool IsOutOfDate( void ) const	{ return m_isOutOfDate; }

	unsigned int GetNavAreaCount( void ) const	{ return m_areaCount; }

	CNavArea *GetNavAreaByID( unsigned int id ) const;

	static MCall<CNavArea*, const Vector&, bool, float, bool, bool, int> mGetNearestNavArea;
	CNavArea* GetNearestNavArea( const Vector &pos, bool anyZ = false, float maxDist = 10000.0f, bool checkLOS = false, bool checkGround = true, int team = TEAM_ANY );

	static MCall<bool, const Vector&, float*, Vector*> mGetGroundHeight;
	bool GetGroundHeight(const Vector&, float*, Vector*);

	//-------------------------------------------------------------------------------------
	/**
	 * Apply the functor to all navigation areas that overlap the given extent.
	 * If functor returns false, stop processing and return false.
	 */
	template < typename Functor >
	bool ForAllAreasOverlappingExtent( Functor &func, const Extent &extent )
	{
		if ( !m_grid.Count() )
		{
			return true;
		}
		static unsigned int searchMarker = RandomInt(0, 1024*1024 );
		if ( ++searchMarker == 0 )
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
				int iGrid = x + y*m_gridSizeX;
				if ( iGrid >= m_grid.Count() )
				{
					ExecuteNTimes( 10, Warning( "** Walked off of the CNavMesh::m_grid in ForAllAreasOverlappingExtent()\n" ) );
					return true;
				}

				NavAreaVector *areaVector = &m_grid[ iGrid ];

				// find closest area in this cell
				FOR_EACH_VEC( (*areaVector), it )
				{
					CNavArea *area = (*areaVector)[ it ];

					// skip if we've already visited this area
					if ( area->m_nearNavSearchMarker == searchMarker )
						continue;

					// mark as visited
					area->m_nearNavSearchMarker = searchMarker;
					area->GetExtent( &areaExtent );

					if ( extent.IsOverlapping( areaExtent ) )
					{
						if ( func( area ) == false )
							return false;
					}
				}
			}
		}
		return true;
	}

private:
	CUtlVector<NavAreaVector> m_grid;
	float m_gridCellSize;										// the width/height of a grid cell for spatially partitioning nav areas for fast access
	int m_gridSizeX;
	int m_gridSizeY;
	float m_minX;
	float m_minY;
	unsigned int m_areaCount;									// total number of nav areas

	bool m_isLoaded;											// true if a Navigation Mesh has been loaded
	bool m_isOutOfDate;											// true if the Navigation Mesh is older than the actual BSP
	bool m_isAnalyzed;											// true if the Navigation Mesh needs analysis

	enum { HASH_TABLE_SIZE = 256 };
	CNavArea *m_hashTable[ HASH_TABLE_SIZE ];					// hash table to optimize lookup by ID
	int ComputeHashKey( unsigned int id ) const;				// returns a hash key for the given nav area ID

	int WorldToGridX( float wx ) const;							// given X component, return grid index
	int WorldToGridY( float wy ) const;							// given Y component, return grid index
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

		CUtlVector< CNavArea * > adjVector;

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

//--------------------------------------------------------------------------------------------------------------
inline int CNavMesh::ComputeHashKey( unsigned int id ) const
{
	return id & 0xFF;
}

//--------------------------------------------------------------------------------------------------------------
inline int CNavMesh::WorldToGridX( float wx ) const
{ 
	int x = (int)( (wx - m_minX) / m_gridCellSize );

	if (x < 0)
		x = 0;
	else if (x >= m_gridSizeX)
		x = m_gridSizeX-1;
	
	return x;
}

//--------------------------------------------------------------------------------------------------------------
inline int CNavMesh::WorldToGridY( float wy ) const
{ 
	int y = (int)( (wy - m_minY) / m_gridCellSize );

	if (y < 0)
		y = 0;
	else if (y >= m_gridSizeY)
		y = m_gridSizeY-1;
	
	return y;
}


#define IGNORE_NAV_BLOCKERS true
bool NavAreaBuildPath(CNavArea *startArea, CNavArea *goalArea, Vector *goalPos, IPathCost &costFunc, CNavArea **closestArea = NULL, float maxPathLength = 0.0f, int teamID = TEAM_ANY, bool ignoreNavBlockers = false);
#endif