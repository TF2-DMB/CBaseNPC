#if defined _CBASENPC_NEXTBOT_INC_
 #endinput
#endif
#define _CBASENPC_NEXTBOT_INC_

enum NextBotDebugType
{
	DEBUG_NONE = 0x0000,
	DEBUG_ANY  = 0xffff,

	DEBUG_BEHAVIOR   = (1 << 0), // NextBotBehavior
	DEBUG_LOOK_AT    = (1 << 1), // NextBotBodyInterface
	DEBUG_PATH       = (1 << 2), // NextBotPath, NextBotPathFollow, NextBotChasePath
	DEBUG_ANIMATION  = (1 << 3),
	DEBUG_LOCOMOTION = (1 << 4), // NextBotLocomotionInterface
	DEBUG_VISION     = (1 << 5), // NextBotVisionInterface
	DEBUG_HEARING    = (1 << 6),
	DEBUG_EVENTS     = (1 << 7), // NextBotEventResponderInterface
	DEBUG_ERRORS     = (1 << 8),
};

methodmap INextBotEventResponder
{
	public native INextBotEventResponder FirstContainedResponder();
	public native INextBotEventResponder NextContainedResponder(INextBotEventResponder prev);

	public native void OnLeaveGround(CBaseEntity ground);
	public native void OnLandOnGround(CBaseEntity ground);

	public native void OnMoveToSuccess(const Path path);
	public native void OnMoveToFailure(const Path path, MoveToFailureType reason);
	public native void OnStuck();
	public native void OnUnStuck();

	public native void OnPostureChanged();

	public native void OnAnimationActivityComplete(int activity);
	public native void OnAnimationActivityInterrupted(int activity);
	//public native void OnAnimationEvent

	public native void OnIgnite();
	//public native void OnInjured
	//public native void OnKilled
	//public native void OnOtherKilled

	public native void OnSight(CBaseEntity subject);
	public native void OnLostSight(CBaseEntity subject);
	
	//public native void OnSound
	//public native void OnSpokeConcept
	public native void OnWeaponFired(CBaseCombatCharacter whoFired, CBaseEntity weapon);

	public native void OnNavAreaChanged(CNavArea newArea, CNavArea oldArea);

	public native void OnModelChanged();

	public native void OnPickUp(CBaseEntity item, CBaseCombatCharacter giver);
	public native void OnDrop(CBaseEntity item);
	public native void OnActorEmoted(CBaseCombatCharacter emoter, int emote);

	public native void OnCommandAttack(CBaseEntity victim);
	public native void OnCommandApproach(const float pos[3], float range = 0.0);
	public native void OnCommandApproachEntity(CBaseEntity goal);
	public native void OnCommandRetreat(CBaseEntity threat, float range = 0.0);
	public native void OnCommandPause(float duration = 0.0);
	public native void OnCommandResume();

	public native void OnCommandString(const char[] command);

	public native void OnShoved(CBaseEntity pusher);
	public native void OnBlinded(CBaseEntity blinder);

	public native void OnTerritoryContested(int territoryID);
	public native void OnTerritoryCaptured(int territoryID);
	public native void OnTerritoryLost(int territoryID);

	public native void OnWin();
	public native void OnLose();
};

typedef TraceFilterActors = function bool(int iEntityCollide, int contentsMask, int desiredcollisiongroup);

/**
 * A single component of a NextBot.
 */
methodmap INextBotComponent < INextBotEventResponder
{
	public native void Reset();
	public native void Update();
	public native void Upkeep();

	/**
	 * Returns the INextBot that this component belongs to.
	 *
	 * @return     INextBot pointer
	 */
	public native INextBot GetBot();
};

/**
 * The main NextBot interface.
 */
