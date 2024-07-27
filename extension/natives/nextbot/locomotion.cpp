#include "locomotion.hpp"
#include "locomotion/ground.hpp"

#include "NextBotLocomotionInterface.h"

namespace natives::nextbot::locomotion {

inline ILocomotion* Get(IPluginContext* context, const cell_t param) {
	ILocomotion* mover = (ILocomotion*)PawnAddressToPtr(param, context);
	if (!mover) {
		context->ThrowNativeError("Locomotion ptr is null!");
		return nullptr;
	}
	return mover;
}

cell_t Approach(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *dstAddr;
	context->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	mover->Approach(dst, sp_ctof(params[3]));
	return 0;
}

cell_t DriveTo(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *dstAddr;
	context->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	mover->DriveTo(dst);
	return 0;
}

cell_t ClimbUpToLedge(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *dstAddr;
	context->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	cell_t *dirAddr;
	context->LocalToPhysAddr(params[3], &dirAddr);
	Vector dir;
	PawnVectorToVector(dirAddr, dir);
	CBaseEntity *entity = gamehelpers->ReferenceToEntity(params[4]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[4]);
	}
	return (mover->ClimbUpToLedge(dst, dir, entity) == true) ? 1 : 0;
}

cell_t JumpAcrossGap(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *dstAddr;
	context->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	cell_t *dirAddr;
	context->LocalToPhysAddr(params[3], &dirAddr);
	Vector dir;
	PawnVectorToVector(dirAddr, dir);
	mover->JumpAcrossGap(dst, dir);
	return 0;
}

cell_t Jump(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->Jump();
	return 0;
}

cell_t IsClimbingOrJumping(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsClimbingOrJumping() == true) ? 1 : 0;
}

cell_t IsClimbingUpToLedge(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsClimbingUpToLedge() == true) ? 1 : 0;
}

cell_t IsJumpingAcrossGap(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsJumpingAcrossGap() == true) ? 1 : 0;
}

cell_t IsScrambling(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsScrambling() == true) ? 1 : 0;
}

cell_t Run(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->Run();
	return 0;
}

cell_t Walk(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->Walk();
	return 0;
}

cell_t Stop(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->Stop();
	return 0;
}

cell_t IsRunning(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsRunning() == true) ? 1 : 0;
}

cell_t SetDesiredSpeed(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->SetDesiredSpeed(sp_ctof(params[2]));
	return 0;
}

cell_t GetDesiredSpeed(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetDesiredSpeed());
}

cell_t SetSpeedLimit(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->SetSpeedLimit(sp_ctof(params[2]));
	return 0;
}

cell_t GetSpeedLimit(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetSpeedLimit());
}

cell_t IsOnGround(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsOnGround() == true) ? 1 : 0;
}

cell_t GetGround(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return gamehelpers->EntityToBCompatRef(mover->GetGround());
}

cell_t GetGroundNormal(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *normalAddr;
	context->LocalToPhysAddr(params[2], &normalAddr);
	Vector normal = mover->GetGroundNormal();
	VectorToPawnVector(normalAddr, normal);
	return 0;
}

cell_t GetGroundSpeed(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetGroundSpeed());
}

cell_t GetGroundMotionVector(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *motionAddr;
	context->LocalToPhysAddr(params[2], &motionAddr);
	Vector motion = mover->GetGroundMotionVector();
	VectorToPawnVector(motionAddr, motion);
	return 0;
}

cell_t ClimbLadder(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->ClimbLadder((CNavLadder*)PawnAddressToPtr(params[2], context), (CNavArea*)PawnAddressToPtr(params[3], context));
	return 0;
}

cell_t DescendLadder(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->DescendLadder((CNavLadder*)PawnAddressToPtr(params[2], context), (CNavArea*)PawnAddressToPtr(params[3], context));
	return 0;
}

cell_t IsUsingLadder(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsUsingLadder() == true) ? 1 : 0;
}

cell_t IsAscendingOrDescendingLadder(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsAscendingOrDescendingLadder() == true) ? 1 : 0;
}

cell_t IsAbleToAutoCenterOnLadder(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsAbleToAutoCenterOnLadder() == true) ? 1 : 0;
}

cell_t FaceTowards(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *dstAddr;
	context->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	mover->FaceTowards(dst);
	return 0;
}

cell_t SetDesiredLean(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *leanAddr;
	context->LocalToPhysAddr(params[2], &leanAddr);
	QAngle lean;
	PawnVectorToVector(leanAddr, lean);
	mover->SetDesiredLean(lean);
	return 0;
}

