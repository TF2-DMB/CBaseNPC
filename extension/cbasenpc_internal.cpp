#include "extension.h"
#include "cbasenpc_internal.h"
#include <bspflags.h>
#include <ai_activity.h>

// ILocomotion
SH_DECL_HOOK0(ILocomotion, IsAbleToJumpAcrossGaps, SH_NOATTRIB, 0, bool);
SH_DECL_HOOK0(ILocomotion, IsAbleToClimb, SH_NOATTRIB, 0, bool);
SH_DECL_HOOK3(ILocomotion, ClimbUpToLedge, SH_NOATTRIB, 0, bool, const Vector&, const Vector&, const CBaseEntity*);
SH_DECL_HOOK0(ILocomotion, GetStepHeight, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(ILocomotion, GetMaxJumpHeight, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(ILocomotion, GetDeathDropHeight, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(ILocomotion, GetWalkSpeed, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(ILocomotion, GetRunSpeed, SH_NOATTRIB, 0, float);
SH_DECL_HOOK1(ILocomotion, ShouldCollideWith, SH_NOATTRIB, 0, bool, const CBaseEntity*);
SH_DECL_HOOK2(ILocomotion, IsEntityTraversable, SH_NOATTRIB, 0, bool, CBaseEntity*, ILocomotion::TraverseWhenType);

SH_DECL_HOOK0(NextBotGroundLocomotion, GetGravity, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetAcceleration, SH_NOATTRIB, 0, const Vector&);
SH_DECL_HOOK1_void(NextBotGroundLocomotion, SetAcceleration, SH_NOATTRIB, 0, const Vector&);
SH_DECL_HOOK1_void(NextBotGroundLocomotion, SetVelocity, SH_NOATTRIB, 0, const Vector&);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetFrictionForward, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetFrictionSideways, SH_NOATTRIB, 0, float);

// IBody
SH_DECL_HOOK2(IBody, StartActivity, SH_NOATTRIB, 0, bool, Activity, unsigned int);
SH_DECL_HOOK0(IBody, GetHullWidth, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(IBody, GetHullHeight, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(IBody, GetStandHullHeight, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(IBody, GetCrouchHullHeight, SH_NOATTRIB, 0, float);
SH_DECL_HOOK0(IBody, GetSolidMask, SH_NOATTRIB, 0, unsigned int);

CBaseNPC::CBaseNPC()
{
	CBaseEntity* pBoss = servertools->CreateEntityByName("base_boss");
	if (!pBoss) return;

	SetEntity(pBoss);
	servertools->SetKeyValue(pBoss, "health", "2147483647");

	m_pMover = (NextBotGroundLocomotion *)m_pBot->GetLocomotionInterface();
	m_pBody = m_pBot->GetBodyInterface();
	m_pVision = m_pBot->GetVisionInterface();

	m_mover = new CBaseNPC_Locomotion(m_pMover);
	m_body = new CBaseNPC_Body(m_pBody);
}

// ============================================
// ILocomotion Hooks
// ============================================

bool CBaseNPC_Locomotion::IsAbleToJumpAcrossGaps()
{
	return (m_flJumpHeight > 0.0);
}

bool CBaseNPC_Locomotion::IsAbleToClimb()
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

float CBaseNPC_Locomotion::GetStepHeight()
{
	return m_flStepSize;
}

float CBaseNPC_Locomotion::GetMaxJumpHeight()
{
	return m_flJumpHeight;
}

float CBaseNPC_Locomotion::GetDeathDropHeight()
{
	return m_flDeathDropHeight;
}

float CBaseNPC_Locomotion::GetWalkSpeed()
{
	return m_flWalkSpeed;
}

float CBaseNPC_Locomotion::GetRunSpeed()
{
	return m_flRunSpeed;
}

float CBaseNPC_Locomotion::GetGravity()
{
	return m_flGravity;
}

bool CBaseNPC_Locomotion::ShouldCollideWith(const CBaseEntity* pEntity)
{
	return false;
}

bool CBaseNPC_Locomotion::IsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when)
{
	return false;
}

float CBaseNPC_Locomotion::GetFrictionForward()
{
	return m_flFrictionForward;
}

float CBaseNPC_Locomotion::GetFrictionSideways()
{
	return m_flFrictionSideways;
}

/*int CBaseNPC_Internal::GiveIDToEntity(const CBaseEntity* pEntity)
{
	for (int ID = 0; ID < MAX_NPCS; ID++)
	{
		if (g_NPCS[ID].GetEntryIndex() < 32)
		{
			g_NPCS[ID] = pEntity;
			return ID;
		}
	}
	return -1;
}*/

// ============================================
// ILocomotion Hooks for Extensions
// ============================================


NPC_BEGIN_HOOK(ILocomotion)
{
	NPC_ADD_HOOK(ILocomotion, IsAbleToJumpAcrossGaps);
	NPC_ADD_HOOK(ILocomotion, IsAbleToClimb);
	NPC_ADD_HOOK(ILocomotion, ClimbUpToLedge);
	NPC_ADD_HOOK(ILocomotion, GetStepHeight);
	NPC_ADD_HOOK(ILocomotion, GetMaxJumpHeight);
	NPC_ADD_HOOK(ILocomotion, GetDeathDropHeight);
	NPC_ADD_HOOK(ILocomotion, GetWalkSpeed);
	NPC_ADD_HOOK(ILocomotion, GetRunSpeed);
	NPC_ADD_HOOK(ILocomotion, ShouldCollideWith);
	NPC_ADD_HOOK(ILocomotion, IsEntityTraversable);
}

NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, IsAbleToJumpAcrossGaps, bool, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, IsAbleToClimb, bool, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, ClimbUpToLedge, bool, (const Vector& a, const Vector& b, const CBaseEntity* c), (a,b,c));
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetStepHeight, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetMaxJumpHeight, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetDeathDropHeight, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetWalkSpeed, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, GetRunSpeed, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, ShouldCollideWith, bool, (const CBaseEntity* a), (a));
NPC_INTERFACE_DECLARE_HANDLER(ILocomotion_Hook, ILocomotion, IsEntityTraversable, bool, (CBaseEntity* a, ILocomotion::TraverseWhenType b), (a,b));

NPC_BEGIN_HOOK(NextBotGroundLocomotion)
{
	NPC_ADD_HOOK(NextBotGroundLocomotion, SetAcceleration);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetAcceleration);
	NPC_ADD_HOOK(NextBotGroundLocomotion, SetVelocity);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetGravity);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetFrictionForward);
	NPC_ADD_HOOK(NextBotGroundLocomotion, GetFrictionSideways);
}

NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetAcceleration, const Vector&, (), ());
NPC_INTERFACE_DECLARE_HANDLER_void(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, SetAcceleration, (const Vector& accel), (accel));
NPC_INTERFACE_DECLARE_HANDLER_void(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, SetVelocity, (const Vector& vel), (vel));
NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetGravity, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetFrictionForward, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(NextBotGroundLocomotion_Hook, NextBotGroundLocomotion, GetFrictionSideways, float, (), ());

// ============================================
// IBody Hooks
// ============================================

bool CBaseNPC_Body::StartActivity(Activity aAct, unsigned int iFlags)
{
	return true;
}

float CBaseNPC_Body::GetHullWidth()
{
	float flWidth = m_vecBodyMaxs.x;
	if (flWidth < m_vecBodyMaxs.y) flWidth = m_vecBodyMaxs.y;

	return flWidth*2.0;
}

float CBaseNPC_Body::GetHullHeight()
{
	return m_vecBodyMaxs.z;
}

float CBaseNPC_Body::GetStandHullHeight()
{
	return m_vecBodyMaxs.z;
}

float CBaseNPC_Body::GetCrouchHullHeight()
{
	return m_vecBodyMaxs.z/2;
}

unsigned int CBaseNPC_Body::GetSolidMask()
{
	return (MASK_NPCSOLID | MASK_PLAYERSOLID);
}

// ============================================
// IBody Hooks for Extensions
// ============================================

NPC_BEGIN_HOOK(IBody)
{
	NPC_ADD_HOOK(IBody, StartActivity);
	NPC_ADD_HOOK(IBody, GetHullWidth);
	NPC_ADD_HOOK(IBody, GetHullHeight);
	NPC_ADD_HOOK(IBody, GetStandHullHeight);
	NPC_ADD_HOOK(IBody, GetCrouchHullHeight);
	NPC_ADD_HOOK(IBody, GetSolidMask);
}

NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, StartActivity, bool, (Activity aAct, unsigned int iFlags), (aAct, iFlags))
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetHullWidth, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetHullHeight, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetStandHullHeight, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetCrouchHullHeight, float, (), ());
NPC_INTERFACE_DECLARE_HANDLER(IBody_Hook, IBody, GetSolidMask, unsigned int, (), ());