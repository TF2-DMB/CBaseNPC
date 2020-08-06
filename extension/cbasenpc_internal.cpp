#include "extension.h"
#include "cbasenpc_internal.h"
#include "sourcesdk/baseentity.h"
#include "NextBot/Path/NextBotPathFollow.h"
#include <bspflags.h>
#include <ai_activity.h>
#include <util.h>

// INextBotEventResponder
SH_DECL_HOOK0_void(INextBotComponent, Update, SH_NOATTRIB, 0);
SH_DECL_HOOK0(INextBotComponent, GetBot, const, 0, INextBot*);

// ILocomotion
SH_DECL_HOOK1_void(ILocomotion, FaceTowards, SH_NOATTRIB, 0, Vector const&);
SH_DECL_HOOK0(ILocomotion, IsAbleToJumpAcrossGaps, const, 0, bool);
SH_DECL_HOOK0(ILocomotion, IsAbleToClimb, const, 0, bool);
SH_DECL_HOOK3(ILocomotion, ClimbUpToLedge, SH_NOATTRIB, 0, bool, const Vector&, const Vector&, const CBaseEntity*);
SH_DECL_HOOK0(ILocomotion, GetStepHeight, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetMaxJumpHeight, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetDeathDropHeight, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetWalkSpeed, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetRunSpeed, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetMaxAcceleration, const, 0, float);
SH_DECL_HOOK1(ILocomotion, ShouldCollideWith, const, 0, bool, const CBaseEntity*);
SH_DECL_HOOK2(ILocomotion, IsEntityTraversable, const, 0, bool, CBaseEntity*, ILocomotion::TraverseWhenType);
SH_DECL_HOOK0(ILocomotion, IsStuck, const, 0, bool);
SH_DECL_HOOK0(ILocomotion, GetStuckDuration, const, 0, float);
SH_DECL_HOOK1_void(ILocomotion, ClearStuckStatus, SH_NOATTRIB, 0, const char*);

SH_DECL_HOOK0(NextBotGroundLocomotion, GetGravity, const, 0, float);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetAcceleration, const, 0, const Vector&);
SH_DECL_HOOK1_void(NextBotGroundLocomotion, SetAcceleration, SH_NOATTRIB, 0, const Vector&);
SH_DECL_HOOK1_void(NextBotGroundLocomotion, SetVelocity, SH_NOATTRIB, 0, const Vector&);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetFrictionForward, const, 0, float);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetFrictionSideways, const, 0, float);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetMaxYawRate, const, 0, float);

// IBody
SH_DECL_HOOK2(IBody, StartActivity, SH_NOATTRIB, 0, bool, Activity, unsigned int);
SH_DECL_HOOK0(IBody, GetHullWidth, const, 0, float);
SH_DECL_HOOK0(IBody, GetHullHeight, const, 0, float);
SH_DECL_HOOK0(IBody, GetStandHullHeight, const, 0, float);
SH_DECL_HOOK0(IBody, GetCrouchHullHeight, const, 0, float);
SH_DECL_HOOK0(IBody, GetHullMins, const, 0, const Vector&);
SH_DECL_HOOK0(IBody, GetHullMaxs, const, 0, const Vector&);
SH_DECL_HOOK0(IBody, GetSolidMask, const, 0, unsigned int);

CBaseNPC::CBaseNPC()
{
	CBaseEntityHack* pBoss = (CBaseEntityHack*)servertools->CreateEntityByName("base_boss");
	if (pBoss)
	{
		m_pBot = pBoss->MyNextBotPointer();
		SetEntity((CBaseEntity *)pBoss);
		servertools->SetKeyValue((CBaseEntity*)pBoss, "health", "2147483647");

		m_pBot = pBoss->MyNextBotPointer();
		m_pMover = (NextBotGroundLocomotion*)m_pBot->GetLocomotionInterface();
		m_pBody = m_pBot->GetBodyInterface();
		m_pVision = m_pBot->GetVisionInterface();
		m_mover = new CBaseNPC_Locomotion(m_pMover);
		m_body = new CBaseNPC_Body(m_pBody);
		m_type[0] = '\0';
	}
}

CBaseNPC::~CBaseNPC()
{
	if (m_mover)
	{
		delete m_mover;
	}
	if (m_body)
	{
		delete m_body;
	}
}

// ============================================
// ILocomotion Hooks
// ============================================

void CBaseNPC_Locomotion::Update()
{
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

	INextBotComponent_Hook::Update();
}

