#if !defined _CBASENPC_EXTENSION_INC_
#define _CBASENPC_EXTENSION_INC_

#include <sdktools>

#define EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS	(1<<14)

#define EF_BONEMERGE			0x001 	// Performs bone merge on client side
#define	EF_BRIGHTLIGHT 			0x002	// DLIGHT centered at entity origin
#define	EF_DIMLIGHT 			0x004	// player flashlight
#define	EF_NOINTERP				0x008	// don't interpolate the next frame
#define	EF_NOSHADOW				0x010	// Don't cast no shadow
#define	EF_NODRAW				0x020	// don't draw entity
#define	EF_NORECEIVESHADOW		0x040	// Don't receive no shadow
#define	EF_BONEMERGE_FASTCULL	0x080	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
										// parent and uses the parent's bbox + the max extents of the aiment.
										// Otherwise, it sets up the parent's bones every frame to figure out where to place
										// the aiment, which is inefficient because it'll setup the parent's bones even if
										// the parent is not in the PVS.
#define	EF_ITEM_BLINK			0x100	// blink an item so that the user notices it.
#define	EF_PARENT_ANIMATES		0x200	// always assume that the parent entity is animating
#define	EF_MAX_BITS = 10


#define MAX_NPCS		200

#define	DAMAGE_NO				0
#define DAMAGE_EVENTS_ONLY		1
#define	DAMAGE_YES				2
#define	DAMAGE_AIM				3

#define TEAM_ANY -1

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

const CBaseNPC INVALID_NPC = view_as<CBaseNPC>(-1);
const CNavArea NULL_AREA = view_as<CNavArea>(0);
const CNavLadder NULL_LADDER_AREA = view_as<CNavLadder>(0);
const CKnownEntity NULL_KNOWN_ENTITY = view_as<CKnownEntity>(0);
const NextBotAction NULL_ACTION = view_as<NextBotAction>(0);
const INextBot NULL_NEXT_BOT = view_as<INextBot>(0);
const Segment NULL_PATH_SEGMENT = view_as<Segment>(0);

native INextBot CBaseNPC_GetNextBotOfEntity(int iEntity);
native void CBaseNPC_HookEventKilled(int iEntity);

enum
{
	NAV_MESH_CROUCH = 0x0001, 
	NAV_MESH_JUMP = 0x0002, 
	NAV_MESH_PRECISE = 0x0004, 
	NAV_MESH_NO_JUMP = 0x0008, 
	NAV_MESH_STOP = 0x0010, 
	NAV_MESH_RUN = 0x0020, 
	NAV_MESH_WALK = 0x0040, 
	NAV_MESH_AVOID = 0x0080, 
	NAV_MESH_TRANSIENT = 0x0100, 
	NAV_MESH_DONT_HIDE = 0x0200, 
	NAV_MESH_STAND = 0x0400, 
	NAV_MESH_NO_HOSTAGES = 0x0800
};

enum NavDirType
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,

	NUM_DIRECTIONS
};

enum NavTraverseType
{
	// NOTE: First 4 directions MUST match NavDirType
	GO_NORTH = 0,
	GO_EAST,
	GO_SOUTH,
	GO_WEST,

	GO_LADDER_UP,
	GO_LADDER_DOWN,
	GO_JUMP,
	GO_ELEVATOR_UP,
	GO_ELEVATOR_DOWN,

	NUM_TRAVERSE_TYPES
};

//Thanks to Pelipoika & Sigsegv
enum TFNavAttributeType
{
	BLOCKED                     = (1 << 0),
	RED_SPAWN_ROOM              = (1 << 1),
	BLUE_SPAWN_ROOM             = (1 << 2),
	SPAWN_ROOM_EXIT             = (1 << 3),
	AMMO                        = (1 << 4),
	HEALTH                      = (1 << 5),
	CONTROL_POINT               = (1 << 6),
	BLUE_SENTRY                 = (1 << 7),
	RED_SENTRY                  = (1 << 8),
	/* bit  9: unused */
	/* bit 10: unused */
	BLUE_SETUP_GATE             = (1 << 11),
	RED_SETUP_GATE              = (1 << 12),
	BLOCKED_AFTER_POINT_CAPTURE = (1 << 13),
	BLOCKED_UNTIL_POINT_CAPTURE = (1 << 14),
	BLUE_ONE_WAY_DOOR           = (1 << 15),
	RED_ONE_WAY_DOOR            = (1 << 16),
	WITH_SECOND_POINT           = (1 << 17),
	WITH_THIRD_POINT            = (1 << 18),
	WITH_FOURTH_POINT           = (1 << 19),
	WITH_FIFTH_POINT            = (1 << 20),
	SNIPER_SPOT                 = (1 << 21),
	SENTRY_SPOT                 = (1 << 22),
	/* bit 23: unused */
	/* bit 24: unused */
	NO_SPAWNING                 = (1 << 25),
	RESCUE_CLOSET               = (1 << 26),
	BOMB_DROP                   = (1 << 27),
	DOOR_NEVER_BLOCKS           = (1 << 28),
	DOOR_ALWAYS_BLOCKS          = (1 << 29),
	UNBLOCKABLE                 = (1 << 30),
	/* bit 31: unused */
};

methodmap SurroundingAreasCollector < Handle
{
	public native CNavArea Get(int index);
	public native int Count();
};

methodmap TSurroundingAreasCollector < Handle
{
	public native CTNavArea Get(int index);
	public native int Count();
};

typeset CollectSurroundingAreasCallback
{
	function void (TSurroundingAreasCollector collector, any data);
};

methodmap CNavMesh
{
	public native SurroundingAreasCollector CollectSurroundingAreas(CNavArea startArea, float travelDistanceLimit = 1500.0, float maxStepUpLimit = 18.0, float maxDropDownLimit = 100.0);
	public native void TCollectSurroundingAreas(CNavArea startArea, float travelDistanceLimit = 1500.0, float maxStepUpLimit = 18.0, float maxDropDownLimit = 100.0, CollectSurroundingAreasCallback callback, any data = 0);
	public native CNavArea GetNearestNavArea(const float pos[3], bool anyZ = false, float maxDist = 10000.0, bool checkLOS = false, bool checkGround = true, int team = -2);
};

enum CNavMesh 
{
	TheNavMesh
};

methodmap CTNavArea
{
	public native NavTraverseType GetParentHow();
	public native float GetCostSoFar();
	public native CNavArea GetRealNavArea();
};

methodmap CNavArea
{
	public native void UpdateBlocked(bool force = false, int teamID = -2);
	public native bool IsBlocked(int teamID, bool ignoreNavBlockers = false);

	public native int GetID();
	
	public native void SetParent(CNavArea parent, NavTraverseType how = NUM_TRAVERSE_TYPES);
	public native CNavArea GetParent();
	public native NavTraverseType GetParentHow();
	
	public native void SetCostSoFar(float value);
	public native float GetCostSoFar();
	
	public native int GetAttributes();
	public bool HasAttributes(int bits) { return ( this.GetAttributes() & bits ) ? true : false; }
	public native void GetCenter(float vecCenter[3]);
	
	public native bool IsConnected(const CNavArea area, NavDirType dir);
	public native bool IsEdge(NavDirType dir);
	
	public native bool Contains(const CNavArea area);
	public native float GetZ(float x, float y);
	public native float GetZVector(float vecPos[3]);
	public native void ComputeNormal(float vecNormal[3], bool alternate = false);
};

methodmap CTFNavArea < CNavArea
{
	public native TFNavAttributeType GetAttributesTF();
	public native void SetAttributeTF(TFNavAttributeType bits);
	public native void ClearAttributeTF(TFNavAttributeType bits);
	public native bool HasAttributeTF(TFNavAttributeType bits);
};

methodmap CNavLadder
{
	property float length
	{
		public native get();
	}
};