methodmap INextBot < INextBotEventResponder
{
	public native void Reset();
	public native void Update();
	public native void Upkeep();

	/**
	 * Whether the NextBot is removed upon the NextBot manager reset or not. By
	 * default this is true.
	 *
	 * @return     True if removed on reset, false otherwise
	 */
	public native bool IsRemovedOnReset();

	/**
	 * Returns the entity index the NextBot is associated with.
	 *
	 * @return     Entity index
	 */
	public native int GetEntity();

	/**
	 * Returns the entity index the NextBot is associated with.
	 *
	 * @return     Entity index, or -1 if entity is not a
	 *             NextBotCombatCharacter.
	 */
	public native int GetNextBotCombatCharacter();

	/**
	 * Returns a pointer to the NextBot's ILocomotion interface.
	 *
	 * @return     ILocomotion pointer
	 */
	public native ILocomotion GetLocomotionInterface();

	/**
	 * Returns a pointer to the NextBot's IBody interface.
	 *
	 * @return     IBody pointer
	 */
	public native IBody GetBodyInterface();

	/**
	 * Returns a pointer to the NextBot's IIntention interface.
	 *
	 * @return     IIntention pointer
	 */
	public native IIntention GetIntentionInterface();

	/**
	 * Returns a pointer to the NextBot's IVision interface.
	 *
	 * @return     IVision pointer
	 */
	public native IVision GetVisionInterface();

	/**
	 * Sets the world position of the NextBot's entity. This is an alias for
	 * GetBodyInterface().SetPosition().
	 *
	 * @param pos        Position vector
	 * @return           True if successful, false otherwise
	 */
	public native bool SetPosition(const float pos[3]);

	/**
	 * Gets the world position of the NextBot's entity.
	 *
	 * @param pos        Position vector
	 */
	public native void GetPosition(float pos[3]);

	/**
	 * Returns if the given entity is an enemy. By default this checks if team
	 * is different.
	 *
	 * @param entity     Entity index
	 * @return           True if enemy, false otherwise
	 */
	public native bool IsEnemy(int entity);

	/**
	 * Returns if the given entity is a friend. By default this checks if team
	 * is the same.
	 *
	 * @param entity     Entity index
	 * @return           True if friend, false otherwise
	 */
	public native bool IsFriend(int entity);

	/**
	 * Returns if the given entity is myself.
	 *
	 * @param entity     Entity index
	 * @return           True if self, false otherwise
	 */
	public native bool IsSelf(int entity);

	/**
	 * Returns if allowed to climb onto the given entity.
	 *
	 * @param entity     Entity index
	 * @return           True if allowed to climb on, false otherwise
	 */
	public native bool IsAbleToClimbOnto(int entity);

	/**
	 * Returns if the entity is breakable.
	 *
	 * @param entity     Entity index
	 * @return           True if breakable, false otherwise
	 */
	public native bool IsAbleToBreak(int entity);

	/**
	 * Returns if the NextBot can block the movement of the other NextBot.
	 *
	 * @param bot        INextBot
	 * @return           True if can block, false otherwise
	 */
	public native bool IsAbleToBlockMovementOf(INextBot bot);

	/**
	 * Returns if the NextBot should care about touching the given entity.
	 * If true, will propogate the OnContact() event to components.
	 *
	 * @param entity     Entity touching
	 * @return           True to care, false otherwise
	 */
	public native bool ShouldTouch(int entity);

	/**
	 * L4D2 only.
	 */
	public native void ReactToSurvivorVisibility();

	/**
	 * L4D2 only.
	 */
	public native void ReactToSurvivorNoise();

	/**
	 * L4D2 only.
	 */
	public native void ReactToSurvivorContact();

	/**
	 * Whether the NextBot has moved recently or not.
	 *
	 * @return     True if haven't moved in a while, false otherwise
	 */
	public native bool IsImmobile();

	/**
	 * Amount of time spent being immobile.
	 *
	 * @return     Immobile time
	 */
	public native float GetImmobileDuration();

	/**
	 * Stops being considered immobile.
	 */
	public native void ClearImmobileStatus();

	/**
	 * The maximum speed of the NextBot to be considered immobile.
	 *
	 * @return     Speed
	 */
	public native float GetImmobileSpeedThreshold();

	/**
	 * Returns a pointer to the current Path object of the NextBot.
	 *
	 * Note: This native is not available in L4D1 or L4D2.
	 *
	 * @return     Path object
	 */
	public native PathFollower GetCurrentPath();

	/**
	 * Sets the current Path object of the NextBot to the given Path.
	 *
	 * Note: This native is not available in L4D1 or L4D2.
	 */
	public native void SetCurrentPath(const PathFollower path);

	/**
	 * Notifies the NextBot that the path was destroyed.
	 *
	 * Note: This native is not available in L4D1 or L4D2.
	 *
	 * @param path     Path object
	 */
	public native void NotifyPathDestruction(const PathFollower path);

	/**
	 * Checks if the given entity is within the given distance.
	 *
	 * @param entity     Entity index
	 * @param dist       Distance
	 * @return           True if within, false otherwise
	 */
	public native bool IsRangeLessThan(int entity, float dist);

	/**
	 * Checks if the given position is within the given distance.
	 *
	 * @param pos        Position vector
	 * @param dist       Distance
	 * @return           True if within, false otherwise
	 */
	public native bool IsRangeLessThanEx(const float pos[3], float dist);

	/**
	 * Checks if the given entity is farther than the given distance.
	 *
	 * @param entity     Entity index
	 * @param dist       Distance
	 * @return           True if farther, false otherwise
	 */
	public native bool IsRangeGreaterThan(int entity, float dist);

	/**
	 * Checks if the given position is farther than the given distance.
	 *
	 * @param pos        Position vector
	 * @param dist       Distance
	 * @return           True if farther, false otherwise
	 */
	public native bool IsRangeGreaterThanEx(const float pos[3], float dist);

	/**
	 * Calculates distance from the NextBot to the entity.
	 *
	 * @param entity     Entity index
	 * @return           Distance
	 */
	public native float GetRangeTo(int entity);

	/**
	 * Calculates distance from the NextBot to the position.
	 *
	 * @param pos        Position vector
	 * @return           Distance
	 */
	public native float GetRangeToEx(const float pos[3]);

	/**
	 * Calculates squared distance from the NextBot to the entity.
	 *
	 * @param entity     Entity index
	 * @return           Squared distance
	 */
	public native float GetRangeSquaredTo(int entity);

	/**
	 * Calculates squared distance from the NextBot to the position.
	 *
	 * @param pos        Position vector
	 * @return           Squared distance
	 */
	public native float GetRangeSquaredToEx(const float pos[3]);

	/**
	 * Whether or not the given debug type is enabled. Debug types can be
	 * toggled via the nb_debug command.
	 *
	 * @param type     Type of debugging
	 * @return         True if debugging, false otherwise
	 */
	public native bool IsDebugging(NextBotDebugType type);

	/**
	 * Retrieves the identifier the NextBot uses while debugging.
	 *
	 * @param name        String buffer
	 * @param maxsize     Size of buffer
	 */
	public native void GetDebugIdentifier(char[] name, int maxsize);

	/**
	 * Whether or not the given name matches the bot's debug identifier.
	 *
	 * @param name        Debug name
	 */
	public native bool IsDebugFilterMatch(const char[] name);

	/**
	 * Shows world entity text of the given string. By default this does not
	 * work.
	 *
	 * @param name        Debug string
	 */
	public native void DisplayDebugText(const char[] name);
};

