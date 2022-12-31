#if defined _CBASENPC_NEXTBOT_PATH_INC_
 #endinput
#endif
#define _CBASENPC_NEXTBOT_PATH_INC_

enum SegmentType
{
	ON_GROUND,
	DROP_DOWN,
	CLIMB_UP,
	JUMP_OVER_GAP,
	LADDER_UP,
	LADDER_DOWN,
	
	NUM_SEGMENT_TYPES
};

enum MoveCursorType
{
	MOVECUR_ABS = 0,
	MOVECUR_REL = 1,
	MOVECUR_MAX,
};

enum MoveToFailureType
{
	FAIL_NO_PATH_EXISTS = 0,
	FAIL_STUCK        = 1,
	FAIL_FELL_OFF     = 2,
};

typeset PathFollowerCostFunctor
{
	/**
	 * Path cost function for PathFollower
	 * 
	 * @param bot          The bot to calculate the cost for
	 * @param area         Area arriving to
	 * @param fromArea     Area leaving from. This will be NULL_AREA if `area` is the first area in the path.
	 * @param ladder       If ladder used to traverse to `area`, CNavLadder pointer, otherwise is NULL_LADDER.
	 * @param elevator     If elevator used to traverse to `area`, entity index of elevator, otherwise is -1.
	 * @param length       Length of connection between areas. If ladder or elevator is used, this is -1.0.
	 * @return             Cost to move from `fromArea` to `area`
	 */
	function float(INextBot bot, CNavArea area, CNavArea fromArea, CNavLadder ladder, int elevator, float length);
	function float(INextBot bot, CNavArea area, CNavArea fromArea, CNavLadder ladder, CBaseEntity elevator, float length);
};

/**
 * A segment of a Path.
 */
methodmap Segment
{
	/**
	 * The CNavArea that this segment traverses.
	 */
	property CNavArea area
	{
		public native get();
	}

	/**
	 * The direction that this segment traverses.
	 */
	property NavTraverseType how
	{
		public native get();
	}

	/**
	 * The end position of the segment. 
	 *
	 * Note: This is not to be confused as the start position; use either the
	 * previous segment's GetPos() method or manually calculate it using this
	 * segment's negative forward vector and length to get the start position.
	 *
	 * @param pos   Position vector
	 */
	public native void GetPos(float pos[3]);

	/**
	 * If how is GO_LADDER_UP or GO_LADDER_DOWN, then this is the CNavLadder
	 * that bots should use to travel along the segment, otherwise will be NULL.
	 */
	property CNavLadder ladder
	{
		public native get();
	}

	/**
	 * The type of movement required to travel this segment.
	 */
	property SegmentType type
	{
		public native get();
	}

	/**
	 * The direction vector from start to end that bots are expected to travel
	 * for this segment.
	 *
	 * @param vec   Forward vector
	 */
	public native void GetForward(float vec[3]);

	/**
	 * The total length of this segment from start to end.
	 */
	property float length
	{
		public native get();
	}

	/**
	 * The total distance of this segment from the start of the path to the
	 * end position of this segment.
	 */
	property float distanceFromStart
	{
		public native get();
	}

	/**
	 * How much the path 'curves' from start to end in the XY plane. 0 = none,
	 * 1 = 180 degree doubleback.
	 */
	property float curvature
	{
		public native get();
	}

	/**
	 * The center position of the portal that connects the previous segment's
	 * area and this segment's area.
	 *
	 * @param vec   Position vector
	 */
	public native void GetPortalCenter(float vec[3]);

	/**
	 * The width of the portal, in half.
	 */
	property float m_portalHalfWidth
	{
		public native get();
	}
};

/**
 * A structure that stores data about the current progress on the Path.
 */
methodmap CursorData
{
	/**
	 * The current position along the path.
	 *
	 * @param vec   Position vector
	 */
	public native void GetPos(float vec[3]);

	/**
	 * The current forward direction along the path.
	 *
	 * @param vec   Forward vector
	 */
	public native void GetForward(float vec[3]);
	
	/**
	 * How much the path 'curves' from start to end in the XY plane. 0 = none,
	 * 1 = 180 degree doubleback.
	 */
	property float curvature
	{
		public native get();
	}

	/**
	 * The previous segment passed.
	 */
	property Segment segmentPrior
	{
		public native get();
	}
};

enum SeekType
{
	SEEK_ENTIRE_PATH,			// search the entire path length
	SEEK_AHEAD,					// search from current cursor position forward toward end of path
	SEEK_BEHIND					// search from current cursor position backward toward path start
};

typeset PathComputeCallback
{
	function void (Path path, bool success, any data);
};