methodmap INextBotEventResponder
{
	public native INextBotEventResponder FirstContainedResponder();
	public native INextBotEventResponder NextContainedResponder(INextBotEventResponder prev);
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

/**
 * An entry of an entity being tracked by a NextBot.
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

enum TraverseWhenType 
{ 
	IMMEDIATELY,
	EVENTUALLY
};

/**
 * Component that manages the movement state of the NextBot.
 */
methodmap ILocomotion < INextBotComponent
{
	/**
	 * Move the bot towards the given position.
	 *
	 * @param dst            Position to move towards
	 * @param goalWeight     Weight. Multiple Approach() calls within a single
	 *                       frame will be combined into a single, weighted
	 *                       average goal.
	 */
	public native void Approach(const float dst[3], float goalWeight = 1.0);

	/**
	 * Move the bot towards the given position precisely.
	 *
	 * @param dst      Position to move towards
	 */
	public native void DriveTo(const float dst[3]);

	/**
	 * Traverses to an adjacent ledge.
	 *
	 * @param dst            Destination vector
	 * @param dir            Direction of movement
	 * @param obstacle       Entity to climb
	 * @return               True if was able to start climbing, false otherwise
	 */
	public native bool ClimbUpToLedge(const float dst[3], const float dir[3], int obstacle);

	/**
	 * Jumps towards the given destination.
	 *
	 * @param dst      Destination vector
	 * @param dir      Direction of movement
	 */
	public native void JumpAcrossGap(const float dst[3], const float dir[3]);

	/**
	 * Performs a simple jump
	 */
	public native void Jump();

	/**
	 * Whether the bot is currently climbing or jumping, or not.
	 *
	 * @return     True if jumping or climbing, false otherwise
	 */
	public native bool IsClimbingOrJumping();

	/**
	 * Whether the bot is currently climbing or not.
	 *
	 * @return     True if climbing, false otherwise
	 */
	public native bool IsClimbingUpToLedge();

	/**
	 * Whether the bot is currently jumping or not.
	 *
	 * @return     True if jumping, false otherwise
	 */
	public native bool IsJumpingAcrossGap();

	/**
	 * Whether the bot is in the middle of a complex action (climbing a ladder,
	 * climbing a ledge, jumping, etc) that shouldn't be interrupted.
	 *
	 * @return     True if scrambling, false otherwise
	 */
	public native bool IsScrambling();

	/**
	 * Sets movement to running.
	 */
	public native void Run();

	/**
	 * Sets movement to walking.
	 */
	public native void Walk();

	/**
	 * Stops all movement.
	 */
	public native void Stop();

	/**
	 * Whether the bot is in a running movement state or not.
	 *
	 * @return     True if running, false otherwise
	 */
	public native bool IsRunning();

	/**
	 * Sets the movement speed of the bot to the given value.
	 *
	 * @param speed     Speed to move at
	 */
	public native void SetDesiredSpeed(float speed);

	/**
	 * Returns the current desired movement speed of the bot.
	 *
	 * @return     Speed
	 */
	public native float GetDesiredSpeed();

	/**
	 * Sets the maximum speed of the bot.
	 *
	 * @param limit     Speed limit
	 */
	public native void SetSpeedLimit(float limit);

	/**
	 * Returns the maximum speed the bot can move at.
	 *
	 * @return     Speed limit
	 */
	public native float GetSpeedLimit();

	/**
	 * Returns whether the bot is on the ground or not.
	 *
	 * @return     True if on ground, false otherwise
	 */
	public native bool IsOnGround();

	/**
	 * Gets the entity that the bot is standing on.
	 *
	 * @return     Entity index of ground entity
	 */
	public native int GetGround();

	/**
	 * Retrieves the normal vector of the plane the bot is standing on.
	 *
	 * @param normal      Directional vector
	 */
	public native void GetGroundNormal(float normal[3]);

	/**
	 * The current speed the bot is moving at on the ground.
	 *
	 * @return     Ground speed
	 */
	public native float GetGroundSpeed();

	/**
	 * Retrieves the direction of ground movement of the bot.
	 *
	 * @param motion      Directional vector
	 */
	public native void GetGroundMotionVector(float motion[3]);

	/**
	 * Starts climbing up the given ladder to dismount to a given area.
	 *
	 * @param ladder             Ladder to climb
	 * @param dismountGoal       Area to dismount ladder from
	 */
	public native void ClimbLadder(CNavLadder ladder, CNavArea dismountGoal);

	/**
	 * Starts climbing down the ladder to dismount to a given area.
	 *
	 * @param ladder             Ladder to descend
	 * @param dismountGoal       Area to dismount ladder from
	 */
	public native void DescendLadder(CNavLadder ladder, CNavArea dismountGoal);

	/**
	 * Whether the bot is currently using a ladder or not.
	 *
	 * @return     True if using a ladder, false otherwise
	 */
	public native bool IsUsingLadder();

	/**
	 * Whether the bot is currently climbing up or down a ladder.
	 *
	 * @return     True if climbing a ladder, false otherwise
	 */
	public native bool IsAscendingOrDescendingLadder();

	public native bool IsAbleToAutoCenterOnLadder();

	/**
	 * Sets the look goal of the bot to the given position.
	 *
	 * @param pos      Position to look at
	 */
	public native void FaceTowards(const float pos[3]);

	public native void SetDesiredLean(const float lean[3]);
	public native void GetDesiredLean(float lean[3]);

	/**
	 * Whether the bot is allowed to jump or not.
	 *
	 * @return     True if allowed, false otherwise
	 */
	public native bool IsAbleToJumpAcrossGaps();

	/**
	 * Whether the bot is allowed to climb or not.
	 *
	 * @return     True if allowed, false otherwise
	 */
	public native bool IsAbleToClimb();

	/**
	 * Retrieves the location of the bot's feet.
	 *
	 * @param feet      Position vector
	 */
	public native void GetFeet(float feet[3]);

	/**
	 * Returns the maximum height the bot can traverse without jumping or
	 * dropping down.
	 *
	 * @return     Step height
	 */
	public native float GetStepHeight();

	/**
	 * Returns the maximum height of an obstacle the bot can traverse by
	 * jumping.
	 *
	 * @return     Jump height
	 */
	public native float GetMaxJumpHeight();

	/**
	 * Returns the maximum height the bot can safely drop down from a cliff.
	 *
	 * @return     Drop down height
	 */
	public native float GetDeathDropHeight();

	/**
	 * The run speed of the bot.
	 *
	 * @return     Speed
	 */
	public native float GetRunSpeed();

	/**
	 * The walk speed of the bot.
	 *
	 * @return     Speed
	 */
	public native float GetWalkSpeed();

	/**
	 * The acceleration of the bot.
	 *
	 * @return     Acceleration
	 */
	public native float GetMaxAcceleration();

	/**
	 * The deceleration of the bot.
	 *
	 * @return     Acceleration
	 */
	public native float GetMaxDeceleration();

	/**
	 * The current world velocity of the bot.
	 *
	 * @param vec      Velocity vector
	 */
	public native void GetVelocity(float vel[3]);

	/**
	 * The current speed of the bot.
	 *
	 * @return     Speed
	 */
	public native float GetSpeed();

	/**
	 * The current desired world movement direction of the bot.
	 *
	 * @param vec      Direction vector
	 */
	public native void GetMotionVector(float motion[3]);

	/**
	 * Whether or not the area can be used for navigation.
	 *
	 * @param area     CNavArea to test
	 * @return         True if traversable, false otherwise
	 */
	public native bool IsAreaTraversable(CNavArea area);

	/**
	 * The Z component of the steepest ground normal vector the bot can traverse.
	 *
	 * @return         Slope limit
	 */
	public native float GetTraversableSlopeLimit();

	/**
	 * Checks if the bot can safely move across a line specified by the given
	 * positions (no gaps, no obstacles). If false, fraction will be filled
	 * with fraction of walkable ray.
	 *
	 * @param from           Start position
	 * @param to             End position
	 * @param when           Types of entities to ignore
	 * @param fraction       Walkable trace fraction
	 * @return               True if traversable, false otherwise
	 */
	public native bool IsPotentiallyTraversable(const float from[3], const float to[3], TraverseWhenType when = EVENTUALLY, float &fraction = 0.0);

	/**
	 * Checks if there's a gap between the two positions that will need to be
	 * jumped over. If false, fraction will be filled with fraction of walkable
	 * ray.
	 *
	 * @param from      Start position
	 * @param to        End position
	 * @param fraction  Walkable trace fraction
	 * @return          True if there's a gap, false otherwise.
	 */
	public native bool HasPotentialGap(const float from[3], const float to[3], float &fraction = 0.0);

	/**
	 * Checks if the given position is a gap while moving in the
	 * specified direction.
	 *
	 * @param pos      Position vector
	 * @param to       Param description
	 * @return         True if position is a gap, false otherwise
	 */
	public native bool IsGap(const float pos[3], const float fwd[3]);

	/**
	 * Checks if the entity is traversable.
	 *
	 * @param entity     Entity index to check
	 * @param when       Type to check
	 * @return           True if traversable, false otherwise
	 */
	public native bool IsEntityTraversable(int entity, TraverseWhenType when);

	/**
	 * Checks if the bot is currently stuck.
	 *
	 * @return     True if stuck, false otherwise
	 */
	public native bool IsStuck();

	/**
	 * Returns how long the bot has been stuck for.
	 *
	 * @return     Stuck duration
	 */
	public native float GetStuckDuration();

	/**
	 * Clears the bot of being stuck with a given reason.
	 *
	 * @param reason     Reason for clearing the stuck status.
	 */
	public native void ClearStuckStatus(const char[] reason = "");

	/**
	 * Whether the bot is attempting to move or not.
	 *
	 * @return     True if trying to move, false otherwise.
	 */
	public native bool IsAttemptingToMove();

	/**
	 * Whether the bot should collide with the given entity or not.
	 *
	 * @param entity     Entity to collide
	 * @return           True to collide, false otherwise
	 */
	public native bool ShouldCollideWith(int entity);

	public native void AdjustPosture(const float lean[3]);
	public native void StuckMonitor();
};

methodmap NextBotGroundLocomotion < ILocomotion
{
	/**
	 * Retrieves the acceleration vector.
	 *
	 * @param acel      Acceleration vector
	 */
	public native void GetAcceleration(float acel[3]);

	/**
	 * Sets the acceleration vector.
	 *
	 * @param acel      Acceleration vector
	 */
	public native void SetAcceleration(const float acel[3]);

	/**
	 * Sets the world velocity.
	 *
	 * @param vel       Velocity vector
	 */
	public native void SetVelocity(const float vel[3]);

	/**
	 * Gets the current gravity.
	 *
	 * @return     Gravity
	 */
	public native float GetGravity();

	/**
	 * Returns friction for forward movement.
	 *
	 * @return     Friction
	 */
	public native float GetFrictionForward();

	/**
	 * Returns friction for sideways movement.
	 *
	 * @return     Friction
	 */
	public native float GetFrictionSideways();

	/**
	 * Returns rate of change of yaw while looking towards a position.
	 *
	 * @return     Yaw rate
	 */
	public native float GetMaxYawRate();
}

typeset NavPathCostFunctor
{
	function float(INextBot bot, CNavArea area, CNavArea fromArea, CNavLadder ladder, int iElevator, float length);
};
typedef TraceFilterActors = function bool(int iEntityCollide, int contentsMask, int desiredcollisiongroup);

enum SegmentType
{
	ON_GROUND,
	DROP_DOWN,
	CLIMB_UP,
	JUMP_OVER_GAP,
	LADDER_UP,
	LADDER_DOWN,
	
	NUM_SEGMENT_TYPES
};

enum MoveCursorType
{
	MOVECUR_ABS = 0,
	MOVECUR_REL = 1,
	MOVECUR_MAX,
};

enum MoveToFailureType
{
	FAIL_NO_PATH_EXISTS = 0,
	FAIL_STUCK        = 1,
	FAIL_FELL_OFF     = 2,
};

/**
 * A segment of a Path.
 */
methodmap Segment
{
	/**
	 * The CNavArea that this segment traverses.
	 */
	property CNavArea area
	{
		public native get();
	}

	/**
	 * The direction that this segment traverses.
	 */
	property NavTraverseType how
	{
		public native get();
	}

	/**
	 * The end position of the segment. 
	 *
	 * Note: This is not to be confused as the start position; use either the
	 * previous segment's GetPos() method or manually calculate it using this
	 * segment's negative forward vector and length to get the start position.
	 *
	 * @param pos   Position vector
	 */
	public native void GetPos(float pos[3]);

	/**
	 * If how is GO_LADDER_UP or GO_LADDER_DOWN, then this is the CNavLadder
	 * that bots should use to travel along the segment, otherwise will be NULL.
	 */
	property CNavLadder ladder
	{
		public native get();
	}

	/**
	 * The type of movement required to travel this segment.
	 */
	property SegmentType type
	{
		public native get();
	}

	/**
	 * The direction vector from start to end that bots are expected to travel
	 * for this segment.
	 *
	 * @param vec   Forward vector
	 */
	public native void GetForward(float vec[3]);

	/**
	 * The total length of this segment from start to end.
	 */
	property float length
	{
		public native get();
	}

	/**
	 * The total distance of this segment from the start of the path to the
	 * end position of this segment.
	 */
	property float distanceFromStart
	{
		public native get();
	}

	/**
	 * How much the path 'curves' from start to end in the XY plane. 0 = none,
	 * 1 = 180 degree doubleback.
	 */
	property float curvature
	{
		public native get();
	}

	/**
	 * The center position of the portal that connects the previous segment's
	 * area and this segment's area.
	 *
	 * @param vec   Position vector
	 */
	public native void GetPortalCenter(float vec[3]);

	/**
	 * The width of the portal, in half.
	 */
	property float m_portalHalfWidth
	{
		public native get();
	}
};

/**
 * A structure that stores data about the current progress on the Path.
 */
methodmap CursorData
{
	/**
	 * The current position along the path.
	 *
	 * @param vec   Position vector
	 */
	public native void GetPos(float vec[3]);

	/**
	 * The current forward direction along the path.
	 *
	 * @param vec   Forward vector
	 */
	public native void GetForward(float vec[3]);
	
	/**
	 * How much the path 'curves' from start to end in the XY plane. 0 = none,
	 * 1 = 180 degree doubleback.
	 */
	property float curvature
	{
		public native get();
	}

	/**
	 * The previous segment passed.
	 */
	property Segment segmentPrior
	{
		public native get();
	}
};

enum SeekType
{
	SEEK_ENTIRE_PATH,			// search the entire path length
	SEEK_AHEAD,					// search from current cursor position forward toward end of path
	SEEK_BEHIND					// search from current cursor position backward toward path start
};

typeset PathComputeCallback
{
	function void (Path path, bool bSuccess, any data);
};

methodmap Path
{
	/**
	 * A Path that is used by NextBots to navigate the world.
	 *
	 * @param costFunction           Callback that calculates travel cost
	 * @param ignoreActorsFilter     Callback that determines which actors to ignore
	 * @param onlyActorsFilter       Callback that determines which actors to hit
	 */
	public native Path(NavPathCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);

	/**
	 * The total length of the Path.
	 *
	 * @return     Length
	 */
	public native float GetLength();

	/**
	 * Returns position along path a certain distance from the start.
	 *
	 * @param dist    Distance along path
	 * @param pos     Position vector
	 * @param seg     Segment to start from.
	 */
	public native void GetPosition(float dist, float pos[3], Segment seg = NULL_PATH_SEGMENT);

	/**
	 * Returns position along path that is closest to the given position.
	 *
	 * @param pos            Position to get closest to
	 * @param vec            Buffer to store closest position
	 * @param start          Segment to start at
	 * @param alongLimit     Maximum distance along path to check
	 */
	public native void GetClosestPosition(const float pos[3], float vec[3], Segment start = NULL_PATH_SEGMENT, float alongLimit = 0.0);

	/**
	 * Start position of the path.
	 *
	 * @param pos      Position vector
	 */
	public native void GetStartPosition(float pos[3]);

	/**
	 * End position of the path.
	 *
	 * @param pos      Position vector
	 */
	public native void GetEndPosition(float pos[3]);

	/**
	 * Entity index of the NextBot currently using this path.
	 *
	 * @param pos      Position vector
	 */
	public native int GetSubject();

	/**
	 * The segment the path has set for the goal.
	 *
	 * @return     Segment pointer
	 */
	public native Segment GetCurrentGoal();

	/**
	 * How much time since the path was last updated.
	 *
	 * @return     Game time
	 */
	public native float GetAge();

	/**
	 * Sets current progress of path to the start.
	 */
	public native void MoveCursorToStart();

	/**
	 * Sets current progress of path to the end.
	 */
	public native void MoveCursorToEnd();

	/**
	 * Sets current progress of path to the specified distance.
	 *
	 * @param dist       Distance to move
	 * @param mctype     Frame of reference to move from
	 */
	public native void MoveCursor(float dist, MoveCursorType mctype);

	/**
	 * Sets current progress of path to a position that is closest to the given position.
	 *
	 * @param pos            Position to move closest to
	 * @param type           Seek type
	 * @param alongLimit     Maximum distance to move along path
	 */
	public native void MoveCursorToClosestPosition(const float pos[3], SeekType type = SEEK_ENTIRE_PATH, float alongLimit = 0.0);

	/**
	 * Current progress on the path.
	 *
	 * @return     Progress
	 */
	public native float GetCursorPosition();

	/**
	 * Data of the current progress on the path.
	 *
	 * @return     CursorData pointer
	 */
	public native CursorData GetCursorData();

	/**
	 * Whether the current path has built a path or not.
	 *
	 * @return     True if valid, false otherwise
	 */
	public native bool IsValid();

	/**
	 * Clears the path.
	 */
	public native void Invalidate();

	/**
	 * Draws the path. By default this does not work.
	 *
	 * @param seg     Segment to start from
	 */
	public native void Draw(Segment seg = NULL_PATH_SEGMENT);
	public native void DrawInterpolated(float from, float to);

	/**
	 * The first segment of the path.
	 *
	 * @return     Segment pointer
	 */
	public native Segment FirstSegment();

	/**
	 * The next segment of the path, starting from the given segment.
	 *
	 * @return     Segment pointer
	 */
	public native Segment NextSegment(Segment seg);

	/**
	 * The previous segment of the path, starting from the given segment.
	 *
	 * @return     Segment pointer
	 */
	public native Segment PriorSegment(Segment seg);

	/**
	 * The last segment of the path.
	 *
	 * @return     Segment pointer
	 */
	public native Segment LastSegment();

	/**
	 * Computes a path to the given position.
	 *
	 * @param bot                        INextBot that will use this path
	 * @param goalPos                    Goal position
	 * @param maxPathLength              Maximum travel distance
	 * @param includeGoalIfPathFails     If true, then the path will include the goal regardless
	 *                                   of failure.
	 * @return                           True if path reached to goal. If false, path may or may
	 *                                   not be valid. Use IsValid() to check.
	 */
	public native bool ComputeToPos(INextBot bot, const float goalPos[3], float maxPathLength = 0.0, bool includeGoalIfPathFails = true);

	/**
	 * Computes a path to the given entity.
	 *
	 * @param bot                        INextBot that will use this path
	 * @param entity                     Goal entity
	 * @param maxPathLength              Maximum travel distance
	 * @param includeGoalIfPathFails     If true, then the path will include the goal regardless
	 *                                   of failure.
	 * @return                           True if path reached to goal. If false, path may or may
	 *                                   not be valid. Use IsValid() to check.
	 */
	public native bool ComputeToTarget(INextBot bot, int entity, float maxPathLength = 0.0, bool includeGoalIfPathFails = true);

	/**
	 * Invalids and destroys the Path from memory.
	 */
	public native void Destroy();
	
	// Threaded
	public native bool ComputeToPosT(INextBot bot, float vecPos[3], float maxPathLength = 0.0, bool includeGoalIfPathFails = true, PathComputeCallback callback, any data = 0);
	public native bool ComputeToTargetT(INextBot bot, int entity, float maxPathLength = 0.0, bool includeGoalIfPathFails = true, PathComputeCallback callback, any data = 0);
};

methodmap PathFollower < Path
{
	public native PathFollower(NavPathCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);

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
	public native ChasePath(SubjectChaseType how = DONT_LEAD_SUBJECT, NavPathCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);
	
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
	public native DirectChasePath(SubjectChaseType how = DONT_LEAD_SUBJECT, NavPathCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);
};

enum LookAtPriorityType
{
	BORING,
	INTERESTING,				
	IMPORTANT,					
	CRITICAL,			
	MANDATORY
};

enum ActivityType 
{ 
	MOTION_CONTROLLED_XY	= 0x0001,
	MOTION_CONTROLLED_Z		= 0x0002,
	ACTIVITY_UNINTERRUPTIBLE= 0x0004,
	ACTIVITY_TRANSITORY		= 0x0008,
	ENTINDEX_PLAYBACK_RATE	= 0x0010,
};
	
enum PostureType
{
	STAND,
	CROUCH,
	SIT,
	CRAWL,
	LIE
};
	
enum ArousalType
{
	NEUTRAL,
	ALERT,
	INTENSE
};

/**
 * Component that acts as an interface to the class that contains the NextBot.
 */
methodmap IBody < INextBotComponent
{
	/**
	 * Sets the world position of the bot.
	 *
	 * @param pos      Position vector
	 * @return         True if successful, false otherwise
	 */
	public native bool SetPosition(const float pos[3]);

	/**
	 * Gets the eye world position.
	 *
	 * @param pos      Position vector
	 */
	public native void GetEyePosition(float pos[3]);

	/**
	 * Gets the world directional vector of where the bot is looking.
	 *
	 * @param view      Directional vector
	 */
	public native void GetViewVector(float view[3]);

	//public native void AimHeadTowards(float pos[3], LookAtPriorityType priority, float duration, PluginBotReply reply, const char[] reason);
	//public native void AimHeadTowardsEx(int entity, LookAtPriorityType priority, float duration, PluginBotReply reply, const char[] reason);

	public native bool IsHeadAimingOnTarget();
	public native bool IsHeadSteady();
	public native float GetHeadSteadyDuration();
	public native float GetHeadAimSubjectLeadTime();
	public native float GetHeadAimTrackingInterval();
	public native void ClearPendingAimReply();
	public native float GetMaxHeadAngularVelocity();

	//public native bool StartActivity(Activity act, int unknown);
	//public native int SelectAnimationSequence(Activity act);
	//public native Activity GetActivity();
	//public native bool IsActivity(Activity act);
	//public native bool HasActivityType(int unknown);

	public native void SetDesiredPosture(PostureType posture);
	public native PostureType GetDesiredPosture();
	public native bool IsDesiredPosture(PostureType posture);
	public native bool IsInDesiredPosture();
	public native PostureType GetActualPosture();
	public native bool IsActualPosture(PostureType posture);
	public native bool IsPostureMobile();
	public native bool IsPostureChanging();

	public native void SetArousal(ArousalType arousal);
	public native ArousalType GetArousal();
	public native bool IsArousal(ArousalType arousal);

	public native float GetHullWidth();
	public native float GetHullHeight();
	public native float GetStandHullHeight();
	public native float GetCrouchHullHeight();
	public native void GetHullMins(float mins[3]);
	public native void GetHullMaxs(float maxs[3]);

	public native int GetSolidMask();
	public native int GetCollisionGroup();
};

methodmap CExtNPC __nullable__
{
	public native int GetEntity();
	property int Index
	{
		public get() { return view_as<int>(this); }
	}
};

methodmap CBaseNPC < CExtNPC
{
	public native CBaseNPC();
	
	// Interfaces
	public native INextBot GetBot();
	public native NextBotGroundLocomotion GetLocomotion();
	public native IBody GetBody();
	public native IVision GetVision();
	public native IIntention GetIntention();
	
	public native void SetType(char[] sBuffer);
	public native void GetType(char[] sBuffer, int length);
	
	// Body Functions
	public native void SetBodyMins(float vec[3]);
	public native void SetBodyMaxs(float vec[3]);
	public native void GetBodyMins(float vec[3]);
	public native void GetBodyMaxs(float vec[3]);
	
	property float flStepSize
	{
		public native get();
		public native set(float StepSize);
	}
	property float flGravity
	{
		public native get();
		public native set(float Gravity);
	}
	property float flAcceleration
	{
		public native get();
		public native set(float Acceleration);
	}
	property float flJumpHeight
	{
		public native get();
		public native set(float JumpHeight);
	}
	property float flDeathDropHeight
	{
		public native get();
		public native set(float DeathHeight);
	}
	property float flWalkSpeed
	{
		public native get();
		public native set(float WalkSpeed);
	}
	property float flRunSpeed
	{
		public native get();
		public native set(float RunSpeed);
	}
	property float flFrictionForward
	{
		public native get();
		public native set(float Friction);
	}
	property float flFrictionSideways
	{
		public native get();
		public native set(float Friction);
	}
	property float flMaxYawRate
	{
		public native get();
		public native set(float YawRate);
	}
}

methodmap CNPCs __nullable__
{
	public native any FindNPCByEntIndex(int ent);
	public native bool IsValidNPC(any npc);
	//To-Do: add npcs relationships?
}

CNPCs TheNPCs;

// Gesture code

#define ANIM_LAYER_ACTIVE		0x0001
#define ANIM_LAYER_AUTOKILL		0x0002
#define ANIM_LAYER_KILLME		0x0004
#define ANIM_LAYER_DONTRESTORE	0x0008
#define ANIM_LAYER_CHECKACCESS	0x0010
#define ANIM_LAYER_DYING		0x0020

#define MAX_OVERLAYS			15

#define ANIM_LAYER_ACTIVE        0x0001 
#define ANIM_LAYER_AUTOKILL      0x0002 
#define ANIM_LAYER_KILLME        0x0004 
#define ANIM_LAYER_DONTRESTORE   0x0008 
#define ANIM_LAYER_CHECKACCESS   0x0010 
#define ANIM_LAYER_DYING         0x0020
#define ANIM_LAYER_NOEVENTS         0x0040

#pragma deprecated Use CAnimationLayer instead
enum //CAnimationLayer 
{ 
    m_fFlags = 0,               //0x0000  
    m_bSequenceFinished = 4,    //0x0004  
    m_bLooping = 6,             //0x0006  
    m_nSequence = 8,            //0x0008  
    m_flCycle = 12,             //0x000C  
    m_flPrevCycle = 16,         //0x0010  
    m_flWeight = 20,            //0x0014  
    m_flPlaybackRate = 24,      //0x0018  
    m_flBlendIn = 28,           //0x001C  
    m_flBlendOut = 32,          //0x0020  
    m_flKillRate = 36,          //0x0024  
    m_flKillDelay = 40,         //0x0028  
    m_flLayerAnimtime = 44,     //0x002C  
    m_flLayerFadeOuttime = 48,  //0x0030  
    m_nActivity = 52,           //0x0034  
    m_nPriority = 56,           //0x0038  
    m_nOrder = 60,              //0x003C  
    m_flLastEventCheck = 64,    //0x0040  
    m_flLastEventAccess = 68,   //0x0044  
    m_pOwnerEntity = 72,        //0x0048  
     
    CAnimationLayer_Size = 76   //0x004C 
}; //Size=0x004C 

methodmap CAnimationOverlay  
{
	#pragma deprecated Use CAnimationLayer instead
	public CAnimationOverlay(Address address)
	{
		return view_as<CAnimationOverlay>(address);
	}
	
	property Address Address  
	{
		public get()  
		{
			return view_as<Address>(this); 
		}
	}
	
	property bool isNull
	{
		public get()  
		{
			return this.Address == Address_Null; 
		}
	}

	public any Get(int iOffset) 
	{ 
		return LoadFromAddress(this.Address + view_as<Address>(iOffset), NumberType_Int32); 
	}
	
	public void Set(int iOffset, any iValue) 
	{ 
		StoreToAddress(this.Address + view_as<Address>(iOffset), iValue, NumberType_Int32); 
	}
	
	#pragma deprecated Use CAnimationLayer.IsActive() instead
	public bool IsActive()    { return ((this.Get(m_fFlags) & ANIM_LAYER_ACTIVE)   != 0); } 
	#pragma deprecated Use CAnimationLayer.IsAutokill() instead
	public bool IsAutokill()  { return ((this.Get(m_fFlags) & ANIM_LAYER_AUTOKILL) != 0); } 
	#pragma deprecated Use CAnimationLayer.IsKillMe() instead
	public bool IsKillMe()    { return ((this.Get(m_fFlags) & ANIM_LAYER_KILLME)   != 0); } 
	#pragma deprecated Use CAnimationLayer.IsDying() instead
	public bool IsDying()     { return ((this.Get(m_fFlags) & ANIM_LAYER_DYING)    != 0); } 
	#pragma deprecated Use CAnimationLayer.NoEvents() instead
	public bool NoEvents()    { return ((this.Get(m_fFlags) & ANIM_LAYER_NOEVENTS) != 0); }
	#pragma deprecated Use CAnimationLayer.KillMe() instead
	public void KillMe()      { int iFlags = this.Get(m_fFlags); this.Set(m_fFlags, (iFlags |= ANIM_LAYER_KILLME)); } 
	#pragma deprecated Use CAnimationLayer.AutoKill() instead
	public void AutoKill()    { int iFlags = this.Get(m_fFlags); this.Set(m_fFlags, (iFlags |= ANIM_LAYER_AUTOKILL)); }
	#pragma deprecated Use CAnimationLayer.Dying() instead
	public void Dying()       { int iFlags = this.Get(m_fFlags); this.Set(m_fFlags, (iFlags |= ANIM_LAYER_DYING));  } 
	#pragma deprecated Use CAnimationLayer.Dead() instead
	public void Dead()        { int iFlags = this.Get(m_fFlags); this.Set(m_fFlags, (iFlags &= ~ANIM_LAYER_DYING)); }
	#pragma deprecated Use CAnimationLayer.IsAlive() instead
	public bool IsAlive()         { int iFlags = this.Get(m_fFlags); return (((iFlags & ANIM_LAYER_ACTIVE) != 0) || ((iFlags & ANIM_LAYER_KILLME) == 0)); }

	#pragma deprecated Use CAnimationLayer.m_nSequence instead
	public int GetLayerSequence() { return (this.Get(m_nSequence)); }
};

methodmap CAnimationLayer < CAnimationOverlay
{
	property int m_fFlags
	{
		public native get();
		public native set(int value);
	}

	property bool m_bSequenceFinished
	{
		public native get();
		public native set(bool value);
	}

	property bool m_bLooping
	{
		public native get();
		public native set(bool value);
	}

	property int m_nSequence
	{
		public native get();
		public native set(int value);
	}

	property float m_flCycle
	{
		public native get();
		public native set(float value);
	}

	property float m_flPrevCycle
	{
		public native get();
		public native set(float value);
	}

	property float m_flWeight
	{
		public native get();
		public native set(float value);
	}

	property float m_flPlaybackRate
	{
		public native get();
		public native set(float value);
	}

	property float m_flBlendIn
	{
		public native get();
		public native set(float value);
	}

	property float m_flBlendOut
	{
		public native get();
		public native set(float value);
	}

	property float m_flKillRate
	{
		public native get();
		public native set(float value);
	}

	property float m_flKillDelay
	{
		public native get();
		public native set(float value);
	}

	property float m_flLayerAnimtime
	{
		public native get();
		public native set(float value);
	}

	property float m_flLayerFadeOuttime
	{
		public native get();
		public native set(float value);
	}

	property Activity m_nActivity
	{
		public native get();
		public native set(Activity value);
	}

	property int m_nPriority
	{
		public native get();
		public native set(int value);
	}

	property int m_nOrder
	{
		public native get();
		public native set(int value);
	}

	property float m_flLastEventCheck
	{
		public native get();
		public native set(float value);
	}

	property float m_flLastAccess
	{
		public native get();
		public native set(float value);
	}

	property int m_pOwnerEntity
	{
		public native get();
		public native set(int value);
	}

	public bool IsActive()    { return (this.m_fFlags & ANIM_LAYER_ACTIVE) != 0; } 
	public bool IsAutokill()  { return (this.m_fFlags & ANIM_LAYER_AUTOKILL) != 0; } 
	public bool IsKillMe()    { return (this.m_fFlags & ANIM_LAYER_KILLME) != 0; } 
	public bool IsDying()     { return (this.m_fFlags & ANIM_LAYER_DYING) != 0; } 
	public bool NoEvents()    { return (this.m_fFlags & ANIM_LAYER_NOEVENTS) != 0; }
	public void KillMe()      { this.m_fFlags |= ANIM_LAYER_KILLME; } 
	public void AutoKill()    { this.m_fFlags |= ANIM_LAYER_AUTOKILL; }
	public void Dying()       { this.m_fFlags |= ANIM_LAYER_DYING; } 
	public void Dead()        { this.m_fFlags &= ~ANIM_LAYER_DYING; }
	public bool IsAlive()     { return (((this.m_fFlags & ANIM_LAYER_ACTIVE) != 0) || ((this.m_fFlags & ANIM_LAYER_KILLME) == 0)); }
};

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
	public native void SetAbsOrigin(float pos[3]);

	/**
	 * Sets the entity absolute angles.
	 *
	 * @param ang		Angles vector.
	 * @error 		Invalid entity.
	 */
	public native void SetAbsAngles(float ang[3]);

	/**
	 * Sets the entity absolute velocity.
	 *
	 * @param ang		Velocity vector.
	 * @error 		Invalid entity.
	 */
	public native void SetAbsVelocity(float vel[3]);

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
	public native void SetLocalOrigin(float pos[3]);

	/**
	 * Sets the entity local angles.
	 *
	 * @param ang		Angles vector.
	 * @error 		Invalid entity.
	 */
	public native void SetLocalAngles(float ang[3]);

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
	 * Alias of DispatchKeyValueFloat().
	 *
	 * @param keyName       Name of the key.
	 * @param value         String value.
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
	 * @param value         String value.
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
}

