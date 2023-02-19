#if defined _CBASENPC_NEXTBOT_BEHAVIOUR_INC_
 #endinput
#endif
#define _CBASENPC_NEXTBOT_BEHAVIOUR_INC_

enum EventResultPriorityType
{
	RESULT_NONE = 0,
	RESULT_TRY,
	RESULT_IMPORTANT,
	RESULT_CRITICAL
}

/**
 * The type of a NextBotActionFactory event callback.
 */
enum EventResponderType
{
	EventResponderType_OnLeaveGround = 0,
	EventResponderType_OnLandOnGround,
	EventResponderType_OnContact,
	EventResponderType_OnMoveToSuccess,
	EventResponderType_OnMoveToFailure,
	EventResponderType_OnStuck,
	EventResponderType_OnUnStuck,
	EventResponderType_OnPostureChanged,
	EventResponderType_OnAnimationActivityComplete,
	EventResponderType_OnAnimationActivityInterrupted,
	EventResponderType_OnAnimationEvent,
	EventResponderType_OnIgnite,
	EventResponderType_OnInjured,
	EventResponderType_OnKilled,
	EventResponderType_OnOtherKilled,
	EventResponderType_OnSight,
	EventResponderType_OnLostSight,
	EventResponderType_OnSound,	// Not the same as EmitSound(); in fact, it doesn't seem to be used at all!
	EventResponderType_OnSpokeConcept,
	EventResponderType_OnWeaponFired,
	EventResponderType_OnNavAreaChanged,
	EventResponderType_OnModelChanged,
	EventResponderType_OnPickUp,
	EventResponderType_OnDrop,
	EventResponderType_OnActorEmoted,

	EventResponderType_OnCommandAttack,
	EventResponderType_OnCommandApproach,
	EventResponderType_OnCommandApproachEntity,
	EventResponderType_OnCommandRetreat,
	EventResponderType_OnCommandPause,
	EventResponderType_OnCommandResume,
	EventResponderType_OnCommandString,

	EventResponderType_OnShoved,
	EventResponderType_OnBlinded,
	EventResponderType_OnTerritoryContested,
	EventResponderType_OnTerritoryCaptured,
	EventResponderType_OnTerritoryLost,
	EventResponderType_OnWin,
	EventResponderType_OnLose,

	EventResponderType_Max
}

