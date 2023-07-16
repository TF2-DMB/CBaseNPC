#ifndef _NAV_AREA_H_
#define _NAV_AREA_H_

enum { MAX_NAV_TEAMS = 2 };
class CToolsNavMesh;

#include "tier1/utlvector.h"
#include "nav.h"
#include "nav_ladder.h"

#if !defined(_X360)
typedef CUtlVectorUltraConservativeAllocator CNavVectorAllocator;
#else
typedef CNavVectorNoEditAllocator CNavVectorAllocator;
#endif

#define DECLARE_CLASS_NOBASE( className )					typedef className ThisClass;

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


//--------------------------------------------------------------------------------------------------------------
/**
 * A HidingSpot is a good place for a bot to crouch and wait for enemies
 */
class HidingSpot
{
public:
	virtual ~HidingSpot() = 0;

	enum 
	{ 
		IN_COVER			= 0x01,							// in a corner with good hard cover nearby
		GOOD_SNIPER_SPOT	= 0x02,							// had at least one decent sniping corridor
		IDEAL_SNIPER_SPOT	= 0x04,							// can see either very far, or a large area, or both
		EXPOSED				= 0x08							// spot in the open, usually on a ledge or cliff
	};

	bool HasGoodCover( void ) const			{ return (m_flags & IN_COVER) ? true : false; }	// return true if hiding spot in in cover
	bool IsGoodSniperSpot( void ) const		{ return (m_flags & GOOD_SNIPER_SPOT) ? true : false; }
	bool IsIdealSniperSpot( void ) const	{ return (m_flags & IDEAL_SNIPER_SPOT) ? true : false; }
	bool IsExposed( void ) const			{ return (m_flags & EXPOSED) ? true : false; }	

	int GetFlags( void ) const		{ return m_flags; }

	const Vector &GetPosition( void ) const		{ return m_pos; }	// get the position of the hiding spot
	unsigned int GetID( void ) const			{ return m_id; }
	const CNavArea *GetArea( void ) const		{ return m_area; }	// return nav area this hiding spot is within

public:
	void SetFlags( int flags )				{ m_flags |= flags; }	// FOR INTERNAL USE ONLY
	void SetPosition( const Vector &pos )	{ m_pos = pos; }		// FOR INTERNAL USE ONLY

private:
	friend class CNavMesh;

	HidingSpot( void );										// must use factory to create

	Vector m_pos;											// world coordinates of the spot
	unsigned int m_id;										// this spot's unique ID
	unsigned int m_marker;									// this spot's unique marker
	CNavArea *m_area;										// the nav area containing this hiding spot

	unsigned char m_flags;									// bit flags
};

typedef CUtlVectorUltraConservative< HidingSpot * > HidingSpotVector;
extern HidingSpotVector TheHidingSpots;

HidingSpot *GetHidingSpotByID( unsigned int id );

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

class CBaseCombatCharacter;
class CNavArea : protected CNavAreaCriticalData
{
	public:
		DECLARE_CLASS_NOBASE( CNavArea )

		virtual ~CNavArea() = 0;
		
		virtual void OnServerActivate( void ) = 0;						// (EXTEND) invoked when map is initially loaded
		virtual void OnRoundRestart( void ) = 0;						// (EXTEND) invoked for each area when the round restarts
		virtual void OnRoundRestartPreEntity( void ) = 0;			// invoked for each area when the round restarts, but before entities are deleted and recreated
		virtual void OnEnter( CBaseCombatCharacter* who, CNavArea *areaJustLeft ) = 0;	// invoked when player enters this area 
		virtual void OnExit( CBaseCombatCharacter* who, CNavArea *areaJustEntered ) = 0; // invoked when player exits this area 

		virtual void OnDestroyNotify( CNavArea *dead ) = 0;				// invoked when given area is going away
		virtual void OnDestroyNotify( CNavLadder *dead ) = 0;			// invoked when given ladder is going away

		virtual void OnEditCreateNotify( CNavArea *newArea ) = 0;		// invoked when given area has just been added to the mesh in edit mode
		virtual void OnEditDestroyNotify( CNavArea *deadArea ) = 0;		// invoked when given area has just been deleted from the mesh in edit mode
		virtual void OnEditDestroyNotify( CNavLadder *deadLadder ) = 0;	// invoked when given ladder has just been deleted from the mesh in edit mode