#include <cbasenpc/activity>

methodmap CBaseAnimating < CBaseEntity
{
	public CBaseAnimating(int entity) 
	{
		return view_as<CBaseAnimating>(entity);
	}
	
	public static native int iHandleAnimEvent();
	
#if defined _dhooks_included
	/**
	 * Hooks CBaseAnimating::HandleAnimEvent.
	 *
	 * @param callback 	DHookcallback that will be used.
	 * @error		Invalid entity.
	 */
	public void Hook_HandleAnimEvent(DHookCallback callback)
	{
		static DynamicHook hHook = null;
		if (hHook == null)
		{
			hHook = new DynamicHook(CBaseAnimating.iHandleAnimEvent(), HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity);
			if (hHook == null) return;
			hHook.AddParam(HookParamType_ObjectPtr);
		}
		hHook.HookEntity(Hook_Pre, this.index, callback);
	}
#endif
	
	public native void StudioFrameAdvance();
	public native void DispatchAnimEvents(CBaseAnimating anim);
	public native int LookupSequence(const char[] sName);
	public native float SequenceDuration(int iSequence);
	public native int SelectWeightedSequence(Activity activity);
	public native void ResetSequence(int sequence);
	public native int LookupAttachment(const char[] sAttachmentName);
	public native bool GetAttachment(int iAttachmentIndex, float vecOrigin[3], float vecAngles[3]);
	public native bool GetAttachmentMatrix(int iAttachmentIndex, float matrix[3][4]);
	
	public native Address GetModelPtr();
	public native int LookupPoseParameter(const char[] sParamName);
	public native float SetPoseParameter(int iPoseParam, float flNewValue);
	public native float GetPoseParameter(int iPoseParam);
};

