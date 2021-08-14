#if !defined _CBASENPC_EXTENSION_INC_
#define _CBASENPC_EXTENSION_INC_

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

CNavMesh TheNavMesh;

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

methodmap INextBot < INextBotEventResponder
{
	public native void Reset();
	public native void Update();
	public native void Upkeep();

	public native bool IsRemovedOnReset();

	public native int GetEntity();
	public native int GetNextBotCombatCharacter();

	public native ILocomotion GetLocomotionInterface();
	public native IBody GetBodyInterface();
	public native IIntention GetIntentionInterface();
	public native IVision GetVisionInterface();

	public native bool SetPosition(const float pos[3]);
	public native void GetPosition(float pos[3]);

	public native bool IsEnemy(int entity);
	public native bool IsFriend(int entity);
	public native bool IsSelf(int entity);

	public native bool IsAbleToClimbOnto(int entity);
	public native bool IsAbleToBreak(int entity);
	public native bool IsAbleToBlockMovementOf(INextBot bot);

	public native bool ShouldTouch(int entity);

	public native void ReactToSurvivorVisibility();
	public native void ReactToSurvivorNoise();
	public native void ReactToSurvivorContact();

	public native bool IsImmobile();
	public native float GetImmobileDuration();
	public native void ClearImmobileStatus();
	public native float GetImmobileSpeedThreshold();

	public native PathFollower GetCurrentPath();
	public native void SetCurrentPath(const PathFollower path);
	public native void NotifyPathDestruction(const PathFollower path);

	public native bool IsRangeLessThan(int entity, float dist);
	public native bool IsRangeLessThanEx(const float pos[3], float dist);
	public native bool IsRangeGreaterThan(int entity, float dist);
	public native bool IsRangeGreaterThanEx(const float pos[3], float dist);

	public native float GetRangeTo(int entity);
	public native float GetRangeToEx(const float pos[3]);
	public native float GetRangeSquaredTo(int entity);
	public native float GetRangeSquaredToEx(const float pos[3]);

	public native bool IsDebugging(NextBotDebugType type);
	public native void GetDebugIdentifier(char[] name, int maxsize);
	public native bool IsDebugFilterMatch(const char[] name);
	public native void DisplayDebugText(const char[] name);
};

methodmap INextBotComponent < INextBotEventResponder
{
	public native void Reset();
	public native void Update();
	public native void Upkeep();

	public native INextBot GetBot();
};

enum FieldOfViewCheckType
{
	USE_FOV,
	DISREGARD_FOV
};

methodmap CKnownEntity
{
	public native void Destroy();
	public native void UpdatePosition();
	public native int GetEntity();
	public native void GetLastKnownPosition(float vecPos[3]);
	public native bool HasLastKnownPositionBeenSeen();
	public native void MarkLastKnownPositionAsSeen();
	public native CNavArea GetLastKnownArea();
	public native float GetTimeSinceLastKnown();
	public native float GetTimeSinceBecameKnown();
	public native void UpdateVisibilityStatus(bool visible);
	public native bool IsVisibleInFOVNow();
	public native bool IsVisibleRecently();
	public native float GetTimeSinceBecameVisible();
	public native float GetTimeWhenBecameVisible();
	public native float GetTimeSinceLastSeen();
	public native bool WasEverVisible();
	public native bool IsObsolete();
	public native bool Is(int iEntity);
}

