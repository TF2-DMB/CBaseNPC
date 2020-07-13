#ifndef CBASENPC_H_SHARED
#define CBASENPC_H_SHARED

#include <ehandle.h>
#include <ai_activity.h>
#include "npctools.h"

#include <NextBot/NextBotInterface.h>
#include <NextBot/NextBotLocomotionInterface.h>
#include <NextBot/NextBotGroundLocomotion.h>
#include <NextBot/NextBotBodyInterface.h>

#define INTERFACE_HOOK(iface, function, ret, params, paramscall) \
	virtual ret function params \
	{ \
		return ((iface *)m_pInternalHook)->function paramscall; \
	};

#define INTERFACE_void(iface, function, params, paramscall) \
	virtual void function params \
	{ \
		((iface *)m_pInternalHook)->function paramscall; \
	};

#define BASE_INTERFACE_HOOK(function, ret, params, paramscall) \
	INTERFACE_HOOK(INextBotEventResponder_Hook, function, ret, params, paramscall)

#define LOCOMOTION_INTERFACE_HOOK(function, ret, params, paramscall) \
	INTERFACE_HOOK(ILocomotion_Hook, function, ret, params, paramscall)

#define GROUNDLOCOMOTION_INTERFACE_HOOK(function, ret, params, paramscall) \
	INTERFACE_HOOK(NextBotGroundLocomotion_Hook, function, ret, params, paramscall)

#define GROUNDLOCOMOTION_INTERFACE_HOOK_void(function, params, paramscall) \
	INTERFACE_void(NextBotGroundLocomotion_Hook, function, params, paramscall)

#define BODY_INTERFACE_HOOK(function, ret, params, paramscall) \
	INTERFACE_HOOK(IBody, function, ret, params, paramscall)

class INextBotEventResponder_Hook
{
public:
	INextBotEventResponder_Hook(INextBotEventResponder_Hook* hook) : m_pInternalHook(hook), m_pRealInterface(hook->m_pRealInterface){};
	~INextBotEventResponder_Hook() {};

	BASE_INTERFACE_HOOK(GetBot, INextBot*, (), ());

	void* m_pInternalHook;
	void* m_pRealInterface;
};

class ILocomotion_Hook : public INextBotEventResponder_Hook
{
public:
	ILocomotion_Hook(ILocomotion* mover) : INextBotEventResponder_Hook(g_pBaseNPCTools->Hook_ILocomotion(mover, this)) {};
	ILocomotion_Hook(INextBotEventResponder_Hook* hook) : INextBotEventResponder_Hook(hook) {}; // For hook classes inheriting us

	LOCOMOTION_INTERFACE_HOOK(IsAbleToJumpAcrossGaps, bool, (), ());
	LOCOMOTION_INTERFACE_HOOK(IsAbleToClimb, bool, (), ());
	LOCOMOTION_INTERFACE_HOOK(ClimbUpToLedge, bool, (const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity), (vecGoal, vecForward, pEntity));
	LOCOMOTION_INTERFACE_HOOK(GetStepHeight, float, (), ());
	LOCOMOTION_INTERFACE_HOOK(GetMaxJumpHeight, float, (), ());
	LOCOMOTION_INTERFACE_HOOK(GetDeathDropHeight, float, (), ());
	LOCOMOTION_INTERFACE_HOOK(GetWalkSpeed, float, (), ());
	LOCOMOTION_INTERFACE_HOOK(GetRunSpeed, float, (), ());
	LOCOMOTION_INTERFACE_HOOK(ShouldCollideWith, bool, (const CBaseEntity* pCollider), (pCollider));
	LOCOMOTION_INTERFACE_HOOK(IsEntityTraversable, bool, (CBaseEntity* pEntity, ILocomotion::TraverseWhenType when), (pEntity, when));
};

class NextBotGroundLocomotion_Hook : public ILocomotion_Hook
{
public:
	NextBotGroundLocomotion_Hook(NextBotGroundLocomotion* mover) : ILocomotion_Hook(g_pBaseNPCTools->Hook_NextBotGroundLocomotion(mover, this)) {};
	NextBotGroundLocomotion_Hook(INextBotEventResponder_Hook* hook) : ILocomotion_Hook(hook) {}; // For hook classes inheriting us

	GROUNDLOCOMOTION_INTERFACE_HOOK_void(SetAcceleration, (const Vector& acc), (acc));
	GROUNDLOCOMOTION_INTERFACE_HOOK(GetAcceleration, const Vector&, (), ());
	GROUNDLOCOMOTION_INTERFACE_HOOK_void(SetVelocity, (const Vector& vel), (vel));
	GROUNDLOCOMOTION_INTERFACE_HOOK(GetGravity, float, (), ());
	GROUNDLOCOMOTION_INTERFACE_HOOK(GetFrictionForward, float, (), ());
	GROUNDLOCOMOTION_INTERFACE_HOOK(GetFrictionSideways, float, (), ());
};

class IBody_Hook : public INextBotEventResponder_Hook
{
public:
	IBody_Hook(IBody* body) : INextBotEventResponder_Hook(g_pBaseNPCTools->Hook_IBody(body, this)) {};
	IBody_Hook(INextBotEventResponder_Hook* hook) : INextBotEventResponder_Hook(hook) {}; // For hook classes inheriting us

	BODY_INTERFACE_HOOK(StartActivity, bool, (Activity aAct, unsigned int iFlags), (aAct, iFlags));
	BODY_INTERFACE_HOOK(GetHullWidth, float, (), ());
	BODY_INTERFACE_HOOK(GetHullHeight, float, (), ());
	BODY_INTERFACE_HOOK(GetStandHullHeight, float, (), ());
	BODY_INTERFACE_HOOK(GetCrouchHullHeight, float, (), ());
	BODY_INTERFACE_HOOK(GetSolidMask, unsigned int, (), ());
};

// Basic NPC object, the extension will give an index so that plugins can coordinate and store information accordingly
// IMPORTANT NOTE: THIS IS NOT A CBASENPC OBJECT AND AS SUCH SHOULDNT BE USED BY PLUGINS TO CALL CBASENPC NATIVES!!!!!
class CExtNPC
{
public:
	CExtNPC() : m_iIndex(-1), m_hEntity(NULL) {};

	unsigned long GetID() { return m_iIndex; };
	CBaseEntity* GetEntity() { return m_hEntity; };
	void SetEntity(CBaseEntity* ent) { m_hEntity = ent; m_iIndex = g_pBaseNPCTools->GrantID(ent, this); };

private:
	int m_iIndex;
	EHANDLE m_hEntity;
};

#endif