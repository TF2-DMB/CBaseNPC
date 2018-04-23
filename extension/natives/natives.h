#ifndef NATIVES_H_INCLUDED_
#define NATIVES_H_INCLUDED_

#include "eventresponder.h"
#include "body.h"
#include "locomotion.h"
#include "nextbot.h"
#include "path.h"
#include "navarea.h"
#include "navmesh.h"
#include "component.h"
#include "cbasecombatcharacter.h"

#pragma once
	
#define NATIVENAME(type, name) \
	{ #type "." #name, type##_##name }, \

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}
SH_DECL_MANUALHOOK0_void(MHook_LocomotionUpdate, 0, 0, 0);

void Hook_LocomotionUpdate()
{
	NextBotGroundLocomotion *mover = META_IFACEPTR(NextBotGroundLocomotion);
	if(!mover) RETURN_META(MRES_IGNORED);
	
	//mover->TryNextBotMove();
	RETURN_META(MRES_IGNORED);
}

cell_t CBaseNPC_GetNextBotOfEntity(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity;
	ENTINDEX_TO_CBASEENTITY(params[1], pEntity);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[1]);
	}
	
	ICallWrapper *pMyNextBotPointerWrapper = nullptr;
	if (!pMyNextBotPointerWrapper)
	{
		SourceMod::PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.size = sizeof(INextBot *);
		ret.type = PassType_Basic;
	
		pMyNextBotPointerWrapper = g_pBinTools->CreateVCall(g_iMyNextBotPointerOffset, 0, 0, &ret, nullptr, 0);
		
		if(!pMyNextBotPointerWrapper) {
			return pContext->ThrowNativeError("Couldn't initialize CBaseEntity::MyNextBotPointer call!");
		}
	}
	
	unsigned char vstk[sizeof(CBaseEntity *)];
	unsigned char *vptr = vstk;
	
	*(CBaseEntity **)vptr = pEntity;

	INextBot *bot = nullptr;
	pMyNextBotPointerWrapper->Execute(vstk, &bot);

	return (cell_t)(bot);
}

