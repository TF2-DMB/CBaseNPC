#ifndef _NAV_AREA_H_
#define _NAV_AREA_H_

#define MAX_NAV_TEAMS 2

#include "tier1/utlvector.h"
#include "sourcesdk/nav.h"
#include "sourcesdk/nav_ladder.h"

#define DECLARE_CLASS_NOBASE( className )					typedef className ThisClass;

#if !defined(_X360)
typedef CUtlVectorUltraConservativeAllocator CNavVectorAllocator;
#else
typedef CNavVectorNoEditAllocator CNavVectorAllocator;
#endif

#ifdef STAGING_ONLY
inline void DebuggerBreakOnNaN_StagingOnly( float val )
{
	if ( IS_NAN( val ) )
		DebuggerBreak();
}
#else
#define DebuggerBreakOnNaN_StagingOnly( _val )
#endif

//-------------------------------------------------------------------------------------------------------------------
/**
 * The NavConnect union is used to refer to connections to areas
 */
struct NavConnect
{
	NavConnect()
	{
		id = 0;
		length = -1;
	}

	union
	{
		unsigned int id;
		CNavArea *area;
	};

	mutable float length;

	bool operator==( const NavConnect &other ) const
	{
		return (area == other.area) ? true : false;
	}
};

typedef CUtlVectorUltraConservative<NavConnect, CNavVectorAllocator> NavConnectVector;

//-------------------------------------------------------------------------------------------------------------------
/**
 * The NavLadderConnect union is used to refer to connections to ladders
 */
union NavLadderConnect
{
	unsigned int id;
	CNavLadder *ladder;

	bool operator==( const NavLadderConnect &other ) const
	{
		return (ladder == other.ladder) ? true : false;
	}
};
typedef CUtlVectorUltraConservative<NavLadderConnect, CNavVectorAllocator> NavLadderConnectVector;


class HidingSpot;

//--------------------------------------------------------------------------------------------------------------
/**
 * Stores a pointer to an interesting "spot", and a parametric distance along a path
 */
struct SpotOrder
{
	float t;						// parametric distance along ray where this spot first has LOS to our path
	union
	{
		HidingSpot *spot;			// the spot to look at
		unsigned int id;			// spot ID for save/load
	};
};
typedef CUtlVector< SpotOrder > SpotOrderVector;

typedef CUtlVectorUltraConservative< HidingSpot * > HidingSpotVector;

/**
 * This struct stores possible path segments thru a CNavArea, and the dangerous spots
 * to look at as we traverse that path segment.
 */
struct SpotEncounter
{
	NavConnect from;
	NavDirType fromDir;
	NavConnect to;
	NavDirType toDir;
	Ray path;									// the path segment
	SpotOrderVector spots;						// list of spots to look at, in order of occurrence
};
typedef CUtlVectorUltraConservative< SpotEncounter * > SpotEncounterVector;

class CNavAreaCriticalData
{
protected:
	// --- Begin critical data, which is heavily hit during pathing operations and carefully arranged for cache performance [7/24/2008 tom] ---

	/* 0  */	Vector m_nwCorner;											// north-west corner position (2D mins)
	/* 12 */	Vector m_seCorner;											// south-east corner position (2D maxs)
	/* 24 */	float m_invDxCorners;
	/* 28 */	float m_invDyCorners;
	/* 32 */	float m_neZ;												// height of the implicit corner defined by (m_seCorner.x, m_nwCorner.y, m_neZ)
	/* 36 */	float m_swZ;												// height of the implicit corner defined by (m_nwCorner.x, m_seCorner.y, m_neZ)
	/* 40 */	Vector m_center;											// centroid of area

	/* 52 */	unsigned char m_playerCount[ MAX_NAV_TEAMS ];				// the number of players currently in this area

	/* 54 */	bool m_isBlocked[ MAX_NAV_TEAMS ];							// if true, some part of the world is preventing movement through this nav area

	/* 56 */	unsigned int m_marker;										// used to flag the area as visited
	/* 60 */	float m_totalCost;											// the distance so far plus an estimate of the distance left
	/* 64 */	float m_costSoFar;											// distance travelled so far

	/* 68 */	CNavArea *m_nextOpen, *m_prevOpen;							// only valid if m_openMarker == m_masterMarker
	/* 76 */	unsigned int m_openMarker;									// if this equals the current marker value, we are on the open list

