#ifndef NATIVES_BODY_H_INCLUDED_
#define NATIVES_BODY_H_INCLUDED_

#pragma once

#include "NextBotBodyInterface.h"

#define BODYNATIVE(name) \
	cell_t IBody_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		IBody *pBody = (IBody *)(params[1]); \
		if(!pBody) { \
			return pContext->ThrowNativeError("Invalid IBody %x", params[1]); \
		} \
//Todo re-add support for it
/*
class PluginBotReply : public INextBotReply
{
	public:
		PluginBotReply(IPluginFunction *OnSuccess, IPluginFunction *OnFail) : m_pOnSuccessFunc(OnSuccess), m_pOnFailFunc(OnFail) {}

		virtual void OnSuccess(INextBot *nextbot) override
		{
			m_pOnSuccessFunc->PushCell((cell_t)nextbot);
			m_pOnSuccessFunc->Execute(nullptr);
		}

		virtual void OnFail(INextBot *nextbot, FailureReason reason) override
		{
			m_pOnFailFunc->PushCell((cell_t)nextbot);
			m_pOnFailFunc->PushCell((cell_t)reason);
			m_pOnFailFunc->Execute(nullptr);
		}

	private:
		IPluginFunction *m_pOnSuccessFunc;
		IPluginFunction *m_pOnFailFunc;
};

cell_t PluginBot_CreateReply(IPluginContext *pContext, const cell_t *params)
{
	PluginBotReply *pReply = new PluginBotReply(pContext->GetFunctionById(params[1]), pContext->GetFunctionById(params[2]));
	return CREATEHANDLE(HANDLENAME(PluginBotReply), pReply)
}

BODYNATIVE(AimHeadTowards)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);
	PluginBotReply *pReply = nullptr;
	HandleSecurity security(pContext->GetIdentity(), myself->GetIdentity());
	READHANDLE(params[1], HANDLENAME(PluginBotReply), pReply)
	char *name;
	pContext->LocalToString(params[6], &name);
	pBody->AimHeadTowards(pos, (IBody::LookAtPriorityType)params[3], sp_ctof(params[4]), pReply, name);
	return 0;
}

BODYNATIVE(AimHeadTowardsEx)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[3]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[3]);
	}
	PluginBotReply *pReply = nullptr;
	HandleSecurity security(pContext->GetIdentity(), myself->GetIdentity());
	READHANDLE(params[1], HANDLENAME(PluginBotReply), pReply)
	char *name;
	pContext->LocalToString(params[6], &name);
	pBody->AimHeadTowards(pEntity, (IBody::LookAtPriorityType)params[3], sp_ctof(params[4]), pReply, name);
	return 0;
}*/

BODYNATIVE(SetPosition)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);
	return (cell_t)(pBody->SetPosition(pos));
}

BODYNATIVE(GetEyePosition)
	cell_t *eyePosAddr;
	pContext->LocalToPhysAddr(params[2], &eyePosAddr);
	Vector eyepos = pBody->GetEyePosition();
	VectorToPawnVector(eyePosAddr, eyepos);
	return 0;
}

BODYNATIVE(GetViewVector)
	cell_t *viewAddr;
	pContext->LocalToPhysAddr(params[2], &viewAddr);
	Vector view = pBody->GetViewVector();
	VectorToPawnVector(viewAddr, view);
	return 0;
}

BODYNATIVE(IsHeadAimingOnTarget)
	return (cell_t)(pBody->IsHeadAimingOnTarget());
}

BODYNATIVE(IsHeadSteady)
	return (cell_t)(pBody->IsHeadSteady());
}

BODYNATIVE(GetHeadSteadyDuration)
	return sp_ftoc(pBody->GetHeadSteadyDuration());
}

BODYNATIVE(GetHeadAimSubjectLeadTime)
	return sp_ftoc(pBody->GetHeadAimSubjectLeadTime());
}

BODYNATIVE(GetHeadAimTrackingInterval)
	return sp_ftoc(pBody->GetHeadAimTrackingInterval());
}

BODYNATIVE(ClearPendingAimReply)
	pBody->ClearPendingAimReply();
	return 0;
}

BODYNATIVE(GetMaxHeadAngularVelocity)
	return sp_ftoc(pBody->GetMaxHeadAngularVelocity());
}
//To-do add activity support
/*
BODYNATIVE(StartActivity)
	return (cell_t)(pBody->StartActivity(static_cast<Activity>(params[2]), (unsigned int)params[3]));
}

BODYNATIVE(SelectAnimationSequence)
	return (cell_t)(pBody->SelectAnimationSequence(static_cast<Activity>(params[2])));
}*/

//To-do add activity support
/*
BODYNATIVE(GetActivity)
	return (cell_t)(pBody->GetActivity());
}

BODYNATIVE(IsActivity)
	return (cell_t)(pBody->IsActivity(static_cast<Activity>(params[2])));
}

BODYNATIVE(HasActivityType)
	return (cell_t)(pBody->HasActivityType((unsigned int)params[2]));
}
*/
BODYNATIVE(SetDesiredPosture)
	pBody->SetDesiredPosture((IBody::PostureType)params[2]);
	return 0;
}

BODYNATIVE(GetDesiredPosture)
	return (cell_t)(pBody->GetDesiredPosture());
}

BODYNATIVE(IsDesiredPosture)
	return (cell_t)(pBody->IsDesiredPosture((IBody::PostureType)params[2]));
}

BODYNATIVE(IsInDesiredPosture)
	return (cell_t)(pBody->IsInDesiredPosture());
}

BODYNATIVE(GetActualPosture)
	return (cell_t)(pBody->GetActualPosture());
}

BODYNATIVE(IsActualPosture)
	return (cell_t)(pBody->IsActualPosture((IBody::PostureType)params[2]));
}

BODYNATIVE(IsPostureMobile)
	return (cell_t)(pBody->IsPostureMobile());
}

BODYNATIVE(IsPostureChanging)
	return (cell_t)(pBody->IsPostureChanging());
}

BODYNATIVE(SetArousal)
	pBody->SetArousal((IBody::ArousalType)params[2]);
	return 0;
}

BODYNATIVE(IsArousal)
	return (cell_t)(pBody->IsArousal((IBody::ArousalType)params[2]));
}

BODYNATIVE(GetArousal)
	return (cell_t)(pBody->GetArousal());
}

BODYNATIVE(GetHullWidth)
	return sp_ftoc(pBody->GetHullWidth());
}

BODYNATIVE(GetHullHeight)
	return sp_ftoc(pBody->GetHullHeight());
}

BODYNATIVE(GetStandHullHeight)
	return sp_ftoc(pBody->GetStandHullHeight());
}

BODYNATIVE(GetCrouchHullHeight)
	return sp_ftoc(pBody->GetCrouchHullHeight());
}

BODYNATIVE(GetHullMins)
	cell_t *minsAddr;
	pContext->LocalToPhysAddr(params[2], &minsAddr);
	Vector mins = pBody->GetHullMins();
	VectorToPawnVector(minsAddr, mins);
	return 0;
}

BODYNATIVE(GetHullMaxs)
	cell_t *maxsAddr;
	pContext->LocalToPhysAddr(params[2], &maxsAddr);
	Vector maxs = pBody->GetHullMaxs();
	VectorToPawnVector(maxsAddr, maxs);
	return 0;
}

BODYNATIVE(GetSolidMask)
	return (cell_t)(pBody->GetSolidMask());
}

BODYNATIVE(GetCollisionGroup)
	return (cell_t)(pBody->GetCollisionGroup());
}

#endif