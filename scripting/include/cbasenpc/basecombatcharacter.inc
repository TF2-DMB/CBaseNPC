#if defined _CBASENPC_BASECOMBATCHARACTER_INC
 #endinput
#endif
#define _CBASENPC_BASECOMBATCHARACTER_INC

methodmap CBaseCombatCharacter < CBaseAnimatingOverlay
{
	public CBaseCombatCharacter(int entity)
	{
		return view_as<CBaseCombatCharacter>(entity);
	}

	/**
	 * Updates the last known nav area, with the one the entity is standing on.
	 * If possible.
	 *
	 * @error 		Invalid entity.
	 */
	public native void UpdateLastKnownArea();

	/**
	 * Gets the last known nav area, the given entity stood on/near.
	 *
	 * @return		The last known nav area, can be NULL_AREA.
	 * @error 		Invalid entity.
	 */
	public native CNavArea GetLastKnownArea();
};