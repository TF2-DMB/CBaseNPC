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