	/* 80 */	int	m_attributeFlags;										// set of attribute bit flags (see NavAttributeType)

	//- connections to adjacent areas -------------------------------------------------------------------
	/* 84 */	NavConnectVector m_connect[ NUM_DIRECTIONS ];				// a list of adjacent areas for each direction
	/* 100*/	NavLadderConnectVector m_ladder[ CNavLadder::NUM_LADDER_DIRECTIONS ];	// list of ladders leading up and down from this area
	/* 108*/	NavConnectVector m_elevatorAreas;							// a list of areas reachable via elevator from this area

	/* 112*/	unsigned int m_nearNavSearchMarker;							// used in GetNearestNavArea()

	/* 116*/	CNavArea *m_parent;											// the area just prior to this on in the search path
	/* 120*/	NavTraverseType m_parentHow;								// how we get from parent to us

	/* 124*/	float m_pathLengthSoFar;									// length of path so far, needed for limiting pathfind max path length

	/* *************** 360 cache line *************** */

	/* 128*/	CFuncElevator *m_elevator;									// if non-NULL, this area is in an elevator's path. The elevator can transport us vertically to another area.

	// --- End critical data --- 
};

class CNavArea : protected CNavAreaCriticalData
{
	public:
		DECLARE_CLASS_NOBASE( CNavArea )
		virtual ~CNavArea() = 0;
		
		virtual void OnServerActivate( void ) = 0;						// (EXTEND) invoked when map is initially loaded
		virtual void OnRoundRestart( void ) = 0;						// (EXTEND) invoked for each area when the round restarts
		virtual void OnRoundRestartPreEntity( void ) { };			// invoked for each area when the round restarts, but before entities are deleted and recreated
		virtual void OnEnter( CBaseCombatCharacter *who, CNavArea *areaJustLeft ) { };	// invoked when player enters this area 
		virtual void OnExit( CBaseCombatCharacter *who, CNavArea *areaJustEntered ) { };// invoked when player exits this area 

		virtual void OnDestroyNotify( CNavArea *dead ) = 0;				// invoked when given area is going away
		virtual void OnDestroyNotify( CNavLadder *dead ) = 0;			// invoked when given ladder is going away

		virtual void OnEditCreateNotify( CNavArea *newArea ) { };		// invoked when given area has just been added to the mesh in edit mode
		virtual void OnEditDestroyNotify( CNavArea *deadArea ) { };		// invoked when given area has just been deleted from the mesh in edit mode
		virtual void OnEditDestroyNotify( CNavLadder *deadLadder ) { };	// invoked when given ladder has just been deleted from the mesh in edit mode

		virtual void Save( CUtlBuffer &fileBuffer, unsigned int version ) const = 0;	// (EXTEND)
		virtual NavErrorType Load( CUtlBuffer &fileBuffer, unsigned int version, unsigned int subVersion ) = 0;		// (EXTEND)
		virtual NavErrorType PostLoad( void ) = 0;								// (EXTEND) invoked after all areas have been loaded - for pointer binding, etc

		virtual void SaveToSelectedSet( KeyValues *areaKey ) const = 0;		// (EXTEND) saves attributes for the area to a KeyValues
		virtual void RestoreFromSelectedSet( KeyValues *areaKey ) = 0;		// (EXTEND) restores attributes from a KeyValues
		
		virtual void UpdateBlocked( bool force = false, int teamID = -2 ) = 0;		// Updates the (un)blocked status of the nav area (throttled)
		virtual bool IsBlocked( int teamID, bool ignoreNavBlockers = false ) const = 0;
	public:
		unsigned int GetID( void ) const	{ return m_id; }
		
		void SetParent( CNavArea *parent, NavTraverseType how = NUM_TRAVERSE_TYPES )	{ m_parent = parent; m_parentHow = how; }
		CNavArea *GetParent( void ) const	{ return m_parent; }
		NavTraverseType GetParentHow( void ) const	{ return m_parentHow; }
		
		void SetCostSoFar( float value )	{ m_costSoFar = value; }
		float GetCostSoFar( void ) const	{ return m_costSoFar; }
		