methodmap Path
{
	/**
	 * A Path that is used by NextBots to navigate the world.
	 *
	 * @param costFunction           Callback that calculates travel cost
	 * @param ignoreActorsFilter     Callback that determines which actors to ignore
	 * @param onlyActorsFilter       Callback that determines which actors to hit
	 */
	public native Path(PathFollowerCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);

	/**
	 * The total length of the Path.
	 *
	 * @return     Length
	 */
	public native float GetLength();

	/**
	 * Returns position along path a certain distance from the start.
	 *
	 * @param dist    Distance along path
	 * @param pos     Position vector
	 * @param seg     Segment to start from.
	 */
	public native void GetPosition(float dist, float pos[3], Segment seg = NULL_PATH_SEGMENT);

	/**
	 * Returns position along path that is closest to the given position.
	 *
	 * @param pos            Position to get closest to
	 * @param vec            Buffer to store closest position
	 * @param start          Segment to start at
	 * @param alongLimit     Maximum distance along path to check
	 */
	public native void GetClosestPosition(const float pos[3], float vec[3], Segment start = NULL_PATH_SEGMENT, float alongLimit = 0.0);

	/**
	 * Start position of the path.
	 *
	 * @param pos      Position vector
	 */
	public native void GetStartPosition(float pos[3]);

	/**
	 * End position of the path.
	 *
	 * @param pos      Position vector
	 */
	public native void GetEndPosition(float pos[3]);

	/**
	 * Entity index of the NextBot currently using this path.
	 *
	 * @param pos      Position vector
	 */
	public native int GetSubject();

	/**
	 * The segment the path has set for the goal.
	 *
	 * @return     Segment pointer
	 */
	public native Segment GetCurrentGoal();

	/**
	 * How much time since the path was last updated.
	 *
	 * @return     Game time
	 */
	public native float GetAge();

	/**
	 * Sets current progress of path to the start.
	 */
	public native void MoveCursorToStart();

	/**
	 * Sets current progress of path to the end.
	 */
	public native void MoveCursorToEnd();

	/**
	 * Sets current progress of path to the specified distance.
	 *
	 * @param dist       Distance to move
	 * @param mctype     Frame of reference to move from
	 */
	public native void MoveCursor(float dist, MoveCursorType mctype);

	/**
	 * Sets current progress of path to a position that is closest to the given position.
	 *
	 * @param pos            Position to move closest to
	 * @param type           Seek type
	 * @param alongLimit     Maximum distance to move along path
	 */
	public native void MoveCursorToClosestPosition(const float pos[3], SeekType type = SEEK_ENTIRE_PATH, float alongLimit = 0.0);

	/**
	 * Current progress on the path.
	 *
	 * @return     Progress
	 */
	public native float GetCursorPosition();

	/**
	 * Data of the current progress on the path.
	 *
	 * @return     CursorData pointer
	 */
	public native CursorData GetCursorData();

	/**
	 * Whether the current path has built a path or not.
	 *
	 * @return     True if valid, false otherwise
	 */
	public native bool IsValid();

	/**
	 * Clears the path.
	 */
	public native void Invalidate();

	/**
	 * Draws the path. By default this does not work.
	 *
	 * @param seg     Segment to start from
	 */
	public native void Draw(Segment seg = NULL_PATH_SEGMENT);
	public native void DrawInterpolated(float from, float to);

	/**
	 * The first segment of the path.
	 *
	 * @return     Segment pointer
	 */
	public native Segment FirstSegment();

	/**
	 * The next segment of the path, starting from the given segment.
	 *
	 * @return     Segment pointer
	 */
	public native Segment NextSegment(Segment seg);

	/**
	 * The previous segment of the path, starting from the given segment.
	 *
	 * @return     Segment pointer
	 */
	public native Segment PriorSegment(Segment seg);

	/**
	 * The last segment of the path.
	 *
	 * @return     Segment pointer
	 */
	public native Segment LastSegment();

	/**
	 * Computes a path to the given position.
	 *
	 * @param bot                        INextBot that will use this path
	 * @param goalPos                    Goal position
	 * @param maxPathLength              Maximum travel distance
	 * @param includeGoalIfPathFails     If true, then the path will include the goal regardless
	 *                                   of failure.
	 * @return                           True if path reached to goal. If false, path may or may
	 *                                   not be valid. Use IsValid() to check.
	 */
	public native bool ComputeToPos(INextBot bot, const float goalPos[3], float maxPathLength = 0.0, bool includeGoalIfPathFails = true);

	/**
	 * Computes a path to the given entity.
	 *
	 * @param bot                        INextBot that will use this path
	 * @param entity                     Goal entity
	 * @param maxPathLength              Maximum travel distance
	 * @param includeGoalIfPathFails     If true, then the path will include the goal regardless
	 *                                   of failure.
	 * @return                           True if path reached to goal. If false, path may or may
	 *                                   not be valid. Use IsValid() to check.
	 */
	public native bool ComputeToTarget(INextBot bot, int entity, float maxPathLength = 0.0, bool includeGoalIfPathFails = true);

	/**
	 * Invalids and destroys the Path from memory.
	 */
	public native void Destroy();
};

