#ifndef NATIVES_LOCOMOTION_H_INCLUDED_
	#define NATIVES_LOCOMOTION_H_INCLUDED_

#pragma once

#include "NextBotLocomotionInterface.h"
#include "NextBotGroundLocomotion.h"

#define LOCOMOTIONNATIVE(name) \
	cell_t ILocomotion_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		ILocomotion *pLocomotion = (ILocomotion *)(params[1]); \
		if(!pLocomotion) { \
			return pContext->ThrowNativeError("Invalid ILocomotion %x", params[1]); \
		} \
		
#define NEXTBOTGROUNDLOCOMOTIONNATIVE(name) \
	cell_t NextBotGroundLocomotion_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		NextBotGroundLocomotion *pLocomotion = (NextBotGroundLocomotion *)(params[1]); \
		if(!pLocomotion) { \
			return pContext->ThrowNativeError("Invalid NextBotGroundLocomotion %x", params[1]); \
		} \
		
LOCOMOTIONNATIVE(Approach)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	pLocomotion->Approach(dst, sp_ctof(params[3]));
	return 0;
}

LOCOMOTIONNATIVE(DriveTo)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	pLocomotion->DriveTo(dst);
	return 0;
}

LOCOMOTIONNATIVE(ClimbUpToLedge)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	cell_t *dirAddr;
	pContext->LocalToPhysAddr(params[3], &dirAddr);
	Vector dir;
	PawnVectorToVector(dirAddr, dir);
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pLocomotion->ClimbUpToLedge(dst, dir, pEntity));
}

LOCOMOTIONNATIVE(JumpAcrossGap)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	cell_t *dirAddr;
	pContext->LocalToPhysAddr(params[3], &dirAddr);
	Vector dir;
	PawnVectorToVector(dirAddr, dir);
	pLocomotion->JumpAcrossGap(dst, dir);
	return 0;
}

LOCOMOTIONNATIVE(Jump)
	pLocomotion->Jump();
	return 0;
}

LOCOMOTIONNATIVE(IsClimbingOrJumping)
	return (cell_t)(pLocomotion->IsClimbingOrJumping());
}

LOCOMOTIONNATIVE(IsClimbingUpToLedge)
	return (cell_t)(pLocomotion->IsClimbingUpToLedge());
}

LOCOMOTIONNATIVE(IsJumpingAcrossGap)
	return (cell_t)(pLocomotion->IsJumpingAcrossGap());
}

LOCOMOTIONNATIVE(IsScrambling)
	return (cell_t)(pLocomotion->IsScrambling());
}

LOCOMOTIONNATIVE(Run)
	pLocomotion->Run();
	return 0;
}

LOCOMOTIONNATIVE(Walk)
	pLocomotion->Walk();
	return 0;
}

LOCOMOTIONNATIVE(Stop)
	pLocomotion->Stop();
	return 0;
}

LOCOMOTIONNATIVE(IsRunning)
	return (cell_t)(pLocomotion->IsRunning());
}

LOCOMOTIONNATIVE(SetDesiredSpeed)
	pLocomotion->SetDesiredSpeed(sp_ctof(params[2]));
	return 0;
}

LOCOMOTIONNATIVE(GetDesiredSpeed)
	return sp_ftoc(pLocomotion->GetDesiredSpeed());
}

LOCOMOTIONNATIVE(SetSpeedLimit)
	pLocomotion->SetSpeedLimit(sp_ctof(params[2]));
	return 0;
}

LOCOMOTIONNATIVE(GetSpeedLimit)
	return sp_ftoc(pLocomotion->GetSpeedLimit());
}

LOCOMOTIONNATIVE(IsOnGround)
	return (cell_t)(pLocomotion->IsOnGround());
}

LOCOMOTIONNATIVE(GetGround)
	return gamehelpers->EntityToBCompatRef(pLocomotion->GetGround());
}

LOCOMOTIONNATIVE(GetGroundNormal)
	cell_t *normalAddr;
	pContext->LocalToPhysAddr(params[2], &normalAddr);
	Vector normal = pLocomotion->GetGroundNormal();
	VectorToPawnVector(normalAddr, normal);
	return 0;
}

LOCOMOTIONNATIVE(GetGroundSpeed)
	return sp_ftoc(pLocomotion->GetGroundSpeed());
}