		virtual void Save( CUtlBuffer &fileBuffer, unsigned int version ) const = 0;	// (EXTEND)
		virtual NavErrorType Load( CUtlBuffer &fileBuffer, unsigned int version, unsigned int subVersion ) = 0;		// (EXTEND)
		virtual NavErrorType PostLoad( void ) = 0;								// (EXTEND) invoked after all areas have been loaded - for pointer binding, etc

		virtual void SaveToSelectedSet( KeyValues *areaKey ) const = 0;		// (EXTEND) saves attributes for the area to a KeyValues
		virtual void RestoreFromSelectedSet( KeyValues *areaKey ) = 0;		// (EXTEND) restores attributes from a KeyValues

		unsigned int GetID( void ) const	{ return m_id; }
		
		int GetAttributes( void ) const			{ return m_attributeFlags; }
		bool HasAttributes( int bits ) const	{ return ( m_attributeFlags & bits ) ? true : false; }

		virtual void UpdateBlocked( bool force = false, int teamID = TEAM_ANY ) = 0;		// Updates the (un)blocked status of the nav area (throttled)
		virtual bool IsBlocked( int teamID, bool ignoreNavBlockers = false ) const = 0;

		bool IsUnderwater( void ) const		{ return m_isUnderwater; }

		bool IsOverlapping( const Vector &pos, float tolerance = 0.0f ) const;	// return true if 'pos' is within 2D extents of area.
		bool IsOverlapping( const CNavArea *area ) const;			// return true if 'area' overlaps our 2D extents
		bool IsOverlapping( const Extent &extent ) const;			// return true if 'extent' overlaps our 2D extents
		bool IsOverlappingX( const CNavArea *area ) const;			// return true if 'area' overlaps our X extent
		bool IsOverlappingY( const CNavArea *area ) const;			// return true if 'area' overlaps our Y extent
		inline float GetZ( const Vector * RESTRICT pPos ) const;
		inline float GetZ( const Vector &pos ) const;
		float GetZ( float x, float y ) const RESTRICT;
		bool Contains( const Vector &pos ) const;
		bool Contains( const CNavArea *area ) const;
		void GetClosestPointOnArea( const Vector * RESTRICT pPos, Vector *close ) const RESTRICT;	// return closest point to 'pos' on this area - returned point in 'close'
		void GetClosestPointOnArea( const Vector &pos, Vector *close ) const { return GetClosestPointOnArea( &pos, close ); }
		float GetDistanceSquaredToPoint( const Vector &pos ) const;	// return shortest distance between point and this area
		
		float ComputeAdjacentConnectionHeightChange( const CNavArea *destinationArea ) const;			// return height change between edges of adjacent nav areas (not actual underlying ground)

		bool IsEdge( NavDirType dir ) const;						// return true if there are no bi-directional links on the given side

		int GetAdjacentCount( NavDirType dir ) const	{ return m_connect[ dir ].Count(); }
		CNavArea *GetAdjacentArea( NavDirType dir, int i ) const;
		
		const NavConnectVector *GetAdjacentAreas( NavDirType dir ) const	{ return &m_connect[dir]; }
		bool IsConnected( const CNavArea *area, NavDirType dir ) const;	// return true if given area is connected in given direction

		const NavConnectVector *GetIncomingConnections( NavDirType dir ) const	{ return &m_incomingConnect[dir]; }	// get areas connected TO this area by a ONE-WAY link (ie: we have no connection back to them)

		const NavLadderConnectVector *GetLadders( CNavLadder::LadderDirectionType dir ) const	{ return &m_ladder[dir]; }
		CFuncElevator *GetElevator( void ) const												{ return ( m_attributeFlags & NAV_MESH_HAS_ELEVATOR ) ? m_elevator : NULL; }
		const NavConnectVector &GetElevatorAreas( void ) const									{ return m_elevatorAreas; };