methodmap PathFollower < Path
{
	public native PathFollower(PathFollowerCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);

	/**
	 * Moves the bot along the path.
	 *
	 * @param nextbot     INextBot that will use this path
	 */
	public native void Update(INextBot nextbot);

	/**
	 * Sets the look ahead distance of the path. This value is used to 
	 * check if the bot can skip redundant path nodes within the given
	 * distance for smoother movement.
	 *
	 * @param dist     Look ahead distance
	 */
	public native void SetMinLookAheadDistance(float dist);

	/**
	 * The entity that is blocking navigation along this path.
	 *
	 * @note       This is only set if the bot's IIntention or one of its contained responders
	 *             returns ANSWER_YES for IsHindrance. If a responder returns ANSWER_YES for
	 *             IsHindrance(-1) AND IsHindrance(entity), then the hindrance entity will be
	 *             set to "entity".
	 * @return     Entity index
	 */
	public native int GetHindrance();

	/**
	 * Return whether there is a the given discontinuity ahead in the path
	 * within the given range or not.
	 *
	 * @param nextbot     INextBot using the path
	 * @param stype       Type of discontinuity
	 * @param range       Maximum distance ahead to search
	 * @return            True if there's a discontinuity, false otherwise
	 */
	public native bool IsDiscontinuityAhead(INextBot nextbot, SegmentType stype, float range = -1.0);
};

enum SubjectChaseType
{
	LEAD_SUBJECT,
	DONT_LEAD_SUBJECT
};

methodmap ChasePath < PathFollower
{
	/**
	 * A Path designed for chasing entities.
	 *
	 * @param how                    Chase behavior
	 * @param costFunction           Callback that calculates travel cost
	 * @param ignoreActorsFilter     Callback that determines which actors to ignore
	 * @param onlyActorsFilter       Callback that determines which actors to hit
	 */
	public native ChasePath(SubjectChaseType how = DONT_LEAD_SUBJECT, PathFollowerCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);
	
	/**
	 * Updates path progress to the goal entity, automatically recomputing
	 * when needed.
	 *
	 * @param bot                 INextBot using the path
	 * @param iTarget             Goal entity
	 * @param vecPredictedPos     If not NULL_VECTOR and path has LEAD_SUBJECT behavior,
	 *                            then will be filled with the predicted position of the
	 *                            goal entity.
	 */
	public native void Update(INextBot bot, int iTarget, float vecPredictedPos[3] = NULL_VECTOR);

	/**
	 * The distance to start predicting lead position for a subject.
	 * 
	 * @return     Lead distance
	 */
	public native float GetLeadRadius();

	/**
	 * Maximum path length.
	 *
	 * @return     Length
	 */
	public native float GetMaxPathLength();

	/**
	 * Tries to predict the subject's lead position.
	 *
	 * @param nextbot     INextBot to use
	 * @param entity      Entity index of the subject
	 * @param pos         Position vector
	 */
	public native void PredictSubjectPosition(INextBot nextbot, int entity, float pos[3]);

	/**
	 * Determines if the path to the goal entity needs to be recomputed.
	 *
	 * @param nextbot     INextBot using the path
	 * @param entity      Goal entity
	 * @return            True if repath is needed, false otherwise
	 */
	public native bool IsRepathNeeded(INextBot nextbot, int entity);

	/**
	 * The duration that this path has been valid.
	 *
	 * @return     Duration
	 */
	public native float GetLifetime();
};

methodmap DirectChasePath < ChasePath
{
	public native DirectChasePath(SubjectChaseType how = DONT_LEAD_SUBJECT, PathFollowerCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);
};

public bool Path_FilterIgnoreActors(int entity, int contentsMask, int desiredcollisiongroup)
{
	if ((entity > 0 && entity <= MaxClients) || !CBaseEntity(entity).MyNextBotPointer())
		return false;
	
	return true;
}

public bool Path_FilterOnlyActors(int entity, int contentsMask, int desiredcollisiongroup)
{
	return ((entity > 0 && entity <= MaxClients) || !!CBaseEntity(entity).MyNextBotPointer());
}