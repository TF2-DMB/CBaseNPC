
#include "cbasenpc_locomotion.h"

#include "sourcesdk/NextBot/NextBotInterface.h"
#include "sourcesdk/NextBot/Path/NextBotPathFollow.h"

void** CBaseNPC_Locomotion::vtable = nullptr;

MCall<void> CBaseNPC_Locomotion::m_OriginalUpdate;
MCall<bool> CBaseNPC_Locomotion::m_OriginalIsAbleToJumpAcrossGaps;
MCall<bool> CBaseNPC_Locomotion::m_OriginalIsJumpingAcrossGap;
MCall<void, const Vector &, const Vector &> CBaseNPC_Locomotion::m_OriginalJumpAcrossGap;
MCall<bool> CBaseNPC_Locomotion::m_OriginalIsAbleToClimb;
MCall<bool> CBaseNPC_Locomotion::m_OriginalIsClimbingUpToLedge;
MCall<bool, const Vector&, const Vector&, const CBaseEntity*> CBaseNPC_Locomotion::m_OriginalClimbUpToLedge;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetStepHeight;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetMaxJumpHeight;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetDeathDropHeight;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetWalkSpeed;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetRunSpeed;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetMaxAcceleration;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetGravity;
MCall<bool, const CBaseEntity*> CBaseNPC_Locomotion::m_OriginalShouldCollideWith;
MCall<bool, CBaseEntity*, ILocomotion::TraverseWhenType> CBaseNPC_Locomotion::m_OriginalIsEntityTraversable;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetFrictionForward;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetFrictionSideways;
MCall<float> CBaseNPC_Locomotion::m_OriginalGetMaxYawRate;

bool CBaseNPC_Locomotion::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	return true;
}

void** CBaseNPC_Locomotion::CreateVTable(BaseClass* pBase)
{
	if (vtable)
		return vtable;
	
	vtable = vtable_dup(pBase, BaseClass::vtable_entries);

	INextBotComponent::vUpdate.Replace(vtable, &V_Update, m_OriginalUpdate);
	ILocomotion::vClimbUpToLedge.Replace(vtable, &V_ClimbUpToLedge, m_OriginalClimbUpToLedge);
	ILocomotion::vJumpAcrossGap.Replace(vtable, &V_JumpAcrossGap, m_OriginalJumpAcrossGap);
	ILocomotion::vIsClimbingUpToLedge.Replace(vtable, &V_IsClimbingUpToLedge, m_OriginalIsClimbingUpToLedge);
	ILocomotion::vIsJumpingAcrossGap.Replace(vtable, &V_IsJumpingAcrossGap, m_OriginalIsJumpingAcrossGap);
	ILocomotion::vIsAbleToJumpAcrossGaps.Replace(vtable, &V_IsAbleToJumpAcrossGaps, m_OriginalIsAbleToJumpAcrossGaps);
	ILocomotion::vIsAbleToClimb.Replace(vtable, &V_IsAbleToClimb, m_OriginalIsAbleToClimb);
	ILocomotion::vGetStepHeight.Replace(vtable, &V_GetStepHeight, m_OriginalGetStepHeight);
	ILocomotion::vGetMaxJumpHeight.Replace(vtable, &V_GetMaxJumpHeight, m_OriginalGetMaxJumpHeight);
	ILocomotion::vGetDeathDropHeight.Replace(vtable, &V_GetDeathDropHeight, m_OriginalGetDeathDropHeight);
	ILocomotion::vGetRunSpeed.Replace(vtable, &V_GetRunSpeed, m_OriginalGetRunSpeed);
	ILocomotion::vGetWalkSpeed.Replace(vtable, &V_GetWalkSpeed, m_OriginalGetWalkSpeed);
	ILocomotion::vGetMaxAcceleration.Replace(vtable, &V_GetMaxAcceleration, m_OriginalGetMaxAcceleration);
	ILocomotion::vIsEntityTraversable.Replace(vtable, &V_IsEntityTraversable, m_OriginalIsEntityTraversable);
	ILocomotion::vShouldCollideWith.Replace(vtable, &V_ShouldCollideWith, m_OriginalShouldCollideWith);
	NextBotGroundLocomotion::vGetGravity.Replace(vtable, &V_GetGravity, m_OriginalGetGravity);
	NextBotGroundLocomotion::vGetFrictionForward.Replace(vtable, &V_GetFrictionForward, m_OriginalGetFrictionForward);
	NextBotGroundLocomotion::vGetFrictionSideways.Replace(vtable, &V_GetFrictionSideways, m_OriginalGetFrictionSideways);
	NextBotGroundLocomotion::vGetMaxYawRate.Replace(vtable, &V_GetMaxYawRate, m_OriginalGetMaxYawRate);

	return vtable;
}

