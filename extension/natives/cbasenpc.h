#ifndef NATIVE_CBASENPC_H_
#define NATIVE_CBASENPC_H_

#pragma once

#include "shared/cbasenpc.h"
#include "cbasenpc_internal.h"
#include "npc_tools_internal.h"

#define CEXTNPCNATIVE(name) \
	cell_t CExtNPC_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CExtNPC* npc = g_objNPC[params[1]]; \
		if (!npc) \
		{ \
			return pContext->ThrowNativeError("Invalid NPC %i", params[1]); \
		} 

#define CBASENPCNATIVE(name) \
	cell_t CBaseNPC_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CBaseNPC_Entity::CBaseNPC* npc = (CBaseNPC_Entity::CBaseNPC *)(g_objNPC[params[1]]); \
		if (!npc) \
		{ \
			return pContext->ThrowNativeError("Invalid NPC %i", params[1]); \
		} 

#define CBASENPCLOCONATIVE(name) \
	cell_t CBaseNPC_Locomotion_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CBaseNPC_Locomotion* loco = (CBaseNPC_Locomotion *)params[1]; \
		if (!loco) { return pContext->ThrowNativeError("Invalid Locomotion %i", params[1]); }

#define CNPCSNATIVE(name) \
	cell_t CNPCs_##name(IPluginContext *pContext, const cell_t *params) \
	{ 

CEXTNPCNATIVE(GetEntity)
	CBaseEntity* ent = npc->GetEntity();
	if (!ent)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(ent);
}

cell_t CBaseNPC_CBaseNPC(IPluginContext * pContext, const cell_t * params)
{
	CBaseNPC_Entity *npc = (CBaseNPC_Entity*)servertools->CreateEntityByName("base_npc");
	if (!npc)
	{
		return INVALID_NPC_ID;
	}

	int index = npc->GetNPC()->GetID();
	if (index == INVALID_NPC_ID)
	{
		servertools->RemoveEntityImmediate(npc);
		return INVALID_NPC_ID;
	}
	return index;
}

CBASENPCNATIVE(GetBot)
	return (cell_t)npc->GetEntity()->MyNextBotPointer();
}

CBASENPCNATIVE(GetLocomotion)
	return (cell_t)npc->m_pMover;
}

CBASENPCNATIVE(GetBody)
	return (cell_t)npc->m_pBody;
}

CBASENPCNATIVE(GetVision)
	return (cell_t)npc->GetEntity()->MyNextBotPointer()->GetVisionInterface();
}

CBASENPCNATIVE(GetIntention)
	return (cell_t)npc->GetEntity()->MyNextBotPointer()->GetIntentionInterface();
}

CBASENPCNATIVE(SetType)
	char* type = nullptr;
	pContext->LocalToString(params[2], &type);
	memcpy(npc->m_type, type, 64); // Not sure if this is ideal
	return 0;
}

CBASENPCNATIVE(GetType)
	// May your soul rest in peace if your string isn't 64 bytes long...
	pContext->StringToLocal(params[2], 64, npc->m_type);
	return 0;
}

CBASENPCNATIVE(SetBodyMins)
	cell_t* addVec = nullptr;
	pContext->LocalToPhysAddr(params[2], &addVec);

	Vector mins;
	PawnVectorToVector(addVec, mins);
	npc->m_pBody->m_vecBodyMins = mins;

	return 0;
}

CBASENPCNATIVE(SetBodyMaxs)
	cell_t* addVec = nullptr;
	pContext->LocalToPhysAddr(params[2], &addVec);

	Vector maxs;
	PawnVectorToVector(addVec, maxs);
	npc->m_pBody->m_vecBodyMaxs = maxs;

	return 0;
}

CBASENPCNATIVE(GetBodyMins)
	cell_t* addVec = nullptr;
	pContext->LocalToPhysAddr(params[2], &addVec);
	VectorToPawnVector(addVec, npc->m_pBody->m_vecBodyMins);
	return 0;
}

CBASENPCNATIVE(GetBodyMaxs)
	cell_t* addVec = nullptr;
	pContext->LocalToPhysAddr(params[2], &addVec);
	VectorToPawnVector(addVec, npc->m_pBody->m_vecBodyMaxs);
	return 0;
}

CBASENPCNATIVE(flStepSizeGet)
	return sp_ftoc(npc->m_pMover->m_flStepSize);
}

CBASENPCNATIVE(flStepSizeSet)
	npc->m_pMover->m_flStepSize = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flGravityGet)
	return sp_ftoc(npc->m_pMover->m_flGravity);
}

CBASENPCNATIVE(flGravitySet)
	npc->m_pMover->m_flGravity = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flAccelerationGet)
	return sp_ftoc(npc->m_pMover->m_flAcceleration);
}

CBASENPCNATIVE(flAccelerationSet)
	npc->m_pMover->m_flAcceleration = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flJumpHeightGet)
	return sp_ftoc(npc->m_pMover->m_flJumpHeight);
}