/**
 * Extension's basic NextBot interface
 */
methodmap ToolsNextBot < INextBot
{
	/**
	 * Creates and attaches a ToolsNextBot interface, to the given entity.
	 *
	 * @param entity     Entity address
	 * @return           The created NextBot interface
	 */
	public native ToolsNextBot(Address entity);
}

stock INextBot ToolsNextBot_Factory(Address entity)
{
	return ToolsNextBot(entity);
}

/**
 * Extension's basic NextBotPlayer interface
 */
methodmap ToolsNextBotPlayer < ToolsNextBot
{
	/**
	 * Creates and attaches a ToolsNextBotPlayer interface, to the given entity.
	 *
	 * @param entity     Entity address
	 * @return           The created NextBot interface
	 */
	public native ToolsNextBotPlayer(Address entity);

	/*
	 * Defines whether or not the nextbot interface should update when
	 * its attached entity is dead. Default is true.
	*/
	property bool IsDormantWhenDead
	{
		public native set(bool value);
		public native get();
	}
}

stock INextBot ToolsNextBotPlayer_Factory(Address entity)
{
	return ToolsNextBotPlayer(entity);
}

#include "nextbot/knownentity.inc"
#include "nextbot/vision.inc"
#include "nextbot/intention.inc"
#include "nextbot/locomotion.inc"
#include "nextbot/path.inc"
#include "nextbot/body.inc"
#include "nextbot/behavior.inc"