#include "ground.hpp"

#include "NextBotGroundLocomotion.h"

namespace natives::nextbot::locomotion::ground  {

inline NextBotGroundLocomotion* Get(IPluginContext* context, const cell_t param) {
	NextBotGroundLocomotion* mover = (NextBotGroundLocomotion*)PawnAddressToPtr(param);
	if (!mover) {
		context->ThrowNativeError("NextBotGroundLocomotion ptr is null!");
		return nullptr;
	}
	return mover;
}

cell_t SetAcceleration(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *dstAddr;
	context->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	mover->SetAcceleration(dst);
	return 0;
}

cell_t SetVelocity(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *dstAddr;
	context->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	mover->SetVelocity(dst);
	return 0;
}

cell_t GetAcceleration(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	cell_t *velAddr;
	context->LocalToPhysAddr(params[2], &velAddr);
	Vector vel = mover->GetAcceleration();
	VectorToPawnVector(velAddr, vel);
	return 0;
}

cell_t GetGravity(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetGravity());
}

cell_t GetFrictionForward(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetFrictionForward());
}

cell_t GetFrictionSideways(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetFrictionSideways());
}

cell_t GetMaxYawRate(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	return sp_ftoc(mover->GetMaxYawRate());
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"NextBotGroundLocomotion.GetAcceleration", GetAcceleration},
		{"NextBotGroundLocomotion.SetAcceleration", SetAcceleration},
		{"NextBotGroundLocomotion.SetVelocity", SetVelocity},
		{"NextBotGroundLocomotion.GetGravity", GetGravity},
		{"NextBotGroundLocomotion.GetFrictionForward", GetFrictionForward},
		{"NextBotGroundLocomotion.GetFrictionSideways", GetFrictionSideways},
		{"NextBotGroundLocomotion.GetMaxYawRate", GetMaxYawRate},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}