void CBaseNPC_Locomotion::FaceTowards(Vector const& vecGoal)
{
	const float deltaT = GetUpdateInterval();

	INextBot* bot = GetBot();
	CBaseCombatCharacterHack* entity = bot->GetEntity();
	QAngle angles = entity->GetLocalAngles();

	float desiredYaw = UTIL_VecToYaw(vecGoal - bot->GetPosition());

	float angleDiff = UTIL_AngleDiff(desiredYaw, angles.y);

	float deltaYaw = GetMaxYawRate() * deltaT;

	if (angleDiff < -deltaYaw)
	{
		angles.y -= deltaYaw;
	}
	else if (angleDiff > deltaYaw)
	{
		angles.y += deltaYaw;
	}
	else
	{
		angles.y += angleDiff;
	}

	entity->SetLocalAngles(angles);
}

bool CBaseNPC_Locomotion::IsAbleToJumpAcrossGaps() const
{
	return (m_flJumpHeight > 0.0);
}

bool CBaseNPC_Locomotion::IsAbleToClimb() const
{
	return (m_flJumpHeight > 0.0);
}

bool CBaseNPC_Locomotion::ClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity)
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

float CBaseNPC_Locomotion::GetStepHeight() const
{
	return m_flStepSize;
}

float CBaseNPC_Locomotion::GetMaxJumpHeight() const
{
	return m_flJumpHeight;
}

float CBaseNPC_Locomotion::GetDeathDropHeight() const
{
	return m_flDeathDropHeight;
}

float CBaseNPC_Locomotion::GetWalkSpeed() const
{
	return m_flWalkSpeed;
}

float CBaseNPC_Locomotion::GetRunSpeed() const
{
	return m_flRunSpeed;
}

float CBaseNPC_Locomotion::GetMaxAcceleration() const
{
	return m_flAcceleration;
}

float CBaseNPC_Locomotion::GetGravity() const
{
	return m_flGravity;
}

bool CBaseNPC_Locomotion::ShouldCollideWith(const CBaseEntity* pEntity) const
{
	return false;
}

bool CBaseNPC_Locomotion::IsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when) const
{
	CBaseEntityHack* ent = (CBaseEntityHack*)pEntity;
	if (ent->MyCombatCharacterPointer()) return true;
	return false;
}

float CBaseNPC_Locomotion::GetFrictionForward() const
{
	return m_flFrictionForward;
}

float CBaseNPC_Locomotion::GetFrictionSideways() const
{
	return m_flFrictionSideways;
}

float CBaseNPC_Locomotion::GetMaxYawRate() const
{
	return 1250.0f;
}

// ============================================
// IBody Hooks
// ============================================

void CBaseNPC_Body::Update()
{
	CBaseCombatCharacterHack* entity = GetBot()->GetEntity();
	entity->DispatchAnimEvents(entity);
	entity->StudioFrameAdvance();

	INextBotComponent_Hook::Update();
}

bool CBaseNPC_Body::StartActivity(Activity aAct, unsigned int iFlags)
{
	return true;
}

float CBaseNPC_Body::GetHullWidth() const
{
	float flWidth = m_vecBodyMaxs.x;
	if (flWidth < m_vecBodyMaxs.y) flWidth = m_vecBodyMaxs.y;

	return flWidth * 2.0;
}

float CBaseNPC_Body::GetHullHeight() const
{
	return m_vecBodyMaxs.z;
}

float CBaseNPC_Body::GetStandHullHeight() const
{
	return m_vecBodyMaxs.z;
}

float CBaseNPC_Body::GetCrouchHullHeight() const
{
	return m_vecBodyMaxs.z / 2;
}

const Vector& CBaseNPC_Body::GetHullMins() const
{
	return m_vecBodyMins;
}

const Vector& CBaseNPC_Body::GetHullMaxs() const
{
	return m_vecBodyMaxs;
}

unsigned int CBaseNPC_Body::GetSolidMask() const
{
	return (MASK_NPCSOLID | MASK_PLAYERSOLID);
}

// ============================================
// ILocomotion Hooks for Extensions
// ============================================

NPC_BEGIN_HOOK(INextBotComponent)
{
	NPC_COPY_FACE(INextBotComponent);
	NPC_ADD_HOOK(INextBotComponent, Update);
	NPC_ADD_HOOK(INextBotComponent, GetBot);
}

NPC_INTERFACE_DECLARE_HANDLER_void(INextBotComponent_Hook, INextBotComponent, Update, SH_NOATTRIB, (), ());
NPC_INTERFACE_DECLARE_HANDLER(INextBotComponent_Hook, INextBotComponent, GetBot, const, INextBot*, (), ());

