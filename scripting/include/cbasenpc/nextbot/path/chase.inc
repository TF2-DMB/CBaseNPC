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