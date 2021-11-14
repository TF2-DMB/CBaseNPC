#if !defined _CBASENPC_NAV_INC_
#define _CBASENPC_NAV_INC_

enum
{
	NAV_MESH_CROUCH = 0x0001, 
	NAV_MESH_JUMP = 0x0002, 
	NAV_MESH_PRECISE = 0x0004, 
	NAV_MESH_NO_JUMP = 0x0008, 
	NAV_MESH_STOP = 0x0010, 
	NAV_MESH_RUN = 0x0020, 
	NAV_MESH_WALK = 0x0040, 
	NAV_MESH_AVOID = 0x0080, 
	NAV_MESH_TRANSIENT = 0x0100, 
	NAV_MESH_DONT_HIDE = 0x0200, 
	NAV_MESH_STAND = 0x0400, 
	NAV_MESH_NO_HOSTAGES = 0x0800
};

enum NavDirType
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,

	NUM_DIRECTIONS
};

stock NavDirType OppositeDirection(NavDirType dir)
{
	switch (dir)
	{
		case NORTH: return SOUTH;
		case SOUTH: return NORTH;
		case EAST:	return WEST;
		case WEST:	return EAST;
	}

	return NORTH;
}

enum NavTraverseType
{
	// NOTE: First 4 directions MUST match NavDirType
	GO_NORTH = 0,
	GO_EAST,
	GO_SOUTH,
	GO_WEST,

	GO_LADDER_UP,
	GO_LADDER_DOWN,
	GO_JUMP,
	GO_ELEVATOR_UP,
	GO_ELEVATOR_DOWN,

	NUM_TRAVERSE_TYPES
};

enum NavCornerType
{
	NORTH_WEST = 0,
	NORTH_EAST = 1,
	SOUTH_EAST = 2,
	SOUTH_WEST = 3,

	NUM_CORNERS
};

enum CNavMesh 
{
	TheNavMesh
};

methodmap SurroundingAreasCollector < Handle
{
	public native CNavArea Get(int index);
	public native int Count();
};

/**
 * The global interface to the Navigation Mesh.
 */
methodmap CNavMesh
{
	/**
	 * Pointer to TheNavMesh.
	 */
	property Address Address
	{
		public native get();
	}

	/**
	 * Whether the nav mesh is loaded or not.
	 * 
	 * @return     True if loaded, false otherwise
	 */
	public native bool IsLoaded();

	/**
	 * Whether the nav mesh is analyzed or not.
	 * 
	 * @return     True if analyzed, false otherwise
	 */
	public native bool IsAnalyzed();

	/**
	 * Whether the nav mesh is out of date with the current map.
	 * 
	 * @return     True if out of date, false otherwise
	 */
	public native bool IsOutOfDate();

	/**
	 * The amount of areas the mesh contains.
	 * 
	 * @return     Amount of areas
	 */
	public native int GetNavAreaCount();

	/**
	 * Starting from startArea, collect adjacent areas via a breadth-first
	 * search continuing outward until travelDistanceLimit is reached.
	 *
	 * Areas in the collection will be "marked", returning true for IsMarked(). 
	 * Each area in the collection's GetCostSoFar() will be approximate travel
	 * distance from startArea.
	 *
	 * @param startArea               Area to start searching from
	 * @param travelDistanceLimit     Maximum travel distance
	 * @param maxStepUpLimit          Maximum step height
	 * @param maxDropDownLimit        Maximum drop down height limit
	 * @return                        SurroundingAreasCollector iterator. You must delete this
	 *                                when you are done with it.
 	 */
	public native SurroundingAreasCollector CollectSurroundingAreas(CNavArea startArea, float travelDistanceLimit = 1500.0, float maxStepUpLimit = 18.0, float maxDropDownLimit = 100.0);
	
	/**
	 * Given an ID, return the associated area.
	 * 
	 * @param id     Area ID
	 * @return       CNavArea, or NULL_AREA if not found.
	 */
	public native CNavArea GetNavAreaByID(int id);

	/**
	 * Finds the nearest CNavArea to the given position.
	 *
	 * @param pos                Position to test for
	 * @param anyZ               If true, then Z will not be taken into account
	 * @param maxDist            Maximum search radius from position
	 * @param checkLOS           If true, will check if areas have LOS to pos
	 * @param checkGround        If true, will adjust the given position to the ground of
	 *                           any overlapping CNavArea.
	 * @param team               Team to check for area is blocked or not
	 * @return                   CNavArea, or NULL_AREA if no area was found
	 */
	public native CNavArea GetNearestNavArea(const float pos[3], bool anyZ = false, float maxDist = 10000.0, bool checkLOS = false, bool checkGround = true, int team = TEAM_ANY);
};

