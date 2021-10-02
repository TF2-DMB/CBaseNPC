#ifndef CBASENPC_LOCOMOTION_H_SHARED
#define CBASENPC_LOCOMOTION_H_SHARED

#include <map>
#include <stack>

#include "sourcesdk/NextBot/NextBotGroundLocomotion.h"
#include "helpers.h"

class CBaseNPC_Locomotion : public NextBotGroundLocomotion
{
public:
	typedef NextBotGroundLocomotion BaseClass;

	enum CallbackType
	{
		CallbackType_IsAbleToJumpAcrossGaps = 0,
		CallbackType_IsJumpingAcrossGap,
		CallbackType_JumpAcrossGap,
		CallbackType_IsAbleToClimb,
		CallbackType_IsClimbingUpToLedge,
		CallbackType_ClimbUpToLedge,
		CallbackType_ShouldCollideWith,
		CallbackType_IsEntityTraversable
	};

	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);
	static CBaseNPC_Locomotion* New(INextBot* bot);
	void Construct();
	void Destroy();

	bool IsInCallback() const { return !m_pCallbackTypeStack->empty(); }
	CallbackType GetCurrentCallbackType() const { return m_pCallbackTypeStack->top(); };
	IPluginFunction* GetCallback(CallbackType cbType) const;
	void SetCallback(CallbackType cbType, IPluginFunction* pCallback);

private:
	// Non-virtual overrides of NextBotGroundLocomotion.

	// NOTE: Must be non-virtual otherwise Windows will create a separate
	// thunk function just to call the function virtually all over again, resulting
	// in an infinite loop.
	void V_Update();
	bool V_IsAbleToJumpAcrossGaps();
	bool V_IsJumpingAcrossGap();
	void V_JumpAcrossGap(const Vector &landingGoal, const Vector &landingForward);
	bool V_IsAbleToClimb();
	bool V_IsClimbingUpToLedge();
	bool V_ClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity);
	float V_GetStepHeight();
	float V_GetMaxJumpHeight();
	float V_GetDeathDropHeight();
	float V_GetWalkSpeed();
	float V_GetRunSpeed() ;
	float V_GetMaxAcceleration();
	float V_GetGravity();
	bool V_ShouldCollideWith(const CBaseEntity* pCollider);
	bool V_IsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when);
	float V_GetFrictionForward();
	float V_GetFrictionSideways();
	float V_GetMaxYawRate();

public:
	bool DefaultIsAbleToClimb();
	bool DefaultIsClimbingUpToLedge();
	bool DefaultClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity);
	bool DefaultIsAbleToJumpAcrossGaps();
	bool DefaultIsJumpingAcrossGap();
	void DefaultJumpAcrossGap(const Vector &landingGoal, const Vector &landingForward);
	bool DefaultIsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when);
	bool DefaultShouldCollideWith(const CBaseEntity* pCollider);

private:
	static void** CreateVTable(BaseClass* pBase);

private:
	static void** vtable;
	
private:
	std::vector<int> *m_pHookIds;
public:
	float m_flJumpHeight;
	float m_flStepSize;
	float m_flGravity;
	float m_flAcceleration;
	float m_flDeathDropHeight;
	float m_flWalkSpeed;
	float m_flRunSpeed;
	float m_flFrictionForward;
	float m_flFrictionSideways;
	float m_flMaxYawRate;

private:
	std::map<CallbackType, IPluginFunction*> *m_pCallbacks;
	std::stack<CallbackType> *m_pCallbackTypeStack;
};

#endif