typeset ActionEventResponderCallback
{
	// OnLeaveGround
	// OnLandOnGround
	// OnSight
	// OnLostSight
	// OnDrop (item = ground)
	// OnCommandApproachEntity (goal = ground)
	function int (NextBotAction action, int actor, int ground);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity ground);
	function void (NextBotAction action, int actor, int ground);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity ground);
	
	// OnContact
	// result is a CGameTrace* structure
	function int (NextBotAction action, int actor, int other, Address result);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity other, Address result);
	function void (NextBotAction action, int actor, int other, Address result);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity other, Address result);

	// OnMoveToSuccess
	function int (NextBotAction action, int actor, Path path);
	function int (NextBotAction action, CBaseCombatCharacter actor, Path path);
	function void (NextBotAction action, int actor, Path path);
	function void (NextBotAction action, CBaseCombatCharacter actor, Path path);

	// OnMoveToFailure
	function int (NextBotAction action, int actor, Path path, MoveToFailureType reason);
	function int (NextBotAction action, CBaseCombatCharacter actor, Path path, MoveToFailureType reason);
	function void (NextBotAction action, int actor, Path path, MoveToFailureType reason);
	function void (NextBotAction action, CBaseCombatCharacter actor, Path path, MoveToFailureType reason);

	// OnStuck
	// OnUnStuck
	// OnPostureChanged
	// OnIgnite
	// OnModelChanged
	// OnCommandResume
	// OnWin
	// OnLose
	function int (NextBotAction action, int actor);
	function int (NextBotAction action, CBaseCombatCharacter actor);
	function void (NextBotAction action, int actor);
	function void (NextBotAction action, CBaseCombatCharacter actor);

	// OnAnimationActivityComplete
	// OnAnimationActivityInterrupted
	function int (NextBotAction action, int actor, Activity activity);
	function int (NextBotAction action, CBaseCombatCharacter actor, Activity activity);
	function void (NextBotAction action, int actor, Activity activity);
	function void (NextBotAction action, CBaseCombatCharacter actor, Activity activity);

	// OnCommandAttack
	// OnShoved (pusher = victim)
	// OnBlinded (blinder = victim)
	// OnTerritoryContested (territoryID = victim)
	// OnTerritoryCaptured (territoryID = victim)
	// OnTerritoryLost (territoryID = victim)
	function int (NextBotAction action, int actor, int victim);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity victim);
	function void (NextBotAction action, int actor, int victim);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity victim);

	// OnAnimationEvent
	function int (NextBotAction action, int actor, int event);
	function int (NextBotAction action, CBaseCombatCharacter actor, int event);

	function void (NextBotAction action, int actor, int event);
	function void (NextBotAction action, CBaseCombatCharacter actor, int event);

	function int (NextBotAction action, int actor, int event, const char[] options);
	function int (NextBotAction action, CBaseCombatCharacter actor, int event, const char[] options);

	function void (NextBotAction action, int actor, int event, const char[] options);
	function void (NextBotAction action, CBaseCombatCharacter actor, int event, const char[] options);

	function int (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime);
	function int (NextBotAction action, CBaseCombatCharacter actor, int event, const char[] options, float cycle, float eventtime);

	function void (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime);
	function void (NextBotAction action, CBaseCombatCharacter actor, int event, const char[] options, float cycle, float eventtime);

	function int (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype);
	function int (NextBotAction action, CBaseCombatCharacter actor, int event, const char[] options, float cycle, float eventtime, int eventtype);

	function void (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype);
	function void (NextBotAction action, CBaseCombatCharacter actor, int event, const char[] options, float cycle, float eventtime, int eventtype);

	function int (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype, int eventSourceEntity);
	function int (NextBotAction action, CBaseCombatCharacter actor, int event, const char[] options, float cycle, float eventtime, int eventtype, CBaseAnimating eventSourceEntity);

	function void (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype, int eventSourceEntity);
	function void (NextBotAction action, CBaseCombatCharacter actor, int event, const char[] options, float cycle, float eventtime, int eventtype, CBaseAnimating eventSourceEntity);

	// OnInjured
	// OnKilled
	function int (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype);

	function void (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype);

	function int (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3]);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype, CBaseEntity weapon, const float damageForce[3], const float damagePosition[3]);

	function void (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3]);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype, CBaseEntity weapon, const float damageForce[3], const float damagePosition[3]);

	function int (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype, CBaseEntity weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);

	function void (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype, CBaseEntity weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);

	// OnOtherKilled
	function int (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter victim, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype);

	function void (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter victim, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype);

	function int (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3]);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter victim, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype, CBaseEntity weapon, const float damageForce[3], const float damagePosition[3]);

	function void (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3]);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter victim, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype, CBaseEntity weapon, const float damageForce[3], const float damagePosition[3]);

	function int (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter victim, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype, CBaseEntity weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);

	function void (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter victim, CBaseEntity attacker, CBaseEntity inflictor, float damage, int damagetype, CBaseEntity weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);

	// OnSound
	function int (NextBotAction action, int actor, int sourceEntity, const float pos[3], KeyValues keys);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity sourceEntity, const float pos[3], KeyValues keys);

	function void (NextBotAction action, int actor, int sourceEntity, const float pos[3], KeyValues keys);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity sourceEntity, const float pos[3], KeyValues keys);

	// OnSpokeConcept
	// response is AI_Response*
	function int (NextBotAction action, int actor, int who, const char[] concept, Address response);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter who, const char[] concept, Address response);

	function void (NextBotAction action, int actor, int who, const char[] concept, Address response);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter who, const char[] concept, Address response);

	// OnWeaponFired
	function int (NextBotAction action, int actor, int whoFired, int weapon);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter whoFired, CBaseEntity weapon);

	function void (NextBotAction action, int actor, int whoFired, int weapon);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter whoFired, CBaseEntity weapon);

	// OnPickUp
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity item, CBaseCombatCharacter giver);

	// OnActorEmoted
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseCombatCharacter emoter, int emote);

	// OnNavAreaChanged
	function int (NextBotAction action, int actor, CNavArea newArea, CNavArea oldArea);
	function int (NextBotAction action, CBaseCombatCharacter actor, CNavArea newArea, CNavArea oldArea);

	function void (NextBotAction action, int actor, CNavArea newArea, CNavArea oldArea);
	function void (NextBotAction action, CBaseCombatCharacter actor, CNavArea newArea, CNavArea oldArea);

	// OnCommandApproach
	function int (NextBotAction action, int actor, const float pos[3], float range);
	function int (NextBotAction action, CBaseCombatCharacter actor, const float pos[3], float range);

	function void (NextBotAction action, int actor, const float pos[3], float range);
	function void (NextBotAction action, CBaseCombatCharacter actor, const float pos[3], float range);

	// OnCommandRetreat
	function int (NextBotAction action, int actor, int threat, float range);
	function int (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity threat, float range);

	function void (NextBotAction action, int actor, int threat, float range);
	function void (NextBotAction action, CBaseCombatCharacter actor, CBaseEntity threat, float range);

	// OnCommandPause
	function int (NextBotAction action, int actor, float duration);
	function int (NextBotAction action, CBaseCombatCharacter actor, float duration);

	function void (NextBotAction action, int actor, float duration);
	function void (NextBotAction action, CBaseCombatCharacter actor, float duration);

	// OnCommandString
	function int (NextBotAction action, int actor, const char[] command);
	function int (NextBotAction action, CBaseCombatCharacter actor, const char[] command);

	function void (NextBotAction action, int actor, const char[] command);
	function void (NextBotAction action, CBaseCombatCharacter actor, const char[] command);
}

