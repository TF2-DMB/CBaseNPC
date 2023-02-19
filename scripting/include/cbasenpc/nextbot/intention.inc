#if defined _CBASENPC_NEXTBOT_INTENTION_INC_
 #endinput
#endif
#define _CBASENPC_NEXTBOT_INTENTION_INC_

/**
 * Component that manages the behavior logic of the NextBot.
 */
methodmap IIntention < INextBotComponent
{
	/**
	 * Determines if the bot should pick up the given item.
	 *
	 * @param item     Entity index
	 * @return         Query result
	 */
	public native QueryResultType ShouldPickUp(int item);

	/**
	 * Determines if the bot should hurry.
	 *
	 * @return     Query result
	 */
	public native QueryResultType ShouldHurry();

	/**
	 * Determines if the bot should retreat.
	 *
	 * @return     Query result
	 */
	public native QueryResultType ShouldRetreat();

	/**
	 * Determines if the bot should attack the given entity.
	 *
	 * @param them     Entity index
	 * @return         Query result
	 */
	public native QueryResultType ShouldAttack(int them);

	/**
	 * Determines if the given entity can block navigation.
	 *
	 * @param blocker     Entity index
	 * @return            Query result
	 */
	public native QueryResultType IsHindrance(int blocker);

	/**
	 * Given a subject, returns a position to target.
	 *
	 * @param subject     Entity index to target
	 * @param buffer      Position vector
	 */
	public native void SelectTargetPoint(int subject, float buffer[3]);

	/**
	 * Determines if the given position is allowed or not.
	 *
	 * @param  pos        Position vector
	 * @return            Query result
	 */
	public native QueryResultType IsPositionAllowed(const float pos[3]);

	/**
	 * Given two entities, selects the most dangerous one. This is heavily 
	 * utilized by IVision::GetPrimaryKnownThreat().
	 *
	 * @param subject     Entity index, used to determine how dangerous
	 *                    threat1 and threat2 are to the subject. This is usually
	 *                    set to the entity index of the NextBot.
	 * @param threat1     First threat
	 * @param threat2     Second threat
	 * @return            Most threatening entity
	 */
	public native CKnownEntity SelectMoreDangerousThreat(int subject, CKnownEntity threat1, CKnownEntity threat2);
};