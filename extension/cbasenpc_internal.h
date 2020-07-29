#ifndef CBASENPC_INTERNAL_H_SHARED
#define CBASENPC_INTERNAL_H_SHARED

#include "shared/cbasenpc.h"
#include "extension.h"
#include <utlvector.h>

#define NPC_DECLARE_INTERFACE(iface) \
class iface##_Hook_Internal : public iface##_Hook

#define NPC_DECLARE_INTERFACE_INHERIT(iface, otheriface) \
class iface##_Hook_Internal : public otheriface##_Hook_Internal

#define NPC_SETUP_HOOK(iface) \
private: \
	CUtlVector<int> m_vec##iface##Hooks; \
	iface* m_pReal##iface##Interface; \
	iface##_Hook* m_pExternal##iface##Hook; \
public: \
	void SetupHook(void* face, iface##_Hook* external); \
public: \
	~iface##_Hook_Internal() \
	{ \
		FOR_EACH_VEC( m_vec##iface##Hooks, i ) \
		{ \
			SH_REMOVE_HOOK_ID(m_vec##iface##Hooks[i]); \
		} \
	}; \
	iface##_Hook_Internal() : m_pReal##iface##Interface(nullptr), m_pExternal##iface##Hook(nullptr) \
	{ \
	} 

#define NPC_BEGIN_HOOK(iface) \
void iface##_Hook_Internal::SetupHook(void* face, iface##_Hook* external) \

#define NPC_COPY_HOOK(iface) \
	((iface##_Hook_Internal*)this)->SetupHook(face, external);

#define NPC_COPY_FACE(iface) \
	m_pReal##iface##Interface = (iface*)face; \
	m_pExternal##iface##Hook = external;

#define NPC_ADD_HOOK(iface, function) \
		m_vec##iface##Hooks.AddToTail(SH_ADD_HOOK(iface, function, (iface *)face, SH_MEMBER(this, &iface##_Hook_Internal::function##_internal), false)); \

#define NPC_INTERFACE_HOOK0(function, rettype, attrib) \
private: \
	rettype function##_internal() attrib; \
public: \
	virtual rettype function() attrib

#define NPC_INTERFACE_HOOK1(function, rettype, pa1, attrib) \
private: \
	rettype function##_internal(pa1 p1) attrib;\
public: \
	virtual rettype function(pa1 p1) attrib

#define NPC_INTERFACE_HOOK2(function, rettype, pa1, pa2, attrib) \
private: \
	rettype function##_internal(pa1 p1, pa2 p2) attrib; \
public: \
	virtual rettype function(pa1 p1, pa2 p2) attrib

#define NPC_INTERFACE_HOOK3(function, rettype, pa1, pa2, pa3, attrib) \
private: \
	rettype function##_internal(pa1 p1, pa2 p2, pa3 p3) attrib; \
public: \
	virtual rettype function(pa1 p1, pa2 p2, pa3 p3) attrib

#define NPC_INTERFACE_HOOK4(function, rettype, pa1, pa2, pa3, pa4, attrib) \
private: \
	rettype function##_internal(pa1 p1, pa2 p2, pa3 p3, pa4 p4) attrib; \
public: \
	virtual rettype function(pa1 p1, pa2 p2, pa3 p3, pa4 p4) attrib

#define NPC_INTERFACE_HOOK5(function, rettype, pa1, pa2, pa3, pa4, pa5, attrib) \
private: \
	rettype function##_internal(pa1 p1, pa2 p2, pa3 p3, pa4 p4, pa5 p5) attrib; \
public: \
	virtual rettype function(pa1 p1, pa2 p2, pa3 p3, pa4 p4, pa5 p5) attrib

#define NPC_INTERFACE_DECLARE_HANDLER(classname, iface, function, attrib, rettype, params, paramscall) \
rettype classname##_Internal::##function##_internal params attrib \
{ \
	RETURN_META_VALUE(MRES_SUPERCEDE, m_pExternal##iface##Hook->function paramscall); \
} \
rettype classname##_Internal::##function params attrib \
{ \
	return SH_CALL(m_pReal##iface##Interface, &##iface::##function) paramscall; \
}


#define NPC_INTERFACE_DECLARE_HANDLER_void(classname, iface, function, attrib, params, paramscall) \
void classname##_Internal::##function##_internal params attrib \
{ \
	m_pExternal##iface##Hook->function paramscall; \
	RETURN_META(MRES_SUPERCEDE); \
} \
void classname##_Internal::##function params attrib \
{ \
	SH_CALL(m_pReal##iface##Interface, &##iface::##function) paramscall; \
	return; \
} \

// ============================================
// Interface Hooks for Extensions
// ============================================

NPC_DECLARE_INTERFACE(INextBotComponent)
{
	NPC_SETUP_HOOK(INextBotComponent);
	NPC_INTERFACE_HOOK0(Update, void, SH_NOATTRIB);
	NPC_INTERFACE_HOOK0(GetBot, INextBot*, const);
};

NPC_DECLARE_INTERFACE_INHERIT(ILocomotion, INextBotComponent)
{
	NPC_SETUP_HOOK(ILocomotion);
	NPC_INTERFACE_HOOK1(FaceTowards, void, Vector const&, SH_NOATTRIB);
	NPC_INTERFACE_HOOK0(IsAbleToJumpAcrossGaps, bool, const);
	NPC_INTERFACE_HOOK0(IsAbleToClimb, bool, const);
	NPC_INTERFACE_HOOK3(ClimbUpToLedge, bool, const Vector&, const Vector&, const CBaseEntity*, SH_NOATTRIB);
	NPC_INTERFACE_HOOK0(GetStepHeight, float, const);
	NPC_INTERFACE_HOOK0(GetMaxJumpHeight, float, const);
	NPC_INTERFACE_HOOK0(GetDeathDropHeight, float, const);
	NPC_INTERFACE_HOOK0(GetWalkSpeed, float, const);
	NPC_INTERFACE_HOOK0(GetRunSpeed, float, const);
	NPC_INTERFACE_HOOK0(GetMaxAcceleration, float, const);
	NPC_INTERFACE_HOOK1(ShouldCollideWith, bool, const CBaseEntity*, const);
	NPC_INTERFACE_HOOK2(IsEntityTraversable, bool, CBaseEntity*, ILocomotion::TraverseWhenType, const);
	NPC_INTERFACE_HOOK0(IsStuck, bool, const);
	NPC_INTERFACE_HOOK0(GetStuckDuration, float, const);
	NPC_INTERFACE_HOOK1(ClearStuckStatus, void, const char*, SH_NOATTRIB);
};

NPC_DECLARE_INTERFACE_INHERIT(NextBotGroundLocomotion, ILocomotion)
{
	NPC_SETUP_HOOK(NextBotGroundLocomotion);
	NPC_INTERFACE_HOOK1(SetAcceleration, void, const Vector&, SH_NOATTRIB);
	NPC_INTERFACE_HOOK0(GetAcceleration, const Vector&, const);
	NPC_INTERFACE_HOOK1(SetVelocity, void, const Vector&, SH_NOATTRIB);
	NPC_INTERFACE_HOOK0(GetGravity, float, const);
	NPC_INTERFACE_HOOK0(GetFrictionForward, float, const);
	NPC_INTERFACE_HOOK0(GetFrictionSideways, float, const);
	NPC_INTERFACE_HOOK0(GetMaxYawRate, float, const);
};

NPC_DECLARE_INTERFACE_INHERIT(IBody, INextBotComponent)
{
	NPC_SETUP_HOOK(IBody);
	NPC_INTERFACE_HOOK2(StartActivity, bool, Activity, unsigned int, SH_NOATTRIB);
	NPC_INTERFACE_HOOK0(GetHullWidth, float, const);
	NPC_INTERFACE_HOOK0(GetHullHeight, float, const);
	NPC_INTERFACE_HOOK0(GetStandHullHeight, float, const);
	NPC_INTERFACE_HOOK0(GetCrouchHullHeight, float, const);
	NPC_INTERFACE_HOOK0(GetHullMins, const Vector&, const);
	NPC_INTERFACE_HOOK0(GetHullMaxs, const Vector&, const);
	NPC_INTERFACE_HOOK0(GetSolidMask, unsigned int, const);
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

	virtual void Update() override;
	virtual bool StartActivity(Activity aAct, unsigned int iFlags) override;
	virtual float GetHullWidth() const override;
	virtual float GetHullHeight() const override;
	virtual float GetStandHullHeight() const override;
	virtual float GetCrouchHullHeight() const override;
	virtual const Vector& GetHullMins() const override;
	virtual const Vector& GetHullMaxs() const override;
	virtual unsigned int GetSolidMask() const override;

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

	virtual void Update() override;
	virtual void FaceTowards(Vector const& vecGoal) override;
	virtual bool IsAbleToJumpAcrossGaps() const override;
	virtual bool IsAbleToClimb() const override;
	virtual bool ClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity) override;
	virtual float GetStepHeight() const override;
	virtual float GetMaxJumpHeight() const override;
	virtual float GetDeathDropHeight() const override;
	virtual float GetWalkSpeed() const override;
	virtual float GetRunSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetGravity() const override;
	virtual bool ShouldCollideWith(const CBaseEntity* pCollider) const override;
	virtual bool IsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when) const override;
	virtual float GetFrictionForward() const override;
	virtual float GetFrictionSideways() const override;
	virtual float GetMaxYawRate() const override;


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
	~CBaseNPC();

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