#if defined _CBASENPC_NEXTBOT_KNOWNENTITY_INC_
 #endinput
#endif
#define _CBASENPC_NEXTBOT_KNOWNENTITY_INC_

/**
 * An entity that a NextBot has recognized.
 *
 * Note: Do not attempt to save pointers of this class as location of the data
 * can (and will!) change.
 */
methodmap CKnownEntity
{
	/**
	 * Invalidates this entry. This will mark the entry for deletion.
	 */
	public native void Destroy();

	/**
	 * Updates the known position of the entity with the entity's current
	 * position.
	 */
	public native void UpdatePosition();

	/**
	 * Returns the entity index that this entry is keeping track of.
	 *
	 * @return     Entity index
	 */
	public native int GetEntity();

	/**
	 * Returns the last known position.
	 *
	 * @param vecPos      Position vector
	 */
	public native void GetLastKnownPosition(float vecPos[3]);

	/**
	 * Whether or not the known position was updated due to the bot seeing the
	 * entity.
	 *
	 * @return
	 */
	public native bool HasLastKnownPositionBeenSeen();

	/**
	 * Determines that the last known position was updated because the bot saw
	 * the entity.
	 */
	public native void MarkLastKnownPositionAsSeen();

	/**
	 * Returns the last known CNavArea of the entity. If the entity is not a
	 * CBaseCombatCharacter, this will always return NULL.
	 *
	 * @return     CNavArea
	 */
	public native CNavArea GetLastKnownArea();

	/**
	 * Returns the amount of time since the entity's location was updated.
	 *
	 * @return     Game time
	 */
	public native float GetTimeSinceLastKnown();

	/**
	 * Returns the amount of time since the bot was first made aware of this
	 * entity's existence.
	 *
	 * @return     Game time
	 */
	public native float GetTimeSinceBecameKnown();

	/**
	 * Toggles whether or not the entity is visible.
	 *
	 * Note: Visibility should take the bot's FOV into account.
	 *
	 * @param visible     Visibility status
	 */
	public native void UpdateVisibilityStatus(bool visible);

	/**
	 * Returns if the entity is visible or not.
	 *
	 * @return     Visibility status
	 */
	public native bool IsVisibleInFOVNow();

	/**
	 * Returns if the entity was seen recently. "Recently" means that the
	 * entity is either visible, or if not visible, the entity was last seen
	 * within the last 3 seconds.
	 *
	 * @return     Visibility status
	 */
	public native bool IsVisibleRecently();

	/**
	 * Returns the amount of time since the bot gained visibility of the entity.
	 *
	 * @return     Game time
	 */
	public native float GetTimeSinceBecameVisible();

	/**
	 * Returns the timestamp since the bot gained visibility of the entity.
	 *
	 * @return     Game time
	 */
	public native float GetTimeWhenBecameVisible();

	/**
	 * Returns the amount of time since the bot last had visibility of the
	 * entity.
	 *
	 * @return     Game time
	 */
	public native float GetTimeSinceLastSeen();

	/**
	 * Whether or not the bot has ever seen this entity.
	 *
	 * @return     True if was visible at any point, false otherwise.
	 */
	public native bool WasEverVisible();

	/**
	 * Whether or not this entry has become obsolete. Entities that are NULL,
	 * dead, or have not been updated for 10 seconds are considered obsolete.
	 *
	 * @return     True if obsolete, false otherwise
	 */
	public native bool IsObsolete();

	/**
	 * Whether or not this entry is keeping track of the given entity.
	 *
	 * @param iEntity     Entity index
	 * @return            True if the given entity is tracked by this entry,
	 *                    false otherwise.
	 */
	public native bool Is(int iEntity);
};