CBASENPCNATIVE(flJumpHeightSet)
	npc->m_pMover->m_flJumpHeight = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flDeathDropHeightGet)
	return sp_ftoc(npc->m_pMover->m_flDeathDropHeight);
}

CBASENPCNATIVE(flDeathDropHeightSet)
	npc->m_pMover->m_flDeathDropHeight = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flWalkSpeedGet)
	return sp_ftoc(npc->m_pMover->m_flWalkSpeed);
}

CBASENPCNATIVE(flWalkSpeedSet)
	npc->m_pMover->m_flWalkSpeed = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flRunSpeedGet)
	return sp_ftoc(npc->m_pMover->m_flRunSpeed);
}

CBASENPCNATIVE(flRunSpeedSet)
	npc->m_pMover->m_flRunSpeed = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flFrictionForwardGet)
	return sp_ftoc(npc->m_pMover->m_flFrictionForward);
}

CBASENPCNATIVE(flFrictionForwardSet)
	npc->m_pMover->m_flFrictionForward = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flFrictionSidewaysGet)
	return sp_ftoc(npc->m_pMover->m_flFrictionSideways);
}

CBASENPCNATIVE(flFrictionSidewaysSet)
	npc->m_pMover->m_flFrictionSideways = sp_ctof(params[2]);
	return 0;
}

CBASENPCNATIVE(flMaxYawRateGet)
	return sp_ftoc(npc->m_pMover->m_flMaxYawRate);
}

CBASENPCNATIVE(flMaxYawRateSet)
	npc->m_pMover->m_flMaxYawRate = sp_ctof(params[2]);
	return 0;
}

CBASENPCLOCONATIVE(SetCallback)
	CBaseNPC_Locomotion::CallbackType cbType = (CBaseNPC_Locomotion::CallbackType)params[2];
	IPluginFunction* pCallback = pContext->GetFunctionById( params[3] );

	loco->SetCallback(cbType, pCallback);
	return 0;
}

CBASENPCLOCONATIVE(CallBaseFunction)
	if (!loco->IsInCallback())
	{
		pContext->ThrowNativeError("CallBaseFunction() can only be used within a callback");
		return 0;
	}

	CBaseNPC_Locomotion::CallbackType cbType = loco->GetCurrentCallbackType();

	cell_t result = 0;

	switch (cbType)
	{
		case CBaseNPC_Locomotion::CallbackType_IsAbleToJumpAcrossGaps:
			result = loco->DefaultIsAbleToJumpAcrossGaps();
			break;
		
		case CBaseNPC_Locomotion::CallbackType_IsJumpingAcrossGap:
			result = loco->DefaultIsJumpingAcrossGap();
			break;
		
		case CBaseNPC_Locomotion::CallbackType_JumpAcrossGap:
		{
			cell_t* goalAddr;
			cell_t* forwardAddr;
			pContext->LocalToPhysAddr(params[2], &goalAddr);
			pContext->LocalToPhysAddr(params[3], &forwardAddr);

			Vector landingGoal;
			Vector landingForward;
			PawnVectorToVector(goalAddr, &landingGoal);
			PawnVectorToVector(forwardAddr, &landingForward);

			loco->DefaultJumpAcrossGap(landingGoal, landingForward);
			break;
		}

		case CBaseNPC_Locomotion::CallbackType_IsAbleToClimb:
			result = loco->DefaultIsAbleToClimb();
			break;
		
		case CBaseNPC_Locomotion::CallbackType_IsClimbingUpToLedge:
			result = loco->DefaultIsClimbingUpToLedge();
			break;
		
		case CBaseNPC_Locomotion::CallbackType_ClimbUpToLedge:
		{
			cell_t* goalAddr;
			cell_t* forwardAddr;
			pContext->LocalToPhysAddr(params[2], &goalAddr);
			pContext->LocalToPhysAddr(params[3], &forwardAddr);
			CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[4]);

			Vector vecGoal;
			Vector vecForward;
			PawnVectorToVector(goalAddr, &vecGoal);
			PawnVectorToVector(forwardAddr, &vecForward);

			result = loco->DefaultClimbUpToLedge(vecGoal, vecForward, pEntity);
			break;
		}

		case CBaseNPC_Locomotion::CallbackType_ShouldCollideWith:
		{
			CBaseEntity* pOther = gamehelpers->ReferenceToEntity(params[2]);
			result = loco->DefaultShouldCollideWith(pOther);
			break;
		}

		case CBaseNPC_Locomotion::CallbackType_IsEntityTraversable:
		{
			CBaseEntity* pOther = gamehelpers->ReferenceToEntity(params[2]);
			ILocomotion::TraverseWhenType when = (ILocomotion::TraverseWhenType)params[3];

			result = loco->DefaultIsEntityTraversable(pOther, when);
			break;
		}
	}

	return result;
}

