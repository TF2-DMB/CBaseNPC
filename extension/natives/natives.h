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
#include <takedamageinfo.h>

#pragma once

extern IForward *g_pForwardEventKilled;
extern CUtlMap<int32_t, int32_t> g_EntitiesHooks;
	
#define NATIVENAME(type, name) \
	{ #type "." #name, type##_##name }, \

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
	NATIVENAME(CNavMesh, GetNearestNavArea)
	
	NATIVENAME(SurroundingAreasCollector, Get)
	NATIVENAME(SurroundingAreasCollector, Count)
	
	{ nullptr, nullptr },
};

#endif //NATIVES_H_INCLUDED_