NPC_BEGIN_HOOK(ILocomotion)
{
	NPC_COPY_FACE(ILocomotion);
	NPC_COPY_HOOK(INextBotComponent);
	NPC_ADD_HOOK(ILocomotion, FaceTowards);
	NPC_ADD_HOOK(ILocomotion, IsAbleToJumpAcrossGaps);
	NPC_ADD_HOOK(ILocomotion, IsAbleToClimb);
	NPC_ADD_HOOK(ILocomotion, ClimbUpToLedge);
	NPC_ADD_HOOK(ILocomotion, GetStepHeight);
	NPC_ADD_HOOK(ILocomotion, GetMaxJumpHeight);
	NPC_ADD_HOOK(ILocomotion, GetDeathDropHeight);
	NPC_ADD_HOOK(ILocomotion, GetWalkSpeed);
	NPC_ADD_HOOK(ILocomotion, GetRunSpeed);
	NPC_ADD_HOOK(ILocomotion, GetMaxAcceleration);
	NPC_ADD_HOOK(ILocomotion, ShouldCollideWith);
	NPC_ADD_HOOK(ILocomotion, IsEntityTraversable);
	NPC_ADD_HOOK(ILocomotion, IsStuck);
	NPC_ADD_HOOK(ILocomotion, GetStuckDuration);
	NPC_ADD_HOOK(ILocomotion, ClearStuckStatus);
}

NPC_INTERFACE_DECLARE_HANDLER_void(ILocomotion_Hook, ILocomotion, FaceTowards, SH_NOATTRIB, (Vector const& vecGoal), (vecGoal));
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, IsAbleToJumpAcrossGaps, const, bool, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, IsAbleToClimb, const, bool, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, ClimbUpToLedge, SH_NOATTRIB, bool, (const Vector& a, const Vector& b, const CBaseEntity* c), (a,b,c));
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetStepHeight, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetMaxJumpHeight, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetDeathDropHeight, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetWalkSpeed, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetRunSpeed, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetMaxAcceleration, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, ShouldCollideWith, const, bool, (const CBaseEntity* a), (a));
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, IsEntityTraversable, const, bool, (CBaseEntity* a, ILocomotion::TraverseWhenType b), (a,b));
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, IsStuck, const, bool, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetStuckDuration, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER_void(ILocomotion_Hook, ILocomotion, ClearStuckStatus, SH_NOATTRIB, (const char* name), (name));

NPC_BEGIN_HOOK(NextBotGroundLocomotion)
{
	NPC_COPY_FACE(NextBotGroundLocomotion);
	NPC_COPY_HOOK(ILocomotion);
	NPC_ADD_HOOK(NextBotGroundLocomotion, SetAcceleration);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetAcceleration);
	NPC_ADD_HOOK(NextBotGroundLocomotion, SetVelocity);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetGravity);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetFrictionForward);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetFrictionSideways);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetMaxYawRate);
}

NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetAcceleration, const, const Vector&, (), ());
NPC_INTERFACE_DECLARE_HANDLER_void(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, SetAcceleration, SH_NOATTRIB, (const Vector& accel), (accel));
NPC_INTERFACE_DECLARE_HANDLER_void(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, SetVelocity, SH_NOATTRIB, (const Vector& vel), (vel));
NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetGravity, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetFrictionForward, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetFrictionSideways, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetMaxYawRate, const, float, (), ());

// ============================================
// IBody Hooks for Extensions
// ============================================

NPC_BEGIN_HOOK(IBody)
{
	NPC_COPY_FACE(IBody);
	NPC_COPY_HOOK(INextBotComponent);
	NPC_ADD_HOOK(IBody, StartActivity);
	NPC_ADD_HOOK(IBody, GetHullWidth);
	NPC_ADD_HOOK(IBody, GetHullHeight);
	NPC_ADD_HOOK(IBody, GetStandHullHeight);
	NPC_ADD_HOOK(IBody, GetCrouchHullHeight);
	NPC_ADD_HOOK(IBody, GetHullMins);
	NPC_ADD_HOOK(IBody, GetHullMaxs);
	NPC_ADD_HOOK(IBody, GetSolidMask);
}

NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, StartActivity, SH_NOATTRIB, bool, (Activity aAct, unsigned int iFlags), (aAct, iFlags))
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetHullWidth, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetHullHeight, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetStandHullHeight, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetCrouchHullHeight, const, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetHullMins, const, const Vector&, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetHullMaxs, const, const Vector&, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetSolidMask, const, unsigned int, (), ());