methodmap CBaseAnimatingOverlay < CBaseAnimating
{
	public CBaseAnimatingOverlay(int entity) 
	{
		return view_as<CBaseAnimatingOverlay>(entity);
	}
	
	public native int AddGestureSequence(int iSequence, float flDuration = 1.0, bool autokill = true);
	public native int AddGesture(Activity activity, float flDuration = 1.0, bool autokill = true);
	public native bool IsPlayingGesture(Activity activity);
	public native void RestartGesture(Activity activity, bool addifmissing = true, bool autokill = true);
	public native void RemoveAllGestures();
	public native int AddLayeredSequence(int iSequence, int iPriority);
	public native void SetLayerPriority(int iLayer, int iPriority);
	public native bool IsValidLayer(int iLayer);
	public native void SetLayerDuration(int iLayer, float flDuration);
	public native float GetLayerDuration(int iLayer);
	public native void SetLayerCycle(int iLayer, float flCycle);
	public native float GetLayerCycle(int iLayer);
	public native void SetLayerPlaybackRate(int iLayer, float flPlaybackRate);
	public native void SetLayerWeight(int iLayer, float flWeight);
	public native float GetLayerWeight(int iLayer);
	public native void SetLayerBlendIn(int iLayer, float flBlendIn);
	public native void SetLayerBlendOut(int iLayer, float flBlendOut);
	public native void SetLayerAutokill(int iLayer, bool bAutokill);
	public native void SetLayerLooping(int iLayer, bool bLooping);
	public native void SetLayerNoRestore(int iLayer, bool bNoRestore);
	public native Activity GetLayerActivity(int iLayer);
	public native int GetLayerSequence(int iLayer);
	public native int FindGestureLayer(Activity activity);
	public native void RemoveLayer(int iLayer, float flKillRate = 0.2, float flKillDelay = 0.0);
	public native void FastRemoveLayer(int iLayer);
	public native CAnimationLayer GetAnimOverlay(int iLayer);
	public native int GetNumAnimOverlays();
	public native void SetNumAnimOverlays(int iNum);
	public native bool HasActiveLayer();
	
	public int FindGestureLayerBySequence(int iSequence)
	{
		// i = layer index
		int iCount = this.GetNumAnimOverlays();
		for(int i = 0; i < iCount; i++) 
		{
			CAnimationLayer pOverlay = this.GetAnimOverlay(i); 
			
			// Validate layer
			if (!pOverlay.IsAlive()) 
			{
				continue; 
			}

			// Validate sequence
			if (pOverlay.m_nSequence == iSequence) 
			{
				return i; 
			}
		}

		// Return on the unsuccess
		return -1; 
	}

};