cell_t GetDesiredLean(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *leanAddr;
	context->LocalToPhysAddr(params[2], &leanAddr);
	QAngle lean = mover->GetDesiredLean();
	VectorToPawnVector(leanAddr, lean);
	return 0;
}

cell_t IsAbleToJumpAcrossGaps(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsAbleToJumpAcrossGaps() == true) ? 1 : 0;
}

cell_t IsAbleToClimb(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsAbleToClimb() == true) ? 1 : 0;
}

cell_t GetFeet(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *feetAddr;
	context->LocalToPhysAddr(params[2], &feetAddr);
	Vector feet = mover->GetFeet();
	VectorToPawnVector(feetAddr, feet);
	return 0;
}

cell_t GetStepHeight(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetStepHeight());
}

cell_t GetMaxJumpHeight(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetMaxJumpHeight());
}

cell_t GetDeathDropHeight(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetDeathDropHeight());
}

cell_t GetRunSpeed(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetRunSpeed());
}

cell_t GetWalkSpeed(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetWalkSpeed());
}

cell_t GetMaxAcceleration(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetMaxAcceleration());
}

cell_t GetMaxDeceleration(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetMaxDeceleration());
}

cell_t GetVelocity(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *velAddr;
	context->LocalToPhysAddr(params[2], &velAddr);
	Vector vel = mover->GetVelocity();
	VectorToPawnVector(velAddr, vel);
	return 0;
}

cell_t GetSpeed(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetSpeed());
}

cell_t GetMotionVector(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *motionAddr;
	context->LocalToPhysAddr(params[2], &motionAddr);
	Vector motion = mover->GetMotionVector();
	VectorToPawnVector(motionAddr, motion);
	return 0;
}

cell_t IsAreaTraversable(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsAreaTraversable((CNavArea *)PawnAddressToPtr(params[2], context)) == true) ? 1 : 0;
}

cell_t GetTraversableSlopeLimit(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetTraversableSlopeLimit());
}

cell_t IsPotentiallyTraversable(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *fromAddr;
	context->LocalToPhysAddr(params[2], &fromAddr);
	Vector from;
	PawnVectorToVector(fromAddr, from);
	cell_t *toAddr;
	context->LocalToPhysAddr(params[3], &toAddr);
	Vector to;
	PawnVectorToVector(toAddr, to);
	ILocomotion::TraverseWhenType when = (ILocomotion::TraverseWhenType)params[4];
	cell_t *fracAddr;
	context->LocalToPhysAddr(params[5], &fracAddr);

	float fraction;
	bool result = mover->IsPotentiallyTraversable(from, to, when, &fraction);
	*fracAddr = sp_ftoc(fraction);
	return result;
}

cell_t HasPotentialGap(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *fromAddr;
	context->LocalToPhysAddr(params[2], &fromAddr);
	Vector from;
	PawnVectorToVector(fromAddr, from);
	cell_t *toAddr;
	context->LocalToPhysAddr(params[3], &toAddr);
	Vector to;
	PawnVectorToVector(toAddr, to);
	cell_t *fracAddr;
	context->LocalToPhysAddr(params[4], &fracAddr);

	float fraction;
	bool result = mover->HasPotentialGap(from, to, &fraction);
	*fracAddr = sp_ftoc(fraction);
	return result;
}

cell_t IsGap(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *fromAddr;
	context->LocalToPhysAddr(params[2], &fromAddr);
	Vector from;
	PawnVectorToVector(fromAddr, from);
	cell_t *toAddr;
	context->LocalToPhysAddr(params[3], &toAddr);
	Vector to;
	PawnVectorToVector(toAddr, to);
	return (mover->IsGap(from, to) == true) ? 1 : 0;
}

cell_t IsEntityTraversable(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (mover->IsEntityTraversable(pEntity, (ILocomotion::TraverseWhenType)(params[3])) == true) ? 1 : 0;
}

cell_t IsStuck(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsStuck() == true) ? 1 : 0;
}

cell_t GetStuckDuration(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetStuckDuration());
}

cell_t ClearStuckStatus(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	char *name;
	context->LocalToString(params[2], &name);
	mover->ClearStuckStatus(name);
	return 0;
}

cell_t IsAttemptingToMove(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return (mover->IsAttemptingToMove() == true) ? 1 : 0;
}

cell_t ShouldCollideWith(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (mover->ShouldCollideWith(pEntity) == true) ? 1 : 0;
}