const sp_nativeinfo_t g_NativesInfo[] =
{
	NATIVENAME(INextBotEventResponder, FirstContainedResponder)
	NATIVENAME(INextBotEventResponder, NextContainedResponder)
	
	NATIVENAME(INextBotComponent, Reset)
	NATIVENAME(INextBotComponent, Update)
	NATIVENAME(INextBotComponent, Upkeep)
	NATIVENAME(INextBotComponent, GetBot)
	
	NATIVENAME(IBody, SetPosition)
	NATIVENAME(IBody, GetEyePosition)
	NATIVENAME(IBody, GetViewVector)
	//NATIVENAME(IBody, AimHeadTowards)
	//NATIVENAME(IBody, AimHeadTowardsEx)
	NATIVENAME(IBody, IsHeadAimingOnTarget)
	NATIVENAME(IBody, IsHeadSteady)
	NATIVENAME(IBody, GetHeadSteadyDuration)
	NATIVENAME(IBody, GetHeadAimSubjectLeadTime)
	NATIVENAME(IBody, GetHeadAimTrackingInterval)
	NATIVENAME(IBody, ClearPendingAimReply)
	NATIVENAME(IBody, GetMaxHeadAngularVelocity)
	//NATIVENAME(IBody, StartActivity)
	//NATIVENAME(IBody, SelectAnimationSequence)
	//NATIVENAME(IBody, GetActivity)
	//NATIVENAME(IBody, IsActivity)
	//NATIVENAME(IBody, HasActivityType)
	NATIVENAME(IBody, SetDesiredPosture)
	NATIVENAME(IBody, GetDesiredPosture)
	NATIVENAME(IBody, IsDesiredPosture)
	NATIVENAME(IBody, IsInDesiredPosture)
	NATIVENAME(IBody, GetActualPosture)
	NATIVENAME(IBody, IsActualPosture)
	NATIVENAME(IBody, IsPostureMobile)
	NATIVENAME(IBody, IsPostureChanging)
	NATIVENAME(IBody, SetArousal)
	NATIVENAME(IBody, GetArousal)
	NATIVENAME(IBody, IsArousal)
	NATIVENAME(IBody, GetHullWidth)
	NATIVENAME(IBody, GetHullHeight)
	NATIVENAME(IBody, GetStandHullHeight)
	NATIVENAME(IBody, GetCrouchHullHeight)
	NATIVENAME(IBody, GetHullMins)
	NATIVENAME(IBody, GetHullMaxs)
	NATIVENAME(IBody, GetSolidMask)
	NATIVENAME(IBody, GetCollisionGroup)

	NATIVENAME(ILocomotion, Approach)
	NATIVENAME(ILocomotion, DriveTo)
	NATIVENAME(ILocomotion, ClimbUpToLedge)
	NATIVENAME(ILocomotion, JumpAcrossGap)
	NATIVENAME(ILocomotion, Jump)
	NATIVENAME(ILocomotion, IsClimbingOrJumping)
	NATIVENAME(ILocomotion, IsClimbingUpToLedge)
	NATIVENAME(ILocomotion, IsJumpingAcrossGap)
	NATIVENAME(ILocomotion, IsScrambling)
	NATIVENAME(ILocomotion, Run)
	NATIVENAME(ILocomotion, Walk)
	NATIVENAME(ILocomotion, Stop)
	NATIVENAME(ILocomotion, IsRunning)
	NATIVENAME(ILocomotion, SetDesiredSpeed)
	NATIVENAME(ILocomotion, GetDesiredSpeed)
	NATIVENAME(ILocomotion, SetSpeedLimit)
	NATIVENAME(ILocomotion, GetSpeedLimit)
	NATIVENAME(ILocomotion, IsOnGround)
	NATIVENAME(ILocomotion, GetGround)
	NATIVENAME(ILocomotion, GetGroundNormal)
	NATIVENAME(ILocomotion, GetGroundSpeed)
	NATIVENAME(ILocomotion, GetGroundMotionVector)
	NATIVENAME(ILocomotion, ClimbLadder)
	NATIVENAME(ILocomotion, DescendLadder)
	NATIVENAME(ILocomotion, IsUsingLadder)
	NATIVENAME(ILocomotion, IsAscendingOrDescendingLadder)
	NATIVENAME(ILocomotion, IsAbleToAutoCenterOnLadder)
	NATIVENAME(ILocomotion, FaceTowards)
	NATIVENAME(ILocomotion, SetDesiredLean)
	NATIVENAME(ILocomotion, GetDesiredLean)
	NATIVENAME(ILocomotion, IsAbleToJumpAcrossGaps)
	NATIVENAME(ILocomotion, IsAbleToClimb)
	NATIVENAME(ILocomotion, GetFeet)
	NATIVENAME(ILocomotion, GetStepHeight)
	NATIVENAME(ILocomotion, GetMaxJumpHeight)
	NATIVENAME(ILocomotion, GetDeathDropHeight)
	NATIVENAME(ILocomotion, GetRunSpeed)
	NATIVENAME(ILocomotion, GetWalkSpeed)
	NATIVENAME(ILocomotion, GetMaxAcceleration)
	NATIVENAME(ILocomotion, GetMaxDeceleration)
	NATIVENAME(ILocomotion, GetVelocity)
	NATIVENAME(ILocomotion, GetSpeed)
	NATIVENAME(ILocomotion, GetMotionVector)
	NATIVENAME(ILocomotion, IsAreaTraversable)
	NATIVENAME(ILocomotion, GetTraversableSlopeLimit)
	NATIVENAME(ILocomotion, IsPotentiallyTraversable)
	NATIVENAME(ILocomotion, HasPotentialGap)
	NATIVENAME(ILocomotion, IsGap)
	NATIVENAME(ILocomotion, IsEntityTraversable)
	NATIVENAME(ILocomotion, IsStuck)
	NATIVENAME(ILocomotion, GetStuckDuration)
	NATIVENAME(ILocomotion, ClearStuckStatus)
	NATIVENAME(ILocomotion, IsAttemptingToMove)
	NATIVENAME(ILocomotion, ShouldCollideWith)
	NATIVENAME(ILocomotion, AdjustPosture)
	NATIVENAME(ILocomotion, StuckMonitor)
	
	NATIVENAME(NextBotGroundLocomotion, GetAcceleration)
	NATIVENAME(NextBotGroundLocomotion, SetAcceleration)
	NATIVENAME(NextBotGroundLocomotion, SetVelocity)
	NATIVENAME(NextBotGroundLocomotion, GetGravity)
	NATIVENAME(NextBotGroundLocomotion, GetFrictionForward)
	NATIVENAME(NextBotGroundLocomotion, GetFrictionSideways)
	NATIVENAME(NextBotGroundLocomotion, GetMaxYawRate)

	NATIVENAME(INextBot, Reset)
	NATIVENAME(INextBot, Update)
	NATIVENAME(INextBot, Upkeep)
	NATIVENAME(INextBot, IsRemovedOnReset)
	NATIVENAME(INextBot, GetEntity)
	NATIVENAME(INextBot, GetNextBotCombatCharacter)
	NATIVENAME(INextBot, GetLocomotionInterface)
	NATIVENAME(INextBot, GetBodyInterface)
	NATIVENAME(INextBot, GetIntentionInterface)
	NATIVENAME(INextBot, GetVisionInterface)
	NATIVENAME(INextBot, SetPosition)
	NATIVENAME(INextBot, GetPosition)
	NATIVENAME(INextBot, IsEnemy)
	NATIVENAME(INextBot, IsFriend)
	NATIVENAME(INextBot, IsSelf)
	NATIVENAME(INextBot, IsAbleToClimbOnto)
	NATIVENAME(INextBot, IsAbleToBreak)
	NATIVENAME(INextBot, IsAbleToBlockMovementOf)
	NATIVENAME(INextBot, ShouldTouch)
	NATIVENAME(INextBot, IsImmobile)
	NATIVENAME(INextBot, GetImmobileDuration)
	NATIVENAME(INextBot, ClearImmobileStatus)
	NATIVENAME(INextBot, GetImmobileSpeedThreshold)
	NATIVENAME(INextBot, GetCurrentPath)
	NATIVENAME(INextBot, SetCurrentPath)
	NATIVENAME(INextBot, NotifyPathDestruction)
	NATIVENAME(INextBot, IsRangeLessThan)
	NATIVENAME(INextBot, IsRangeLessThanEx)
	NATIVENAME(INextBot, IsRangeGreaterThan)
	NATIVENAME(INextBot, IsRangeGreaterThanEx)
	NATIVENAME(INextBot, GetRangeTo)
	NATIVENAME(INextBot, GetRangeToEx)
	NATIVENAME(INextBot, GetRangeSquaredTo)
	NATIVENAME(INextBot, GetRangeSquaredToEx)
	NATIVENAME(INextBot, IsDebugging)
	NATIVENAME(INextBot, GetDebugIdentifier)
	NATIVENAME(INextBot, IsDebugFilterMatch)
	NATIVENAME(INextBot, DisplayDebugText)
	
	NATIVENAME(Segment, GetPos)

	NATIVENAME(Path, Path)
	NATIVENAME(Path, GetLength)
	NATIVENAME(Path, GetPosition)
	//NATIVENAME(Path, GetClosestPosition)
	NATIVENAME(Path, GetStartPosition)
	NATIVENAME(Path, GetEndPosition)
	NATIVENAME(Path, GetSubject)
	NATIVENAME(Path, GetCurrentGoal)
	NATIVENAME(Path, GetAge)
	//NATIVENAME(Path, MoveCursorToClosestPosition)
	NATIVENAME(Path, MoveCursorToStart)
	NATIVENAME(Path, MoveCursorToEnd)
	NATIVENAME(Path, MoveCursor)
	NATIVENAME(Path, GetCursorPosition)
	//NATIVENAME(Path, GetCursorData)
	NATIVENAME(Path, IsValid)
	NATIVENAME(Path, Invalidate)
	NATIVENAME(Path, Draw)
	NATIVENAME(Path, DrawInterpolated)
	NATIVENAME(Path, FirstSegment)
	NATIVENAME(Path, NextSegment)
	NATIVENAME(Path, PriorSegment)
	NATIVENAME(Path, LastSegment)
	NATIVENAME(Path, Copy)
	NATIVENAME(Path, ComputeToPos)
	NATIVENAME(Path, ComputeToTarget)
	NATIVENAME(Path, Destroy)

	NATIVENAME(PathFollower, PathFollower)
	NATIVENAME(PathFollower, Update)
	NATIVENAME(PathFollower, SetMinLookAheadDistance)
	NATIVENAME(PathFollower, GetHindrance)
	NATIVENAME(PathFollower, IsDiscontinuityAhead)
	NATIVENAME(PathFollower, SetGoalTolerance)
	NATIVENAME(PathFollower, Destroy)
	
	NATIVENAME(ChasePath, ChasePath)
	NATIVENAME(ChasePath, Update)
	NATIVENAME(ChasePath, GetLeadRadius)
	NATIVENAME(ChasePath, GetMaxPathLength)
	NATIVENAME(ChasePath, PredictSubjectPosition)
	NATIVENAME(ChasePath, IsRepathNeeded)
	NATIVENAME(ChasePath, GetLifetime)
	NATIVENAME(ChasePath, Destroy)
	
	NATIVENAME(DirectChasePath, DirectChasePath)
	NATIVENAME(DirectChasePath, Destroy)
	
	NATIVENAME(CBaseCombatCharacter, GetLastKnownArea)
	
	{ "CBaseNPC_GetNextBotOfEntity", &CBaseNPC_GetNextBotOfEntity },
	
	NATIVENAME(CNavArea, GetCostSoFar)
	NATIVENAME(CNavArea, GetAttributes)
	NATIVENAME(CNavArea, GetCenter)
	{ "CNavLadder.length.get", &CNavLadder_length },
	
	NATIVENAME(CNavMesh, CollectSurroundingAreas)
	NATIVENAME(CNavMesh, GetNearestNavArea)
	
	NATIVENAME(SurroundingAreasCollector, Get)
	NATIVENAME(SurroundingAreasCollector, Count)
	
	{ nullptr, nullptr },
};

#endif //NATIVES_H_INCLUDED_