methodmap CBaseCombatCharacter < CBaseAnimatingOverlay
{
	public CBaseCombatCharacter(int iEntity)
	{
		return view_as<CBaseCombatCharacter>(iEntity);
	}
	public native void UpdateLastKnownArea();
	public native CNavArea GetLastKnownArea();
};

enum EventResultPriorityType
{
	RESULT_NONE = 0,
	RESULT_TRY,
	RESULT_IMPORTANT,
	RESULT_CRITICAL
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
	function void (NextBotAction action, int actor, int ground);
	
	// OnContact
	// result is a CGameTrace structure
	function int (NextBotAction action, int actor, int other, Address result);
	function void (NextBotAction action, int actor, int other, Address result);

	// OnMoveToSuccess
	function int (NextBotAction action, int actor, Path path);
	function void (NextBotAction action, int actor, Path path);

	// OnMoveToFailure
	function int (NextBotAction action, int actor, Path path, MoveToFailureType reason);
	function void (NextBotAction action, int actor, Path path, MoveToFailureType reason);

	// OnStuck
	// OnUnStuck
	// OnPostureChanged
	// OnIgnite
	// OnModelChanged
	// OnCommandResume
	// OnWin
	// OnLose
	function int (NextBotAction action, int actor);
	function void (NextBotAction action, int actor);

