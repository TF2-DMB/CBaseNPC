#if defined _CBASENPC_NAV_INC_
 #endinput
#endif
#define _CBASENPC_NAV_INC_

enum GetNavAreaFlags_t
{
	GETNAVAREA_CHECK_LOS			= 0x1,
	GETNAVAREA_ALLOW_BLOCKED_AREAS	= 0x2,
	GETNAVAREA_CHECK_GROUND			= 0x4,
};

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

stock NavDirType DirectionLeft(NavDirType dir)
{
	switch (dir)
	{
		case NORTH: return WEST;
		case SOUTH: return EAST;
		case EAST:	return NORTH;
		case WEST:	return SOUTH;
	}

	return NORTH;
}

stock NavDirType DirectionRight(NavDirType dir)
{
	switch (dir)
	{
		case NORTH: return EAST;
		case SOUTH: return WEST;
		case EAST:	return SOUTH;
		case WEST:	return NORTH;
	}

	return NORTH;
}

stock float DirectionToAngle(NavDirType dir)
{
	switch (dir)
	{
		case NORTH:	return 270.0;
		case SOUTH:	return 90.0;
		case EAST:	return 0.0;
		case WEST:	return 180.0;
	}

	return 0.0;
}

stock NavDirType AngleToDirection(float angle)
{
	while (angle < 0.0)
		angle += 360.0;

	while (angle > 360.0)
		angle -= 360.0;

	if (angle < 45.0 || angle > 315.0)
		return EAST;

	if (angle >= 45.0 && angle < 135.0)
		return SOUTH;

	if (angle >= 135.0 && angle < 225.0)
		return WEST;

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

typeset NavPathCostFunctor
{
	/**
	 * Path cost function
	 * 
	 * @param area         Area arriving to
	 * @param fromArea     Area leaving from. This will be NULL_AREA if `area` is the first area in the path.
	 * @param ladder       If ladder used to traverse to `area`, CNavLadder pointer, otherwise is NULL_LADDER.
	 * @param elevator     If elevator used to traverse to `area`, entity index of elevator, otherwise is -1.
	 * @param length       Length of connection between areas. If ladder or elevator is used, this is -1.0.
	 * @return             Cost to move from `fromArea` to `area`
	 */
	function float(CNavArea area, CNavArea fromArea, CNavLadder ladder, int elevator, float length);
	function float(CNavArea area, CNavArea fromArea, CNavLadder ladder, CBaseEntity elevator, float length);
};

enum CNavMesh 
{
	TheNavMesh
};

methodmap SurroundingAreasCollector < Handle
{
	#pragma deprecated Use AreasCollector.Get() instead.
	public native CNavArea Get(int index);

	#pragma deprecated Use AreasCollector.Count() instead.
	public native int Count();
};

methodmap AreasCollector < SurroundingAreasCollector
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
	 * Given a position, returns the associated nav area.
	 * 
	 * @param pos           World position.
	 * @param beneathLimit  How far beneath the position can we go.
	 * @return              CNavArea, or NULL_AREA if not found.
	 */
	public native CNavArea GetNavArea(const float pos[3], float beneathLimit = 120.0);

	/**
	 * Given an entity, returns the associated nav area.
	 * 
	 * @param entity           Entity to use as position.
	 * @param beneathLimit     How far beneath the position can we go.
	 * @param nGetNavAreaFlags Conditions the search should follow.
	 * @return                 CNavArea, or NULL_AREA if not found.
	 */
	public native CNavArea GetNavAreaEntity(int entity, GetNavAreaFlags_t nGetNavAreaFlags, float flBeneathLimit = 120.0);

	/**
	 * Starting from startArea, collect adjacent areas via a breadth-first
	 * search continuing outward until travelDistanceLimit is reached.
	 *
	 * Areas in the collection will be "marked", returning true for IsMarked(). 
	 * Each area in the collection's GetCostSoFar() will be approximate travel
	 * distance from startArea.
	 *
	 * @note                          Travel cost is calculated between the centers of the areas. This will introduce some inaccuracy to the true travel distance.
	 * @note                          This utilizes the area's SetCostSoFar() and SetTotalCost() functions, which will change the cost stored on the area which is shared with BuildPath(). Be wary of using this with BuildPath(), as doing so may produce unexpected results.
	 * @param startArea               Area to start searching from
	 * @param travelDistanceLimit     Maximum travel distance
	 * @param maxStepUpLimit          Maximum step height
	 * @param maxDropDownLimit        Maximum drop down height limit
	 * @return                        AreasCollector iterator. You must delete this
	 *                                when you are done with it.
 	 */
	public native AreasCollector CollectSurroundingAreas(CNavArea startArea, float travelDistanceLimit = 1500.0, float maxStepUpLimit = 18.0, float maxDropDownLimit = 100.0);
	
	/**
	 * Collects all areas that overlap the box defined by the given `mins` and `maxs`
	 * vectors.
	 * 
	 * @param mins     World vector of the minimum corner of the box.
	 * @param maxs     World vector of the maximum corner of the box.
	 * @return         AreasCollector iterator. You must delete this when you are done with it.
	 */
	public native AreasCollector CollectAreasOverlappingExtent(const float mins[3], const float maxs[3]);

	/**
	 * Collects all areas whose center is within the radius of the given position.
	 * 
	 * @param pos        Position to search for.
	 * @param radius     Radius
	 * @return           AreasCollector iterator. You must delete this when you are done with it.
	 */
	public native AreasCollector CollectAreasInRadius(const float pos[3], float radius);

	/**
	 * Steps through the nav mesh and collects all areas along the line between `startArea` and `endArea`.
	 * 
	 * @param startArea      Start area
	 * @param endArea        End area
	 * @param reachedEnd     Optional. Set to true if collection reached the end area, false if it doesn't reach it 
	 *                       (no mesh between, bad connection, etc).
	 * @return               AreasCollector iterator. You must delete this when you are done with it.
	 */
	public native AreasCollector CollectAreasAlongLine(CNavArea startArea, CNavArea endArea, bool &reachedEnd = false);

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

	/**
	 * Find path from startArea to goalArea via an A* search, using supplied cost heuristic.
	 * 
	 * @note                        To use the created path, travel using pointers from closestArea to the startArea via the GetParent() method until GetParent() is NULL.
	 * @note                        This utilizes the area's SetPathLengthSoFar() and SetTotalCost() functions, which will change the cost stored on the area which is shared with CollectSurroundingAreas(). Be wary of using this with CollectSurroundingAreas(), as doing so may produce unexpected results.
	 * @param startArea             Start area
	 * @param goalArea              Goal area. If NULL_AREA, will compute a path as close as possible to goalPos.
	 * @param goalPos               Goal position. If NULL_VECTOR, then will use the center of goalArea as the goal position.
	 * @param costFunc              Cost function. If cost function returns -1 for an area, that area is considered a dead end.
	 * @param closestArea           If path fails, this contains the closest area to goal. Otherwise, contains the goal area.
	 * @param maxPathLength         If non-zero, the maximum length of the path.
	 * @param teamID                If non-zero, the team ID to use for pathfinding.
	 * @param ignoreNavBlockers     If true, ignores nav blockers.
	 * @return                      True if path was found to goal, false otherwise
	 * @error                       Invalid start area.
	 */
	public native bool BuildPath( CNavArea startArea, 
		CNavArea goalArea, 
		const float goalPos[3], 
		NavPathCostFunctor costFunc = INVALID_FUNCTION,
		CNavArea &closestArea = NULL_AREA, 
		float maxPathLength = 0.0,
		int teamID = TEAM_ANY,
		bool ignoreNavBlockers = false);
};