CBaseNPC_Locomotion* CBaseNPC_Locomotion::New(INextBot* bot)
{
	CBaseNPC_Locomotion* mover = (CBaseNPC_Locomotion*)::operator new(sizeof(CBaseNPC_Locomotion));
	NextBotGroundLocomotion::NextBotGroundLocomotion_Ctor(mover, bot);
	if (!vtable) CreateVTable(mover);
	vtable_replace(mover, vtable);
	mover->Construct();

	return mover;
}

void CBaseNPC_Locomotion::Construct()
{
	m_pHookIds = new std::vector<int>();
	m_pCallbacks = new std::map<CallbackType, IPluginFunction*>();
	m_pCallbackTypeStack = new std::stack<CallbackType>();
	m_flJumpHeight = 0.0;
	m_flStepSize = 18.0;
	m_flGravity = 800.0;
	m_flAcceleration = 4000.0;
	m_flDeathDropHeight = 1000.0;
	m_flWalkSpeed = 400.0;
	m_flRunSpeed = 400.0;
	m_flFrictionForward = 0.0;
	m_flFrictionSideways = 3.0;
	m_flMaxYawRate = 1250.0;
}

void CBaseNPC_Locomotion::Destroy()
{
	for (auto it = m_pHookIds->begin(); it != m_pHookIds->end(); it++)
	{
		SH_REMOVE_HOOK_ID((*it));
	}

	delete m_pHookIds;
	delete m_pCallbacks;
	delete m_pCallbackTypeStack;
}

IPluginFunction* CBaseNPC_Locomotion::GetCallback(CallbackType cbType) const
{
	auto iter = m_pCallbacks->find(cbType);
	if (iter != m_pCallbacks->end())
	{
		return m_pCallbacks->operator[](cbType);
	}
	
	return nullptr;
}

void CBaseNPC_Locomotion::SetCallback(CallbackType cbType, IPluginFunction* pCallback)
{
	m_pCallbacks->insert_or_assign(cbType, pCallback);
}

void CBaseNPC_Locomotion::V_Update()
{
	// VPROF_ENTER_SCOPE("CBaseNPC_Locomotion::Update");

	CBaseCombatCharacterHack* entity = GetBot()->GetEntity();
	entity->UpdateLastKnownArea();

	if (IsStuck())
	{
		INextBot* bot = GetBot();
		PathFollower* path = bot->GetCurrentPath();
		if (path && GetStuckDuration() > 1.0f)
		{
			const Path::Segment* seg = path->GetCurrentGoal();
			const Path::Segment* finalGoal = path->LastSegment();
			const Path::Segment* prior = nullptr;
			if (seg)
				prior = path->PriorSegment(seg);

			if (prior && prior != finalGoal)
			{
				bot->SetPosition(prior->pos);
				ClearStuckStatus("Un-Stuck moved to previous segment");
			}
			else if (seg && seg != finalGoal)
			{
				bot->SetPosition(seg->pos);
				ClearStuckStatus("Un-Stuck moved to previous segment");
			}
			else if (prior)
			{
				bot->SetPosition(path->GetPosition(40.0, prior));
				ClearStuckStatus("Un-Stuck");
			}
			else if (seg)
			{
				bot->SetPosition(path->GetPosition(40.0, seg));
				ClearStuckStatus("Un-Stuck");
			}
		}
	}

	m_OriginalUpdate(this);
}

bool CBaseNPC_Locomotion::DefaultIsAbleToClimb()
{
	return m_flJumpHeight > 0.0;
}

bool CBaseNPC_Locomotion::V_IsAbleToClimb()
{
	m_pCallbackTypeStack->push(CallbackType_IsAbleToClimb);

	bool result = false;

	IPluginFunction* pCallback = GetCallback(CallbackType_IsAbleToClimb);
	if (pCallback && pCallback->IsRunnable())
	{
		cell_t _result = 0;

		pCallback->PushCell((cell_t)this);
		pCallback->Execute(&_result);

		result = !!_result;
	}
	else 
	{
		result = DefaultIsAbleToClimb();
	}

	m_pCallbackTypeStack->pop();

	return result;
}

bool CBaseNPC_Locomotion::DefaultIsClimbingUpToLedge()
{
	return m_OriginalIsClimbingUpToLedge(this);
}