		void ComputePortal( const CNavArea *to, NavDirType dir, Vector *center, float *halfWidth ) const;		// compute portal to adjacent area
		void ComputeClosestPointInPortal( const CNavArea *to, NavDirType dir, const Vector &fromPos, Vector *closePos ) const; // compute closest point within the "portal" between to adjacent areas
		NavDirType ComputeDirection( Vector *point ) const;

		//- hiding spots ------------------------------------------------------------------------------------
		const HidingSpotVector *GetHidingSpots( void ) const	{ return &m_hidingSpots; }

		//- "danger" ----------------------------------------------------------------------------------------
		virtual float GetDangerDecayRate( void ) const = 0;				// return danger decay rate per second

		//- extents -----------------------------------------------------------------------------------------
		float GetSizeX( void ) const			{ return m_seCorner.x - m_nwCorner.x; }
		float GetSizeY( void ) const			{ return m_seCorner.y - m_nwCorner.y; }
		void GetExtent( Extent *extent ) const;						// return a computed extent (XY is in m_nwCorner and m_seCorner, Z is computed)
		const Vector &GetCenter( void ) const	{ return m_center; }
		Vector GetCorner( NavCornerType corner ) const;
		void ComputeNormal( Vector *normal, bool alternate = false ) const;

		//- lighting ----------------------------------------------------------------------------------------
		float GetLightIntensity( const Vector &pos ) const;			// returns a 0..1 light intensity for the given point
		float GetLightIntensity( float x, float y ) const;			// returns a 0..1 light intensity for the given point
		float GetLightIntensity( void ) const;						// returns a 0..1 light intensity averaged over the whole area

		//- A* pathfinding algorithm ------------------------------------------------------------------------
		static void MakeNewMarker( void )	{ ++m_masterMarker; if (m_masterMarker == 0) m_masterMarker = 1; }
		void Mark( void )					{ m_marker = m_masterMarker; };
		BOOL IsMarked( void ) const			{ return (m_marker == m_masterMarker) ? true : false; };

		void SetParent( CNavArea *parent, NavTraverseType how = NUM_TRAVERSE_TYPES )	{ m_parent = parent; m_parentHow = how; }
		CNavArea *GetParent( void ) const	{ return m_parent; }
		NavTraverseType GetParentHow( void ) const	{ return m_parentHow; }
		
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

		static void ClearSearchLists( void );

		void SetTotalCost( float value )	{ m_totalCost = value; }
		float GetTotalCost( void ) const	{ return m_totalCost; }

		void SetCostSoFar( float value )	{ m_costSoFar = value; }
		float GetCostSoFar( void ) const	{ return m_costSoFar; }
		
		void SetPathLengthSoFar( float value )	{ m_pathLengthSoFar = value; }
		float GetPathLengthSoFar( void ) const	{ return m_pathLengthSoFar; }
		
		//- editing -----------------------------------------------------------------------------------------
		virtual void Draw( void ) const = 0;							// draw area for debugging & editing
		virtual void DrawFilled( int r, int g, int b, int a, float deltaT = 0.1f, bool noDepthTest = true, float margin = 5.0f ) const = 0;	// draw area as a filled rect of the given color
		virtual void DrawSelectedSet( const Vector &shift ) const = 0;
		
		//- generation and analysis -------------------------------------------------------------------------
		virtual void ComputeHidingSpots( void ) = 0;					// analyze local area neighborhood to find "hiding spots" in this area - for map learning
		virtual void ComputeSniperSpots( void ) = 0;					// analyze local area neighborhood to find "sniper spots" in this area - for map learning
		virtual void ComputeSpotEncounters( void ) = 0;					// compute spot encounter data - for map learning
		virtual void ComputeEarliestOccupyTimes( void ) = 0;
		virtual void CustomAnalysis( bool isIncremental = false ) = 0;	// for game-specific analysis
		virtual bool ComputeLighting( void ) = 0;						// compute 0..1 light intensity at corners and center (requires client via listenserver)
		virtual bool IsAbleToMergeWith( CNavArea *other ) const = 0;

		virtual void InheritAttributes( CNavArea *first, CNavArea *second = nullptr ) = 0;

		//- visibility -------------------------------------------------------------------------------------
		struct AreaBindInfo							// for pointer loading and binding
		{
			union
			{
				CNavArea *area;
				unsigned int id;
			};