cell_t AdjustPosture(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *leanAddr;
	context->LocalToPhysAddr(params[2], &leanAddr);
	Vector lean;
	PawnVectorToVector(leanAddr, lean);
	mover->AdjustPosture(lean);
	return 0;
}

cell_t StuckMonitor(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	mover->StuckMonitor();
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	ground::setup(natives);
	
	sp_nativeinfo_t list[] = {
		{"ILocomotion.Approach", Approach},
		{"ILocomotion.DriveTo", DriveTo},
		{"ILocomotion.ClimbUpToLedge", ClimbUpToLedge},
		{"ILocomotion.JumpAcrossGap", JumpAcrossGap},
		{"ILocomotion.Jump", Jump},
		{"ILocomotion.IsClimbingOrJumping", IsClimbingOrJumping},
		{"ILocomotion.IsClimbingUpToLedge", IsClimbingUpToLedge},
		{"ILocomotion.IsJumpingAcrossGap", IsJumpingAcrossGap},
		{"ILocomotion.IsScrambling", IsScrambling},
		{"ILocomotion.Run", Run},
		{"ILocomotion.Walk", Walk},
		{"ILocomotion.Stop", Stop},
		{"ILocomotion.IsRunning", IsRunning},
		{"ILocomotion.SetDesiredSpeed", SetDesiredSpeed},
		{"ILocomotion.GetDesiredSpeed", GetDesiredSpeed},
		{"ILocomotion.SetSpeedLimit", SetSpeedLimit},
		{"ILocomotion.GetSpeedLimit", GetSpeedLimit},
		{"ILocomotion.IsOnGround", IsOnGround},
		{"ILocomotion.GetGround", GetGround},
		{"ILocomotion.GetGroundNormal", GetGroundNormal},
		{"ILocomotion.GetGroundSpeed", GetGroundSpeed},
		{"ILocomotion.GetGroundMotionVector", GetGroundMotionVector},
		{"ILocomotion.ClimbLadder", ClimbLadder},
		{"ILocomotion.DescendLadder", DescendLadder},
		{"ILocomotion.IsUsingLadder", IsUsingLadder},
		{"ILocomotion.IsAscendingOrDescendingLadder", IsAscendingOrDescendingLadder},
		{"ILocomotion.IsAbleToAutoCenterOnLadder", IsAbleToAutoCenterOnLadder},
		{"ILocomotion.FaceTowards", FaceTowards},
		{"ILocomotion.SetDesiredLean", SetDesiredLean},
		{"ILocomotion.GetDesiredLean", GetDesiredLean},
		{"ILocomotion.IsAbleToJumpAcrossGaps", IsAbleToJumpAcrossGaps},
		{"ILocomotion.IsAbleToClimb", IsAbleToClimb},
		{"ILocomotion.GetFeet", GetFeet},
		{"ILocomotion.GetStepHeight", GetStepHeight},
		{"ILocomotion.GetMaxJumpHeight", GetMaxJumpHeight},
		{"ILocomotion.GetDeathDropHeight", GetDeathDropHeight},
		{"ILocomotion.GetRunSpeed", GetRunSpeed},
		{"ILocomotion.GetWalkSpeed", GetWalkSpeed},
		{"ILocomotion.GetMaxAcceleration", GetMaxAcceleration},
		{"ILocomotion.GetMaxDeceleration", GetMaxDeceleration},
		{"ILocomotion.GetVelocity", GetVelocity},
		{"ILocomotion.GetSpeed", GetSpeed},
		{"ILocomotion.GetMotionVector", GetMotionVector},
		{"ILocomotion.IsAreaTraversable", IsAreaTraversable},
		{"ILocomotion.GetTraversableSlopeLimit", GetTraversableSlopeLimit},
		{"ILocomotion.IsPotentiallyTraversable", IsPotentiallyTraversable},
		{"ILocomotion.HasPotentialGap", HasPotentialGap},
		{"ILocomotion.IsGap", IsGap},
		{"ILocomotion.IsEntityTraversable", IsEntityTraversable},
		{"ILocomotion.IsStuck", IsStuck},
		{"ILocomotion.GetStuckDuration", GetStuckDuration},
		{"ILocomotion.ClearStuckStatus", ClearStuckStatus},
		{"ILocomotion.IsAttemptingToMove", IsAttemptingToMove},
		{"ILocomotion.ShouldCollideWith", ShouldCollideWith},
		{"ILocomotion.AdjustPosture", AdjustPosture},
		{"ILocomotion.StuckMonitor", StuckMonitor},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}