	// OnAnimationActivityComplete
	// OnAnimationActivityInterrupted
	function int (NextBotAction action, int actor, Activity activity);
	function void (NextBotAction action, int actor, Activity activity);

	// OnCommandAttack
	// OnShoved (pusher = victim)
	// OnBlinded (blinder = victim)
	// OnTerritoryContested (territoryID = victim)
	// OnTerritoryCaptured (territoryID = victim)
	// OnTerritoryLost (territoryID = victim)
	function int (NextBotAction action, int actor, int victim);
	function void (NextBotAction action, int actor, int victim);

	// OnAnimationEvent
	function int (NextBotAction action, int actor, int event);
	function void (NextBotAction action, int actor, int event);
	function int (NextBotAction action, int actor, int event, const char[] options);
	function void (NextBotAction action, int actor, int event, const char[] options);
	function int (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime);
	function void (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime);
	function int (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype);
	function void (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype);
	function int (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype, int eventSourceEntity);
	function void (NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype, int eventSourceEntity);

	// OnInjured
	// OnKilled
	function int (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype);
	function void (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype);
	function int (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3]);
	function void (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3]);
	function int (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);
	function void (NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);

	// OnOtherKilled
	function int (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype);
	function void (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype);
	function int (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3]);
	function void (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3]);
	function int (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);
	function void (NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom);

	// OnSound
	function int (NextBotAction action, int actor, int sourceEntity, const float pos[3], KeyValues keys);
	function void (NextBotAction action, int actor, int sourceEntity, const float pos[3], KeyValues keys);

	// OnSpokeConcept
	// response is AI_Response*
	function int (NextBotAction action, int actor, int who, const char[] concept, Address response);
	function void (NextBotAction action, int actor, int who, const char[] concept, Address response);

	// OnWeaponFired
	// OnPickUp (item = whoFired, giver = weapon)
	// OnActorEmoted (emoter = whoFired, emote = weapon)
	function int (NextBotAction action, int actor, int whoFired, int weapon);
	function void (NextBotAction action, int actor, int whoFired, int weapon);

	// OnNavAreaChanged
	function int (NextBotAction action, int actor, CNavArea newArea, CNavArea oldArea);
	function void (NextBotAction action, int actor, CNavArea newArea, CNavArea oldArea);

	// OnCommandApproach
	function int (NextBotAction action, int actor, const float pos[3], float range);
	function void (NextBotAction action, int actor, const float pos[3], float range);

	// OnCommandRetreat
	function int (NextBotAction action, int actor, int threat, float range);
	function void (NextBotAction action, int actor, int threat, float range);

	// OnCommandPause
	function int (NextBotAction action, int actor, float duration);
	function void (NextBotAction action, int actor, float duration);

	// OnCommandString
	function int (NextBotAction action, int actor, const char[] command);
	function void (NextBotAction action, int actor, const char[] command);
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
	ContextualQueryType_IsHindrance,
	ContextualQueryType_SelectTargetPoint,
	ContextualQueryType_IsPositionAllowed,
	ContextualQueryType_SelectMoreDangerousThreat,
	ContextualQueryType_Max
}