enum QueryResultType
{
	ANSWER_NO,
	ANSWER_YES,
	ANSWER_UNDEFINED
}

enum ContextualQueryType
{
	ContextualQueryType_ShouldPickup = 0,
	ContextualQueryType_ShouldHurry,
	ContextualQueryType_ShouldRetreat,
	ContextualQueryType_ShouldAttack,
	ContextualQueryType_IsHindrance, /* see `PathFollower::GetHindrance()` on how to use this */
	ContextualQueryType_SelectTargetPoint,
	ContextualQueryType_IsPositionAllowed,
	ContextualQueryType_SelectMoreDangerousThreat,
	ContextualQueryType_Max
}

typeset NextBotActionCallback
{
	function void (NextBotAction action, int actor);
	function void (NextBotAction action, CBaseCombatCharacter actor);

	// OnStart
	// OnSuspend (interruptingAction = priorAction)
	// OnResume (interruptingAction = priorAction)
	function int (NextBotAction action, int actor, NextBotAction priorAction);
	function int (NextBotAction action, CBaseCombatCharacter actor, NextBotAction priorAction);

	function void (NextBotAction action, int actor, NextBotAction priorAction);
	function void (NextBotAction action, CBaseCombatCharacter actor, NextBotAction priorAction);

	// Update
	function int (NextBotAction action, int actor, float interval);
	function int (NextBotAction action, CBaseCombatCharacter actor, float interval);

	function void (NextBotAction action, int actor, float interval);
	function void (NextBotAction action, CBaseCombatCharacter actor, float interval);

	// OnEnd
	function void (NextBotAction action, int actor, NextBotAction nextAction);
	function void (NextBotAction action, CBaseCombatCharacter actor, NextBotAction nextAction);

	// InitialContainedAction
	function NextBotAction (NextBotAction action, int actor);
	function NextBotAction (NextBotAction action, CBaseCombatCharacter actor);

	// CreateInitialAction
	function void (NextBotAction action);

	// IsAbleToBlockMovementOf
	function bool (NextBotAction action, INextBot bot);
}

