#ifndef CBASENPC_H_SHARED
#define CBASENPC_H_SHARED

#include <ehandle.h>
#include <ai_activity.h>
#include "npctools.h"

#include <NextBot/NextBotInterface.h>
#include <NextBot/NextBotLocomotionInterface.h>
#include <NextBot/NextBotGroundLocomotion.h>
#include <NextBot/NextBotBodyInterface.h>

#define DECLARE_HOOK_INTERFACE(iface) \
	typedef iface##_Hook ThisClass;

#define INTERFACE_HOOK(function, ret, params, paramscall, attrib) \
	virtual ret function params attrib \
	{ \
		return ((ThisClass *)m_pInternalHook)->function paramscall; \
	};

#define INTERFACE_HOOK_void(function, params, paramscall, attrib) \
	virtual void function params attrib \
	{ \
		((ThisClass *)m_pInternalHook)->function paramscall; \
	};

class INextBotComponent_Hook
{
public:
	DECLARE_HOOK_INTERFACE(INextBotComponent);

	INextBotComponent_Hook() : m_pInternalHook(nullptr), m_pRealInterface(nullptr)
	{
	};
	virtual ~INextBotComponent_Hook()
	{
		INextBotComponent_Hook* internal = (INextBotComponent_Hook*)m_pInternalHook;
		m_pInternalHook = nullptr;
		if (internal)
		{
			internal->m_pInternalHook = nullptr; // Avoid delete loop...
			delete internal;
		}
	};

	INTERFACE_HOOK_void(Update, (), (), SH_NOATTRIB);
	INTERFACE_HOOK(GetBot, INextBot*, (), (), const);
	inline float GetUpdateInterval() { return m_pRealInterface->GetUpdateInterval(); };

	void* m_pInternalHook;
	INextBotComponent* m_pRealInterface;
};

class ILocomotion_Hook : public INextBotComponent_Hook
{
public:
	DECLARE_HOOK_INTERFACE(ILocomotion);

	ILocomotion_Hook(ILocomotion* mover) : INextBotComponent_Hook() { g_pBaseNPCTools->Hook_ILocomotion(mover, this); };
	ILocomotion_Hook() : INextBotComponent_Hook() {}; // For hook classes inheriting us

	INTERFACE_HOOK_void(FaceTowards, (Vector const& vecGoal), (vecGoal), SH_NOATTRIB);
	INTERFACE_HOOK(IsAbleToJumpAcrossGaps, bool, (), (), const);
	INTERFACE_HOOK(IsAbleToClimb, bool, (), (), const);
	INTERFACE_HOOK(ClimbUpToLedge, bool, (const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity), (vecGoal, vecForward, pEntity), SH_NOATTRIB);
	INTERFACE_HOOK(GetStepHeight, float, (), (), const);
	INTERFACE_HOOK(GetMaxJumpHeight, float, (), (), const);
	INTERFACE_HOOK(GetDeathDropHeight, float, (), (), const);
	INTERFACE_HOOK(GetWalkSpeed, float, (), (), const);
	INTERFACE_HOOK(GetRunSpeed, float, (), (), const);
	INTERFACE_HOOK(GetMaxAcceleration, float, (), (), const);
	INTERFACE_HOOK(ShouldCollideWith, bool, (const CBaseEntity* pCollider), (pCollider), const);
	INTERFACE_HOOK(IsEntityTraversable, bool, (CBaseEntity* pEntity, ILocomotion::TraverseWhenType when), (pEntity, when), const);
	INTERFACE_HOOK(IsStuck, bool, (), (), const);
	INTERFACE_HOOK(GetStuckDuration, float, (), (), const);
	INTERFACE_HOOK(ClearStuckStatus, void, (const char* name), (name), SH_NOATTRIB);
};

class NextBotGroundLocomotion_Hook : public ILocomotion_Hook
{
public:
	DECLARE_HOOK_INTERFACE(NextBotGroundLocomotion);

	NextBotGroundLocomotion_Hook(NextBotGroundLocomotion* mover) : ILocomotion_Hook() { g_pBaseNPCTools->Hook_NextBotGroundLocomotion(mover, this); };
	NextBotGroundLocomotion_Hook() : ILocomotion_Hook() {}; // For hook classes inheriting us

	INTERFACE_HOOK_void(SetAcceleration, (const Vector& acc), (acc), SH_NOATTRIB);
	INTERFACE_HOOK(GetAcceleration, const Vector&, (), (), const);
	INTERFACE_HOOK_void(SetVelocity, (const Vector& vel), (vel), SH_NOATTRIB);
	INTERFACE_HOOK(GetGravity, float, (), (), const);
	INTERFACE_HOOK(GetFrictionForward, float, (), (), const);
	INTERFACE_HOOK(GetFrictionSideways, float, (), (), const);
	INTERFACE_HOOK(GetMaxYawRate, float, (), (), const);
};

class IBody_Hook : public INextBotComponent_Hook
{
public:
	DECLARE_HOOK_INTERFACE(IBody);

	IBody_Hook(IBody* body) : INextBotComponent_Hook() { g_pBaseNPCTools->Hook_IBody(body, this); };
	IBody_Hook() : INextBotComponent_Hook() {}; // For hook classes inheriting us

	INTERFACE_HOOK(StartActivity, bool, (Activity aAct, unsigned int iFlags), (aAct, iFlags), SH_NOATTRIB);
	INTERFACE_HOOK(GetHullWidth, float, (), (), const);
	INTERFACE_HOOK(GetHullHeight, float, (), (), const);
	INTERFACE_HOOK(GetStandHullHeight, float, (), (), const);
	INTERFACE_HOOK(GetCrouchHullHeight, float, (), (), const);
	INTERFACE_HOOK(GetHullMins, const Vector&, (), (), const);
	INTERFACE_HOOK(GetHullMaxs, const Vector&, (), (), const);
	INTERFACE_HOOK(GetSolidMask, unsigned int, (), (), const);
};

// Basic NPC object, the extension will give an index so that plugins can coordinate and store information accordingly
// IMPORTANT NOTE: THIS IS NOT A CBASENPC OBJECT AND AS SUCH SHOULDNT BE USED BY PLUGINS TO CALL CBASENPC NATIVES!!!!!
class CExtNPC
{
public:
	CExtNPC() : m_iIndex(INVALID_NPC_ID), m_hEntity(NULL) {};
	virtual ~CExtNPC() { g_pBaseNPCTools->DeleteNPC(this); };

	inline int GetID() { return m_iIndex; };
	CBaseEntity* GetEntity()
	{
		return m_hEntity.Get();
	};
	void SetEntity(CBaseEntity* ent)
	{
		m_hEntity = ent;
		m_iIndex = g_pBaseNPCTools->GrantID(ent, this);
	};

private:
	int m_iIndex;
	EHANDLE m_hEntity;
};

#endif