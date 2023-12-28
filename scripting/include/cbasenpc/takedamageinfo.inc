#if defined _CBASENPC_TAKEDAMAGEINFO_INC_
 #endinput
#endif
#define _CBASENPC_TAKEDAMAGEINFO_INC_

enum TakeDamageInfo_CritType
{
	TakeDamageInfo_CritType_None,
	TakeDamageInfo_CritType_Mini,
	TakeDamageInfo_CritType_Full
};

methodmap CTakeDamageInfo
{
	/**
	 * Retrieves the CTakeDamageInfo class from the given address ptr.
	 *
	 * @return		The CTakeDamageInfo.
	 */
	public native CTakeDamageInfo(Address ptr);

	/**
	 * Initializes the CTakeDamageInfo's values.
	 * 
	 * @param inflictor            The inflictor entity index.
	 * @param attacker             The attacker entity index.
	 * @param weapon               The weapon entity index.
	 * @param damageForce          The damage force vector.
	 * @param damagePosition       The damage position vector.
	 * @param damage               The amount of damage to set.
	 * @param bitsDamageType       The damage type bitmask.
	 * @param customDamage         The custom damage to set. 
	 * @param reportedPosition     The position to report to players on where the damage came from.
	 * @error                      Invalid takedamageinfo, or invalid inflictor, attacker, and weapon entity index.
	 */
	public native void Init(int inflictor = -1, int attacker = -1, int weapon = -1, const float damageForce[3] = NULL_VECTOR, const float damagePosition[3] = NULL_VECTOR, float damage = 0.0, int bitsDamageType = 0, int customDamage = 0, const float reportedPosition[3] = NULL_VECTOR);

	/**
	 * Gets the inflictor.
	 *
	 * @return		The inflictor entity index.
	 * @error		Invalid takedamageinfo.
	 */
	public native int GetInflictor();

	/**
	 * Sets the inflictor.
	 *
	 * @param inflictor		The inflictor entity index.
	 * @error				Invalid takedamageinfo.
	 */
	public native void SetInflictor(int inflictor);

	/**
	 * Gets the weapon.
	 *
	 * @return		The weapon entity index.
	 * @error		Invalid takedamageinfo.
	 */
	public native int GetWeapon();

	/**
	 * Sets the weapon.
	 *
	 * @param weapon		The weapon entity index.
	 * @error				Invalid takedamageinfo.
	 */
	public native void SetWeapon(int weapon);

	/**
	 * Gets the attacker.
	 *
	 * @return		The acttacker entity index.
	 * @error		Invalid takedamageinfo.
	 */
	public native int GetAttacker();

	/**
	 * Sets the attacker.
	 *
	 * @param attacker		The attacker entity index.
	 * @error				Invalid takedamageinfo.
	 */
	public native void SetAttacker(int attacker);

	/**
	 * Gets the damage.
	 *
	 * @return		The damage amount.
	 * @error		Invalid takedamageinfo.
	 */
	public native float GetDamage();

	/**
	 * Sets the damage.
	 *
	 * @param damage		The damage amount to set.
	 * @error				Invalid takedamageinfo.
	 */
	public native void SetDamage(float damage);

	/**
	 * Gets the max damage.
	 *
	 * @return		The max damage amount.
	 * @error		Invalid takedamageinfo.
	 */
	public native float GetMaxDamage();

	/**
	 * Sets the max damage.
	 *
	 * @param maxdamage		The max damage amount to set.
	 * @error				Invalid takedamageinfo.
	 */
	public native void SetMaxDamage(float maxdamage);

	/**
	 * Scales the damage.
	 *
	 * @param scale		The scale damage amount.
	 * @error			Invalid takedamageinfo.
	 */
	public native void ScaleDamage(float scale);

	/**
	 * Adds damage to the already registered damage.
	 *
	 * @param damage		The damage amount to add.
	 * @error				Invalid takedamageinfo.
	 */
	public native void AddDamage(float damage);

	/**
	 * Subtracts damage to the already registered damage.
	 *
	 * @param damage		The damage amount to remove.
	 * @error				Invalid takedamageinfo.
	 */
	public native void SubtractDamage(float damage);

	/**
	 * Gets the bonus damage.
	 *
	 * @return		Returns the bonus damage.
	 * @error		Invalid takedamageinfo.
	 */
	public native float GetDamageBonus();

	/**
	 * Sets the bonus damage.
	 *
	 * @param damage	The bonus damage to set.
	 * @error			Invalid takedamageinfo.
	 */
	public native void SetDamageBonus(float damage);

	/**
	 * Gets the bonus damage provider.
	 *
	 * @return		The bonus damage provider entity index.
	 * @error		Invalid takedamageinfo.
	 */
	public native int GetDamageBonusProvider();

	/**
	 * Gets the base damage if set, otherwise returns damage.
	 *
	 * @return		The base damage or damage.
	 * @error		Invalid takedamageinfo.
	 */
	public native float GetBaseDamage();

	/**
	 * Was base damage set?
	 *
	 * @return		Returns true if base damage was set, false othwerwise.
	 * @error		Invalid takedamageinfo.
	 */
	public native bool BaseDamageIsValid();

	/**
	 * Gets the damage force.
	 *
	 * @param vector The float array to store the damage force into.
	 * @error		 Invalid takedamageinfo.
	 */
	public native void GetDamageForce(float vector[3]);

	/**
	 * Sets the damage force.
	 *
	 * @param vector The float array to set the damage force.
	 * @error		 Invalid takedamageinfo.
	 */
	public native void SetDamageForce(const float vector[3]);

	/**
	 * Scales the damage force by the amount provided.
	 *
	 * @param scale The scale amount.
	 * @error		Invalid takedamageinfo.
	 */
	public native void ScaleDamageForce(float scale);

	/**
	 * Gets the damage for force.
	 *
	 * @return		Returns the force damage.
	 * @error		Invalid takedamageinfo.
	 */
	public native float GetDamageForForceCalc();

	/**
	 * Sets the damage for force.
	 *
	 * @param damage	The force damage to set.
	 * @error			Invalid takedamageinfo.
	 */
	public native void SetDamageForForceCalc(float damage);

	/**
	 * Gets the damage position.
	 *
	 * @param vector The float array to store the damage position into.
	 * @error		 Invalid takedamageinfo.
	 */
	public native void GetDamagePosition(float vector[3]);

	/**
	 * Sets the damage position.
	 *
	 * @param vector The float array to set the damage position.
	 * @error		 Invalid takedamageinfo.
	 */
	public native void SetDamagePosition(const float vector[3]);

	/**
	 * Gets the damage reported position.
	 *
	 * @param vector The float array to store the damage position into.
	 * @error		 Invalid takedamageinfo.
	 */
	public native void GetReportedPosition(float vector[3]);

	/**
	 * Sets the damage reported position.
	 *
	 * @param vector The float array to set the damage reported position.
	 * @error		 Invalid takedamageinfo.
	 */
	public native void SetReportedPosition(const float vector[3]);

	/**
	 * Gets the damage type bitmask.
	 *
	 * @return 	The damage type bitmask.
	 * @error	Invalid takedamageinfo.
	 */
	public native int GetDamageType();

	/**
	 * Sets the damage type bitmask.
	 *
	 * @param bitmask 	The damage type bitmask.
	 * @error			Invalid takedamageinfo.
	 */
	public native int SetDamageType(int bitmask);

	/**
	 * Adds the damage bitmask to the current damage bitmask.
	 *
	 * @param bitmask 	The damage type bitmask to add.
	 * @error			Invalid takedamageinfo.
	 */
	public native int AddDamageType(int bitmask);

	/**
	 * Gets the custom damage.
	 *
	 * @return		Returns the custom damage.
	 * @error		Invalid takedamageinfo.
	 */
	public native float GetDamageCustom();

	/**
	 * Sets the custom damage.
	 *
	 * @param damage	The custom damage to set.
	 * @error			Invalid takedamageinfo.
	 */
	public native void SetDamageCustom(float damage);

	/**
	 * Gets the damage stats.
	 *
	 * @return		The damage stats.
	 * @error		Invalid takedamageinfo.
	 */
	public native int GetDamageStats();

	/**
	 * Sets the damage stats.
	 *
	 * @param stats	The damage stats.
	 * @error		Invalid takedamageinfo.
	 */
	public native void SetDamageStats(int stats);

	/**
	 * Sets whether the damage is force friendly fire or not.
	 *
	 * @param state	The force friendly fire state.
	 * @error		Invalid takedamageinfo.
	 */
	public native void SetForceFriendlyFire(bool state);

	/**
	 * Gets whether the damage is force friendly fire or not.
	 *
	 * @return		The force friendly fire state.
	 * @error		Invalid takedamageinfo.
	 */
	public native bool IsForceFriendlyFire();

	/**
	 * Gets the ammo type.
	 *
	 * @return		The ammo type.
	 * @error		Invalid takedamageinfo.
	 */
	public native int GetAmmoType();

	/**
	 * Sets the ammo type.
	 *
	 * @param type	The ammo type.
	 * @error		Invalid takedamageinfo.
	 */
	public native void SetAmmoType(int type);

	/**
	 * Gets the damage penetration count.
	 *
	 * @return		The penetration count.
	 * @error		Invalid takedamageinfo.
	 */
	public native int GetPlayerPenetrationCount();

	/**
	 * Sets the damage penetration count.
	 *
	 * @param count	The penetration count.
	 * @error		Invalid takedamageinfo.
	 */
	public native void SetPlayerPenetrationCount(int count);

	/**
	 * Gets the damaged other player amount.
	 *
	 * @return			The damaged other players amount.
	 * @error			Invalid takedamageinfo.
	 */
	public native int GetDamagedOtherPlayers();

	/**
	 * Sets damaged other player amount.
	 *
	 * @param value		The damage amount to set.
	 * @error			Invalid takedamageinfo.
	 */
	public native void SetDamagedOtherPlayers(int value);

	/**
	 * Sets the damage crit type.
	 *
	 * @param type		The crit type to set.
	 * @error			Invalid takedamageinfo.
	 */
	public native void SetCritType(TakeDamageInfo_CritType type);

	/**
	 * Gets the damage crit type.
	 *
	 * @return		The crit type.
	 * @error		Invalid takedamageinfo.
	 */
	public native TakeDamageInfo_CritType GetCritType();
};

/**
 * Returns the address of a global CTakeDamageInfo object. Make sure to
 * initialize the values of the object first using CTakeDamageInfo.Init()
 * before modifying and using it in a function call.
 * 
 * @return     Address to the global CTakeDamageInfo object.
 */
native CTakeDamageInfo GetGlobalDamageInfo();