/**
 * A single unit of logic of the NextBot behavior system. An Action can do work
 * and change to/suspend for other Actions. Actions can also contain a single
 * child Action, which runs concurrently with the parent Action.
 *
 * Actions are instantiated using a NextBotActionFactory.
 *
 * See: "The AI Systems of Left 4 Dead" by Mike Booth, "Behaviors and decision making"
 * https://steamcdn-a.akamaihd.net/apps/valve/2009/ai_systems_of_l4d_mike_booth.pdf
 *
 */
methodmap NextBotAction < INextBotEventResponder
{
	/**
	 * The entity index of the CBaseNPC utilizing this Action.
	 *
	 * Note: This is not set until before the OnStart() callback is invoked.
	 * 
	 * @return     Entity index
	 */
	property int Actor
	{
		public native get();
	}

	/**
	 * The parent of this Action.
	 *
	 * @return     The parent Action
	 */
	property NextBotAction Parent
	{
		public native get();
	}

	/**
	 * The active child of this Action.
	 *
	 * @return     The active child Action
	 */
	property NextBotAction ActiveChild
	{
		public native get();
	}

	/**
	 * The Action buried by this Action.
	 *
	 * @return     Buried action
	 */
	property NextBotAction ActionBuriedUnderMe
	{
		public native get();
	}

	/**
	 * The Action that buried this Action
	 *
	 * @return     Action above this Action
	 */
	property NextBotAction ActionCoveringMe
	{
		public native get();
	}

	/**
	 * The identifying name of the Action.
	 *
	 * @param sBuffer        Buffer
	 * @param iBufferLen     Buffer size
	 */
	public native void GetName(char[] sBuffer, int iBufferLen);

	/**
	 * The full name of the Action. This displays the hierarchy of Actions
	 * from the root parent Action to this Action.
	 *
	 * @param sBuffer        Buffer
	 * @param iBufferLen     Buffer size
	 */
	public native void GetFullName(char[] sBuffer, int iBufferLen);

	/**
	 * Whether or not the given name matches this Action's name.
	 *
	 * @param sName          Name
	 * @return               True if name matches, false otherwise.
	 */
	public native bool IsNamed(const char[] sName);

	/**
	 * Deletes the action.
	 *
	 * Normally you don't have to use this as finished Actions are
	 * automatically disposed; only use if the Action never ends up being used
	 * in a (Try)ChangeTo/(Try)SuspendFor call.
	 */
	public native void Destroy();

	/**
	 * Checks if the datamap property exists on the action.
	 *
	 * @param sPropName     Property name
	 * @return              True if exists, false otherwise.
	 */
	public native bool HasData(const char[] sPropName);

	/**
	 * Retrieves an integer from the Action's custom data. Size of the given
	 * value is auto-detected.
	 * 
	 * This is synonymous with GetEntProp().
	 *
	 * @param sPropName     Property name
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Value at the given property offset.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native any GetData(const char[] sPropName, int element=0);

	/**
	 * Sets an integer in the Action's custom data. Size of the given value is
	 * auto-detected.
	 * 
	 * This is synonymous with SetEntProp().
	 *
	 * @param sPropName     Property name
	 * @param value         Value to set
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native void SetData(const char[] sPropName, any value, int element=0);

	/**
	 * Retrieves a float value from the Action's custom data.
	 * 
	 * This is synonymous with GetEntPropFloat().
	 *
	 * @param sPropName     Property name
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Value at the given property offset.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native float GetDataFloat(const char[] sPropName, int element=0);

	/**
	 * Sets a float value in the Action's custom data.
	 * 
	 * This is synonymous with SetEntPropFloat().
	 *
	 * @param sPropName     Property name
	 * @param value         Value to set
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native void SetDataFloat(const char[] sPropName, float value, int element=0);

	/**
	 * Retrieves a Vector value from the Action's custom data.
	 * 
	 * This is synonymous with GetEntPropVector().
	 *
	 * @param sPropName     Property name
	 * @param buffer        Vector buffer to store data in.
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native void GetDataVector(const char[] sPropName, float buffer[3], int element=0);

	/**
	 * Sets a Vector in the Action's custom data.
	 * 
	 * This is synonymous with SetEntPropVector().
	 *
	 * @param sPropName     Property name
	 * @param value         Value to set
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native void SetDataVector(const char[] sPropName, const float value[3], int element=0);

	/**
	 * Retrieves a string from the Action's custom data.
	 * 
	 * This is synonymous with GetEntPropString().
	 *
	 * @param sPropName     Property name
	 * @param sBuffer       Buffer to store value
	 * @param iBufferLen    Size of buffer
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native int GetDataString(const char[] sPropName, char[] sBuffer, int iBufferLen, int element=0);

	/**
	 * Sets a string in the Action's custom data.
	 * 
	 * This is synonymous with SetEntPropString().
	 *
	 * @param sPropName     Property name
	 * @param sValue        Value to set
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native int SetDataString(const char[] sPropName, const char[] sValue, int element=0);

	/**
	 * Retrieves a color from the Action's custom data.
	 * 
	 * @param sPropName     Property name
	 * @param r             Red component
	 * @param g             Green component
	 * @param b             Blue component
	 * @param a             Alpha component
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public void GetDataColor(const char[] sPropName, int &r = 0, int &g = 0, int &b = 0, int &a = 0, int element=0)
	{
		int value = this.GetData(sPropName, element);
		r = value & 0xff;
		g = (value >> 8) & 0xff;
		b = (value >> 16) & 0xff;
		a = (value >> 24) & 0xff;
	}

	/**
	 * Sets a color from the Action's custom data.
	 * 
	 * @param sPropName     Property name
	 * @param r             Red component
	 * @param g             Green component
	 * @param b             Blue component
	 * @param a             Alpha component
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public void SetDataColor(const char[] sPropName, int r = 255, int g = 255, int b = 255, int a = 255, int element=0)
	{
		r = r & 0xff;
		g = (g & 0xff) << 8;
		b = (b & 0xff) << 16;
		a = (a & 0xff) << 24;

		this.SetData(sPropName, r | g | b | a, element);
	}

	/**
	 * Retrieves an entity index from the Action's custom data.
	 * 
	 * This is synonymous with GetEntPropEnt().
	 *
	 * @param sPropName     Property name
	 * @param element       Element # (starting from 0) if property is an array.
	 * @return              Value at the given property offset.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native int GetDataEnt(const char[] sPropName, int element=0);

	/**
	 * Sets an entity index in the Action's custom data.
	 * 
	 * This is synonymous with SetEntPropEnt().
	 *
	 * @param sPropName     Property name
	 * @param value         Entity index to set
	 * @param element       Element # (starting from 0) if property is an array.
	 * @error               Invalid property or offset out of reasonable bounds.
	 */
	public native void SetDataEnt(const char[] sPropName, int value, int element=0);

	/**
	 * Whether or not the Action is suspended.
	 */
	property bool IsSuspended
	{
		public native get();
	}

	/**
	 * Action should keep running. This is the default result if no other
	 * result is specified.
	 * 
	 * Note: Do not use this in event callbacks. Use TryContinue() instead.
	 */
	public native int Continue();

	/**
	 * Action should change to another Action.
	 *
	 * Notes: 
	 * 1. This is not allowed within an OnSuspend callback.
	 * 2. Do not use this in event callbacks. Use TryChangeTo() instead.
	 *
	 * @param action        Action to change to
	 * @param reason        Reason for changing to the new action.
	 * @error Action is NULL_ACTION
	 */
	public native int ChangeTo(NextBotAction action, const char[] reason = NULL_STRING);

	/**
	 * Action should suspend itself for an interrupting Action.
	 *
	 * About suspending: Suspending an Action will "bury" the Action underneath
	 * the interrupting Action, forming a stack. The buried Action will no
	 * longer update, but can still react to events.
	 *
	 * If a buried Action returns TrySuspendFor() as a response to an event,
	 * the new interrupting Action will be placed on the very top of the stack.
	 *
	 * If a buried Action returns TryChangeTo() or TryDone() as a response 
	 * to an event, every Action above the buried Action will be considered
	 * "out of scope" and will exit on the next update.
	 *
	 * Notes:
	 * 1. This is not allowed within an OnSuspend() callback.
	 * 2. Do not use this in event callbacks. Use TrySuspendFor() instead.
	 * 3. This is not to be confused with child Actions. Child Actions continue
	 * to update along with its parent, while buried Actions do not update
	 * until all Actions on top of the buried Action exit.
	 *
	 * @param interruptingAction        Interrupting Action to suspend for
	 * @param reason                    Reason for suspending for the interrupting Action.
	 * @error Action is NULL_ACTION
	 */
	public native int SuspendFor(NextBotAction interruptingAction, const char[] reason = NULL_STRING);

	/**
	 * Action should exit.
	 * 
	 * Note: Do not use this in event callbacks. Use TryDone() instead.
	 *
	 * @param reason        Reason for exiting
	 */
	public native int Done(const char[] reason = NULL_STRING);

	/**
	 * Request that the Action should keep running. This is the default event
	 * result returned if no other event result is given.
	 * 
	 * Note: This is used within event callbacks only.
	 * 
	 * @param priority      How important this response is
	 */
	public native int TryContinue(EventResultPriorityType priority = RESULT_TRY);

	/**
	 * Request that the Action should change to another Action.
	 *
	 * Note: This is used within event callbacks only.
	 *
	 * @param action        The new Action to change to.
	 * @param priority      How important this response is
	 * @param reason        The reason for changing to the new Action.
	 */
	public native int TryChangeTo(NextBotAction action, EventResultPriorityType priority = RESULT_TRY, const char[] reason = NULL_STRING);

	/**
	 * Request that the Action should suspend itself for another Action.
	 *
	 * Note: This is used within event callbacks only.
	 *
	 * @param interruptingAction        The interrupting Action to suspend for.
	 * @param priority                  How important this response is
	 * @param reason                    The reason for suspending for the interrupting Action.
	 */
	public native int TrySuspendFor(NextBotAction interruptingAction, EventResultPriorityType priority = RESULT_TRY, const char[] reason = NULL_STRING);

	/**
	 * Request that the Action should exit.
	 *
	 * Note: This is used within event callbacks only.
	 *
	 * @param priority      How important this response is
	 * @param reason        The reason for exiting
	 */
	public native int TryDone(EventResultPriorityType priority = RESULT_TRY, const char[] reason = NULL_STRING);

	/**
	 * Request that the Action should keep running. This is the same as
	 * TryContinue() except with an added reason parameter.
	 *
	 * Note: This is used within event callbacks only.
	 *
	 * @param priority      How important this response is
	 * @param reason        The reason for continuing
	 */
	public native int TryToSustain(EventResultPriorityType priority = RESULT_TRY, const char[] reason = NULL_STRING);
}