LOCOMOTIONNATIVE(GetGroundMotionVector)
	cell_t *motionAddr;
	pContext->LocalToPhysAddr(params[2], &motionAddr);
	Vector motion = pLocomotion->GetGroundMotionVector();
	VectorToPawnVector(motionAddr, motion);
	return 0;
}

LOCOMOTIONNATIVE(ClimbLadder)
	pLocomotion->ClimbLadder((CNavLadder *)(params[2]), (CNavArea *)(params[3]));
	return 0;
}

LOCOMOTIONNATIVE(DescendLadder)
	pLocomotion->DescendLadder((CNavLadder *)(params[2]), (CNavArea *)(params[3]));
	return 0;
}

LOCOMOTIONNATIVE(IsUsingLadder)
	return (cell_t)(pLocomotion->IsUsingLadder());
}

LOCOMOTIONNATIVE(IsAscendingOrDescendingLadder)
	return (cell_t)(pLocomotion->IsAscendingOrDescendingLadder());
}

LOCOMOTIONNATIVE(IsAbleToAutoCenterOnLadder)
	return (cell_t)(pLocomotion->IsAbleToAutoCenterOnLadder());
}

LOCOMOTIONNATIVE(FaceTowards)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	pLocomotion->FaceTowards(dst);
	return 0;
}

LOCOMOTIONNATIVE(SetDesiredLean)
	cell_t *leanAddr;
	pContext->LocalToPhysAddr(params[2], &leanAddr);
	QAngle lean;
	PawnVectorToVector(leanAddr, lean);
	pLocomotion->SetDesiredLean(lean);
	return 0;
}

LOCOMOTIONNATIVE(GetDesiredLean)
	cell_t *leanAddr;
	pContext->LocalToPhysAddr(params[2], &leanAddr);
	QAngle lean = pLocomotion->GetDesiredLean();
	VectorToPawnVector(leanAddr, lean);
	return 0;
}

LOCOMOTIONNATIVE(IsAbleToJumpAcrossGaps)
	return (cell_t)(pLocomotion->IsAbleToJumpAcrossGaps());
}

LOCOMOTIONNATIVE(IsAbleToClimb)
	return (cell_t)(pLocomotion->IsAbleToClimb());
}

LOCOMOTIONNATIVE(GetFeet)
	cell_t *feetAddr;
	pContext->LocalToPhysAddr(params[2], &feetAddr);
	Vector feet = pLocomotion->GetFeet();
	VectorToPawnVector(feetAddr, feet);
	return 0;
}

LOCOMOTIONNATIVE(GetStepHeight)
	return sp_ftoc(pLocomotion->GetStepHeight());
}

LOCOMOTIONNATIVE(GetMaxJumpHeight)
	return sp_ftoc(pLocomotion->GetMaxJumpHeight());
}

LOCOMOTIONNATIVE(GetDeathDropHeight)
	return sp_ftoc(pLocomotion->GetDeathDropHeight());
}

LOCOMOTIONNATIVE(GetRunSpeed)
	return sp_ftoc(pLocomotion->GetRunSpeed());
}

LOCOMOTIONNATIVE(GetWalkSpeed)
	return sp_ftoc(pLocomotion->GetWalkSpeed());
}

LOCOMOTIONNATIVE(GetMaxAcceleration)
	return sp_ftoc(pLocomotion->GetMaxAcceleration());
}

LOCOMOTIONNATIVE(GetMaxDeceleration)
	return sp_ftoc(pLocomotion->GetMaxDeceleration());
}

LOCOMOTIONNATIVE(GetVelocity)
	cell_t *velAddr;
	pContext->LocalToPhysAddr(params[2], &velAddr);
	Vector vel = pLocomotion->GetVelocity();
	VectorToPawnVector(velAddr, vel);
	return 0;
}

LOCOMOTIONNATIVE(GetSpeed)
	return sp_ftoc(pLocomotion->GetSpeed());
}

LOCOMOTIONNATIVE(GetMotionVector)
	cell_t *motionAddr;
	pContext->LocalToPhysAddr(params[2], &motionAddr);
	Vector motion = pLocomotion->GetMotionVector();
	VectorToPawnVector(motionAddr, motion);
	return 0;
}

LOCOMOTIONNATIVE(IsAreaTraversable)
	return (cell_t)(pLocomotion->IsAreaTraversable((CNavArea *)(params[2])));
}

