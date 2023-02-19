#if defined _CBASENPC_NEXTBOT_VISION_INC_
 #endinput
#endif
#define _CBASENPC_NEXTBOT_VISION_INC_

enum FieldOfViewCheckType
{
	USE_FOV,
	DISREGARD_FOV
};

/**
 * Component that acts as an interface to the NextBot's sense of vision and
 * keeps track of known entities.
 *
 * About known entities: This component does NOT automatically add entities
 * to its memory. This must be done manually using AddKnownEntity(). All
 * known entities must be actors (players or NextBots).
 *
 * Although the component will automatically prune obsolete entities in
 * its memory (see CKnownEntity.IsObsolete()), it's still good practice to
 * call ForgetEntity() when an entity stops existing or should no longer be in
 * a knowable state by the bot.
 *
 * The component will automatically keep track of the visibility status
 * of each known entity.
 */
methodmap IVision < INextBotComponent
{
	/**
	 * Returns the most threatening known entity.
	 *
	 * @param bVisible     Whether or not to take visibility into account.
	 * @return             Known entity
	 */
	public native CKnownEntity GetPrimaryKnownThreat(bool bVisible);

	/**
	 * Returns the amount of time since the bot saw any member of the given
	 * team.
	 *
	 * @param iTeam     Team to search for
	 * @return          Game time
	 */
	public native float GetTimeSinceVisible(int iTeam = TEAM_ANY);

	/**
	 * Returns the closest known entity of the given team.
	 *
	 * @param iTeam     Team to search for
	 * @return          Known entity
	 */
	public native CKnownEntity GetClosestKnown(int iTeam = TEAM_ANY);

	/**
	 * Returns the amount of known entities that belong to the given team.
	 *
	 * @param team             Team to search for
	 * @param onlyVisible      Whether or not to only include visible entities
	 * @param rangeLimit       Maximum range
	 * @return                 Amount of known entities
	 */
	public native int GetKnownCount(int team = TEAM_ANY, bool onlyVisible = false, float rangeLimit = -1.0);

	/**
	 * Returns the known entity entry of the given entity.
	 *
	 * @param iEntity     Entity index
	 * @return            Known entity, or NULL_KNOWN_ENTITY if not known.
	 */
	public native CKnownEntity GetKnown(int iEntity);

	/**
	 * Introduce a known entity to the bot. The entity is assumed to be known
	 * so position will be updated. If entity is already known then this won't
	 * do anything.
	 *
	 * @param iEntity     Entity index
	 */
	public native void AddKnownEntity(int iEntity);

	/**
	 * Forgets the entity.
	 *
	 * @param iEntity     Entity index
	 */
	public native void ForgetEntity(int iEntity);

	/**
	 * Forgets all entities.
	 */
	public native void ForgetAllKnownEntities();

	/**
	 * Returns the maximum range the bot can see.
	 *
	 * @return     Range
	 */
	public native float GetMaxVisionRange();

	/**
	 * Returns the vision reaction time of the bot. When an entity first
	 * becomes visible, this is how long to wait before invoking the
	 * OnSight() event of the bot.
	 *
	 * @return     Reaction time
	 */
	public native float GetMinRecognizeTime();

	/**
	 * Returns whether or not the bot can see the given entity.
	 *
	 * @param iEntity         Entity index
	 * @param checkFOV        Whether or not to take FOV into account.
	 * @param vecPos          Optional vector that will be filled with in the visible position of the entity
	 * @return                True if able to see, false otherwise
	 */
	public native bool IsAbleToSeeTarget(int iEntity, FieldOfViewCheckType checkFOV, float vecPos[3] = NULL_VECTOR);

	/**
	 * Returns whether or not the bot can see the given position.
	 *
	 * @param vecPos       Position to check
	 * @param checkFOV     Whether or not to take FOV into account.
	 * @return             True if able to see, false otherwise
	 */
	public native bool IsAbleToSee(const float vecPos[3], FieldOfViewCheckType checkFOV);

	/**
	 * Returns if the entity should be ignored.
	 *
	 * @param iEntity     Entity index
	 * @return            True to ignore, false otherwise
	 */
	public native bool IsIgnored(int iEntity);

	/**
	 * Returns true if the bot noticed the entity, regardless of LOS.
	 *
	 * NOTE: By default this returns true.
	 *
	 * @param iEntity     Entity index
	 * @return            Always true
	 */
	public native bool IsVisibleEntityNoticed(int iEntity);

	/**
	 * Returns if the position is within the bot's field of view.
	 *
	 * @param vecPos     Position vector
	 * @return           True if within FOV, false otherwise
	 */
	public native bool IsInFieldOfView(const float vecPos[3]);

	/**
	 * Returns if the entity is within the bot's field of view.
	 *
	 * @param iEntity     Entity index.
	 * @return            True if within FOV, false otherwise
	 */
	public native bool IsInFieldOfViewTarget(int iEntity);

	/**
	 * Returns the default field of view of the bot, in degrees. The default
	 * value is 90 degrees.
	 *
	 * @return     Field of view, in degrees
	 */
	public native float GetDefaultFieldOfView();

	/**
	 * Returns the field of view of the bot, in degrees.
	 *
	 * @return     Field of view, in degrees
	 */
	public native float GetFieldOfView();

	/**
	 * Sets the field of view of the bot, in degrees.
	 *
	 * @param horizAngle     Amount of degrees
	 */
	public native void SetFieldOfView(float horizAngle);

	/**
	 * Returns if there's nothing blocking the vision between the bot and the
	 * position. This performs a trace from the bot's eye position to the
	 * given position.
	 *
	 * @param vecPos      Position to test
	 * @return            True if nothing is blocking, false otherwise.
	 */
	public native bool IsLineOfSightClear(const float vecPos[3]);

	/**
	 * Returns if there's nothing blocking the vision between the bot and the
	 * entity. This performs a trace from the bot's eye position to the
	 * entity's WorldSpaceCenter().
	 *
	 * @param iActor          Entity index
	 * @param vecPos          Optional vector to fill in position of the entity
	 * @return                True if nothing is blocing, false otherwise
	 */
	public native bool IsLineOfSightClearToEntity(int iActor, float vecPos[3] = NULL_VECTOR);

	/**
	 * Returns whether the bot is looking at the given position or not.
	 *
	 * @param vecPos          Position to test
	 * @param 0.95            Cosine of theta threshold (1.0 = exact, 0.0 = within 90 degrees)
	 * @return                True if looking at position, false otherwise
	 */
	public native bool IsLookingAt(const float vecPos[3], float cosTolerance = 0.95);

	/**
	 * Returns whether the bot is looking at the given entity or not.
	 *
	 * @param iActor          Entity index
	 * @param 0.95            Cosine of theta threshold (1.0 = exact, 0.0 = within 90 degrees)
	 * @return                True if looking at entity, false otherwise
	 */
	public native bool IsLookingAtTarget(int iActor, float cosTolerance = 0.95);
};