typeset ActionContextualQueryCallback
{
	// ShouldPickup
	// IsHindrance
	function QueryResultType (NextBotAction action, INextBot bot, int item);
	function QueryResultType (NextBotAction action, INextBot bot, CBaseEntity item);

	// ShouldHurry
	// ShouldRetreat
	function QueryResultType (NextBotAction action, INextBot bot);

	// ShouldAttack
	function QueryResultType (NextBotAction action, INextBot bot, CKnownEntity knownEntity);

	// SelectTargetPoint
	// Leave pos unmodified for no response
	function void (NextBotAction action, INextBot bot, int subject, float pos[3]);
	function void (NextBotAction action, INextBot bot, CBaseCombatCharacter subject, float pos[3]);

	// IsPositionAllowed
	function QueryResultType (NextBotAction action, INextBot bot, const float pos[3]);

	// SelectMoreDangerousThreat
	// Return NULL_KNOWN_ENTITY for no response
	function CKnownEntity (NextBotAction action, INextBot bot, int subject, CKnownEntity threat1, CKnownEntity threat2); 
	function CKnownEntity (NextBotAction action, INextBot bot, CBaseCombatCharacter subject, CKnownEntity threat1, CKnownEntity threat2); 
}

enum NextBotActionCallbackType
{
	NextBotActionCallbackType_OnStart = 0,                  // Action started
	NextBotActionCallbackType_Update,                       // Action does work
	NextBotActionCallbackType_OnSuspend,                    // Action is suspended for another Action. Only Continue() and Done() are allowed.
	NextBotActionCallbackType_OnResume,                     // Action resumes after suspension
	NextBotActionCallbackType_OnEnd,                        // Action exits
	NextBotActionCallbackType_InitialContainedAction,       // Creation of a child Action
	NextBotActionCallbackType_CreateInitialAction,          // Action was created as the initial Action of the Behavior
	NextBotActionCallbackType_IsAbleToBlockMovementOf,      // Returning false will skip collision resolution with the other bot; default is true.
	NextBotActionCallbackType_Max
}