methodmap IVision < INextBotComponent
{
	public native CKnownEntity GetPrimaryKnownThreat(bool bVisible);
	public native float GetTimeSinceVisible(int iTeam);
	public native CKnownEntity GetClosestKnown(int iTeam);
	public native int GetKnownCount(int team, bool onlyVisible = false, float rangeLimit = -1.0);
	public native CKnownEntity GetKnown(int iEntity);
	public native void AddKnownEntity(int iEntity);
	public native void ForgetEntity(int iEntity);
	public native void ForgetAllKnownEntities();
	public native float GetMaxVisionRange();
	public native float GetMinRecognizeTime();
	public native bool IsAbleToSeeTarget(int iEntity, FieldOfViewCheckType checkFOV, float vecPos[3] = NULL_VECTOR);
	public native bool IsAbleToSee(float vecPos[3], FieldOfViewCheckType checkFOV);
	public native bool IsIgnored(int iEntity);
	public native bool IsVisibleEntityNoticed(int iEntity);
	public native bool IsInFieldOfView(float vecPos[3]);
	public native bool IsInFieldOfViewTarget(int iEntity);
	public native float GetDefaultFieldOfView();
	public native float GetFieldOfView();
	public native void SetFieldOfView(float horizAngle);
	public native bool IsLineOfSightClear(float vecPos[3]);
	public native bool IsLineOfSightClearToEntity(int iActor, float vecPos[3]=NULL_VECTOR);
	public native bool IsLookingAt(float vecPos[3], float cosTolerance = 0.95);
	public native bool IsLookingAtTarget(int iActor, float cosTolerance = 0.95);
};

methodmap IIntention < INextBotComponent
{
	public native void Reset();
	public native void Update();
	public native bool ShouldPickUp( int item );
	public native bool ShouldHurry();
	public native bool ShouldRetreat();
	public native bool ShouldAttack( int them );
	public native bool IsHindrance( int blocker );
	public native void SelectTargetPoint( int subject, float buffer[3] );
	public native bool IsPositionAllowed( const float pos[3] );
	public native CKnownEntity SelectMoreDangerousThreat( int subject, CKnownEntity threat1, CKnownEntity threat2 );
}

enum TraverseWhenType 
{ 
	IMMEDIATELY,
	EVENTUALLY
};

methodmap ILocomotion < INextBotComponent
{
	public native void Approach(const float dst[3], float unknown);
	public native void DriveTo(const float dst[3]);

	public native bool ClimbUpToLedge(const float dst[3], const float dir[3], int entity);
	public native void JumpAcrossGap(const float dst[3], const float dir[3]);
	public native void Jump();
	public native bool IsClimbingOrJumping();
	public native bool IsClimbingUpToLedge();
	public native bool IsJumpingAcrossGap();
	public native bool IsScrambling();

	public native void Run();
	public native void Walk();
	public native void Stop();

	public native bool IsRunning();

	public native void SetDesiredSpeed(float speed);
	public native float GetDesiredSpeed();

	public native void SetSpeedLimit(float limit);
	public native float GetSpeedLimit();

	public native bool IsOnGround();
	public native int GetGround();
	public native void GetGroundNormal(float normal[3]);
	public native float GetGroundSpeed();
	public native void GetGroundMotionVector(float motion[3]);

	public native void ClimbLadder(CNavLadder ladder, CNavArea area);
	public native void DescendLadder(CNavLadder ladder, CNavArea area);

	public native bool IsUsingLadder();
	public native bool IsAscendingOrDescendingLadder();
	public native bool IsAbleToAutoCenterOnLadder();

	public native void FaceTowards(const float pos[3]);

	public native void SetDesiredLean(const float lean[3]);
	public native void GetDesiredLean(float lean[3]);

	public native bool IsAbleToJumpAcrossGaps();
	public native bool IsAbleToClimb();

	public native void GetFeet(float feet[3]);

	public native float GetStepHeight();
	public native float GetMaxJumpHeight();
	public native float GetDeathDropHeight();
	public native float GetRunSpeed();
	public native float GetWalkSpeed();

	public native float GetMaxAcceleration();
	public native float GetMaxDeceleration();

	public native void GetVelocity(float vel[3]);

	public native float GetSpeed();

	public native void GetMotionVector(float motion[3]);

	public native bool IsAreaTraversable(CNavArea area);
	public native float GetTraversableSlopeLimit();

	public native bool IsPotentiallyTraversable(const float from[3], const float to[3], TraverseWhenType when);
	public native bool HasPotentialGap(const float from[3], const float to[3]);
	public native bool IsGap(const float from[3], const float to[3]);
	public native bool IsEntityTraversable(int entity, TraverseWhenType when);

	public native bool IsStuck();
	public native float GetStuckDuration();
	public native void ClearStuckStatus(const char[] reason);

	public native bool IsAttemptingToMove();

	public native bool ShouldCollideWith(int entity);

	public native void AdjustPosture(const float lean[3]);
	public native void StuckMonitor();
};