CNPCSNATIVE(IsValidNPC)
	if (params[2] < 0 || params[2] >= MAX_NPCS)
	{
		return 0;
	}
	CExtNPC *npc = g_objNPC[params[2]];
	if (!npc || !(npc->GetEntity()))
	{
		return 0;
	}

	return 1;
}

CNPCSNATIVE(FindNPCByEntIndex)
	if (params[2] <= 0 || params[2] > 2048)
	{
		return INVALID_NPC_ID;
	}
	CBaseEntity* ent = gamehelpers->ReferenceToEntity(params[2]);
	if (!ent || !g_objNPC2[params[2]] || g_objNPC2[params[2]]->GetEntity() != ent)
	{
		return INVALID_NPC_ID;
	}

	return g_objNPC2[params[2]]->GetID();
}

// Deprecated - These used to be implemented in the deleted cbasenpc.smx plugin
// Now they live inside the extension for backwards compatibility, they will be commented out in the near future

CBASENPCNATIVE(Approach)
	cell_t* dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	npc->m_pMover->Approach(dst, 1.0);
	return 0;
}

CBASENPCNATIVE(FaceTowards)
	cell_t* dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	npc->m_pMover->FaceTowards(dst);
	return 0;
}

CBASENPCNATIVE(Walk)
	npc->m_pMover->Walk();
	return 0;
}

CBASENPCNATIVE(Run)
	npc->m_pMover->Run();
	return 0;
}

CBASENPCNATIVE(Stop)
	npc->m_pMover->Stop();
	return 0;
}

CBASENPCNATIVE(Jump)
	npc->m_pMover->Jump();
	return 0;
}

CBASENPCNATIVE(IsOnGround)
	return npc->m_pMover->IsOnGround();
}

CBASENPCNATIVE(IsClimbingOrJumping)
	return npc->m_pMover->IsClimbingOrJumping();
}

CBASENPCNATIVE(GetVelocity)
	cell_t* velAddr;
	pContext->LocalToPhysAddr(params[2], &velAddr);
	Vector vel = npc->m_pMover->GetVelocity();
	VectorToPawnVector(velAddr, vel);
	return 0;
}

CBASENPCNATIVE(SetVelocity)
	cell_t* dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	npc->m_pMover->SetVelocity(dst);
	return 0;
}

CBASENPCNATIVE(GetLastKnownArea)
	return (cell_t)(((CBaseCombatCharacterHack*)npc->GetEntity())->GetLastKnownArea());
}

CBASENPCNATIVE(Spawn)
	servertools->DispatchSpawn(npc->GetEntity());
	return 0;
}

CBASENPCNATIVE(Teleport)
	cell_t* nullAdd = pContext->GetNullRef(SP_NULL_VECTOR);
	cell_t* originAdd, * angAdd, * velAdd;
	pContext->LocalToPhysAddr(params[2], &originAdd);
	pContext->LocalToPhysAddr(params[3], &angAdd);
	pContext->LocalToPhysAddr(params[4], &velAdd);

	Vector origin = vec3_origin;
	Vector vel = vec3_origin;
	QAngle ang = QAngle(0, 0, 0);
	PawnVectorToVector(originAdd, origin);
	PawnVectorToVector(originAdd, ang);
	PawnVectorToVector(originAdd, vel);
	
	CBaseEntityHack* entity = (CBaseEntityHack*)npc->GetEntity();
	if (!entity)
	{
		return pContext->ThrowNativeError("Invalid linked entity to NPC");
	}
	entity->Teleport((originAdd == nullAdd) ? nullptr : &origin, (angAdd == nullAdd) ? nullptr : &ang, (velAdd == nullAdd) ? nullptr : &vel);
	
	return 0;
}

CBASENPCNATIVE(GetVectors)
	cell_t* nullAdd = pContext->GetNullRef(SP_NULL_VECTOR);
	cell_t* upAdd, * rigthAdd, * forwardAdd;

	pContext->LocalToPhysAddr(params[2], &forwardAdd);
	pContext->LocalToPhysAddr(params[3], &rigthAdd);
	pContext->LocalToPhysAddr(params[4], &upAdd);

	Vector forward, right, up;
	PawnVectorToVector(forwardAdd, forward);
	PawnVectorToVector(rigthAdd, right);
	PawnVectorToVector(upAdd, up);

	((CBaseEntityHack *)npc->GetEntity())->GetVectors((forwardAdd == nullAdd) ? nullptr : &forward, (rigthAdd == nullAdd) ? nullptr : &right, (upAdd == nullAdd) ? nullptr : &up);

	VectorToPawnVector(forwardAdd, forward);
	VectorToPawnVector(rigthAdd, right);
	VectorToPawnVector(upAdd, up);

	return 0;
}

CBASENPCNATIVE(SetModel)
	char* model = nullptr;
	pContext->LocalToString(params[2], &model);
	((CBaseEntityHack *)npc->GetEntity())->SetModel(model);
	return 0;
}



#endif