		void SetTotalCost( float value )	{ m_totalCost = value; }
		float GetTotalCost( void ) const	{ return m_totalCost; }
		
		void SetPathLengthSoFar( float value )	{ m_pathLengthSoFar = value; }
		float GetPathLengthSoFar( void ) const	{ return m_pathLengthSoFar; }
		
		static void ClearSearchLists( void );
		
		int GetAttributes( void ) const			{ return m_attributeFlags; }
		bool HasAttributes( int bits ) const	{ return ( m_attributeFlags & bits ) ? true : false; }
		
		void ComputeNormal( Vector *normal, bool alternate = false ) const;
		float GetSizeX( void ) const			{ return m_seCorner.x - m_nwCorner.x; }
		float GetSizeY( void ) const			{ return m_seCorner.y - m_nwCorner.y; }
		const Vector &GetCenter( void ) const	{ return m_center; }
		inline float GetZ( const Vector * RESTRICT pPos ) const;
		inline float GetZ( const Vector &pos ) const;
		float GetZ( float x, float y ) const RESTRICT;
		//static bool (CNavArea::*func_Contains)(const Vector &pos) const;
		bool Contains(const Vector &pos) const
		{
			//return (this->*func_Contains)(pos);
			return false;
		};
		bool Contains( const CNavArea *area ) const;
		
		NavDirType ComputeDirection( Vector *point );
		
		int GetAdjacentCount( NavDirType dir ) const	{ return m_connect[ dir ].Count(); }
		CNavArea *GetAdjacentArea( NavDirType dir, int i ) const;
		const NavConnectVector *GetAdjacentAreas( NavDirType dir ) const	{ return &m_connect[dir]; }
		const NavLadderConnectVector *GetLadders( CNavLadder::LadderDirectionType dir ) const	{ return &m_ladder[dir]; }
		
		static void MakeNewMarker( void )	{ ++m_masterMarker; if (m_masterMarker == 0) m_masterMarker = 1; }
		void Mark( void )					{ m_marker = m_masterMarker; };
		BOOL IsMarked( void ) const			{ return (m_marker == m_masterMarker) ? true : false; };
		
		float ComputeAdjacentConnectionHeightChange( const CNavArea *destinationArea ) const;			// return height change between edges of adjacent nav areas (not actual underlying ground)
		
		void GetClosestPointOnArea( const Vector * RESTRICT pPos, Vector *close ) const RESTRICT;	// return closest point to 'pos' on this area - returned point in 'close'
		void GetClosestPointOnArea( const Vector &pos, Vector *close ) const { return GetClosestPointOnArea( &pos, close ); }
		
		bool IsConnected( const CNavArea *area, NavDirType dir ) const;	// return true if given area is connected in given direction
		
		bool IsEdge( NavDirType dir ) const;						// return true if there are no bi-directional links on the given side
		
		CFuncElevator *GetElevator( void ) const												{ return ( m_attributeFlags & NAV_MESH_HAS_ELEVATOR ) ? m_elevator : NULL; }
		const NavConnectVector &GetElevatorAreas( void ) const									{ return m_elevatorAreas; };
		
		void ComputeClosestPointInPortal( const CNavArea *to, NavDirType dir, const Vector &fromPos, Vector *closePos ) const; // compute closest point within the "portal" between to adjacent areas
		void ComputePortal( const CNavArea *to, NavDirType dir, Vector *center, float *halfWidth ) const;		// compute portal to adjacent area
		
		bool IsOpen( void ) const;									// true if on "open list"
		void AddToOpenList( void );									// add to open list in decreasing value order
		void AddToOpenListTail( void );								// add to tail of the open list
		void UpdateOnOpenList( void );								// a smaller value has been found, update this area on the open list
		void RemoveFromOpenList( void );
		static bool IsOpenListEmpty( void );
		static CNavArea *PopOpenList( void );
		bool IsClosed( void ) const;
		void AddToClosedList( void );
		void RemoveFromClosedList( void );
	private:
		friend class CNavMesh;
		friend class CNavLadder;
		friend class CCSNavArea;	
	
		static bool m_isReset;										// if true, don't bother cleaning up in destructor since everything is going away

		/*
		m_nwCorner
			nw           ne
			 +-----------+
			 | +-->x     |
			 | |         |
			 | v         |
			 | y         |
			 |           |
			 +-----------+
			sw           se
						m_seCorner
		*/

