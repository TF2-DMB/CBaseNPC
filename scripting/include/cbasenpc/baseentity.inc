#if defined _CBASENPC_BASEENTITY_INC_
 #endinput
#endif
#define _CBASENPC_BASEENTITY_INC_

#include "takedamageinfo.inc"

methodmap CBaseEntity
{
	public CBaseEntity(int iEnt) 
	{
		return view_as<CBaseEntity>(iEnt);
	}

	property int index
	{
		public get()
		{
			return view_as<int>(this);
		}
	}

	property int iEnt
	{
		#pragma deprecated Use CBaseEntity.index instead
		public get() 
		{
			return view_as<int>(this);
		}
	}

	/**
	 * Explicitly deletes an entity on the network in order to refresh it.
	 * You can compute a state for each player. If that state changes, the entity will be deleted.
	 *
	 * @param player The player index to update the state for.
	 * @param state  State for the player.
	 */
	public native void RefreshNetwork(int player, bool state);

	/**
	 * This is the vtable offset of CBaseEntity::UpdateOnRemove.
	 *
	 * @return       VTable offset.
	 */
	public static native int iUpdateOnRemove();
	
	/**
	 * Obtains the entity's CServerNetworkProperty.
	 * 
	 * Note: Non networked entity also have a CServerNetworkProperty.
	 *
	 * @return		Memory address of CServerNetworkProperty.
	 * @error 		Invalid entity.
	 */
	public native Address NetworkProp();

	/**
	 * Obtains the entity's CCollisionProperty.
	 *
	 * @return		Memory address of CCollisionProperty.
	 * @error 		Invalid entity.
	 */
	public native Address CollisionProp();

	/**
	 * Informs the server that the network properties of this entity changed.
	 * 
	 * Note: You should avoid using the function recklessly.
	 *
	 * @error 		Invalid entity.
	 */
	public native void NetworkStateChanged();

	/**
	 * Informs the server that this specific networked var changed value.
	 *
	 * @param varAddr	Entity networked property address.
	 * @error 		Invalid entity.
	 */
	public native void NetworkStateChangedVar(Address varAddr);

	/**
	 * Gets the entity simulation time.
	 *
	 * @return		Simulation time.
	 * @error 		Invalid entity.
	 */
	public native float GetSimulationTime();

	/**
	 * Sets the entity simulation time.
	 *
	 * @param time		Simulation time.
	 * @error 		Invalid entity.
	 */
	public native float SetSimulationTime(float time);

	/**
	 * Updates the global transmit state if a transmission rule changed.
	 *
	 * @error 		Invalid entity.
	 */
	public native void DispatchUpdateTransmitState();

	/**
	 * Gets an entity's flags.
	 *
	 * Note: This is equivalent to GetEntityFlags
	 *
	 * @return		The entity flags.
	 * @error		Invalid entity.
	 */
	public int GetFlags()
	{
		return GetEntityFlags(this.index);
	}

	/**
	 * Adds flags to the entity.
	 *
	 * @error		Invalid entity.
	 */
	public void AddFlag(int flags)
	{
		SetEntityFlags(this.index, this.GetFlags() | flags);
	}

	/**
	 * Removes flags from the entity.
	 *
	 * @error		Invalid entity.
	 */
	public void RemoveFlag(int flagsToRemove)
	{
		SetEntityFlags(this.index, this.GetFlags() & ~flagsToRemove);
	}

	/**
	 * Clears the entity's flags.
	 *
	 * @error		Invalid entity.
	 */
	public void ClearFlags()
	{
		SetEntityFlags(this.index, 0);
	}

	/**
	 * Toggles flags on the entity.
	 *
	 * @error		Invalid entity.
	 */
	public void ToggleFlag(int flagToToggle)
	{
		SetEntityFlags(this.index, this.GetFlags() ^ flagToToggle);
	}

	/**
	 * Gets an edict's flags.
	 *
	 * @return		The edict flags.
	 * @error		Invalid entity.
	 */
	public native int GetEFlags();

	/**
	 * Is the given edict flag set.
	 *
	 * @return		true if it's set, false otherwise.
	 * @error		Invalid entity.
	 */
	public native bool IsEFlagSet(int flag);

	/**
	 * Sets the edict flags.
	 *
	 * @param flags		The edict flags to set.
	 * @error		Invalid entity.
	 */
	public native void SetEFlags(int flags);

	/**
	 * Adds the given edict flags.
	 *
	 * @param flags		The edict flags to add.
	 * @error		Invalid entity.
	 */
	public native void AddEFlags(int flags);

	/**
	 * Removes the given edict flags.
	 *
	 * @param flags		The edict flags to remove.
	 * @error		Invalid entity.
	 */
	public native void RemoveEFlags(int flags);

	/**
	 * Updates an entity model keyvalue.
	 * 
	 * Note: This is **NOT** Equivalent to SetModel/SetEntityModel.
	 * You will know whether you need to use this function. Most will not!
	 *
	 * @param model		Model name to set.
	 * @error 		Invalid entity.
	 */
	public native void SetModelName(const char[] model);

	/**
	 * Obtains the model name stored model keyvalue.
	 *
	 * @param model		Destination char array to store the model name in.
	 * @param len		Size of the destination char array.
	 * @error 		Invalid entity.
	 */
	public native void GetModelName(char[]  model, int len);

	/**
	 * Registers a new think context.
	 *
	 * @param context	Name of the think context.
	 * @return		Index value of the think context.
	 * @error 		Invalid entity.
	 */
	public native int RegisterThinkContext(const char[] context = NULL_STRING);

	/**
	 * Sets when the given think context should fire.
	 *
	 * @param time		Game time that reprents when the think function should be called.
	 * @param context	Name of the think context.
	 * @error 		Invalid entity.
	 */
	public native void SetNextThink(float time, const char[] context = NULL_STRING);

	/**
	 * When is the entity going to think next.
	 *
	 * @param context	Name of the think context.
	 * @return		Game time of the next think.
	 * @error 		Invalid entity.
	 */
	public native float GetNextThink(const char[] context = NULL_STRING);

	/**
	 * When has the entity last thought.
	 *
	 * @param context	Name of the think context.
	 * @return		Game time of the last think.
	 * @error 		Invalid entity.
	 */
	public native float GetLastThink(const char[] context = NULL_STRING);

	/**
	 * When is the entity going to think next.
	 *
	 * @param context	Name of the think context.
	 * @return		Game tick of the next think.
	 * @error 		Invalid entity.
	 */
	public native int GetNextThinkTick(const char[] context = NULL_STRING);

	/**
	 * When has the entity last thought.
	 *
	 * @param context	Name of the think context.
	 * @return		Game tick of the last think.
	 * @error 		Invalid entity.
	 */
	public native int GetLastThinkTick(const char[] context = NULL_STRING);

	/**
	 * Does the entity think?
	 *
	 * @return		Whether or not the entity will think.
	 * @error 		Invalid entity.
	 */
	public native bool WillThink();

	/**
	 * Updates the entity flags, based on whether or not the entity thinks
	 *
	 * @param isThinking	Hint parameter to help the function search less.
	 * @error 		Invalid entity.
	 */
	public native bool CheckHasThinkFunction(bool isThinking);

	/**
	 * Gets the entity absolute position.
	 *
	 * @param pos		Destination vector to store the position in.
	 * @error 		Invalid entity.
	 */
	public native void GetAbsOrigin(float pos[3]);

	/**
	 * Gets the entity absolute angles.
	 *
	 * @param ang		Destination vector to store the angles in.
	 * @error 		Invalid entity.
	 */
	public native void GetAbsAngles(float ang[3]);

	/**
	 * Gets the entity absolute velocity.
	 *
	 * @param vel		Destination vector to store the velocity in.
	 * @error 		Invalid entity.
	 */
	public native void GetAbsVelocity(float vel[3]);

	/**
	 * Sets the entity absolute position.
	 *
	 * @param pos		Position vector.
	 * @error 		Invalid entity.
	 */
	public native void SetAbsOrigin(const float pos[3]);

	/**
	 * Sets the entity absolute angles.
	 *
	 * @param ang		Angles vector.
	 * @error 		Invalid entity.
	 */
	public native void SetAbsAngles(const float ang[3]);

	/**
	 * Sets the entity absolute velocity.
	 *
	 * @param ang		Velocity vector.
	 * @error 		Invalid entity.
	 */
	public native void SetAbsVelocity(const float vel[3]);

	/**
	 * Gets the entity local origin.
	 *
	 * @param ang		Destination vector to store the position in.
	 * @error 		Invalid entity.
	 */
	public native void GetLocalOrigin(float pos[3]);

	/**
	 * Gets the entity local angles.
	 *
	 * @param ang		Destination vector to store the angles in.
	 * @error 		Invalid entity.
	 */
	public native void GetLocalAngles(float ang[3]);

	/**
	 * Sets the entity local position.
	 *
	 * @param pos		Position vector.
	 * @error 		Invalid entity.
	 */
	public native void SetLocalOrigin(const float pos[3]);

	/**
	 * Sets the entity local angles.
	 *
	 * @param ang		Angles vector.
	 * @error 		Invalid entity.
	 */
	public native void SetLocalAngles(const float ang[3]);

#if defined _dhooks_included
	/**
	 * Hooks CBaseEntity::UpdateOnRemove
	 *
	 * @param callback 	DHookcallback that will be used.
	 * @error		Invalid entity.
	 */
	public void Hook_UpdateOnRemove(DHookCallback callback)
	{
		static DynamicHook hHook = null;
		if (hHook == null)
		{
			hHook = new DynamicHook(CBaseEntity.iUpdateOnRemove(), HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity);
			if (hHook == null) return;
		}
		hHook.HookEntity(Hook_Pre, this.index, callback);
	}
#endif
	/**
	 * Spawns an entity into the game.
	 * 
	 * Note: Equivalent to DispatchSpawn.
	 *
	 * @error 		Invalid entity.
	 */
	public native void Spawn();

	/**
	 * Sets the model to a given entity.
	 * 
	 * Note: Equivalent to SetEntityModel.
	 *
	 * @param model		Model name.
	 * @error 		Invalid entity.
	 */
	public native void SetModel(const char[] model);

	/**
	 * Teleports an entity.
	 * 
	 * Note: Equivalent to TeleportEntity.
	 *
	 * @param origin	New origin, or NULL_VECTOR for no change.
	 * @param angles	New angles, or NULL_VECTOR for no change.
	 * @param velocity	New velocity, or NULL_VECTOR for no change.
	 * @error 		Invalid entity.
	 */
	public native void Teleport(const float origin[3] = NULL_VECTOR, const float angles[3] = NULL_VECTOR, const float velocity[3] = NULL_VECTOR);

	/**
	 * Finds the entity's space center.
	 *
	 * @param center	Destination vector to store the entity's space center.
	 * @error 		Invalid entity.
	 */
	public native void WorldSpaceCenter(float center[3]);

	/**
	 * Caculates an entity's vectors.
	 *
	 * @param front		Destination vector to store the entity's forward vector.
	 * @param right		Destination vector to store the entity's right vector.
	 * @param up		Destination vector to store the entity's up vector.
	 * @error 		Invalid entity.
	 */
	public native void GetVectors(float front[3], float right[3], float up[3]);

	/**
	 * Obtains the entity-to-world transform.
	 *
	 * @return		Destination matrix to store the return value in.
	 * @error 		Invalid entity.
	 */
	public native void EntityToWorldTransform(float matrix[3][4]);

	/**
	 * Obtains pointer to the entity's INextBot.
	 *
	 * @return     INextBot, or NULL_NEXT_BOT if not a NextBot entity.
	 * @error      Invalid entity.
	 */
	public native INextBot MyNextBotPointer();

	/**
	 * Attempts to cast this entity as CBaseAnimating.
	 *
	 * @return     CBaseAnimating; use IsValid() to check for validity. If the
	 *             entity is a CBaseAnimating, will be valid; false otherwise.
	 * @error      Invalid entity.
	 */
	public native CBaseAnimating GetBaseAnimating();

	/**
	 * Attempts to cast this entity as a CBaseCombatCharacter.
	 *
	 * @return     CBaseCombatCharacter; use IsValid() to check for validity.
	 *             If the entity is a CBaseCombatCharacter, will be valid;
	 *             false otherwise.
	 * @error      Invalid entity.
	 */
	public native CBaseCombatCharacter MyCombatCharacterPointer();

	/**
	 * Checks if entity is a CBaseCombatCharacter.
	 *
	 * @return     True if CBaseCombatCharacter; false otherwise.
	 * @error      Invalid entity.
	 */
	public native bool IsCombatCharacter();

	/**
	 * Alias of ActivateEntity().
	 *
	 * @error               Invalid entity.
	 */
	public void Activate()
	{
		ActivateEntity(this.index);
	}

	/**
	 * Alias of IsValidEntity().
	 *
	 * @return              True if valid, false otherwise.
	 */
	public bool IsValid()
	{
		return IsValidEntity(this.index);
	}

	/**
	 * Alias of IsEntNetworkable().
	 *
	 * @return              True if networkable, false if invalid or not networkable
	 */
	public bool IsNetworkable()
	{
		return IsEntNetworkable(this.index);
	}

	/**
	 * Alias of GetEntityClassname().
	 *
	 * @param clsname       Buffer to store the classname.
	 * @param maxlength     Maximum length of the buffer.
	 * @return              True on success, false if there is no classname set.
	 */
	public bool GetClassname(char[] clsname, int maxlength)
	{
		return GetEntityClassname(this.index, clsname, maxlength);
	}

	/**
	* Alias of HasEntProp().
	*
	* @param type          Property type.
	* @param prop          Property name.
	* @return              Whether the property exists on the entity.
	* @error               Invalid entity.
	*/
	public bool HasProp(PropType type, const char[] prop)
	{
		return HasEntProp(this.index, type, prop);
	}

	/**
	 * Alias of GetEntProp().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @param size          Number of bytes to write (valid values are 1, 2, or 4).
	 *                      This value is auto-detected, and the size parameter is
	 *                      only used as a fallback in case detection fails.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Value at the given property offset.
	 * @error               Invalid entity or property not found.
	 */
	public int GetProp(PropType type, const char[] prop, int size=4, int element=0)
	{
		return GetEntProp(this.index, type, prop, size, element);
	}

	/**
	 * Alias of SetEntProp().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @param value         Value to set.
	 * @param size          Number of bytes to write (valid values are 1, 2, or 4).
	 *                      This value is auto-detected, and the size parameter is
	 *                      only used as a fallback in case detection fails.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid entity or offset out of reasonable bounds.
	*/
	public void SetProp(PropType type, const char[] prop, any value, int size=4, int element=0)
	{
		SetEntProp(this.index, type, prop, value, size, element);
	}

	/**
	 * Alias of GetEntPropFloat().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Value at the given property offset.
	 * @error               Invalid entity or offset out of reasonable bounds.
	 */
	public float GetPropFloat(PropType type, const char[] prop, int element=0)
	{
		return GetEntPropFloat(this.index, type, prop, element);
	}

	/**
	 * Alias of SetEntPropFloat().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @param value         Value to set.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid entity or offset out of reasonable bounds.
	*/
	public void SetPropFloat(PropType type, const char[] prop, float value, int element=0)
	{
		SetEntPropFloat(this.index, type, prop, value, element);
	}

	/**
	 * Alias of GetEntPropEnt().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Value at the given property offset.
	 * @error               Invalid entity or offset out of reasonable bounds.
	 */
	public int GetPropEnt(PropType type, const char[] prop, int element=0)
	{
		return GetEntPropEnt(this.index, type, prop, element);
	}

	/**
	 * Alias of SetEntPropEnt().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @param other         Entity index to set, or -1 to unset.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid entity or offset out of reasonable bounds.
	*/
	public void SetPropEnt(PropType type, const char[] prop, int other, int element=0)
	{
		SetEntPropEnt(this.index, type, prop, other, element);
	}

	/**
	 * Alias of GetEntPropVector().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @param vec           Vector buffer to store data in.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid entity, property not found, or property not
	 *                      actually a vector data type.
	 */
	public void GetPropVector(PropType type, const char[] prop, float vec[3], int element=0)
	{
		GetEntPropVector(this.index, type, prop, vec, element);
	}

	/**
	 * Alias of SetEntPropVector().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @param vec           Vector to set.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid entity, property not found, or property not
	 *                      actually a vector data type.
	 */
	public void SetPropVector(PropType type, const char[] prop, const float vec[3], int element=0)
	{
		SetEntPropVector(this.index, type, prop, vec, element);
	}

	/**
	 * Alias of GetEntPropString().
	 *
	 * @param type          Property type.
	 * @param prop          Property to use.
	 * @param buffer        Destination string buffer.
	 * @param maxlen        Maximum length of output string buffer.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Number of non-null bytes written.
	 * @error               Invalid entity, offset out of reasonable bounds, or property is not a valid string.
	 */
	public int GetPropString(PropType type, const char[] prop, char[] buffer, int maxlen, int element=0)
	{
		return GetEntPropString(this.index, type, prop, buffer, maxlen, element);
	}

	/**
	 * Alias of SetEntPropString().
	 *
	 * @param type          Property type.
	 * @param prop          Property to use.
	 * @param buffer        String to set.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Number of non-null bytes written.
	 * @error               Invalid entity, offset out of reasonable bounds, or property is not a valid string.
	 */
	public int SetPropString(PropType type, const char[] prop, const char[] buffer, int element=0)
	{
		return SetEntPropString(this.index, type, prop, buffer, element);
	}

	/**
	 * Gets a network property as a color.
	 *
	 * @param type          Property type.
	 * @param prop          Property to use.
	 * @param r             Red component
	 * @param g             Green component
	 * @param b             Blue component
	 * @param a             Alpha component
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid entity, offset out of reasonable bounds, or property is not valid.
	 */
	public void GetPropColor(PropType type, const char[] prop, int &r = 0, int &g = 0, int &b = 0, int &a = 0, int element=0)
	{
		int value = this.GetProp(type, prop, element);
		r = value & 0xff;
		g = (value >> 8) & 0xff;
		b = (value >> 16) & 0xff;
		a = (value >> 24) & 0xff;
	}

	/**
	 * Sets a network property as a color.
	 *
	 * @param type          Property type.
	 * @param prop          Property to use.
	 * @param r             Red component
	 * @param g             Green component
	 * @param b             Blue component
	 * @param a             Alpha component
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Number of non-null bytes written.
	 * @error               Invalid entity, offset out of reasonable bounds, or property is not valid.
	 */
	public void SetPropColor(PropType type, const char[] prop, int r = 255, int g = 255, int b = 255, int a = 255, int element=0)
	{
		r = r & 0xff;
		g = (g & 0xff) << 8;
		b = (b & 0xff) << 16;
		a = (a & 0xff) << 24;

		this.SetProp(type, prop, r | g | b | a, element);
	}

	/**
	 * Alias of GetEntPropArraySize().
	 *
	 * @param type          Property type.
	 * @param prop          Property name.
	 * @return              Size of array (in elements) or 1 if property is not an array.
	 * @error               Invalid entity or property not found.
	 */
	public int GetPropArraySize(PropType type, const char[] prop)
	{
		return GetEntPropArraySize(this.index, type, prop);
	}

	/**
	 * Alias of DispatchKeyValue().
	 *
	 * @param keyName       Name of the key.
	 * @param value         String value.
	 * @return              True on success, false otherwise.
	 * @error               Invalid entity, or no mod support.
	 */
	public bool KeyValue(const char[] keyName, const char[] value)
	{
		return DispatchKeyValue(this.index, keyName, value);
	}

	/**
	 * Alias of DispatchKeyValueInt().
	 *
	 * @param keyName       Name of the key.
	 * @param value         Integer value.
	 * @return              True on success, false otherwise.
	 * @error               Invalid entity, or no mod support.
	 */
	public bool KeyValueInt(const char[] keyName, int value)
	{
		return DispatchKeyValueInt(this.index, keyName, value);
	}

	/**
	 * Alias of DispatchKeyValueFloat().
	 *
	 * @param keyName       Name of the key.
	 * @param value         Float value.
	 * @return              True on success, false otherwise.
	 * @error               Invalid entity, or no mod support.
	 */
	public bool KeyValueFloat(const char[] keyName, float value)
	{
		return DispatchKeyValueFloat(this.index, keyName, value);
	}

	/**
	 * Alias of DispatchKeyValueVector().
	 *
	 * @param keyName       Name of the key.
	 * @param value         Vector value.
	 * @return              True on success, false otherwise.
	 * @error               Invalid entity, or no mod support.
	 */
	public bool KeyValueVector(const char[] keyName, const float value[3])
	{
		return DispatchKeyValueVector(this.index, keyName, value);
	}

	/**
	 * Alias of AcceptEntityInput().
	 *
	 * @param input         Input action.
	 * @param activator     Entity index which initiated the sequence of actions (-1 for a NULL entity).
	 * @param caller        Entity index from which this event is sent (-1 for a NULL entity).
	 * @param outputid      Unknown.
	 * @return              True if successful otherwise false.
	 * @error               Invalid entity or no mod support.
	 */
	public bool AcceptInput(const char[] input, int activator = -1, int caller = -1, int outputid = 0)
	{
		return AcceptEntityInput(this.index, input, activator, caller, outputid);
	}

	/**
	 * Alias of FireEntityOutput().
	 *
	 * @param output        Output name.
	 * @param activator     Entity index which initiated the sequence of actions (-1 for a NULL entity).
	 * @param delay         Delay before firing the output.
	 * @error               Invalid entity or no mod support.
	 */
	public void FireOutput(const char[] output, int activator = -1, float delay = 0.0)
	{
		FireEntityOutput(this.index, output, activator, delay);
	}

	/**
	 * Gets an entity's gravity.
	 *
	 * @return              Entity's m_flGravity value.
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public float GetGravity()
	{
		return GetEntityGravity(this.index);
	}

	/**
	 * Sets an entity's gravity.
	 *
	 * @return              Entity's m_flGravity value.
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public void SetGravity(float gravity)
	{
		SetEntityGravity(this.index, gravity);
	}

	/**
	 * Gets an entity's color.
	 *
	 * @param r             Amount of red (0-255)
	 * @param g             Amount of green (0-255)
	 * @param b             Amount of blue (0-255)
	 * @param a             Amount of alpha (0-255)
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public void GetRenderColor(int &r, int &g, int &b, int &a)
	{
		GetEntityRenderColor(this.index, r, g, b, a);
	}

	/**
	 * Sets an entity's color.
	 *
	 * @param r             Amount of red (0-255)
	 * @param g             Amount of green (0-255)
	 * @param b             Amount of blue (0-255)
	 * @param a             Amount of alpha (0-255)
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public void SetRenderColor(int r = 255, int g = 255, int b = 255, int a = 255)
	{
		SetEntityRenderColor(this.index, r, g, b, a);
	}

	/**
	 * Gets an entity's render Fx.
	 *
	 * @return              RenderFx value.
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public RenderFx GetRenderFx()
	{
		return GetEntityRenderFx(this.index);
	}

	/**
	 * Sets an entity's render Fx.
	 *
	 * @param fx            RenderFx value.
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public void SetRenderFx(RenderFx fx)
	{
		SetEntityRenderFx(this.index, fx);
	}

	/**
 	 * Gets an entity's render mode.
	 *
	 * @return              RenderMode value.
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public RenderMode GetRenderMode()
	{
		return GetEntityRenderMode(this.index);
	}

	/**
	 * Sets an entity's render mode.
	 *
	 * @param mode          RenderMode value.
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public void SetRenderMode(RenderMode mode)
	{
		SetEntityRenderMode(this.index, mode);
	}

	/**
	 * Gets an entity's movetype.
	 *
	 * @return              Movetype
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public MoveType GetMoveType()
	{
		return GetEntityMoveType(this.index);
	}

	/**
	 * Sets an entity's movetype.
	 *
	 * @param mt            Movetype
	 * @error               Invalid entity, or lack of mod compliance.
	 */
	public void SetMoveType(MoveType val)
	{
		SetEntityMoveType(this.index, val);
	}

	/**
	 * Causes the entity to take damage with the given parameters.
	 * Filters are checked and damage scaling is applied before it calls the
	 * entity's OnTakeDamage().
	 *
	 * @param inputInfo    The damage parameters to use
	 * @error              Invalid entity or a NULL pointer is given
	 */
	public native void TakeDamage(CTakeDamageInfo inputInfo);
}