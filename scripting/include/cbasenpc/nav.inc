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
	 * The center position of the area.
	 *
	 * @param vecCenter      Position vector
	 */
	public native void GetCenter(float vecCenter[3]);
	
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
};

#endif