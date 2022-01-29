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

#define TEAM_ANY -2

const CBaseNPC INVALID_NPC = view_as<CBaseNPC>(-1);
const CNavArea NULL_AREA = view_as<CNavArea>(0);
const CNavLadder NULL_LADDER_AREA = view_as<CNavLadder>(0);
const HidingSpot NULL_HIDING_SPOT = view_as<HidingSpot>(0);
const CKnownEntity NULL_KNOWN_ENTITY = view_as<CKnownEntity>(0);
const NextBotAction NULL_ACTION = view_as<NextBotAction>(0);
const INextBot NULL_NEXT_BOT = view_as<INextBot>(0);
const Segment NULL_PATH_SEGMENT = view_as<Segment>(0);

#include <cbasenpc/nav>
#include <cbasenpc/entityfactory>
#include <cbasenpc/baseentity>
#include <cbasenpc/baseanimating>
#include <cbasenpc/baseanimatingoverlay>
#include <cbasenpc/basecombatcharacter>
#include <cbasenpc/nextbot>

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
	public native CBaseNPC_Locomotion GetLocomotion();
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
};

native INextBot CBaseNPC_GetNextBotOfEntity(int iEntity);
native void CBaseNPC_HookEventKilled(int iEntity);

enum LocomotionCallbackType
{
	LocomotionCallback_IsAbleToJumpAcrossGaps = 0,
	LocomotionCallback_IsJumpingAcrossGap,
	LocomotionCallback_JumpAcrossGap,
	LocomotionCallback_IsAbleToClimb,
	LocomotionCallback_IsClimbingUpToLedge,
	LocomotionCallback_ClimbUpToLedge,
	LocomotionCallback_ShouldCollideWith,
	LocomotionCallback_IsEntityTraversable,
	LocomotionCallback_Max
};

typeset LocomotionCallback
{
	// IsAbleToJumpAcrossGaps
	// IsJumpingAcrossGap
	// IsAbleToClimb
	// IsClimbingUpToLedge
	function bool (CBaseNPC_Locomotion loco);

	// JumpAcrossGap
	function void (CBaseNPC_Locomotion loco, const float landingGoal[3], const float landingForward[3]);

	// ClimbUpToLedge
	function bool (CBaseNPC_Locomotion loco, const float vecGoal[3], const float vecForward[3], int entity);

	// ShouldCollideWith
	function bool (CBaseNPC_Locomotion loco, int other);

	// IsEntityTraversable
	function bool (CBaseNPC_Locomotion loco, int other, TraverseWhenType when);
};

methodmap CBaseNPC_Locomotion < NextBotGroundLocomotion
{
	/**
	 * Sets a callback function on the locomotion.
	 * 
	 * @param callbackType     Type of callback
	 * @param callback         Callback function
	 * @error                  Locomotion is invalid
	 */
	public native void SetCallback(LocomotionCallbackType callbackType, LocomotionCallback callback);

	/**
	 * Calls the base function of the current callback. This can only be used
	 * within a callback function.
	 * 
	 * @param any     Function arguments
	 * @return        Function return
	 * @error         Locomotion is invalid or used outside of a callback function
	 */
	public native any CallBaseFunction(any ...);
};

enum CNPCs
{
	TheNPCs
};

methodmap CNPCs __nullable__
{
	public native any FindNPCByEntIndex(int ent);
	public native bool IsValidNPC(any npc);
	//To-Do: add npcs relationships?
};

forward Action CBaseEntity_SetLocalAngles(int iEntity, float vecNewAngles[3]);
forward Action CBaseCombatCharacter_EventKilled(int iEntity, int &iAttacker, int &iInflictor, float &flDamage, int &iDamagetype, int &iWeapon, float vecDamageForce[3], float vecDamagePosition[3], int iDamagecustom);

public float Path_Cost(INextBot bot, CNavArea area, CNavArea fromArea, CNavLadder ladder, int iElevator, float length)
{
	if ( fromArea == NULL_AREA )
	{
		// At the start of the path
		return 0.0;
	}

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