methodmap NextBotActionFactory < Handle
{
	/**
	 * A class that declares the data structure and callbacks of a particular
	 * Action and instantiates Actions.
	 *
	 * @param sName     Name of the Action, which can be retrieved using GetName().
	 * @return          Handle to NextBotActionFactory object.
	 */
	public native NextBotActionFactory(const char[] sName);

	/**
	 * Defines a callback for the Action.
	 * 
	 * Note: Be cautious when using KillTimer() in the OnEnd() callback on timers flagged
	 * with TIMER_FLAG_NO_MAPCHANGE. When a map changes, timers with this flag are removed 
	 * first before it is called, so consider letting the timers end on their own instead 
	 * (i.e. returning Plugin_Stop).
	 *
	 * @param callbackType     Type of callback
	 * @param callback
	 * @error                  Invalid handle.
	 */
	public native void SetCallback(NextBotActionCallbackType callbackType, NextBotActionCallback callback);

	/**
	 * Defines a contextual query callback for the Action.
	 *
	 * @param callbackType     Type of callback
	 * @param callback
	 * @error                  Invalid handle.
	 */
	public native void SetQueryCallback(ContextualQueryType callbackType, ActionContextualQueryCallback callback);

	/**
	 * Defines an event callback for the Action.
	 *
	 * @param callbackType     Type of callback
	 * @param callback
	 * @error                  Invalid handle.
	 */
	public native void SetEventCallback(EventResponderType callbackType, ActionEventResponderCallback callback);

	/**
	 * Begins defining properties stored on the Action.
	 *
	 * @return     The NextBotActionFactory to chain calls
	 * @error      Invalid handle.
	 */
	public native NextBotActionFactory BeginDataMapDesc();

	/**
	 * Defines an integer property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 * @error                Invalid handle.
	 */
	public native NextBotActionFactory DefineIntField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a float property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 * @error                Invalid handle.
	 */
	public native NextBotActionFactory DefineFloatField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a character property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 * @error                Invalid handle.
	 */
	public native NextBotActionFactory DefineCharField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a boolean property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 * @error                Invalid handle.
	 */
	public native NextBotActionFactory DefineBoolField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a Vector property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 * @error                Invalid handle.
	 */
	public native NextBotActionFactory DefineVectorField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a string property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 * @error                Invalid handle.
	 */
	public native NextBotActionFactory DefineStringField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a color property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 * @error                Invalid handle.
	 */
	public native NextBotActionFactory DefineColorField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines an entity property on the Action.
	 * 
	 * @param sFieldName      Property name
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @return                The NextBotActionFactory to chain calls
	 * @error                 Invalid handle.
	 */
	public native NextBotActionFactory DefineEntityField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Finishes the datamap definition.
	 * 
	 * @error                Invalid handle.
	 */
	public native void EndDataMapDesc();

	/**
	 * Creates an Action.
	 *
	 * @return     The new Action
	 * @error                Invalid handle.
	 */
	public native NextBotAction Create();
}