typeset ActionContextualQueryCallback
{
	// ShouldPickup
	// IsHindrance
	function QueryResultType (NextBotAction action, INextBot bot, int item);

	// ShouldHurry
	// ShouldRetreat
	function QueryResultType (NextBotAction action, INextBot bot);

	// ShouldAttack
	function QueryResultType (NextBotAction action, INextBot bot, CKnownEntity knownEntity);

	// SelectTargetPoint
	// Leave pos unmodified for no response
	function void (NextBotAction action, INextBot bot, int subject, float pos[3]);

	// IsPositionAllowed
	function QueryResultType (NextBotAction action, INextBot bot, const float pos[3]);

	// SelectMoreDangerousThreat
	// Return NULL_KNOWN_ENTITY for no response
	function CKnownEntity (NextBotAction action, INextBot bot, int botEntity, CKnownEntity threat1, CKnownEntity threat2); 
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

typeset NextBotActionCallback
{
	function void (NextBotAction action, int actor)

	// OnStart
	// OnSuspend (interruptingAction = priorAction)
	// OnResume (interruptingAction = priorAction)
	function int (NextBotAction action, int actor, NextBotAction priorAction);
	function void (NextBotAction action, int actor, NextBotAction priorAction);

	// Update
	function int (NextBotAction action, int actor, float interval);
	function void (NextBotAction action, int actor, float interval);

	// OnEnd
	function void (NextBotAction action, int actor, NextBotAction nextAction);

	// InitialContainedAction
	function NextBotAction (NextBotAction action, int actor);

	// CreateInitialAction
	function void (NextBotAction action);

	// IsAbleToBlockMovementOf
	function bool (NextBotAction action, INextBot bot);
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

typeset InputFuncCallback
{
	// Void
	function void (int entity, int activator, int caller);

	// Integer
	function void (int entity, int activator, int caller, int value);

	// Float
	function void (int entity, int activator, int caller, float value);

	// Color32
	function void (int entity, int activator, int caller, const int value[4]);

	// Vector
	function void (int entity, int activator, int caller, const float value[3]);

	// String
	function void (int entity, int activator, int caller, const char[] value);

	// Integer/Float/Unknown
	function void (int entity, int activator, int caller, any value);
}

enum InputFuncValueType
{
	InputFuncValueType_Void = 0,
	InputFuncValueType_String,
	InputFuncValueType_Boolean,
	InputFuncValueType_Color32,
	InputFuncValueType_Float,
	InputFuncValueType_Integer,
	InputFuncValueType_Vector,
	InputFuncValueType_Max
}

typeset CEntityFactoryPostConstructor
{
	function void (int entity);
}

typeset CEntityFactoryOnRemoveCallback
{
	function void (int entity);
}

methodmap CEntityFactory < Handle
{
	/**
	 * A class that facilites the creation of entities of a specified class.
	 * Factories created this way must derive from an existing installed factory
	 * via the `DeriveFrom*()` functions.
	 *
	 * Once a CEntityFactory is installed via Install(), the game will utilize
	 * the factory to create entities under its classname, and can also be
	 * utilized by maps.
	 *
	 * @param classname         The entity's classname. This cannot match the
	 *                          classname of an already installed factory.
	 * @param postConstructor   Called when the factory finishes creating the entity.
	 *                          This is the best place to initialize your entity.
	 * @param onRemove          Called when an entity created by this factory is removed.
	 *                          Perform any needed cleanup for your entity here.
	 * @param error             Invalid handle, classname is NULL or empty, or out of
	 *                          memory.
	 */
	public native CEntityFactory(const char[] classname, CEntityFactoryPostConstructor postConstructor=INVALID_FUNCTION, CEntityFactoryOnRemoveCallback onRemove=INVALID_FUNCTION);
	
	/**
	 * Instructs the factory to use CBaseNPC (NextBot) as the base class.
	 */
	public native void DeriveFromNPC();

	/**
	 * If this is a CBaseNPC entity factory, then this will set the action
	 * factory to use to instantiate the NPC's main action.
	 *
	 * This will override the action factory set by any base factories.
	 *
	 * @param factory      The action factory used to initialize the main action
	 * @error              Invalid handle.
	 */
	public native void SetInitialActionFactory(NextBotActionFactory factory);

	/**
	 * Instructs the factory to use CBaseEntity as the base class.
	 *
	 * @param bServerOnly       If true, entity will be server side only and not
	 *                          networked to clients.
	 * @error                   Invalid handle.
	 */
	public native void DeriveFromBaseEntity(bool bServerOnly=false);

	/**
	 * Instructs the factory to use an existing installed factory as the base factory.
	 *
	 * You can specify a base factory that was created via CEntityFactory(), even
	 * if the base factory was created by another plugin. However, if the base 
	 * factory is uninstalled, this factory will be uninstalled first.
	 *
	 * @param classname     The classname of the installed factory.
	 * @error               Invalid handle.
	 */
	public native void DeriveFromClass(const char[] classname);

	/**
	 * Instructs the factory to use the specified factory as a base.
	 *
	 * @param baseFactory     The base factory
	 * @error                 Invalid handle.
	 */
	public native void DeriveFromFactory(CEntityFactory baseFactory);

	/**
	 * Instructs the factory to initialize the entity using the given
	 * entity size and constructor function signature/address. Only use this
	 * if you are absolutely sure of what you're doing! Most will not!
	 * 
	 * The constructor function must be parameterless.
	 * 
	 * @param entitySize      The size of the entity, in bytes
	 * @param config          Gamedata to read signature from
	 * @param source          Offsets, Signatures, or Addresses
	 * @param name            Name of entry in gamedata.
	 * @error                 Invalid handle or function not found.
	 */
	public native void DeriveFromConf(int entitySize, GameData config, SDKFuncConfSource source, const char[] name);

	/**
	 * Installs the factory. This marks the factory available for use.
	 * For non-abstract factories, the factory will be added to the global
	 * entity factory dictionary.
	 *
	 * @error   Invalid handle, An installed factory already has classname, or
	 *          base factory/class doesn't exist.
	 */
	public native void Install();

	/**
	 * Uninstalls the factory. If any CEntityFactory's are using this factory
	 * as a base, then those factories will be uninstalled first.
	 *
	 * Upon uninstalling a factory, all entities that were directly created
	 * by this factory (not including derived factories) will be removed.
	 *
	 * @error   Invalid handle.
	 */
	public native void Uninstall();

	/**
	 * Tells if the factory is installed or not.
	 *
	 * @return     Installed
	 * @error      Invalid handle.
	 */
	property bool IsInstalled
	{
		public native get();
	}

	/**
	 * Tells if the factory is abstract or not.
	 * 
	 * An abstract factory is never added to the entity factory dictionary,
	 * and is used only as a base for other factories. This is synonymous with
	 * abstract classes in C++.
	 *
	 * You are still required to install abstract factories in order for them
	 * to be used by other factories.
	 *
	 * To use an abstract factory as a base, you need to use DeriveFromFactory().
	 */
	property bool IsAbstract
	{
		public native get();
		public native set(bool bIsAbstract);
	}

	/**
	 * The entity classname the factory was created with.
	 *
	 * @param sBuffer        Buffer
	 * @param iBufferLen     Size of buffer
	 * @error                Invalid handle.
	 */
	public native void GetClassname(char[] sBuffer, int iBufferLen);

	/**
	 * The CEntityFactory that created the entity.
	 *
	 * @param entity     Entity index/reference
	 * @return           CEntityFactory, or null if not created by a CEntityFactory.
	 * @error            Invalid entity.
	 */
	public static native CEntityFactory GetFactoryOfEntity(int entity);

	/**
	 * Number of installed CEntityFactory's.
	 *
	 * @return     Number of installed CEntityFactory's
	 */
	public static native int GetNumInstalledFactories();

	/**
	 * Gets list of installed CEntityFactory's.
	 *
	 * @param array         Buffer
	 * @param arraySize     Buffer size
	 * @return              Number of installed CEntityFactory's
	 */
	public static native int GetInstalledFactories(CEntityFactory[] array, int arraySize);

	// Datamap

	/**
	 * Begin defining the datamap structure. The factory uses this datamap to
	 * create and map out custom, user-defined properties on the entity. These 
	 * properties can be accessed using Get/SetEntProp. Use EndDataMapDesc()
	 * to finish defining the datamap.
	 *
	 * If deriving from another CEntityFactory (whether by factory or classname), 
	 * you must specify and install the base factory before calling 
	 * BeginDataMapDesc() on this factory, as offsets are calculated based on 
	 * the entity size of the base factory. You also must not change the base
	 * factory's datamap during/after defining this factory's datamap, as this
	 * will lead to undefined behavior.
	 *
	 * About key fields: If sKeyName is given, this will declare the property
	 * as a key field, which can be accessed by the map and the DispatchKeyValue
	 * function using sKeyName. Key fields cannot be declared as an array,
	 * meaning iNumElements must be set to 1.
	 *
	 * @param sDataClass      Name of the datamap. If null or empty, will use
	 *                        the entity's classname instead.
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle, or beginning datamap definition
	 *                        before base factory is installed
	 */
	public native CEntityFactory BeginDataMapDesc(const char[] sDataClass=NULL_STRING);

	/**
	 * Defines an integer field in the datamap.
	 *
	 * @param sFieldName      Name of the field
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @param sKeyName        Key field name. See BeginDataMapDesc's documentation
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle, or tried to declare key field as
	 *                        array.
	 */
	public native CEntityFactory DefineIntField(const char[] sFieldName, int iNumElements=1, const char[] sKeyName=NULL_STRING);

	/**
	 * Defines a float field in the datamap.
	 *
	 * @param sFieldName      Name of the field
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @param sKeyName        Key field name. See BeginDataMapDesc's documentation
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle, or tried to declare key field as
	 *                        array.
	 */
	public native CEntityFactory DefineFloatField(const char[] sFieldName, int iNumElements=1, const char[] sKeyName=NULL_STRING);

	/**
	 * Defines a character field in the datamap.
	 *
	 * @param sFieldName      Name of the field
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @param sKeyName        Key field name. See BeginDataMapDesc's documentation
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle, or tried to declare key field as
	 *                        array.
	 */
	public native CEntityFactory DefineCharField(const char[] sFieldName, int iNumElements=1, const char[] sKeyName=NULL_STRING);

	/**
	 * Defines a boolean field in the datamap.
	 *
	 * @param sFieldName      Name of the field
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @param sKeyName        Key field name. See BeginDataMapDesc's documentation
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle, or tried to declare key field as
	 *                        array.
	 */
	public native CEntityFactory DefineBoolField(const char[] sFieldName, int iNumElements=1, const char[] sKeyName=NULL_STRING);

	/**
	 * Defines a Vector field in the datamap.
	 *
	 * @param sFieldName      Name of the field
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @param sKeyName        Key field name. See BeginDataMapDesc's documentation
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle, or tried to declare key field as
	 *                        array.
	 */
	public native CEntityFactory DefineVectorField(const char[] sFieldName, int iNumElements=1, const char[] sKeyName=NULL_STRING);

	/**
	 * Defines a string field in the datamap.
	 *
	 * @param sFieldName      Name of the field
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @param sKeyName        Key field name. See BeginDataMapDesc's documentation
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle, or tried to declare key field as
	 *                        array.
	 */
	public native CEntityFactory DefineStringField(const char[] sFieldName, int iNumElements=1, const char[] sKeyName=NULL_STRING);

	/**
	 * Defines an entity field in the datamap.
	 *
	 * @param sFieldName      Name of the field
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle.
	 */
	public native CEntityFactory DefineEntityField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines an input function. Inputs can be invoked using AcceptEntityInput,
	 * ent_fire, or from outputs.
	 *
	 * @param sInputName      Name of the input
	 * @param valueType       Type of value that the input function callback
	 *                        should handle. The engine will attempt to convert
	 *                        the given value to this type and will pass it to
	 *                        the callback.
	 * @param handler         Callback to handle the input.
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle.
	 */
	public native CEntityFactory DefineInputFunc(const char[] sInputName, InputFuncValueType valueType, InputFuncCallback handler);

	/**
	 * Defines an output. Outputs can be fired using FireEntityOutput.
	 *
	 * @param sOutputName     Name of the output
	 * @return                The CEntityFactory to chain calls
	 * @error                 Invalid handle.
	 */
	public native CEntityFactory DefineOutput(const char[] sOutputName);

	/**
	 * Finishes the datamap definition.
	 *
	 * @error                 Invalid handle.
	 */
	public native void EndDataMapDesc();
}

/**
 * Called after CEntityFactory was installed.
 *
 * @param classname     The classname of the factory
 * @param factory       Factory that was installed
 */
forward void CEntityFactory_OnInstalled(const char[] classname, CEntityFactory factory);

/**
 * Called after CEntityFactory was uninstalled.
 *
 * @param classname     The classname of the factory
 * @param factory       Factory that was uninstalled
 */
forward void CEntityFactory_OnUninstalled(const char[] classname, CEntityFactory factory);

#if !defined _navmesh_included
public float Path_Cost(INextBot bot, CNavArea area, CNavArea fromArea, CNavLadder ladder, int iElevator, float length)
{
	static int iTime = 0;
	if (iTime < GetTime())
	{
		iTime = GetTime();
	}
	
	float dist;
	if ( ladder != NULL_LADDER_AREA )
	{
		dist = ladder.length;
	}
	else if ( length > 0.0 )
	{
		dist = length;
	}
	else
	{
		float vecAreaCenter[3], vecFromAreaCenter[3];
		area.GetCenter(vecAreaCenter);
		fromArea.GetCenter(vecFromAreaCenter);
		dist = GetVectorDistance(vecAreaCenter, vecFromAreaCenter);
	}

	float cost = dist + fromArea.GetCostSoFar();
	
	// if this is a "crouch" area, add penalty
	int attributes = area.GetAttributes();
	if ( (attributes & NAV_MESH_CROUCH) )
	{
		const float crouchPenalty = 20.0;
		cost += crouchPenalty * dist;
	}

	// if this is a "jump" area, add penalty
	if ( (attributes & NAV_MESH_JUMP) )
	{
		const float jumpPenalty = 5.0;
		cost += jumpPenalty * dist;
	}
	return cost;
}
#endif

public bool Path_FilterIgnoreActors(int entity, int contentsMask, int desiredcollisiongroup)
{
	if ((0 < entity <= MaxClients) || (view_as<Address>(CBaseNPC_GetNextBotOfEntity(entity)) != Address_Null))
	{
		return false;
	}
	return true;
}

public bool Path_FilterOnlyActors(int entity, int contentsMask, int desiredcollisiongroup)
{
	return ((0 < entity <= MaxClients) || TheNPCs.FindNPCByEntIndex(entity) == INVALID_NPC);
}

forward Action CBaseEntity_SetLocalAngles(int iEntity, float vecNewAngles[3]);
forward Action CBaseCombatCharacter_EventKilled(int iEntity, int &iAttacker, int &iInflictor, float &flDamage, int &iDamagetype, int &iWeapon, float vecDamageForce[3], float vecDamagePosition[3], int iDamagecustom);

public Extension __ext_cbasenpc = 
{
	name = "cbasenpc",
	file = "cbasenpc.ext",
	autoload = 1,
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#endif
