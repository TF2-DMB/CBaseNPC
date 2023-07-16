#include "shared/cbasenpc.h"
#include "cbasenpc_internal.h"
#include "npc_tools_internal.h"
#include "cbasenpc.hpp"

namespace natives::cbasenpc {

namespace thenpcs {

cell_t IsValidNPC(IPluginContext* context, const cell_t * params) {
	if (params[2] < 0 || params[2] >= MAX_NPCS) {
		return 0;
	}	

	CExtNPC *npc = g_objNPC[params[2]];
	if (!npc || !(npc->GetEntity())) {
		return 0;
	}

	return 1;
}

cell_t FindNPCByEntIndex(IPluginContext* context, const cell_t * params) {
	if (params[2] <= 0 || params[2] >= (sizeof(g_objNPC2) / sizeof(CExtNPC*))) {
		return INVALID_NPC_ID;
	}

	CBaseEntity* ent = gamehelpers->ReferenceToEntity(params[2]);
	if (!ent || !g_objNPC2[params[2]] || g_objNPC2[params[2]]->GetEntity() != ent) {
		return INVALID_NPC_ID;
	}

	return g_objNPC2[params[2]]->GetID();
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CNPCs.FindNPCByEntIndex", FindNPCByEntIndex},
		{"CNPCs.IsValidNPC", IsValidNPC},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}

namespace locomotion {

inline CBaseNPC_Locomotion* Get(IPluginContext* context, const cell_t param) {	
	CBaseNPC_Locomotion* mover = (CBaseNPC_Locomotion*)PawnAddressToPtr(param);
	if (!mover) {
		context->ThrowNativeError("Invalid Locomotion %i", param);
		return nullptr;
	}
	return mover;
}

cell_t SetCallback(IPluginContext* context, const cell_t * params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	CBaseNPC_Locomotion::CallbackType cbType = (CBaseNPC_Locomotion::CallbackType)params[2];
	IPluginFunction* pCallback = context->GetFunctionById( params[3] );

	mover->SetCallback(cbType, pCallback);
	return 0;
}

inline bool ExpectParamCount(IPluginContext* context, const cell_t *params, cell_t expected, bool hasThis) {
	if (params[0] < expected) {
		context->ReportError("Not enough parameters (expected %d, got %d)", 
			hasThis ? expected - 1 : expected, 
			hasThis ? params[0] - 1 : params[0]);
		return false;
	}

	return true;
}

cell_t CallBaseFunction(IPluginContext* context, const cell_t* params) {
	auto mover = Get(context, params[1]);
	if (!mover) {
		return 0;
	}

	if (!mover->IsInCallback()) {
		context->ReportError("CallBaseFunction() can only be used within a callback");
		return 0;
	}

	CBaseNPC_Locomotion::CallbackType cbType = mover->GetCurrentCallbackType();

	cell_t result = 0;
	cell_t expectedParams = 0;

	switch (cbType) {
		case CBaseNPC_Locomotion::CallbackType_IsAbleToJumpAcrossGaps: {
			result = mover->DefaultIsAbleToJumpAcrossGaps();
			} break;
		
		case CBaseNPC_Locomotion::CallbackType_IsJumpingAcrossGap: {
			result = mover->DefaultIsJumpingAcrossGap();
			} break;
		
		case CBaseNPC_Locomotion::CallbackType_JumpAcrossGap: {
			if (!ExpectParamCount(context, params, 3, true)) {
				return 0;
			}

			cell_t* goalAddr;
			cell_t* forwardAddr;
			context->LocalToPhysAddr(params[2], &goalAddr);
			context->LocalToPhysAddr(params[3], &forwardAddr);

			Vector landingGoal;
			Vector landingForward;
			PawnVectorToVector(goalAddr, &landingGoal);
			PawnVectorToVector(forwardAddr, &landingForward);

			mover->DefaultJumpAcrossGap(landingGoal, landingForward);
			} break;

		case CBaseNPC_Locomotion::CallbackType_IsAbleToClimb: {
			result = mover->DefaultIsAbleToClimb();
			} break;
		
		case CBaseNPC_Locomotion::CallbackType_IsClimbingUpToLedge: {
			result = mover->DefaultIsClimbingUpToLedge();
			} break;
		
		case CBaseNPC_Locomotion::CallbackType_ClimbUpToLedge: {
			if (!ExpectParamCount(context, params, 4, true)) {
				return 0;
			}

			cell_t* goalAddr;
			cell_t* forwardAddr;
			cell_t* entityAddr;
			context->LocalToPhysAddr(params[2], &goalAddr);
			context->LocalToPhysAddr(params[3], &forwardAddr);
			context->LocalToPhysAddr(params[4], &entityAddr);
			CBaseEntity* entity = gamehelpers->ReferenceToEntity(*entityAddr);

			Vector vecGoal;
			Vector vecForward;
			PawnVectorToVector(goalAddr, &vecGoal);
			PawnVectorToVector(forwardAddr, &vecForward);

			result = mover->DefaultClimbUpToLedge(vecGoal, vecForward, entity);
			} break;

		case CBaseNPC_Locomotion::CallbackType_ShouldCollideWith: {
			if (!ExpectParamCount(context, params, 2, true)) {
				return 0;
			}

			cell_t *colliderAddr;
			context->LocalToPhysAddr(params[2], &colliderAddr);

			CBaseEntity* collider = gamehelpers->ReferenceToEntity(*colliderAddr);

			if (!collider)
			{
				context->ReportError("Invalid entity index/reference %d", *colliderAddr);
				return 0;
			}

			result = mover->DefaultShouldCollideWith(collider);
			} break;

		case CBaseNPC_Locomotion::CallbackType_IsEntityTraversable: {
			if (!ExpectParamCount(context, params, 3, true)) {
				return 0;
			}

			cell_t *obstacleAddr;
			cell_t *whenAddr;
			context->LocalToPhysAddr(params[2], &obstacleAddr);
			context->LocalToPhysAddr(params[3], &whenAddr);

			CBaseEntity* obstacle = gamehelpers->ReferenceToEntity(*obstacleAddr);

			if (!obstacle)
			{
				context->ReportError("Invalid entity index/reference %d", *obstacleAddr);
				return 0;
			}

			ILocomotion::TraverseWhenType when = (ILocomotion::TraverseWhenType)(*whenAddr);

			result = mover->DefaultIsEntityTraversable(obstacle, when);
			} break;
	}

	return result;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CBaseNPC_Locomotion.SetCallback", SetCallback},
		{"CBaseNPC_Locomotion.CallBaseFunction", CallBaseFunction},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}

cell_t GetEntity(IPluginContext* context, const cell_t * params) {
	CExtNPC* npc = g_objNPC[params[1]];
	if (!npc) {
		return context->ThrowNativeError("Invalid NPC %i", params[1]);
	}
	return gamehelpers->EntityToBCompatRef(npc->GetEntity());
}

inline CBaseNPC_Entity::CBaseNPC* Get(IPluginContext* context, const cell_t param) {	
	CBaseNPC_Entity::CBaseNPC* npc = (CBaseNPC_Entity::CBaseNPC*)(g_objNPC[param]);
	if (!npc) {
		context->ThrowNativeError("Invalid NPC %d", param);
		return nullptr;
	}
	return npc;
}

cell_t CBaseNPCCtor(IPluginContext* context, const cell_t * params) {
	CBaseNPC_Entity *npc = (CBaseNPC_Entity*)servertools->CreateEntityByName("base_npc");
	if (!npc) {
		return INVALID_NPC_ID;
	}

	int index = npc->GetNPC()->GetID();
	if (index == INVALID_NPC_ID) {
		servertools->RemoveEntityImmediate(npc);
		return INVALID_NPC_ID;
	}
	return index;
}

cell_t GetBot(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return PtrToPawnAddress(npc->GetEntity()->MyNextBotPointer());
}

cell_t GetLocomotion(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return PtrToPawnAddress(npc->m_pMover);
}

cell_t GetBody(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return PtrToPawnAddress(npc->m_pBody);
}

cell_t GetVision(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return PtrToPawnAddress(npc->GetEntity()->MyNextBotPointer()->GetVisionInterface());
}

cell_t GetIntention(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return PtrToPawnAddress(npc->GetEntity()->MyNextBotPointer()->GetIntentionInterface());
}

cell_t SetType(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	char* type = nullptr;
	context->LocalToString(params[2], &type);
	memcpy(npc->m_type, type, 64); // Not sure if this is ideal
	return 0;
}

cell_t GetType(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	context->StringToLocal(params[2], params[3], npc->m_type);
	return 0;
}

cell_t SetBodyMins(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	cell_t* addVec = nullptr;
	context->LocalToPhysAddr(params[2], &addVec);

	Vector mins;
	PawnVectorToVector(addVec, mins);
	npc->m_pBody->m_vecBodyMins = mins;

	return 0;
}

cell_t SetBodyMaxs(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	cell_t* addVec = nullptr;
	context->LocalToPhysAddr(params[2], &addVec);

	Vector maxs;
	PawnVectorToVector(addVec, maxs);
	npc->m_pBody->m_vecBodyMaxs = maxs;

	return 0;
}

cell_t GetBodyMins(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	cell_t* addVec = nullptr;
	context->LocalToPhysAddr(params[2], &addVec);
	VectorToPawnVector(addVec, npc->m_pBody->m_vecBodyMins);
	return 0;
}

cell_t GetBodyMaxs(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	cell_t* addVec = nullptr;
	context->LocalToPhysAddr(params[2], &addVec);
	VectorToPawnVector(addVec, npc->m_pBody->m_vecBodyMaxs);
	return 0;
}

cell_t GetflStepSize(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}
	