			unsigned char attributes;				// VisibilityType

			bool operator==( const AreaBindInfo &other ) const
			{
				return ( area == other.area );
			}
		};

		virtual bool IsEntirelyVisible( const Vector &eye, const CBaseEntity *ignore = nullptr ) const = 0;				// return true if entire area is visible from given eyepoint (CPU intensive)
		virtual bool IsPartiallyVisible( const Vector &eye, const CBaseEntity *ignore = nullptr ) const = 0;				// return true if any portion of the area is visible from given eyepoint (CPU intensive)

		virtual bool IsPotentiallyVisible( const CNavArea *area ) const = 0;		// return true if given area is potentially visible from somewhere in this area (very fast)
		virtual bool IsPotentiallyVisibleToTeam( int team ) const = 0;				// return true if any portion of this area is visible to anyone on the given team (very fast)

		virtual bool IsCompletelyVisible( const CNavArea *area ) const = 0;			// return true if given area is completely visible from somewhere in this area (very fast)
		virtual bool IsCompletelyVisibleToTeam( int team ) const = 0;				// return true if given area is completely visible from somewhere in this area by someone on the team (very fast)

	private:
		friend class CNavMesh;
		friend class CToolsNavMesh;
		friend class CNavLadder;
		friend class CCSNavArea;	
	
		//static bool m_isReset;										// if true, don't bother cleaning up in destructor since everything is going away

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

		//static unsigned int m_nextID;								// used to allocate unique IDs
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

		// A case can arise where both this and the game's CNavArea::m_masterMarker may conflict and introduce confusion to pathfinding algorithms, especially if both
		// CBaseNPC and the game's NextBots are used simultaneously. It's not a common case, but the possibility is there.
		// Consider modifying the engine's variables instead to prevent this from happening?
		static unsigned int m_masterMarker;

		static CNavArea *m_openList;
		static CNavArea *m_openListTail;

		//- connections to adjacent areas -------------------------------------------------------------------
		NavConnectVector m_incomingConnect[ NUM_DIRECTIONS ];		// a list of adjacent areas for each direction that connect TO us, but we have no connection back to them

		//---------------------------------------------------------------------------------------------------
		CNavNode *m_node[ NUM_CORNERS ];							// nav nodes at each corner of the area

#if 1 // NEXT_BOT
		CUtlVector< CBaseHandle > m_prerequisiteVector;		// list of prerequisites that must be met before this area can be traversed
#endif

		CNavArea *m_prevHash, *m_nextHash;							// for hash table in CNavMesh
		int m_damagingTickCount;									// this area is damaging through this tick count

		typedef CUtlVectorConservative<AreaBindInfo> CAreaBindInfoArray;

		AreaBindInfo m_inheritVisibilityFrom;						// if non-NULL, m_potentiallyVisibleAreas becomes a list of additions and deletions (NOT_VISIBLE) to the list of this area
		CAreaBindInfoArray m_potentiallyVisibleAreas;				// list of areas potentially visible from inside this area (after PostLoad(), use area portion of union)
		bool m_isInheritedFrom;										// latch used during visibility inheritance computation

		uint32 m_nVisTestCounter;

		CUtlVector< CBaseHandle > m_funcNavCostVector;	// active, overlapping cost entities
};

typedef CUtlVector< CNavArea * > NavAreaVector;
extern NavAreaVector TheNavAreas;

inline float CNavArea::GetZ( const Vector * RESTRICT pos ) const
{
	return GetZ( pos->x, pos->y );
}

inline float CNavArea::GetZ( const Vector & pos ) const
{
	return GetZ( pos.x, pos.y );
}

inline Vector CNavArea::GetCorner( NavCornerType corner ) const
{
	Vector pos;

	switch( corner )
	{
	case NORTH_WEST:
		return m_nwCorner;

	case NORTH_EAST:
		pos.x = m_seCorner.x;
		pos.y = m_nwCorner.y;
		pos.z = m_neZ;
		return pos;

	case SOUTH_WEST:
		pos.x = m_nwCorner.x;
		pos.y = m_seCorner.y;
		pos.z = m_swZ;
		return pos;

	case SOUTH_EAST:
		return m_seCorner;
	}

	return vec3_origin;
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