methodmap NextBotGroundLocomotion < ILocomotion
{
	public native void GetAcceleration(float acel[3]);
	public native void SetAcceleration(const float acel[3]);
	public native void SetVelocity(const float vel[3]);
	public native float GetGravity();
	public native float GetFrictionForward();
	public native float GetFrictionSideways();
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

methodmap Segment
{
	public native void GetPos(float pos[3]);
};

methodmap CursorData
{
};

typeset PathComputeCallback
{
	function void (Path path, bool bSuccess, any data);
};

methodmap Path
{
	public native Path(NavPathCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);
	public native float GetLength();
	public native void GetPosition(float dist, Segment seg, float pos[3]);
	public native void GetStartPosition(float pos[3]);
	public native void GetEndPosition(float pos[3]);
	public native int GetSubject();
	public native Segment GetCurrentGoal();
	public native float GetAge();
	public native void MoveCursorToStart();
	public native void MoveCursorToEnd();
	public native void MoveCursor(float dist, MoveCursorType mctype);
	public native float GetCursorPosition();
	public native CursorData GetCursorData();
	public native bool IsValid();
	public native void Invalidate();
	public native void Draw(Segment seg);
	public native void DrawInterpolated(float from, float to);
	public native Segment FirstSegment();
	public native Segment NextSegment(Segment seg);
	public native Segment PriorSegment(Segment seg);
	public native Segment LastSegment();
	public native bool ComputeToPos(INextBot bot, float vecPos[3], float maxPathLength = 0.0, bool includeGoalIfPathFails = true);
	public native bool ComputeToTarget(INextBot bot, int entity, float maxPathLength = 0.0, bool includeGoalIfPathFails = true);
	public native void Destroy();
	
	// Threaded
	public native bool ComputeToPosT(INextBot bot, float vecPos[3], float maxPathLength = 0.0, bool includeGoalIfPathFails = true, PathComputeCallback callback, any data = 0);
	public native bool ComputeToTargetT(INextBot bot, int entity, float maxPathLength = 0.0, bool includeGoalIfPathFails = true, PathComputeCallback callback, any data = 0);
};

methodmap PathFollower < Path
{
	public native PathFollower(NavPathCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);
	public native void Update(INextBot nextbot);
	public native void SetMinLookAheadDistance(float dist);
	public native int GetHindrance();
	public native bool IsDiscontinuityAhead(INextBot nextbot, SegmentType stype, float max_dist);
	public native void Destroy();
};

enum SubjectChaseType
{
	LEAD_SUBJECT,
	DONT_LEAD_SUBJECT
};

methodmap ChasePath < PathFollower
{
	public native ChasePath(SubjectChaseType how = DONT_LEAD_SUBJECT, NavPathCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);
	public native void Update(INextBot bot, int iTarget, float vecPredictedPos[3]=NULL_VECTOR);
	public native float GetLeadRadius();
	public native float GetMaxPathLength();
	public native void PredictSubjectPosition(INextBot nextbot, int entity, float pos[3]);
	public native bool IsRepathNeeded(INextBot nextbot, int entity);
	public native float GetLifetime();
	public native void Destroy();
};

methodmap DirectChasePath < ChasePath
{
	public native DirectChasePath(SubjectChaseType how = DONT_LEAD_SUBJECT, NavPathCostFunctor costfunction = INVALID_FUNCTION, TraceFilterActors ignoreActorsFilter, TraceFilterActors onlyActorsFilter);
	public native void Destroy();
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

methodmap IBody < INextBotComponent
{
	public native bool SetPosition(float pos[3]);

	public native void GetEyePosition(float eyepos[3]);
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
	
	public bool IsActive()    { return ((this.Get(m_fFlags) & ANIM_LAYER_ACTIVE)   != 0); } 
	public bool IsAutokill()  { return ((this.Get(m_fFlags) & ANIM_LAYER_AUTOKILL) != 0); } 
	public bool IsKillMe()    { return ((this.Get(m_fFlags) & ANIM_LAYER_KILLME)   != 0); } 
	public bool IsDying()     { return ((this.Get(m_fFlags) & ANIM_LAYER_DYING)    != 0); } 
	public bool NoEvents()    { return ((this.Get(m_fFlags) & ANIM_LAYER_NOEVENTS) != 0); }
	public void KillMe()      { int iFlags = this.Get(m_fFlags); this.Set(m_fFlags, (iFlags |= ANIM_LAYER_KILLME)); } 
	public void AutoKill()    { int iFlags = this.Get(m_fFlags); this.Set(m_fFlags, (iFlags |= ANIM_LAYER_AUTOKILL)); }
	public void Dying()       { int iFlags = this.Get(m_fFlags); this.Set(m_fFlags, (iFlags |= ANIM_LAYER_DYING));  } 
	public void Dead()        { int iFlags = this.Get(m_fFlags); this.Set(m_fFlags, (iFlags &= ~ANIM_LAYER_DYING)); }
	public bool IsAlive()         { int iFlags = this.Get(m_fFlags); return (((iFlags & ANIM_LAYER_ACTIVE) != 0) || ((iFlags & ANIM_LAYER_KILLME) == 0)); }
	public int GetLayerSequence() { return (this.Get(m_nSequence)); }
};

methodmap CBaseEntity
{
	public CBaseEntity(int iEnt) 
	{
		return view_as<CBaseEntity>(iEnt);
	}

	property int iEnt
	{
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
	 * Updates the global transmit state if a transmission rule changed.
	 *
	 * @error 		Invalid entity.
	 */
	public native void DispatchUpdateTransmitState();

	public native int GetFlags();

	public native int GetEFlags();

	public native bool IsEFlagSet(int flag);

	public native void AddEFlags(int flags);

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

	public native int GetNextThinkTick(const char[] context = NULL_STRING);

	public native int GetLastThinkTick(const char[] context = NULL_STRING);

	/**
	 * Does the entity think?
	 *
	 * @return		Whether or not the entity will think.
	 * @error 		Invalid entity.
	 */
	public native bool WillThink();

	public native void GetAbsOrigin(float pos[3]);

	public native void GetAbsAngles(float ang[3]);

	public native void GetAbsVelocity(float vel[3]);

	public native void SetAbsOrigin(float pos[3]);

	public native void SetAbsAngles(float ang[3]);

	public native void SetAbsVelocity(float vel[3]);

	public native void NetworkStateChanged();

	public native void NetworkStateChangedVar(Address var);

	public native float GetSimulationTime();

	public native float SetSimulationTime(float time);

	public native void GetLocalOrigin(float pos[3]);

	public native void GetLocalAngles(float ang[3]);

	public native void SetLocalOrigin(float pos[3]);

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
		static Handle hHook = null;
		if (hHook == null)
		{
			hHook = DHookCreate(CBaseEntity.iUpdateOnRemove(), HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity);
			if (hHook == null) return;
		}
		DHookEntity(hHook, false, this.iEnt, _, callback);
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
	 * @param forward	Destination vector to store the entity's forward vector.
	 * @param right		Destination vector to store the entity's right vector.
	 * @param up		Destination vector to store the entity's up vector.
	 * @error 		Invalid entity.
	 */
	public native void GetVectors(float forward[3], float right[3], float up[3]);

	public native void EntityToWorldTransform(float matrix[3][4]);
}

methodmap CBaseAnimating < CBaseEntity
{
	public CBaseAnimating(int entity) 
	{
		return view_as<CBaseAnimating>(entity);
	}
	
	public static native int iHandleAnimEvent();
	
#if defined _dhooks_included
	public void Hook_HandleAnimEvent(DHookCallback callback)
	{
		static Handle hHook = null;
		if (hHook == null)
		{
			hHook = DHookCreate(CBaseAnimating.iHandleAnimEvent(), HookType_Entity, ReturnType_Void, ThisPointer_CBaseEntity);
			if (hHook == null) return;
			DHookAddParam(hHook, HookParamType_ObjectPtr, -1);
		}
		DHookEntity(hHook, false, this.iEnt, _, callback);
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
	public native bool GetAttachmentMatrix( int iAttachmentIndex, float matrix[3][4] );
	
	public native Address GetModelPtr();
	public native int LookupPoseParameter(const char[] sParamName);
	public native float SetPoseParameter(int iPoseParam, float flNewValue);
	public native float GetPoseParameter(int iPoseParam);
	
	#pragma deprecated Please implement this function inside your plugin
	public void PlayAnimation(const char[] sName)
	{
		int iSequence = this.LookupSequence(sName);
		if (iSequence != -1)
		{
			this.ResetSequence(iSequence);
			SetEntPropFloat(this.iEnt, Prop_Data, "m_flCycle", 0.0);
		}
	}
	#pragma deprecated Please use CBaseAnimating.LookupAttachment instead and don't add 1 to return value
	public native int FindAttachment(const char[] sAttachmentName);
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
	public native CAnimationOverlay GetAnimOverlay(int iLayer);
	public native int GetNumAnimOverlays();
	public native void SetNumAnimOverlays(int iNum);
	public native bool HasActiveLayer();
	
	public int FindGestureLayerBySequence(int iSequence)
	{
		// i = layer index
		int iCount = this.GetNumAnimOverlays();
		for(int i = 0; i < iCount; i++) 
		{
			CAnimationOverlay pOverlay = this.GetAnimOverlay(i); 
			
			// Validate layer
			if (!pOverlay.IsAlive()) 
			{
				continue; 
			}

			// Validate sequence
			if (pOverlay.GetLayerSequence() == iSequence) 
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
	 * Action should keep running.
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
	public native int ChangeTo( NextBotAction action, const char[] reason=NULL_STRING );

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
	public native int SuspendFor( NextBotAction interruptingAction, const char[] reason=NULL_STRING );

	/**
	 * Action should exit.
	 * 
	 * Note: Do not use this in event callbacks. Use TryDone() instead.
	 *
	 * @param reason        Reason for exiting
	 */
	public native int Done( const char[] reason=NULL_STRING );

	/**
	 * Request that the Action should keep running.
	 * 
	 * Note: This is used within event callbacks only.
	 * 
	 * @param priority      How important this response is
	 */
	public native int TryContinue( EventResultPriorityType priority = RESULT_TRY );

	/**
	 * Request that the Action should change to another Action.
	 *
	 * Note: This is used within event callbacks only.
	 *
	 * @param action        The new Action to change to.
	 * @param priority      How important this response is
	 * @param reason        The reason for changing to the new Action.
	 */
	public native int TryChangeTo( NextBotAction action, EventResultPriorityType priority = RESULT_TRY, const char[] reason=NULL_STRING );

	/**
	 * Request that the Action should suspend itself for another Action.
	 *
	 * Note: This is used within event callbacks only.
	 *
	 * @param interruptingAction        The interrupting Action to suspend for.
	 * @param priority                  How important this response is
	 * @param reason                    The reason for suspending for the interrupting Action.
	 */
	public native int TrySuspendFor( NextBotAction interruptingAction, EventResultPriorityType priority = RESULT_TRY, const char[] reason=NULL_STRING );

	/**
	 * Request that the Action should exit.
	 *
	 * Note: This is used within event callbacks only.
	 *
	 * @param priority      How important this response is
	 * @param reason        The reason for exiting
	 */
	public native int TryDone( EventResultPriorityType priority = RESULT_TRY, const char[] reason=NULL_STRING );

	/**
	 * Request that the Action should keep running. This is the same as
	 * TryContinue() except with an added reason parameter.
	 *
	 * Note: This is used within event callbacks only.
	 *
	 * @param priority      How important this response is
	 * @param reason        The reason for continuing
	 */
	public native int TryToSustain( EventResultPriorityType priority = RESULT_TRY, const char[] reason=NULL_STRING );
}

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
	function int ( NextBotAction action, int actor, int ground );
	
	// OnContact
	// result is a CGameTrace structure
	function int ( NextBotAction action, int actor, int other, Address result );

	// OnMoveToSuccess
	function int ( NextBotAction action, int actor, Path path );

	// OnMoveToFailure
	function int ( NextBotAction action, int actor, Path path, MoveToFailureType reason );

	// OnStuck
	// OnUnStuck
	// OnPostureChanged
	// OnIgnite
	// OnModelChanged
	// OnCommandResume
	// OnWin
	// OnLose
	function int ( NextBotAction action, int actor );

	// OnAnimationActivityComplete
	// OnAnimationActivityInterrupted
	// OnCommandAttack (victim = activity)
	// OnShoved (pusher = activity)
	// OnBlinded (blinder = activity)
	// OnTerritoryContested (territoryID = activity)
	// OnTerritoryCaptured (territoryID = activity)
	// OnTerritoryLost (territoryID = activity)
	function int ( NextBotAction action, int actor, int activity );

	// OnAnimationEvent
	function int ( NextBotAction action, int actor, int event );
	function int ( NextBotAction action, int actor, int event, const char[] options );
	function int ( NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime );
	function int ( NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventtype );
	function int ( NextBotAction action, int actor, int event, const char[] options, float cycle, float eventtime, int eventSourceEntity );

	// OnInjured
	// OnKilled
	function int ( NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype );
	function int ( NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3] );
	function int ( NextBotAction action, int actor, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom );

	// OnOtherKilled
	function int ( NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype );
	function int ( NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3] );
	function int ( NextBotAction action, int actor, int victim, int attacker, int inflictor, float damage, int damagetype, int weapon, const float damageForce[3], const float damagePosition[3], int damagecustom );

	// OnSound
	function int ( NextBotAction action, int actor, int sourceEntity, const float pos[3], KeyValues keys );

	// OnSpokeConcept
	// response is AI_Response*
	function int ( NextBotAction action, int actor, int who, const char[] concept, Address response );

	// OnWeaponFired
	// OnPickUp (item = whoFired, giver = weapon)
	// OnActorEmoted (emoter = whoFired, emote = weapon)
	function int ( NextBotAction action, int actor, int whoFired, int weapon );

	// OnNavAreaChanged
	function int ( NextBotAction action, int actor, CNavArea newArea, CNavArea oldArea );

	// OnCommandApproach
	function int ( NextBotAction action, int actor, const float pos[3], float range );

	// OnCommandRetreat
	function int ( NextBotAction action, int actor, int threat, float range );

	// OnCommandPause
	function int ( NextBotAction action, int actor, float duration );

	// OnCommandString
	function int ( NextBotAction action, int actor, const char[] command );
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
	// OnStart
	// OnSuspend (interruptingAction = priorAction)
	// OnResume (interruptingAction = priorAction)
	function int ( NextBotAction action, int actor, NextBotAction priorAction );

	// Update
	function int ( NextBotAction action, int actor, float interval );

	// OnEnd
	function void ( NextBotAction action, int actor, NextBotAction nextAction );

	// InitialContainedAction
	function NextBotAction ( NextBotAction action, int actor );

	// CreateInitialAction
	function void ( NextBotAction action );

	// IsAbleToBlockMovementOf
	function bool ( NextBotAction action, INextBot bot );
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
	 */
	public native void SetCallback(NextBotActionCallbackType callbackType, NextBotActionCallback callback);

	/**
	 * Defines a contextual query callback for the Action.
	 *
	 * @param callbackType     Type of callback
	 * @param callback
	 */
	public native void SetQueryCallback(ContextualQueryType callbackType, ActionContextualQueryCallback callback);

	/**
	 * Defines an event callback for the Action.
	 *
	 * @param callbackType     Type of callback
	 * @param callback
	 */
	public native void SetEventCallback(EventResponderType callbackType, ActionEventResponderCallback callback);

	/**
	 * Begins defining properties stored on the Action.
	 *
	 * @return     The NextBotActionFactory to chain calls
	*/
	public native NextBotActionFactory BeginDataMapDesc();

	/**
	 * Defines an integer property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 */
	public native NextBotActionFactory DefineIntField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a float property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 */
	public native NextBotActionFactory DefineFloatField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a character property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 */
	public native NextBotActionFactory DefineCharField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a boolean property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 */
	public native NextBotActionFactory DefineBoolField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a Vector property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 */
	public native NextBotActionFactory DefineVectorField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines a string property on the Action.
	 *
	 * @param sFieldName     Property name
	 * @param iNumElements   Number of elements. If > 1, will declare the field
	 *                       as an array.
	 * @return               The NextBotActionFactory to chain calls
	 */
	public native NextBotActionFactory DefineStringField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Finishes the datamap definition.
	 */
	public native void EndDataMapDesc();

	/**
	 * Creates an Action.
	 *
	 * @return     The new Action
	*/
	public native NextBotAction Create();
}

typeset CEntityFactoryPostConstructor
{
	function void (int entity);
}

typeset CEntityFactoryOnRemoveCallback
{
	function void (int entity);
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
	 * @param error             Classname is NULL or empty, or out of memory.
	 */
	public native CEntityFactory(const char[] classname, CEntityFactoryPostConstructor postConstructor=INVALID_FUNCTION, CEntityFactoryOnRemoveCallback onRemove=INVALID_FUNCTION);
	
	/**
	 * Instructs the factory to use CBaseNPC (NextBot) as the base class.
	 */
	public native void DeriveFromNPC();

	public native void SetInitialActionFactory( NextBotActionFactory factory );

	/**
	 * Instructs the factory to use CBaseEntity as the base class.
	 *
	 * @param bServerOnly       If true, entity will be server side only and not
	 *                          networked to clients.
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
	 */
	public native void DeriveFromClass(const char[] classname);

	/**
	 * Instructs the factory to use the specified factory as a base.
	 *
	 * @param baseFactory     The base factory
	 */
	public native void DeriveFromFactory(CEntityFactory baseFactory);

	/**
	 * Installs the factory. This marks the factory available for use.
	 * For non-abstract factories, the factory will be added to the global
	 * entity factory dictionary.
	 *
	 * @error	An installed factory already has classname, or base factory/class
	 *          doesn't exist.
	 */
	public native void Install();

	/**
	 * Uninstalls the factory. If any CEntityFactory's are using this factory
	 * as a base, then those factories will be uninstalled first.
	 *
	 * Upon uninstalling a factory, all entities that were directly created
	 * by this factory (not including derived factories) will be removed.
	 */
	public native void Uninstall();

	/**
	 * Tells if the factory is installed or not.
	 *
	 * @return     Installed
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
	 */
	public native void GetClassname(char[] sBuffer, int iBufferLen);

	/**
	 * The CEntityFactory that created the entity.
	 *
	 * @param entity     Entity index/reference
	 * @return           CEntityFactory, or null if not created by a CEntityFactory.
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
	 *                        entity's classname.
	 * @return                The CEntityFactory to chain calls
	 * @error                 Beginning datamap definition before base factory 
	 *                        is installed
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
	 */
	public native CEntityFactory DefineStringField(const char[] sFieldName, int iNumElements=1, const char[] sKeyName=NULL_STRING);

	/**
	 * Defines an entity field in the datamap.
	 *
	 * @param sFieldName      Name of the field
	 * @param iNumElements    Number of elements. If > 1, will declare the field
	 *                        as an array.
	 * @return                The CEntityFactory to chain calls
	 */
	public native CEntityFactory DefineEntityField(const char[] sFieldName, int iNumElements=1);

	/**
	 * Defines an input function. Inputs can be invoked using AcceptEntityInput,
	 * ent_fire, or from outputs.
	 *
	 * @param sInputName      Name of the input
	 * @param valueType       Type of value that the input function callback
	 *                        should handle.
	 * @param handler         Callback to handle the input.
	 * @return                The CEntityFactory to chain calls
	 */
	public native CEntityFactory DefineInputFunc(const char[] sInputName, InputFuncValueType valueType, InputFuncCallback handler);

	/**
	 * Defines an output. Outputs can be fired using FireEntityOutput.
	 *
	 * @param sOutputName     Name of the output
	 * @return                The CEntityFactory to chain calls
	 */
	public native CEntityFactory DefineOutput(const char[] sOutputName);

	/**
	 * Finishes the datamap definition.
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