methodmap CNavLadder
{
	/**
	 * The length of the ladder.
	 */
	property float length
	{
		public native get();
	}
};

enum HidingSpotFlags
{ 
	IN_COVER			= 0x01,							// in a corner with good hard cover nearby
	GOOD_SNIPER_SPOT	= 0x02,							// had at least one decent sniping corridor
	IDEAL_SNIPER_SPOT	= 0x04,							// can see either very far, or a large area, or both
	EXPOSED				= 0x08							// spot in the open, usually on a ledge or cliff
};

methodmap HidingSpot
{
	/**
	 * Gets the ID of the hiding spot.
	 * 
	 * @return     ID
	 */
	public native int GetID();

	/**
	 * Gets the flags of the hiding spot.
	 * 
	 * @return     HidingSpotFlags
	 */
	public native HidingSpotFlags GetFlags();

	/**
	 * Gets the world position of the hiding spot.
	 * 
	 * @param vec     Vector buffer
	 */
	public native void GetPosition(float vec[3]);

	/**
	 * Gets the CNavArea containing this hiding spot.
	 * 
	 * @return     CNavArea pointer
	 */
	public native CNavArea GetArea();

	/**
	 * Whether or not the hiding spot is in good cover.
	 * 
	 * @return     True if in good cover, false otherwise.
	 */
	public bool HasGoodCover()
	{ 
		return (this.GetFlags() & IN_COVER) ? true : false;
	}

	/**
	 * Whether or not the hiding spot is a good sniper spot.
	 * 
	 * @return     True if it's a good sniper spot, false otherwise.
	 */
	public bool IsGoodSniperSpot()
	{ 
		return (this.GetFlags() & GOOD_SNIPER_SPOT) ? true : false;
	}

	/**
	 * Whether or not the hiding spot is an ideal sniper spot.
	 * 
	 * @return     True if it's an ideal sniper spot, false otherwise.
	 */
	public bool IsIdealSniperSpot()
	{ 
		return (this.GetFlags() & IDEAL_SNIPER_SPOT) ? true : false;
	}

	/**
	 * Whether or not the hiding spot is exposed; a bad hiding spot.
	 * 
	 * @return     True if it's exposed, false otherwise.
	 */
	public bool IsExposed()
	{ 
		return (this.GetFlags() & EXPOSED) ? true : false;
	}
}

enum TheHidingSpotsVector
{
	TheHidingSpots
}

/**
 * A global list vector object that contains all the hiding spots loaded from the mesh.
 */
methodmap TheHidingSpotsVector
{
	/**
	 * The amount of hiding spots loaded.
	 * 
	 * @return     Amount of hiding spots
	 */
	property int Length
	{
		public native get();
	}

	/**
	 * Returns the hiding spot at the given index in the list.
	 * 
	 * @param i     Index
	 * @return      HidingSpot pointer, or NULL_HIDING_SPOT if out of bounds
	 */
	public native HidingSpot Get(int i);
};

/**
 * Gets a HidingSpot by ID.
 * 
 * @param id     ID to search
 * @return       HidingSpot pointer, or NULL_HIDING_SPOT if not found.
 */
native HidingSpot GetHidingSpotByID(int id);

/**
 * A rectangular region defining a walkable area in the environment.
 */