LOCOMOTIONNATIVE(GetTraversableSlopeLimit)
	return sp_ftoc(pLocomotion->GetTraversableSlopeLimit());
}

LOCOMOTIONNATIVE(IsPotentiallyTraversable)
	cell_t *fromAddr;
	pContext->LocalToPhysAddr(params[2], &fromAddr);
	Vector from;
	PawnVectorToVector(fromAddr, from);
	cell_t *toAddr;
	pContext->LocalToPhysAddr(params[3], &toAddr);
	Vector to;
	PawnVectorToVector(toAddr, to);
	return (cell_t)(pLocomotion->IsPotentiallyTraversable(from, to, (ILocomotion::TraverseWhenType)(params[4])));
}

LOCOMOTIONNATIVE(HasPotentialGap)
	cell_t *fromAddr;
	pContext->LocalToPhysAddr(params[2], &fromAddr);
	Vector from;
	PawnVectorToVector(fromAddr, from);
	cell_t *toAddr;
	pContext->LocalToPhysAddr(params[3], &toAddr);
	Vector to;
	PawnVectorToVector(toAddr, to);
	return (cell_t)(pLocomotion->HasPotentialGap(from, to));
}

LOCOMOTIONNATIVE(IsGap)
	cell_t *fromAddr;
	pContext->LocalToPhysAddr(params[2], &fromAddr);
	Vector from;
	PawnVectorToVector(fromAddr, from);
	cell_t *toAddr;
	pContext->LocalToPhysAddr(params[3], &toAddr);
	Vector to;
	PawnVectorToVector(toAddr, to);
	return (cell_t)(pLocomotion->IsGap(from, to));
}

LOCOMOTIONNATIVE(IsEntityTraversable)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pLocomotion->IsEntityTraversable(pEntity, (ILocomotion::TraverseWhenType)(params[3])));
}

LOCOMOTIONNATIVE(IsStuck)
	return (cell_t)(pLocomotion->IsStuck());
}

LOCOMOTIONNATIVE(GetStuckDuration)
	return sp_ftoc(pLocomotion->GetStuckDuration());
}

LOCOMOTIONNATIVE(ClearStuckStatus)
	char *name;
	pContext->LocalToString(params[2], &name);
	pLocomotion->ClearStuckStatus(name);
	return 0;
}

LOCOMOTIONNATIVE(IsAttemptingToMove)
	return (cell_t)(pLocomotion->IsAttemptingToMove());
}

LOCOMOTIONNATIVE(ShouldCollideWith)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pLocomotion->ShouldCollideWith(pEntity));
}

LOCOMOTIONNATIVE(AdjustPosture)
	cell_t *leanAddr;
	pContext->LocalToPhysAddr(params[2], &leanAddr);
	Vector lean;
	PawnVectorToVector(leanAddr, lean);
	pLocomotion->AdjustPosture(lean);
	return 0;
}

LOCOMOTIONNATIVE(StuckMonitor)
	pLocomotion->StuckMonitor();
	return 0;
}

NEXTBOTGROUNDLOCOMOTIONNATIVE(SetAcceleration)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	pLocomotion->SetAcceleration(dst);
	return 0;
}

NEXTBOTGROUNDLOCOMOTIONNATIVE(SetVelocity)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	pLocomotion->SetVelocity(dst);
	return 0;
}

NEXTBOTGROUNDLOCOMOTIONNATIVE(GetAcceleration)
	cell_t *velAddr;
	pContext->LocalToPhysAddr(params[2], &velAddr);
	Vector vel = pLocomotion->GetAcceleration();
	VectorToPawnVector(velAddr, vel);
	return 0;
}

NEXTBOTGROUNDLOCOMOTIONNATIVE(GetGravity)
	return sp_ftoc(pLocomotion->GetGravity());
}

NEXTBOTGROUNDLOCOMOTIONNATIVE(GetFrictionForward)
	return sp_ftoc(pLocomotion->GetFrictionForward());
}

NEXTBOTGROUNDLOCOMOTIONNATIVE(GetFrictionSideways)
	return sp_ftoc(pLocomotion->GetFrictionSideways());
}

NEXTBOTGROUNDLOCOMOTIONNATIVE(GetMaxYawRate)
	return sp_ftoc(pLocomotion->GetMaxYawRate());
}

#endif