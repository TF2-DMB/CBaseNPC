#if !defined _CBASENPC_EXTENSION_INC_
#define _CBASENPC_EXTENSION_INC_

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

native INextBot CBaseNPC_GetNextBotOfEntity(int entity);

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

methodmap SurroundingAreasCollector
{
	public native CNavArea Get(int index);
	public native int Count();
};

methodmap CNavMesh
{
	public native SurroundingAreasCollector CollectSurroundingAreas(CNavArea startArea, float travelDistanceLimit = 1500.0, float maxStepUpLimit = 18.0, float maxDropDownLimit = 100.0);
	public native CNavArea GetNearestNavArea(const float pos[3], bool anyZ = false, float maxDist = 10000.0, bool checkLOS = false, bool checkGround = true, int team = -2);
};

CNavMesh TheNavMesh;

methodmap CNavArea
{
	public native float GetCostSoFar();
	public native int GetAttributes();
	public native void GetCenter(float vecCenter[3]);
};

methodmap CNavLadder
{
	property float length
	{
		public native get();
	}
};

methodmap CBaseCombatCharacter
{
	public CBaseCombatCharacter(int iEntity)
	{
		return view_as<CBaseCombatCharacter>(iEntity);
	}
	public native CNavArea GetLastKnownArea();
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
	//public native IIntention GetIntentionInterface();
	//public native IVision GetVisionInterface();

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

methodmap Segment
{
};

methodmap CursorData
{
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
	public native void ComputeToPos(INextBot bot, float vecPos[3], float maxPathLength = 0.0, bool includeGoalIfPathFails = true);
	public native void ComputeToTarget(INextBot bot, int entity, float maxPathLength = 0.0, bool includeGoalIfPathFails = true);
	public native void Destroy();
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

methodmap CBaseNPC __nullable__
{
	public native CBaseNPC();
	public native int GetEntity();
	public native void Teleport(float origin[3]=NULL_VECTOR, float angles[3]=NULL_VECTOR, float velocity[3]=NULL_VECTOR);
	public native void Spawn();
	public native void SetModel(const char[] model);
	public native void SetCollisionBounds(float vecMins[3], float vecMaxs[3]);
	public native void GetVectors(float vecForward[3], float vecRight[3], float vecUp[3]);
	
	//Interfaces
	public native INextBot GetBot();
	public native NextBotGroundLocomotion GetLocomotion();
	public native IBody GetBody();
	
	public native void SetType(char[] sBuffer);
	public native void GetType(char[] sBuffer, int length);
	
	public void SetThinkFunction(SDKHookCB callback)
	{
		SDKHook(this.GetEntity(), SDKHook_Think, callback);
	}
	public void SetOnTakeDamageFunction(SDKHookCB callback)
	{
		SDKHook(this.GetEntity(), SDKHook_OnTakeDamage, callback);
	}
	
	public native int EquipItem(const char[] attachment, const char[] model, const char[] anim = "", int skin = 0);
	
	//Locomotion Functions
	public native void Approach(const float vecPos[3]);
	public native void FaceTowards(const float vecPos[3]);
	public native void Walk();
	public native void Run();
	public native void Stop();
	public native void Jump();
	public native bool IsOnGround();
	public native bool IsClimbingOrJumping();
	public native void SetVelocity(float vec[3]);
	public native void GetVelocity(float vec[3]);
	public native void ResetPath();
	
	property int Index
	{
		public get() { return view_as<int>(this); }
	}
	property int iMaxHealth
	{
		public native get();
		public native set(int value);
	}
	property int iHealth
	{
		public native get();
		public native set(int value);
	}
	property int iTeamNum
	{
		public get() { return GetEntProp(this.GetEntity(), Prop_Data, "m_iTeamNum"); }
		public set(int iTeam) { SetEntProp(this.GetEntity(), Prop_Data, "m_iTeamNum", iTeam); }
	}
	property int nSkin
	{
		public get() { return GetEntProp(this.GetEntity(), Prop_Data, "m_nSkin"); }
		public set(int skin) { SetEntProp(this.GetEntity(), Prop_Data, "m_nSkin", skin); }
	}
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
	property int BloodColor
	{
		public get() { return GetEntProp(this.GetEntity(), Prop_Data, "m_bloodColor"); }
		public set(int iBloodColor) { SetEntProp(this.GetEntity(), Prop_Data, "m_bloodColor", iBloodColor); }
	}
}

methodmap CNPCs __nullable__
{
	public native any FindNPCByEntIndex(int ent);
	public native bool IsValidNPC(any npc);
	//To-Do: add npcs relationships?
}

#if !defined CBASENPC_CORE
CNPCs TheNPCs;
#endif

// Gesture code

#define ANIM_LAYER_ACTIVE		0x0001
#define ANIM_LAYER_AUTOKILL		0x0002
#define ANIM_LAYER_KILLME		0x0004
#define ANIM_LAYER_DONTRESTORE	0x0008
#define ANIM_LAYER_CHECKACCESS	0x0010
#define ANIM_LAYER_DYING		0x0020

#define MAX_OVERLAYS			15

//If we are lucky these will never change
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

methodmap CAnimationLayer __nullable__
{
	property Address Address 
	{
		public get() 
		{
			return view_as<Address>(this);
		}
	}

	public any Get(int offset)
	{
		return LoadFromAddress(this.Address + view_as<Address>(offset), NumberType_Int32);
	}
	
	public void Set(int offset, any value)
	{
		StoreToAddress(this.Address + view_as<Address>(offset), value, NumberType_Int32);
	}
	
	public bool IsActive()   { return ((this.Get(m_fFlags) & ANIM_LAYER_ACTIVE)   != 0); }
	public bool IsAutokill() { return ((this.Get(m_fFlags) & ANIM_LAYER_AUTOKILL) != 0); }
	public bool IsKillMe()   { return ((this.Get(m_fFlags) & ANIM_LAYER_KILLME)   != 0); }
	public bool IsDying()    { return ((this.Get(m_fFlags) & ANIM_LAYER_DYING)    != 0); }
	public bool IsAutoramp() { return ((this.Get(m_flBlendIn) != 0.0) || (this.Get(m_flBlendOut) != 0.0)); }
	public void KillMe()     { this.Set(m_fFlags, this.Get(m_fFlags|ANIM_LAYER_KILLME)); }
//	public void Dying()      { this.Get(m_fFlags) |= ANIM_LAYER_DYING; }
//	public void Dead()       { this.Get(m_fFlags) &= ~ANIM_LAYER_DYING; }
}

methodmap CBaseAnimating __nullable__
{
	public CBaseAnimating(int entity) 
	{
		return view_as<CBaseAnimating>(entity);
	}
	
	property int iEnt
	{
		public get() 
		{
			return view_as<int>(this);
		}
	}
	
	public native int LookupSequence(const char[] sName);
	public native void ResetSequence(int sequence);
	public void PlayAnimation(const char[] sName)
	{
		int iSequence = this.LookupSequence(sName);
		if (iSequence != -1)
		{
			this.ResetSequence(iSequence);
			this.ResetSequence(iSequence);
		}
	}
	public native Address GetModelPtr();
	public native int LookupPoseParameter(Address pStudioHdr, const char[] sParamName);
	public native void SetPoseParameter(Address pStudioHdr, int iPoseParam, float flNewValue);
}

methodmap CBaseAnimatingOverlay < CBaseAnimating
{
	public CBaseAnimatingOverlay(int entity) 
	{
		return view_as<CBaseAnimatingOverlay>(entity);
	}
	
	/*to-do get an access to m_Size member on the CUltVector*/
	property Address m_AnimOverlay
	{
		public get() 
		{
			static int m_AnimOverlay = 0;
			if (m_AnimOverlay == 0) m_AnimOverlay = FindDataMapInfo(this.iEnt, "m_AnimOverlay");
			
			int iOffset = (view_as<int>(GetEntityAddress(this.iEnt)) + m_AnimOverlay) + view_as<int>(0x10);
			Address Overlay = view_as<Address>(LoadFromAddress(view_as<Address>(iOffset), NumberType_Int32));
		
			return Overlay;
		}
	}
	
	public CAnimationLayer GetLayer(int iLayer)
	{
		return view_as<CAnimationLayer>(this.m_AnimOverlay + view_as<Address>(iLayer * CAnimationLayer_Size));
	}
	
	public bool IsValidLayer( int iLayer )
	{
		return (iLayer >= 0 && iLayer < MAX_OVERLAYS && this.GetLayer(iLayer).IsActive());
	}
	
	public void RemoveLayer(int iLayer, float flKillRate = 0.2, float flKillDelay = 0.0)
	{
		if (!this.IsValidLayer( iLayer ))
			return;

		CAnimationLayer layer = this.GetLayer(iLayer);
		
		if (flKillRate > 0.0)
		{
			layer.Set(m_flKillRate, layer.Get(m_flWeight)/flKillRate);
		}
		else
		{
			layer.Set(m_flKillRate, 100.0);
		}

		layer.Set(m_flKillDelay,flKillDelay);

		layer.KillMe();
	}
	
	public void RemoveAllGestures()
	{
		for (int i = 0; i < MAX_OVERLAYS; i++)
		{
			this.RemoveLayer(i);
		}
	}
	
	public int FindGestureLayer(int activity)
	{
		for (int i = 0; i < MAX_OVERLAYS; i++)
		{
			CAnimationLayer layer = this.GetLayer(i);
			
			if(!(layer.IsActive()))
				continue;				
			
			if(layer.IsKillMe())
				continue;
			
			if(layer.Get(m_nActivity) == activity || layer.Get(m_nSequence) == activity)
				return i;
		}
		return -1;
	}
	
	public bool IsPlayingGesture(int activity)
	{
		return this.FindGestureLayer(activity) != -1 ? true : false;
	}
	
	public native void AddGestureSequence(int sequence, bool bAutokill = true);
}

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
	return ((0 < entity <= MaxClients) || (view_as<Address>(CBaseNPC_GetNextBotOfEntity(entity)) != Address_Null));
}

forward Action CBaseEntity_SetLocalAngles(int iEntity, float vecNewAngles[3]);
forward Action NextBotGroundLocomotion_UpdatePosition(NextBotGroundLocomotion mover, float vecFromPos[3], float vecToPos[3], float vecAdjustedPos[3], float vecEditedPos[3]);

public SharedPlugin __pl_cbasenpc = 
{
	name = "cbasenpc",
	file = "cbasenpc.smx",
#if defined REQUIRE_PLUGIN
	required = 1,
#else
	required = 0,
#endif
};

public Extension __ext_cbasenpcextension = 
{
	name = "Nextbot Pathing",
	file = "nextbot_pathing.ext",
	autoload = 1,
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#endif