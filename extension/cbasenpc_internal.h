#ifndef CBASENPC_INTERNAL_H_SHARED
#define CBASENPC_INTERNAL_H_SHARED

#include "shared/cbasenpc.h"
#include "extension.h"
#include <utlvector.h>

#define NPC_DECLARE_INTERFACE(iface) \
class iface##_Hook_Internal : public iface##_Hook 

#define NPC_SETUP_HOOK(iface) \
private: \
	CUtlVector<int> m_vecHooks; \
	void SetupHook(); \
public: \
	~iface##_Hook_Internal() \
	{ \
		FOR_EACH_VEC( m_vecHooks, i ) \
		{ \
			SH_REMOVE_HOOK_ID(m_vecHooks[i]); \
		} \
	}; \
	iface##_Hook_Internal(iface##_Hook hook, iface* inter) : iface##_Hook(hook) \
	{ \
		m_pRealInterface = inter; \
		SetupHook(); \
	} 

#define NPC_BEGIN_HOOK(iface) \
void iface##_Hook_Internal::SetupHook() \

#define NPC_ADD_HOOK(iface, function) \
		m_vecHooks.AddToTail(SH_ADD_HOOK(iface, function, (iface *)m_pRealInterface, SH_MEMBER(this, &iface##_Hook_Internal::function##_internal), false))

#define NPC_INTERFACE_HOOK0(function, rettype) \
private: \
	rettype function##_internal(); \
public: \
	virtual rettype function()

#define NPC_INTERFACE_HOOK1(function, rettype, pa1) \
private: \
	rettype function##_internal(pa1 p1); \
public: \
	virtual rettype function(pa1 p1)

#define NPC_INTERFACE_HOOK2(function, rettype, pa1, pa2) \
private: \
	bool m_bIn##function; \
	rettype function##_internal(pa1 p1, pa2 p2); \
public: \
	virtual rettype function(pa1 p1, pa2 p2)

#define NPC_INTERFACE_HOOK3(function, rettype, pa1, pa2, pa3) \
private: \
	rettype function##_internal(pa1 p1, pa2 p2, pa3 p3); \
public: \
	virtual rettype function(pa1 p1, pa2 p2, pa3 p3)

#define NPC_INTERFACE_HOOK4(function, rettype, pa1, pa2, pa3, pa4) \
private: \
	rettype function##_internal(pa1 p1, pa2 p2, pa3 p3, pa4 p4); \
public: \
	virtual rettype function(pa1 p1, pa2 p2, pa3 p3, pa4 p4)

#define NPC_INTERFACE_HOOK5(function, rettype, pa1, pa2, pa3, pa4, pa5) \
private: \
	rettype function##_internal(pa1 p1, pa2 p2, pa3 p3, pa4 p4, pa5 p5); \
public: \
	virtual rettype function(pa1 p1, pa2 p2, pa3 p3, pa4 p4, pa5 p5)

#define NPC_INTERFACE_DECLARE_HANDLER(classname, iface, function, rettype, params, paramscall) \
rettype classname##_Internal::##function##_internal params \
{ \
	RETURN_META_VALUE(MRES_SUPERCEDE, ((classname *)m_pInternalHook)->function paramscall); \
} \
rettype classname##_Internal::##function params \
{ \
	return SH_CALL((iface *)m_pRealInterface, &##iface::##function) paramscall; \
}


#define NPC_INTERFACE_DECLARE_HANDLER_void(classname, iface, function, params, paramscall) \
void classname##_Internal::##function params \
{ \
	SH_CALL((iface *)m_pRealInterface, &##iface::##function) paramscall; \
	return; \
} \
void classname##_Internal::##function##_internal params \
{ \
	((classname *)m_pInternalHook)->function paramscall; \
	RETURN_META(MRES_SUPERCEDE); \
}

// ============================================
// Interface Hooks for Extensions
// ============================================

NPC_DECLARE_INTERFACE(ILocomotion)
{
	NPC_SETUP_HOOK(ILocomotion);
	NPC_INTERFACE_HOOK1(FaceTowards, void, Vector const&);
	NPC_INTERFACE_HOOK0(IsAbleToJumpAcrossGaps, bool);
	NPC_INTERFACE_HOOK0(IsAbleToClimb, bool);
	NPC_INTERFACE_HOOK3(ClimbUpToLedge, bool, const Vector&, const Vector&, const CBaseEntity*);
	NPC_INTERFACE_HOOK0(GetStepHeight, float);
	NPC_INTERFACE_HOOK0(GetMaxJumpHeight, float);
	NPC_INTERFACE_HOOK0(GetDeathDropHeight, float);
	NPC_INTERFACE_HOOK0(GetWalkSpeed, float);
	NPC_INTERFACE_HOOK0(GetRunSpeed, float);
	NPC_INTERFACE_HOOK1(ShouldCollideWith, bool, const CBaseEntity*);
	NPC_INTERFACE_HOOK2(IsEntityTraversable, bool, CBaseEntity*, ILocomotion::TraverseWhenType);
};


NPC_DECLARE_INTERFACE(NextBotGroundLocomotion)
{
	NPC_SETUP_HOOK(NextBotGroundLocomotion);
	NPC_INTERFACE_HOOK1(SetAcceleration, void, const Vector&);
	NPC_INTERFACE_HOOK0(GetAcceleration, const Vector&);
	NPC_INTERFACE_HOOK1(SetVelocity, void, const Vector&);
	NPC_INTERFACE_HOOK0(GetGravity, float);
	NPC_INTERFACE_HOOK0(GetFrictionForward, float);
	NPC_INTERFACE_HOOK0(GetFrictionSideways, float);
};

NPC_DECLARE_INTERFACE(IBody)
{
	NPC_SETUP_HOOK(IBody);
	NPC_INTERFACE_HOOK2(StartActivity, bool, Activity, unsigned int);
	NPC_INTERFACE_HOOK0(GetHullWidth, float);
	NPC_INTERFACE_HOOK0(GetHullHeight, float);
	NPC_INTERFACE_HOOK0(GetStandHullHeight, float);
	NPC_INTERFACE_HOOK0(GetCrouchHullHeight, float);
	NPC_INTERFACE_HOOK0(GetSolidMask, unsigned int);
};

// =====================================================
// CBaseNPC for plugins - TODO: Move to a seperate file
// =====================================================

class CBaseNPC_Body : public IBody_Hook
{
public:
	CBaseNPC_Body(IBody* body) :
		IBody_Hook(body)
	{
		m_vecBodyMins = Vector(-10.0, -10.0, 0.0);
		m_vecBodyMaxs = Vector(10.0, 10.0, 90.0);
	};

	virtual bool StartActivity(Activity aAct, unsigned int iFlags);
	virtual float GetHullWidth();
	virtual float GetHullHeight();
	virtual float GetStandHullHeight();
	virtual float GetCrouchHullHeight();
	virtual unsigned int GetSolidMask();

	Vector m_vecBodyMins;
	Vector m_vecBodyMaxs;
};

class CBaseNPC_Locomotion : public NextBotGroundLocomotion_Hook
{
public:
	CBaseNPC_Locomotion(NextBotGroundLocomotion* mover) : 
		m_flFrictionSideways(3.0),
		m_flFrictionForward(0.0),
		m_flJumpHeight(0.0),
		m_flStepSize(18.0),
		m_flGravity(800.0),
		m_flAcceleration(4000.0),
		m_flDeathDropHeight(1000.0),
		m_flWalkSpeed(400.0),
		m_flRunSpeed(400.0),
		NextBotGroundLocomotion_Hook(mover)
	{
	};

	virtual void FaceTowards(Vector const& vecGoal);
	virtual bool IsAbleToJumpAcrossGaps();
	virtual bool IsAbleToClimb();
	virtual bool ClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity);
	virtual float GetStepHeight();
	virtual float GetMaxJumpHeight();
	virtual float GetDeathDropHeight();
	virtual float GetWalkSpeed();
	virtual float GetRunSpeed();
	virtual float GetGravity();
	virtual bool ShouldCollideWith(const CBaseEntity* pCollider);
	virtual bool IsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when);
	virtual float GetFrictionForward();
	virtual float GetFrictionSideways();


	float m_flJumpHeight;
	float m_flStepSize;
	float m_flGravity;
	float m_flAcceleration;
	float m_flDeathDropHeight;
	float m_flWalkSpeed;
	float m_flRunSpeed;
	float m_flFrictionForward;
	float m_flFrictionSideways;
};

class CBaseNPC : public CExtNPC
{
public:
	CBaseNPC();

public:
	INextBot* m_pBot;
	NextBotGroundLocomotion* m_pMover;
	IVision* m_pVision;
	IBody* m_pBody;

	CBaseNPC_Locomotion *m_mover;
	CBaseNPC_Body *m_body;
	char m_type[64];
};


#endif