	return sp_ftoc(npc->m_pMover->m_flStepSize);
}

cell_t SetflStepSize(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flStepSize = sp_ctof(params[2]);
	return 0;
}

cell_t GetflGravity(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flGravity);
}

cell_t SetflGravity(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flGravity = sp_ctof(params[2]);
	return 0;
}

cell_t GetflAcceleration(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flAcceleration);
}

cell_t SetflAcceleration(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flAcceleration = sp_ctof(params[2]);
	return 0;
}

cell_t GetflJumpHeight(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flJumpHeight);
}

cell_t SetflJumpHeight(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flJumpHeight = sp_ctof(params[2]);
	return 0;
}

cell_t GetflDeathDropHeight(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flDeathDropHeight);
}

cell_t SetflDeathDropHeight(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flDeathDropHeight = sp_ctof(params[2]);
	return 0;
}

cell_t GetflWalkSpeed(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flWalkSpeed);
}

cell_t SetflWalkSpeed(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flWalkSpeed = sp_ctof(params[2]);
	return 0;
}

cell_t GetflRunSpeed(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flRunSpeed);
}

cell_t SetflRunSpeed(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flRunSpeed = sp_ctof(params[2]);
	return 0;
}

cell_t GetflFrictionForward(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flFrictionForward);
}

