#ifndef NATIVES_H_INCLUDED_
#define NATIVES_H_INCLUDED_

#include "baseent.h"
#include "baseanim.h"
#include "baseanimoverlay.h"
#include "eventresponder.h"
#include "body.h"
#include "intention.h"
#include "locomotion.h"
#include "nextbot.h"
#include "path.h"
#include "navarea.h"
#include "navmesh.h"
#include "component.h"
#include "cbasecombatcharacter.h"
#include "vision.h"
#include "cbasenpc.h"
#include "entityfactory.h"
#include "behavior.h"
#include <takedamageinfo.h>

#pragma once

extern IForward *g_pForwardEventKilled;
extern CUtlMap<int32_t, int32_t> g_EntitiesHooks;
	
#define NATIVENAME(type, name) \
	{ #type "." #name, type##_##name }, \

#define NATIVENAMEGETSET(type, name) \
	{ #type "." #name ".get", type##_##name##Get }, \
	{ #type "." #name ".set", type##_##name##Set }, \

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}

class CTakeDamageInfoHack : public CTakeDamageInfo
{
public:
	inline int GetAttacker() const { return m_hAttacker.IsValid() ? m_hAttacker.GetEntryIndex() : -1; }
	inline int GetInflictor() const { return m_hInflictor.IsValid() ? m_hInflictor.GetEntryIndex() : -1; }
	inline int GetWeapon() const { return m_hWeapon.IsValid() ? m_hWeapon.GetEntryIndex() : -1; }

	inline void SetDamageForce(vec_t x, vec_t y, vec_t z)
	{
		m_vecDamageForce.x = x;
		m_vecDamageForce.y = y;
		m_vecDamageForce.z = z;
	}

	inline void SetDamagePosition(vec_t x, vec_t y, vec_t z)
	{
		m_vecDamagePosition.x = x;
		m_vecDamagePosition.y = y;
		m_vecDamagePosition.z = z;
	}
};

SH_DECL_MANUALHOOK1_void(MEvent_Killed, 0, 0, 0, CTakeDamageInfoHack &);

void Event_Killed(CTakeDamageInfoHack &info)
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META(MRES_IGNORED);
	
	int entity = gamehelpers->EntityToBCompatRef(pEntity);
	int attacker = info.GetAttacker();
	int inflictor = info.GetInflictor();
	float damage = info.GetDamage();
	int damagetype = info.GetDamageType();
	int weapon = info.GetWeapon();
	Vector force = info.GetDamageForce();
	cell_t damageForce[3] = { sp_ftoc(force.x), sp_ftoc(force.y), sp_ftoc(force.z) };
	Vector pos = info.GetDamagePosition();
	cell_t damagePosition[3] = { sp_ftoc(pos.x), sp_ftoc(pos.y), sp_ftoc(pos.z) };
	cell_t res, ret = Pl_Continue;
	
	if (g_pForwardEventKilled != NULL)
	{
		g_pForwardEventKilled->PushCell(entity);
		g_pForwardEventKilled->PushCellByRef(&attacker);
		g_pForwardEventKilled->PushCellByRef(&inflictor);
		g_pForwardEventKilled->PushFloatByRef(&damage);
		g_pForwardEventKilled->PushCellByRef(&damagetype);
		g_pForwardEventKilled->PushCellByRef(&weapon);
		g_pForwardEventKilled->PushArray(damageForce, 3, SM_PARAM_COPYBACK);
		g_pForwardEventKilled->PushArray(damagePosition, 3, SM_PARAM_COPYBACK);
		g_pForwardEventKilled->PushCell(info.GetDamageCustom());
		g_pForwardEventKilled->Execute(&res);
		
		if (res >= ret)
		{
			ret = res;
			if (ret == Pl_Changed)
			{
				CBaseEntity *pEntAttacker = gamehelpers->ReferenceToEntity(attacker);
				if (pEntAttacker)
					info.SetAttacker(pEntAttacker);
				CBaseEntity *pEntInflictor = gamehelpers->ReferenceToEntity(inflictor);
				if (pEntInflictor)
					info.SetInflictor(pEntInflictor);
				info.SetDamage(damage);
				info.SetDamageType(damagetype);
				info.SetWeapon(gamehelpers->ReferenceToEntity(weapon));
				info.SetDamageForce(
					sp_ctof(damageForce[0]),
					sp_ctof(damageForce[1]),
					sp_ctof(damageForce[2]));
				info.SetDamagePosition(
					sp_ctof(damagePosition[0]),
					sp_ctof(damagePosition[1]),
					sp_ctof(damagePosition[2]));
			}
		}

		if (ret >= Pl_Handled)
			RETURN_META(MRES_SUPERCEDE);

		if (ret == Pl_Changed)
			RETURN_META(MRES_HANDLED);
	}
	
	RETURN_META(MRES_IGNORED);
}

cell_t CBaseNPC_GetNextBotOfEntity(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity;
	ENTINDEX_TO_CBASEENTITY(params[1], pEntity);
	
	return (cell_t)(g_pBaseNPCTools->GetNextBotOfEntity(pEntity));
}

cell_t CBaseNPC_HookEventKilled(IPluginContext *pContext, const cell_t *params)
{
	CBaseEntity *pEntity;
	ENTINDEX_TO_CBASEENTITY(params[1], pEntity);
	
	auto iIndex = g_EntitiesHooks.Find(gamehelpers->EntityToReference(pEntity));
	if (g_EntitiesHooks.IsValidIndex(iIndex))
		return 1;
	
	int iHookID = SH_ADD_MANUALHOOK(MEvent_Killed, pEntity, Event_Killed, false);
	g_EntitiesHooks.Insert(gamehelpers->EntityToReference(pEntity), iHookID);
	return 1;
}

const sp_nativeinfo_t g_NativesInfo[] =
{
	// Base Entity
	NATIVENAME(CBaseEntity, iUpdateOnRemove)
	NATIVENAME(CBaseEntity, Spawn)
	NATIVENAME(CBaseEntity, Teleport)
	NATIVENAME(CBaseEntity, SetModel)
	NATIVENAME(CBaseEntity, GetVectors)
	NATIVENAME(CBaseEntity, WorldSpaceCenter)

	// Base Animating
	NATIVENAME(CBaseAnimating, iHandleAnimEvent)
	NATIVENAME(CBaseAnimating, LookupAttachment)
	NATIVENAME(CBaseAnimating, GetAttachment)
	NATIVENAME(CBaseAnimating, StudioFrameAdvance)
	NATIVENAME(CBaseAnimating, DispatchAnimEvents)
	NATIVENAME(CBaseAnimating, LookupSequence)
	NATIVENAME(CBaseAnimating, SelectWeightedSequence)
	NATIVENAME(CBaseAnimating, SequenceDuration)
	NATIVENAME(CBaseAnimating, ResetSequence)
	NATIVENAME(CBaseAnimating, GetModelPtr)
	NATIVENAME(CBaseAnimating, LookupPoseParameter)
	NATIVENAME(CBaseAnimating, SetPoseParameter)
	NATIVENAME(CBaseAnimating, GetPoseParameter)

	// Deprecated
	NATIVENAME(CBaseAnimating, FindAttachment)

	// Base Animating Overlay
	NATIVENAME(CBaseAnimatingOverlay, AddGestureSequence)
	NATIVENAME(CBaseAnimatingOverlay, AddGesture)
	NATIVENAME(CBaseAnimatingOverlay, IsPlayingGesture)
	NATIVENAME(CBaseAnimatingOverlay, RestartGesture)
	NATIVENAME(CBaseAnimatingOverlay, RemoveAllGestures)
	NATIVENAME(CBaseAnimatingOverlay, AddLayeredSequence)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerPriority)
	NATIVENAME(CBaseAnimatingOverlay, IsValidLayer)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerDuration)
	NATIVENAME(CBaseAnimatingOverlay, GetLayerDuration)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerCycle)
	NATIVENAME(CBaseAnimatingOverlay, GetLayerCycle)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerPlaybackRate)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerWeight)
	NATIVENAME(CBaseAnimatingOverlay, GetLayerWeight)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerBlendIn)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerBlendOut)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerAutokill)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerLooping)
	NATIVENAME(CBaseAnimatingOverlay, SetLayerNoRestore)
	NATIVENAME(CBaseAnimatingOverlay, GetLayerActivity)
	NATIVENAME(CBaseAnimatingOverlay, GetLayerSequence)
	NATIVENAME(CBaseAnimatingOverlay, FindGestureLayer)
	NATIVENAME(CBaseAnimatingOverlay, RemoveLayer)
	NATIVENAME(CBaseAnimatingOverlay, FastRemoveLayer)
	NATIVENAME(CBaseAnimatingOverlay, GetAnimOverlay)
	NATIVENAME(CBaseAnimatingOverlay, GetNumAnimOverlays)
	NATIVENAME(CBaseAnimatingOverlay, SetNumAnimOverlays)
	NATIVENAME(CBaseAnimatingOverlay, HasActiveLayer)

	// NPC
	NATIVENAME(CExtNPC, GetEntity)

	NATIVENAME(CBaseNPC, CBaseNPC)

	// INextbot
	NATIVENAME(CBaseNPC, GetBot)
	NATIVENAME(CBaseNPC, GetLocomotion)
	NATIVENAME(CBaseNPC, GetBody)
	NATIVENAME(CBaseNPC, GetVision)

	NATIVENAME(CBaseNPC, SetType)
	NATIVENAME(CBaseNPC, GetType)
	
	// IBody
	NATIVENAME(CBaseNPC, SetBodyMins)
	NATIVENAME(CBaseNPC, SetBodyMaxs)
	NATIVENAME(CBaseNPC, GetBodyMins)
	NATIVENAME(CBaseNPC, GetBodyMaxs)

	NATIVENAMEGETSET(CBaseNPC, flStepSize)
	NATIVENAMEGETSET(CBaseNPC, flGravity)
	NATIVENAMEGETSET(CBaseNPC, flAcceleration)
	NATIVENAMEGETSET(CBaseNPC, flJumpHeight)
	NATIVENAMEGETSET(CBaseNPC, flDeathDropHeight)
	NATIVENAMEGETSET(CBaseNPC, flWalkSpeed)
	NATIVENAMEGETSET(CBaseNPC, flRunSpeed)
	NATIVENAMEGETSET(CBaseNPC, flFrictionForward)
	NATIVENAMEGETSET(CBaseNPC, flFrictionSideways)

	// Implemented but deprecated so plugins that used this can still continue to work
	// TO-DO: Remove in the next two updates
	NATIVENAME(CBaseNPC, Approach)
	NATIVENAME(CBaseNPC, FaceTowards)
	NATIVENAME(CBaseNPC, Walk)
	NATIVENAME(CBaseNPC, Run)
	NATIVENAME(CBaseNPC, Stop)
	NATIVENAME(CBaseNPC, Jump)
	NATIVENAME(CBaseNPC, IsOnGround)
	NATIVENAME(CBaseNPC, IsClimbingOrJumping)
	NATIVENAME(CBaseNPC, SetVelocity)
	NATIVENAME(CBaseNPC, GetVelocity)
	NATIVENAME(CBaseNPC, GetLastKnownArea)
	NATIVENAME(CBaseNPC, Spawn)
	NATIVENAME(CBaseNPC, Teleport)
	NATIVENAME(CBaseNPC, SetCollisionBounds)
	NATIVENAME(CBaseNPC, GetLastKnownArea)
	NATIVENAME(CBaseNPC, GetVectors)
	NATIVENAME(CBaseNPC, SetModel)

	NATIVENAME(CNPCs, IsValidNPC)
	NATIVENAME(CNPCs, FindNPCByEntIndex)

	NATIVENAME(INextBotEventResponder, FirstContainedResponder)
	NATIVENAME(INextBotEventResponder, FirstContainedResponder)
	NATIVENAME(INextBotEventResponder, FirstContainedResponder)
	NATIVENAME(INextBotEventResponder, FirstContainedResponder)
	NATIVENAME(INextBotEventResponder, FirstContainedResponder)
	NATIVENAME(INextBotEventResponder, FirstContainedResponder)

	NATIVENAME(INextBotEventResponder, FirstContainedResponder)
	NATIVENAME(INextBotEventResponder, NextContainedResponder)
	
	NATIVENAME(INextBotComponent, Reset)
	NATIVENAME(INextBotComponent, Update)
	NATIVENAME(INextBotComponent, Upkeep)
	NATIVENAME(INextBotComponent, GetBot)
	
	NATIVENAME(CKnownEntity, Destroy)
	NATIVENAME(CKnownEntity, UpdatePosition)
	NATIVENAME(CKnownEntity, GetEntity)
	NATIVENAME(CKnownEntity, GetLastKnownPosition)
	NATIVENAME(CKnownEntity, HasLastKnownPositionBeenSeen)
	NATIVENAME(CKnownEntity, MarkLastKnownPositionAsSeen)
	NATIVENAME(CKnownEntity, GetLastKnownArea)
	NATIVENAME(CKnownEntity, GetTimeSinceLastKnown)
	NATIVENAME(CKnownEntity, GetTimeSinceBecameKnown)
	NATIVENAME(CKnownEntity, UpdateVisibilityStatus)
	NATIVENAME(CKnownEntity, IsVisibleInFOVNow)
	NATIVENAME(CKnownEntity, IsVisibleRecently)
	NATIVENAME(CKnownEntity, GetTimeSinceBecameVisible)
	NATIVENAME(CKnownEntity, GetTimeWhenBecameVisible)
	NATIVENAME(CKnownEntity, GetTimeSinceLastSeen)
	NATIVENAME(CKnownEntity, WasEverVisible)
	NATIVENAME(CKnownEntity, IsObsolete)
	NATIVENAME(CKnownEntity, Is)
	
	NATIVENAME(IVision, GetPrimaryKnownThreat)
	NATIVENAME(IVision, GetTimeSinceVisible)
	NATIVENAME(IVision, GetClosestKnown)
	NATIVENAME(IVision, GetKnownCount)
	NATIVENAME(IVision, GetKnown)
	NATIVENAME(IVision, AddKnownEntity)
	NATIVENAME(IVision, ForgetEntity)
	NATIVENAME(IVision, ForgetAllKnownEntities)
	NATIVENAME(IVision, GetMaxVisionRange)
	NATIVENAME(IVision, GetMinRecognizeTime)
	NATIVENAME(IVision, IsAbleToSee)
	NATIVENAME(IVision, IsAbleToSeeTarget)
	NATIVENAME(IVision, IsIgnored)
	NATIVENAME(IVision, IsVisibleEntityNoticed)
	NATIVENAME(IVision, IsInFieldOfView)
	NATIVENAME(IVision, IsInFieldOfViewTarget)
	NATIVENAME(IVision, GetDefaultFieldOfView)
	NATIVENAME(IVision, GetFieldOfView)
	NATIVENAME(IVision, SetFieldOfView)
	NATIVENAME(IVision, IsLineOfSightClear)
	NATIVENAME(IVision, IsLineOfSightClearToEntity)
	NATIVENAME(IVision, IsLookingAt)
	NATIVENAME(IVision, IsLookingAtTarget)
	
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

	NATIVENAME(IIntention, Reset)
	NATIVENAME(IIntention, Update)
	NATIVENAME(IIntention, ShouldPickUp)
	NATIVENAME(IIntention, ShouldHurry)
	NATIVENAME(IIntention, ShouldRetreat)
	NATIVENAME(IIntention, ShouldAttack)
	NATIVENAME(IIntention, IsHindrance)
	NATIVENAME(IIntention, SelectTargetPoint)
	NATIVENAME(IIntention, IsPositionAllowed)
	NATIVENAME(IIntention, SelectMoreDangerousThreat)

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
	NATIVENAME(Path, ComputeToPosT)
	NATIVENAME(Path, ComputeToTargetT)

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
	NATIVENAME(CBaseCombatCharacter, UpdateLastKnownArea)
	
	{ "CBaseNPC_GetNextBotOfEntity", &CBaseNPC_GetNextBotOfEntity },
	{ "CBaseNPC_HookEventKilled", &CBaseNPC_HookEventKilled },
	
	NATIVENAME(CNavArea, UpdateBlocked)
	NATIVENAME(CNavArea, IsBlocked)
	NATIVENAME(CNavArea, GetID)
	NATIVENAME(CNavArea, SetParent)
	NATIVENAME(CNavArea, GetParent)
	NATIVENAME(CNavArea, GetParentHow)
	NATIVENAME(CNavArea, GetCostSoFar)
	NATIVENAME(CNavArea, GetAttributes)
	NATIVENAME(CNavArea, GetCenter)
	NATIVENAME(CNavArea, IsConnected)
	NATIVENAME(CNavArea, IsEdge)
	NATIVENAME(CNavArea, Contains)
	NATIVENAME(CNavArea, GetSizeX)
	NATIVENAME(CNavArea, GetSizeY)
	NATIVENAME(CNavArea, GetZ)
	NATIVENAME(CNavArea, GetZVector)
	NATIVENAME(CNavArea, ComputeNormal)
	{ "CNavLadder.length.get", &CNavLadder_length },
	
	NATIVENAME(CNavMesh, CollectSurroundingAreas)
	NATIVENAME(CNavMesh, TCollectSurroundingAreas)
	NATIVENAME(CNavMesh, GetNearestNavArea)
	
	NATIVENAME(SurroundingAreasCollector, Get)
	NATIVENAME(SurroundingAreasCollector, Count)
	NATIVENAME(TSurroundingAreasCollector, Get)
	NATIVENAME(TSurroundingAreasCollector, Count)

	NATIVENAME(CTNavArea, GetParentHow)
	NATIVENAME(CTNavArea, GetCostSoFar)
	NATIVENAME(CTNavArea, GetRealNavArea)
	
	{ "CEntityFactory.CEntityFactory", &CPluginEntityFactory_CPluginEntityFactory },
	{ "CEntityFactory.DeriveFromBaseEntity", &CPluginEntityFactory_DeriveFromBaseEntity },
	{ "CEntityFactory.DeriveFromClass", &CPluginEntityFactory_DeriveFromClass },
	{ "CEntityFactory.DeriveFromNPC", &CPluginEntityFactory_DeriveFromNPC },
	{ "CEntityFactory.DeriveFromFactory", &CPluginEntityFactory_DeriveFromFactory },
	{ "CEntityFactory.SetInitialActionFactory", &CPluginEntityFactory_SetInitialActionFactory },
	{ "CEntityFactory.Install", &CPluginEntityFactory_Install },
	{ "CEntityFactory.Uninstall", &CPluginEntityFactory_Uninstall },
	{ "CEntityFactory.IsInstalled.get", &CPluginEntityFactory_Installed },
	{ "CEntityFactory.IsAbstract.get", &CPluginEntityFactory_AbstractGet },
	{ "CEntityFactory.IsAbstract.set", &CPluginEntityFactory_AbstractSet },

	{ "CEntityFactory.GetClassname", &CPluginEntityFactory_GetClassname },

	{ "CEntityFactory.GetFactoryOfEntity", &CPluginEntityFactory_GetFactoryOfEntity },

	{ "CEntityFactory.GetNumInstalledFactories", &CPluginEntityFactory_GetNumInstalledFactories },
	{ "CEntityFactory.GetInstalledFactories", &CPluginEntityFactory_GetInstalledFactories },

	{ "CEntityFactory.BeginDataMapDesc", &CPluginEntityFactory_BeginDataMapDesc },
	{ "CEntityFactory.DefineIntField", &CPluginEntityFactory_DefineIntField },
	{ "CEntityFactory.DefineFloatField", &CPluginEntityFactory_DefineFloatField },
	{ "CEntityFactory.DefineCharField", &CPluginEntityFactory_DefineCharField },
	{ "CEntityFactory.DefineBoolField", &CPluginEntityFactory_DefineBoolField },
	{ "CEntityFactory.DefineVectorField", &CPluginEntityFactory_DefineVectorField },
	{ "CEntityFactory.DefineStringField", &CPluginEntityFactory_DefineStringField },
	{ "CEntityFactory.DefineEntityField", &CPluginEntityFactory_DefineEntityField },
	{ "CEntityFactory.DefineInputFunc", &CPluginEntityFactory_DefineInputFunc },
	{ "CEntityFactory.DefineOutput", &CPluginEntityFactory_DefineOutput },
	{ "CEntityFactory.EndDataMapDesc", &CPluginEntityFactory_EndDataMapDesc },

	// NextBotAction
	{ "NextBotAction.Actor.get", &NextBotAction_GetActor },
	{ "NextBotAction.Parent.get", &NextBotAction_GetParent },
	{ "NextBotAction.ActiveChild.get", &NextBotAction_GetActiveChild },
	NATIVENAME(NextBotAction, GetName)
	NATIVENAME(NextBotAction, GetFullName)

	NATIVENAME(NextBotAction, GetData)
	NATIVENAME(NextBotAction, SetData)
	NATIVENAME(NextBotAction, GetDataFloat)
	NATIVENAME(NextBotAction, SetDataFloat)
	NATIVENAME(NextBotAction, GetDataVector)
	NATIVENAME(NextBotAction, SetDataVector)
	NATIVENAME(NextBotAction, GetDataString)
	NATIVENAME(NextBotAction, SetDataString)

	{ "NextBotAction.IsSuspended.get", &NextBotAction_IsSuspended },

	NATIVENAME(NextBotAction, Continue)
	NATIVENAME(NextBotAction, ChangeTo)
	NATIVENAME(NextBotAction, SuspendFor)
	NATIVENAME(NextBotAction, Done)

	NATIVENAME(NextBotAction, TryContinue)
	NATIVENAME(NextBotAction, TryChangeTo)
	NATIVENAME(NextBotAction, TrySuspendFor)
	NATIVENAME(NextBotAction, TryDone)
	NATIVENAME(NextBotAction, TryToSustain)

	NATIVENAME(NextBotActionFactory, NextBotActionFactory)
	NATIVENAME(NextBotActionFactory, SetCallback)
	NATIVENAME(NextBotActionFactory, SetQueryCallback)
	NATIVENAME(NextBotActionFactory, SetEventCallback)
	NATIVENAME(NextBotActionFactory, Create)
	NATIVENAME(NextBotActionFactory, BeginDataMapDesc)
	NATIVENAME(NextBotActionFactory, DefineIntField)
	NATIVENAME(NextBotActionFactory, DefineFloatField)
	NATIVENAME(NextBotActionFactory, DefineCharField)
	NATIVENAME(NextBotActionFactory, DefineBoolField)
	NATIVENAME(NextBotActionFactory, DefineVectorField)
	NATIVENAME(NextBotActionFactory, DefineStringField)
	NATIVENAME(NextBotActionFactory, EndDataMapDesc)

	{ nullptr, nullptr },
};

#endif //NATIVES_H_INCLUDED_