bool CBaseNPC_Locomotion::V_IsClimbingUpToLedge()
{
	m_pCallbackTypeStack->push(CallbackType_IsClimbingUpToLedge);

	bool result = false;

	IPluginFunction* pCallback = GetCallback(CallbackType_IsClimbingUpToLedge);
	if (pCallback && pCallback->IsRunnable())
	{
		cell_t _result = 0;

		pCallback->PushCell((cell_t)this);
		pCallback->Execute(&_result);

		result = !!_result;
	}
	else 
	{
		result = DefaultIsClimbingUpToLedge();
	}

	m_pCallbackTypeStack->pop();

	return result;
}

bool CBaseNPC_Locomotion::DefaultClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity)
{
	Vector vecMyPos = GetBot()->GetPosition();
	vecMyPos.z += m_flStepSize;

	float flActualHeight = vecGoal.z - vecMyPos.z;
	float height = flActualHeight;
	if (height < 16.0)
	{
		height = 16.0;
	}

	float additionalHeight = 20.0;
	if (height < 32)
	{
		additionalHeight += 8.0;
	}

	height += additionalHeight;

	float speed = sqrt(2.0 * m_flGravity * height);
	float time = speed / m_flGravity;

	time += sqrt((2.0 * additionalHeight) / m_flGravity);

	Vector vecJumpVel = vecGoal - vecMyPos;
	vecJumpVel /= time;
	vecJumpVel.z = speed;

	float flJumpSpeed = vecJumpVel.Length();
	float flMaxSpeed = 650.0;
	if (flJumpSpeed > flMaxSpeed)
	{
		vecJumpVel[0] *= flMaxSpeed / flJumpSpeed;
		vecJumpVel[1] *= flMaxSpeed / flJumpSpeed;
		vecJumpVel[2] *= flMaxSpeed / flJumpSpeed;
	}

	GetBot()->SetPosition(vecMyPos);
	SetVelocity(vecJumpVel);
	return true;
}

bool CBaseNPC_Locomotion::V_ClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity)
{
	m_pCallbackTypeStack->push(CallbackType_ClimbUpToLedge);

	bool result = false;

	IPluginFunction* pCallback = GetCallback(CallbackType_ClimbUpToLedge);
	if (pCallback && pCallback->IsRunnable())
	{
		cell_t goalArr[3];
		cell_t forwardArr[3];
		VectorToPawnVector(goalArr, vecGoal);
		VectorToPawnVector(forwardArr, vecForward);
		cell_t entRef = gamehelpers->EntityToBCompatRef(const_cast<CBaseEntity*>(pEntity));
		cell_t _result = 0;

		pCallback->PushCell((cell_t)this);
		pCallback->PushArray(goalArr, 3);
		pCallback->PushArray(forwardArr, 3);
		pCallback->PushCell(entRef);
		pCallback->Execute(&_result);

		result = !!_result;
	}
	else 
	{
		result = DefaultClimbUpToLedge(vecGoal, vecForward, pEntity);
	}

	m_pCallbackTypeStack->pop();

	return result;
}

bool CBaseNPC_Locomotion::DefaultIsAbleToJumpAcrossGaps()
{
	return m_flJumpHeight > 0.0;
}

bool CBaseNPC_Locomotion::V_IsAbleToJumpAcrossGaps()
{
	m_pCallbackTypeStack->push(CallbackType_IsAbleToJumpAcrossGaps);

	bool result = false;

	IPluginFunction* pCallback = GetCallback(CallbackType_IsAbleToJumpAcrossGaps);
	if (pCallback && pCallback->IsRunnable())
	{
		cell_t _result = 0;

		pCallback->PushCell((cell_t)this);
		pCallback->Execute(&_result);

		result = !!_result;
	}
	else 
	{
		result = DefaultIsAbleToJumpAcrossGaps();
	}

	m_pCallbackTypeStack->pop();

	return result;
} 

bool CBaseNPC_Locomotion::DefaultIsJumpingAcrossGap()
{
	return m_OriginalIsJumpingAcrossGap(this);
}

bool CBaseNPC_Locomotion::V_IsJumpingAcrossGap()
{
	m_pCallbackTypeStack->push(CallbackType_IsJumpingAcrossGap);

	bool result = false;

	IPluginFunction* pCallback = GetCallback(CallbackType_IsJumpingAcrossGap);
	if (pCallback && pCallback->IsRunnable())
	{
		cell_t _result = 0;

		pCallback->PushCell((cell_t)this);
		pCallback->Execute(&_result);

		result = !!_result;
	}
	else 
	{
		result = DefaultIsJumpingAcrossGap();
	}

	m_pCallbackTypeStack->pop();

	return result;
}