cell_t SetflFrictionForward(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flFrictionForward = sp_ctof(params[2]);
	return 0;
}

cell_t GetflFrictionSideways(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flFrictionSideways);
}

cell_t SetflFrictionSideways(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flFrictionSideways = sp_ctof(params[2]);
	return 0;
}

cell_t GetflMaxYawRate(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	return sp_ftoc(npc->m_pMover->m_flMaxYawRate);
}

cell_t SetflMaxYawRate(IPluginContext* context, const cell_t* params) {
	auto npc = Get(context, params[1]);
	if (!npc) {
		return 0;
	}

	npc->m_pMover->m_flMaxYawRate = sp_ctof(params[2]);
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	locomotion::setup(natives);
	thenpcs::setup(natives);

	sp_nativeinfo_t list[] = {
		{"CExtNPC.GetEntity", GetEntity},

		{"CBaseNPC.CBaseNPC", CBaseNPCCtor},
		{"CBaseNPC.GetBot", GetBot},
		{"CBaseNPC.GetLocomotion", GetLocomotion},
		{"CBaseNPC.GetBody", GetBody},
		{"CBaseNPC.GetVision", GetVision},
		{"CBaseNPC.GetIntention", GetIntention},
		{"CBaseNPC.SetType", SetType},
		{"CBaseNPC.GetType", GetType},
		{"CBaseNPC.SetBodyMins", SetBodyMins},
		{"CBaseNPC.SetBodyMaxs", SetBodyMaxs},
		{"CBaseNPC.GetBodyMins", GetBodyMins},
		{"CBaseNPC.GetBodyMaxs", GetBodyMaxs},
		{"CBaseNPC.flStepSize.get", GetflStepSize},
		{"CBaseNPC.flStepSize.set", SetflStepSize},
		{"CBaseNPC.flGravity.get", GetflGravity},
		{"CBaseNPC.flGravity.set", SetflGravity},
		{"CBaseNPC.flAcceleration.get", GetflAcceleration},
		{"CBaseNPC.flAcceleration.set", SetflAcceleration},
		{"CBaseNPC.flJumpHeight.get", GetflJumpHeight},
		{"CBaseNPC.flJumpHeight.set", SetflJumpHeight},
		{"CBaseNPC.flDeathDropHeight.get", GetflDeathDropHeight},
		{"CBaseNPC.flDeathDropHeight.set", SetflDeathDropHeight},
		{"CBaseNPC.flWalkSpeed.get", GetflWalkSpeed},
		{"CBaseNPC.flWalkSpeed.set", SetflWalkSpeed},
		{"CBaseNPC.flRunSpeed.get", GetflRunSpeed},
		{"CBaseNPC.flRunSpeed.set", SetflRunSpeed},
		{"CBaseNPC.flFrictionForward.get", GetflFrictionForward},
		{"CBaseNPC.flFrictionForward.set", SetflFrictionForward},
		{"CBaseNPC.flFrictionSideways.get", GetflFrictionSideways},
		{"CBaseNPC.flFrictionSideways.set", SetflFrictionSideways},
		{"CBaseNPC.flMaxYawRate.get", GetflMaxYawRate},
		{"CBaseNPC.flMaxYawRate.set", SetflMaxYawRate},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}