		static unsigned int m_nextID;								// used to allocate unique IDs
		unsigned int m_id;											// unique area ID
		unsigned int m_debugid;

		Place m_place;												// place descriptor

		CountdownTimer m_blockedTimer;								// Throttle checks on our blocked state while blocked
		void UpdateBlockedFromNavBlockers( void );					// checks if nav blockers are still blocking the area

		bool m_isUnderwater;										// true if the center of the area is underwater

		bool m_isBattlefront;

		float m_avoidanceObstacleHeight;							// if nonzero, a prop is obstructing movement through this nav area
		CountdownTimer m_avoidanceObstacleTimer;					// Throttle checks on our obstructed state while obstructed

		//- for hunting -------------------------------------------------------------------------------------
		float m_clearedTimestamp[ MAX_NAV_TEAMS ];					// time this area was last "cleared" of enemies

		//- "danger" ----------------------------------------------------------------------------------------
		float m_danger[ MAX_NAV_TEAMS ];							// danger of this area, allowing bots to avoid areas where they died in the past - zero is no danger
		float m_dangerTimestamp[ MAX_NAV_TEAMS ];					// time when danger value was set - used for decaying
		void DecayDanger( void );

		//- hiding spots ------------------------------------------------------------------------------------
		HidingSpotVector m_hidingSpots;
		bool IsHidingSpotCollision( const Vector &pos ) const;		// returns true if an existing hiding spot is too close to given position

		//- encounter spots ---------------------------------------------------------------------------------
		SpotEncounterVector m_spotEncounters;						// list of possible ways to move thru this area, and the spots to look at as we do
		void AddSpotEncounters( const CNavArea *from, NavDirType fromDir, const CNavArea *to, NavDirType toDir );	// add spot encounter data when moving from area to area

		float m_earliestOccupyTime[ MAX_NAV_TEAMS ];				// min time to reach this spot from spawn

	#ifdef DEBUG_AREA_PLAYERCOUNTS
		CUtlVector< int > m_playerEntIndices[ MAX_NAV_TEAMS ];
	#endif

		//- lighting ----------------------------------------------------------------------------------------
		float m_lightIntensity[ NUM_CORNERS ];						// 0..1 light intensity at corners

		//- A* pathfinding algorithm ------------------------------------------------------------------------
		static unsigned int m_masterMarker;

		static CNavArea *m_openList;
		static CNavArea *m_openListTail;
};

inline float CNavArea::GetZ( const Vector * RESTRICT pos ) const
{
	return GetZ( pos->x, pos->y );
}

inline float CNavArea::GetZ( const Vector & pos ) const
{
	return GetZ( pos.x, pos.y );
}


//--------------------------------------------------------------------------------------------------------------
inline CNavArea *CNavArea::GetAdjacentArea( NavDirType dir, int i ) const
{
	if ( ( i < 0 ) || ( i >= m_connect[dir].Count() ) )
		return NULL;
	return m_connect[dir][i].area;
}

//--------------------------------------------------------------------------------------------------------------
inline bool CNavArea::IsOpen( void ) const
{
	return (m_openMarker == m_masterMarker) ? true : false;
}

//--------------------------------------------------------------------------------------------------------------
inline bool CNavArea::IsOpenListEmpty( void )
{
	return (m_openList) ? false : true;
}

//--------------------------------------------------------------------------------------------------------------
inline CNavArea *CNavArea::PopOpenList( void )
{
	if ( m_openList )
	{
		CNavArea *area = m_openList;
	
		// disconnect from list
		area->RemoveFromOpenList();
		area->m_prevOpen = NULL;
		area->m_nextOpen = NULL;

		return area;
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------------------------
inline bool CNavArea::IsClosed( void ) const
{
	if (IsMarked() && !IsOpen())
		return true;

	return false;
}

//--------------------------------------------------------------------------------------------------------------
inline void CNavArea::AddToClosedList( void )
{
	Mark();
}

//--------------------------------------------------------------------------------------------------------------
inline void CNavArea::RemoveFromClosedList( void )
{
	// since "closed" is defined as visited (marked) and not on open list, do nothing
}

#endif // _NAV_AREA_H_