void CBaseNPC_Locomotion::DefaultJumpAcrossGap(const Vector &landingGoal, const Vector &landingForward)
{
	m_OriginalJumpAcrossGap(this, landingGoal, landingForward);
}

void CBaseNPC_Locomotion::V_JumpAcrossGap(const Vector& landingGoal, const Vector& landingForward)
{
	m_pCallbackTypeStack->push(CallbackType_JumpAcrossGap);

	IPluginFunction* pCallback = GetCallback(CallbackType_JumpAcrossGap);
	if (pCallback && pCallback->IsRunnable())
	{
		cell_t goalArr[3];
		cell_t forwardArr[3];
		VectorToPawnVector(goalArr, landingGoal);
		VectorToPawnVector(forwardArr, landingForward);
		cell_t result = 0;

		pCallback->PushCell((cell_t)this);
		pCallback->PushArray(goalArr, 3);
		pCallback->PushArray(forwardArr, 3);
		pCallback->Execute(&result);
	}
	else 
	{
		DefaultJumpAcrossGap(landingGoal, landingForward);
	}

	m_pCallbackTypeStack->pop();
}

bool CBaseNPC_Locomotion::DefaultIsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when)
{
	if (((CBaseEntityHack *)pEntity)->MyCombatCharacterPointer())
	{
		return true;
	}
	
	return false;
}

bool CBaseNPC_Locomotion::V_IsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when)
{
	m_pCallbackTypeStack->push(CallbackType_IsEntityTraversable);

	bool result = false;

	IPluginFunction* pCallback = GetCallback(CallbackType_IsEntityTraversable);
	if (pCallback && pCallback->IsRunnable())
	{
		cell_t entRef = gamehelpers->EntityToBCompatRef(const_cast<CBaseEntity*>(pEntity));
		cell_t _result = 0;

		pCallback->PushCell((cell_t)this);
		pCallback->PushCell(entRef);
		pCallback->PushCell((cell_t)when);
		pCallback->Execute(&_result);

		result = !!_result;
	}
	else 
	{
		result = DefaultIsEntityTraversable(pEntity, when);
	}

	m_pCallbackTypeStack->pop();

	return result;
}

bool CBaseNPC_Locomotion::DefaultShouldCollideWith(const CBaseEntity* pCollider)
{
	return false;
}

bool CBaseNPC_Locomotion::V_ShouldCollideWith(const CBaseEntity* pEntity)
{
	m_pCallbackTypeStack->push(CallbackType_ShouldCollideWith);

	bool result = false;

	IPluginFunction* pCallback = GetCallback(CallbackType_ShouldCollideWith);
	if (pCallback && pCallback->IsRunnable())
	{
		cell_t entRef = gamehelpers->EntityToBCompatRef(const_cast<CBaseEntity*>(pEntity));
		cell_t _result = 0;

		pCallback->PushCell((cell_t)this);
		pCallback->PushCell(entRef);
		pCallback->Execute(&_result);

		result = !!_result;
	}
	else
	{
		result = DefaultShouldCollideWith(pEntity);
	}

	m_pCallbackTypeStack->pop();

	RETURN_META_VALUE(MRES_SUPERCEDE, result);
}

float CBaseNPC_Locomotion::V_GetStepHeight()
{
	return m_flStepSize;
}

float CBaseNPC_Locomotion::V_GetMaxJumpHeight()
{
	return m_flJumpHeight;
}

float CBaseNPC_Locomotion::V_GetDeathDropHeight()
{
	return m_flDeathDropHeight;
}

float CBaseNPC_Locomotion::V_GetWalkSpeed()
{
	return m_flWalkSpeed;
}

float CBaseNPC_Locomotion::V_GetRunSpeed()
{
	return m_flRunSpeed;
}

float CBaseNPC_Locomotion::V_GetMaxAcceleration()
{
	return m_flAcceleration;
}

float CBaseNPC_Locomotion::V_GetGravity()
{
	return m_flGravity;
}

float CBaseNPC_Locomotion::V_GetFrictionForward()
{
	return m_flFrictionForward;
}

float CBaseNPC_Locomotion::V_GetFrictionSideways()
{
	return m_flFrictionSideways;
}

float CBaseNPC_Locomotion::V_GetMaxYawRate()
{
	return m_flMaxYawRate;
}