methodmap CNavArea
{
	/**
	 * Updates the blocked status of the area.
	 *
	 * @param force      If true, will ignore the throttle timer and update
	 * @param teamID     Team to check for blocked status for
	 */
	public native void UpdateBlocked(bool force = false, int teamID = TEAM_ANY);

	/**
	 * Checks if the area is currently blocked.
	 *
	 * @param teamID                 Team to check is blocked for
	 * @param ignoreNavBlockers      If true, will not consider "func_nav_blocker"
	 *                               entities blocking the area.
	 * @return                       True if blocked, false otherwise
	 */
	public native bool IsBlocked(int teamID, bool ignoreNavBlockers = false);

	/**
	 * The ID of the area.
	 *
	 * @return     ID
	 */
	public native int GetID();
	
	/**
	 * Sets the parent of the area. This is mainly used in area collection and
	 * pathfinding algorithms.
	 *
	 * @param parent    Area to set parent to
	 * @param how       The traversal type
	 */
	public native void SetParent(CNavArea parent, NavTraverseType how = NUM_TRAVERSE_TYPES);

	/**
	 * Gets the parent of the area. This is mainly used in area collection and
	 * pathfinding algorithms.
	 *
	 * @return     CNavArea
	 */
	public native CNavArea GetParent();

	/**
	 * The traversal type from parent to this area. This is mainly used in area
	 * collection and pathfinding algorithms.
	 *
	 * @return     Traversal type
	 */
	public native NavTraverseType GetParentHow();
	
	/**
	 * Sets the travel cost so far. This is mainly used in area collection and pathfinding
	 * algorithms.
	 *
	 * @param value     Cost so far
	 */
	public native void SetCostSoFar(float value);

	/**
	 * Travel cost so far. This is mainly used in area collection and pathfinding
	 * algorithms.
	 *
	 * @return     Cost so far
	 */
	public native float GetCostSoFar();
	
	/**
	 * Attributes of the area.
	 *
	 * @return     Attribute flags
	 */
	public native int GetAttributes();

	/**
	 * Checks if the area has attribute flags set.
	 *
	 * @param bits     Bitflags to check
	 * @return         True if has attribute, false otherwise
	 */
	public bool HasAttributes(int bits) { return ( this.GetAttributes() & bits ) ? true : false; }

	/**
	 * World position of an area's corner.
	 * 
	 * @param cornerType
	 * @param vec            Position vector
	 * @error                Invalid corner type
	 */
	public native void GetCorner(NavCornerType cornerType, float vec[3]);

	/**
	 * The center position of the area.
	 *
	 * @param vecCenter      Position vector
	 */
	public native void GetCenter(float vecCenter[3]);
	
	/**
	 * The amount of outgoing connections to areas in a given direction.
	 * 
	 * @param dir     Direction
	 * @return        Number of outgoing connections
	 */
	public native int GetAdjacentCount(NavDirType dir);

	/**
	 * Returns an area that this area has an outgoing connection to in the given direction.
	 * 
	 * @param dir     Direction
	 * @param i       Index in list
	 * @return        CNavArea, or NULL_AREA if out of bounds
	 */
	public native CNavArea GetAdjacentArea(NavDirType dir, int i);

	/**
	 * Pushes a list of areas that this area has an outgoing connection to in the given direction.
	 * 
	 * @param dir     Direction
	 * @return        Amount of areas inserted into list
	 */
	public int GetAdjacentAreas(NavDirType dir, ArrayList areaList)
	{
		int count = this.GetAdjacentCount(dir);
		for (int i = 0; i < count; i++)
		{
			areaList.Push(this.GetAdjacentArea(dir, i));
		}

		return count;
	}

	/**
	 * Checks if the this area is connected to the other area in a given direction.
	 * This does not check incoming one-way connections.
	 *
	 * @param area     Area to check
	 * @param dir      Direction to check. If NUM_DIRECTIONS, will check all
	 *                 directions.
	 * @return         True if connected, false otherwise
	 */
	public native bool IsConnected(CNavArea area, NavDirType dir);

	/**
	 * The amount of incoming, one-way connections from areas in a given direction.
	 * 
	 * @param dir     Direction
	 * @return        Number of incoming connections
	 */
	public native int GetIncomingConnectionCount(NavDirType dir);

	/**
	 * Returns an area that has an incoming, one-way connection from the given direction.
	 * 
	 * @param dir     Direction
	 * @param i       Index in list
	 * @return        CNavArea, or NULL_AREA if out of bounds
	 */
	public native CNavArea GetIncomingConnection(NavDirType dir, int i);

	/**
	 * Pushes a list of areas that this area has an outgoing connection to in the given direction.
	 * 
	 * @param dir     Direction
	 * @return        Amount of areas inserted into list
	 */
	public int GetIncomingConnections(NavDirType dir, ArrayList areaList)
	{
		int count = this.GetIncomingConnectionCount(dir);
		for (int i = 0; i < count; i++)
		{
			areaList.Push(this.GetIncomingConnection(dir, i));
		}

		return count;
	}

	/**
	 * Checks if there are no bi-directional links on the given side.
	 *
	 * @param dir     Direction to check
	 * @return        True if no bi-directional links, false otherwise
	 */
	public native bool IsEdge(NavDirType dir);
	
	/**
	 * Checks if this area completely contains the given area.
	 *
	 * @param area     Area to check
	 * @return         True if completely contains area, false otherwise
	 */
	public native bool Contains(CNavArea area);

	/**
	 * Returns the Z component of the given world XY values.
	 *
	 * @param x     x
	 * @param y     y
	 * @return      z
	 */
	public native float GetZ(float x, float y);

	/**
	 * Returns the Z component of the given world position.
	 *
	 * @param vecPos      Position vector
	 * @return            z
	 */
	public native float GetZVector(float vecPos[3]);

	/**
	 * Computes the ground normal of the area.
	 *
	 * @param vecNormal     Direction vector
	 * @param alternate
	 */
	public native void ComputeNormal(float vecNormal[3], bool alternate = false);

	/**
	 * Returns a 0..1 light intensity averaged over the whole area.
	 * 
	 * Light intensity is calculated by either performing the `nav_generate` 
	 * or `nav_update_lighting` commands on a LISTEN server. Light intensity 
	 * is not automatically calculated if you generate the navmesh 
	 * incrementally via `nav_generate_incremental`. 
	 *
	 * If light intensity is not calculated then the area is assumed to be fully
	 * lit (1.0).
	 *
	 * @return     0.0 - 1.0, 0 = dark, 1 = light
	 */
	public native float GetLightIntensity();

	/**
	 * Returns a 0..1 light intensity for the given position in the area. For
	 * more detail about light intensity, see `GetLightIntensity()`.
	 * 
	 * @param vecPos     Position to check in area
	 * @return           0.0 - 1.0, 0 = dark, 1 = light
	 */
	public native float GetPositionLightIntensity(const float vecPos[3]);

	/**
	 * Returns the number of HidingSpots contained by this area.
	 * 
	 * @return     Number of HidingSpots
	 */
	public native int GetHidingSpotCount();

	/**
	 * Returns a HidingSpot contained within this area.
	 * 
	 * @param i     Index
	 * @return      A pointer to a HidingSpot, or NULL_HIDING_SPOT if out of bounds.
	 */
	public native HidingSpot GetHidingSpot(int i);

	/**
	 * Inserts the area's HidingSpots into the given list.
	 * 
	 * @param spots     List to store HidingSpot pointers
	 * @return          Number of HidingSpots inserted
	 */
	public int GetHidingSpots(ArrayList spots)
	{
		int count = this.GetHidingSpotCount();
		for (int i = 0; i < count; i++)
		{
			spots.Push(this.GetHidingSpot(i));
		}

		return count;
	}

	/**
	 * Clears the current open list. Used at the beginning of a
	 * area collection/pathfinding algorithm.
	 */
	public static native void ClearSearchLists();

	/**
	 * Whether the open list is empty or not.
	 * 
	 * @return     True if empty, false otherwise.
	 */
	public static native bool IsOpenListEmpty();

	/**
	 * Pops a CNavArea from the beginning of the open list and returns it.
	 * 
	 * @return     CNavArea pointer
	 */
	public static native CNavArea PopOpenList();

	/**
	 * Checks whether the area is in the open list.
	 * 
	 * @return     True if in open list, false otherwise.
	 */
	public native bool IsOpen();

	/**
	 * Inserts the area at the beginning of the open list.
	 */
	public native void AddToOpenList();

	/**
	 * Inserts the area at the end of the open list.
	 */
	public native void AddToOpenListTail();

	/**
	 * If the area is in the open list, updates the area's position in the
	 * open list.
	 */
	public native void UpdateOnOpenList();

	/**
	 * Checks whether the area is in the closed (visited) list.
	 * 
	 * @return     True if in the closed list, false otherwise.
	 */
	public native bool IsClosed();

	/**
	 * Adds the area to the closed list.
	 */
	public native void AddToClosedList();
};

enum TheNavAreasVector
{
	TheNavAreas
}

/**
 * A global list vector object that contains all the areas loaded from the mesh.
 */
methodmap TheNavAreasVector
{
	/**
	 * The amount of areas loaded. This returns the same value as CNavMesh::GetNavAreaCount().
	 * 
	 * @return     Amount of areas
	 */
	property int Length
	{
		public native get();
	}

	/**
	 * Returns the area at the given index in the list.
	 * 
	 * @param i     Index
	 * @return      CNavArea pointer, or NULL_AREA if out of bounds
	 */
	public native CNavArea Get(int i);
};

#endif