methodmap CNavLadder
{
	/**
	 * The length of the ladder.
	 */
	property float Length
	{
		public native get();
	}

	#pragma deprecated Use Length instead
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
	property int Count
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

	#pragma deprecated Use Count instead
	property int Length
	{
		public native get();
	}
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
	 * @note             This does nothing in Team Fortress 2.
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
	 * Sets the total travel cost. This is mainly used in area collection and pathfinding
	 * algorithms.
	 *
	 * @param value     Total cost
	 */
	public native void SetTotalCost(float value);

	/**
	 * Total travel cost. This is mainly used in area collection and pathfinding
	 * algorithms.
	 *
	 * @return     Total cost
	 */
	public native float GetTotalCost();

	/**
	 * Sets the path length so far. This is mainly used in area collection and
	 * pathfinding algorithms.
	 *
	 * @param parent    Area to set parent to
	 */
	public native void SetPathLengthSoFar(float value);

	/**
	 * Path length so far. This is mainly used in area collection and pathfinding
	 * algorithms.
	 *
	 * @return     Path length so far
	 */
	public native float GetPathLengthSoFar();

	/**
	 * Calculates the portal between this area and the other.
	 *
	 * @param area          Area to calculate portal to
	 * @param dir           Direction to calculate portal to
	 * @param center        Buffer to store portal position
	 * @param halfWidth     Optional. Half the width of the portal
	 * @error               Invalid area
	 */
	public native void ComputePortal(CNavArea area, NavDirType dir, float center[3], float &halfWidth = 0.0);

	/**
	 * Calculates the closest point to pos within the computed portal.
	 *
	 * @param area        Area to calculate portal to
	 * @param dir         Direction to calculate portal to
	 * @param pos         Position to calculate closest point to
	 * @param close       Buffer to store closest point
	 * @error             Invalid area
	 */
	public native void ComputeClosestPointInPortal(CNavArea area, NavDirType dir, const float pos[3], float close[3]);

	/**
	 * Gets the closest point to pos on the area.
	 *
	 * @param pos      Position to try to get closest to. This does not have to be on the area.
	 * @param close    Buffer to store the closest point
	 */
	public native void GetClosestPointOnArea(const float pos[3], float close[3]);

	/**
	 * Gets the shortest squared distance from area to point.
	 *
	 * @param pos      Position to calculate distance to.
	 * @return         Squared distance
	 */
	public native float GetDistanceSquaredToPoint(const float pos[3]);

	/**
	 * Computes the height difference between this area and the other area.
	 *
	 * @param other    Area to compute height change from
	 * @return         Height difference
	 * @error          Invalid area
	 */
	public native float ComputeAdjacentConnectionHeightChange(CNavArea other);

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
	 * Returns the length of the connection to the adjacent area.
	 * Length is calculated by distance between the areas' centers.
	 * 
	 * @param dir     Direction
	 * @param i       Index in list
	 * @return        Length of connection
	 */
	public native float GetAdjacentLength(NavDirType dir, int i);

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
	 * Returns the length of the connection from the incoming connection.
	 * Length is calculated by distance between the areas' centers.
	 * 
	 * @param dir     Direction
	 * @param i       Index in list
	 * @return        Length of connection
	 */
	public native float GetIncomingConnectionLength(NavDirType dir, int i);

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
	 * Checks if this area completely contains the given position.
	 *
	 * @param pos      Position to check
	 * @return         True if completely contains position, false otherwise
	 */
	public native bool ContainsPoint(const float pos[3]);

	/**
	 * Gets the size of the area along the x axis.
	 *
	 * @return         Size along X-axis
	 */
	public native float GetSizeX();

	/**
	 * Gets the size of the area along the y axis.
	 *
	 * @return         Size along Y-axis
	 */
	public native float GetSizeY();

	/**
	 * Returns true if 'area' overlaps our X extent.
	 *
	 * @return         True if overlaps, false otherwise
	 */
	public native bool IsOverlappingX(CNavArea area);

	/**
	 * Returns true if 'area' overlaps our Y extent.
	 *
	 * @return         True if overlaps, false otherwise
	 */
	public native bool IsOverlappingY(CNavArea area);

	/**
	 * Checks if the point overlaps the 2D extents of the area.
	 *
	 * @param pos          Position to check
	 * @param tolerance    Tolerance to use
	 * @return             True if overlaps, false otherwise
	 */
	public native bool IsOverlappingPoint(const float pos[3], float tolerance = 0.0);

	/**
	 * Checks if the given area overlaps the 2D extents of the area.
	 *
	 * @param area         Area to check
	 * @return             True if overlaps, false otherwise
	 */
	public native bool IsOverlappingArea(CNavArea area);

	/**
	 * Checks if the given extent overlaps the 2D extents of the area.
	 *
	 * @param mins       Minimum 2D extents
	 * @param maxs       Maximum 2D extents
	 * @return           True if overlaps, false otherwise
	 */
	public native bool IsOverlappingExtent(const float mins[3], const float maxs[3]);

	/**
	 * Return a computed extent (XY is in m_nwCorner and m_seCorner, Z is computed).
	 *
	 * @param mins       Minimum 2D extents
	 * @param maxs       Maximum 2D extents
	 */
	public native void GetExtent(float mins[3], float maxs[3]);

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

	/**
	 * Checks if the entire area is visible from the given eyepoint.
	 *
	 * @note                  This is CPU-intensive as this performs trace checks against the four corners and the center of the area.
	 * @param eye             Position to check
	 * @param ignoreEntity    Entity to ignore for trace checks
	 * @return                True if totally visible, false otherwise
	 */
	public native bool IsEntirelyVisible(const float eye[3], int ignoreEntity = -1);

	/**
	 * Checks if the area is partially visible from the given eyepoint.
	 *
	 * @note                  This is CPU-intensive as this performs trace checks against the four corners and the center of the area.
	 * @param eye             Position to check
	 * @param ignoreEntity    Entity to ignore for trace checks
	 * @return                True if partially visible, false otherwise
	 */
	public native bool IsPartiallyVisible(const float eye[3], int ignoreEntity = -1);

	/**
	 * Checks if the given area is potentially visible from somewhere in this area.
	 *
	 * @note          This is very fast as it relies on precomputed visibility data.
	 * @note          This only works if the navigation mesh is analyzed.
	 * @param area    Area to check
	 * @return        True if potentially visible, false otherwise
	 * @error         Invalid area
	 */
	public native bool IsPotentiallyVisible(CNavArea area);

	/**
	 * Checks if the given area is completely visible from somewhere in this area.
	 *
	 * @note          This is very fast as it relies on precomputed visibility data.
	 * @note          This only works if the navigation mesh is analyzed.
	 * @param area    Area to check
	 * @return        True if potentially visible, false otherwise
	 * @error         Invalid area
	 */
	public native bool IsCompletelyVisible(CNavArea area);
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
	property int Count
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

	#pragma deprecated Use Count instead
	property int Length
	{
		public native get();
	}
};

/**
 * Default shortest path cost function. This is the default cost function used by BuildPath().
 */
public float Path_Cost(CNavArea area, CNavArea fromArea, CNavLadder ladder, int elevator, float length)
{
	if ( fromArea == NULL_AREA )
	{
		// At the start of the path
		return 0.0;
	}

	float dist;
	if ( ladder != NULL_LADDER_AREA )
	{
		dist = ladder.length;
	}
	else if ( length > 0.0 )
	{
		dist = length;
	}
	else
	{
		float vecAreaCenter[3], vecFromAreaCenter[3];
		area.GetCenter(vecAreaCenter);
		fromArea.GetCenter(vecFromAreaCenter);
		dist = GetVectorDistance(vecAreaCenter, vecFromAreaCenter);
	}

	float cost = dist + fromArea.GetCostSoFar();
	
	// if this is a "crouch" area, add penalty
	int attributes = area.GetAttributes();
	if ( (attributes & NAV_MESH_CROUCH) )
	{
		const float crouchPenalty = 20.0;
		cost += crouchPenalty * dist;
	}

	// if this is a "jump" area, add penalty
	if ( (attributes & NAV_MESH_JUMP) )
	{
		const float jumpPenalty = 5.0;
		cost += jumpPenalty * dist;
	}
	
	return cost;
}