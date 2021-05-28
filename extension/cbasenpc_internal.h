#ifndef CBASENPC_INTERNAL_H_SHARED
#define CBASENPC_INTERNAL_H_SHARED

#include <utlvector.h>

#include "shared/cbasenpc.h"
#include "extension.h"
#include "sourcesdk/customfactory.h"

// ======================
// CBaseNPC for plugins
// ======================

class CBaseNPC_Body : public IBody
{
public:
	CBaseNPC_Body(INextBot* bot);

	virtual void Update() override final;
	virtual bool StartActivity(Activity aAct, unsigned int iFlags) override final;
	virtual float GetHullWidth() const override final;
	virtual float GetHullHeight() const override final;
	virtual float GetStandHullHeight() const override final;
	virtual float GetCrouchHullHeight() const override final;
	virtual const Vector& GetHullMins() const override final;
	virtual const Vector& GetHullMaxs() const override final;
	virtual unsigned int GetSolidMask() const override final;

	Vector m_vecBodyMins;
	Vector m_vecBodyMaxs;
};

class CBaseNPC_Locomotion : public NextBotGroundLocomotion
{
public:
	static CBaseNPC_Locomotion* New(INextBot* bot);
	void Destroy();
	void Init();

	void Hook_Update();
	bool Hook_IsAbleToJumpAcrossGaps() const;
	bool Hook_IsAbleToClimb() const;
	bool Hook_ClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity);
	float Hook_GetStepHeight() const;
	float Hook_GetMaxJumpHeight() const;
	float Hook_GetDeathDropHeight() const;
	float Hook_GetWalkSpeed() const;
	float Hook_GetRunSpeed() const;
	float Hook_GetMaxAcceleration() const;
	float Hook_GetGravity() const;
	bool Hook_ShouldCollideWith(const CBaseEntity* pCollider) const;
	bool Hook_IsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when) const;
	float Hook_GetFrictionForward() const;
	float Hook_GetFrictionSideways() const;
	float Hook_GetMaxYawRate() const;

private:
	std::vector<int> m_hookids;
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
};

class NextBotCombatCharacter;

class CBaseNPC_Entity : public NextBotCombatCharacter
{
public:
	class CBaseNPC : public CExtNPC
	{
	public:
		CBaseNPC(NextBotCombatCharacter* ent);
		~CBaseNPC();

		std::vector<int> m_hookids;
		CBaseNPC_Locomotion* m_pMover;
		CBaseNPC_Body* m_pBody;
		char m_type[64];

		void Hook_Spawn(void);
		ILocomotion* Hook_GetLocomotionInterface(void) const;
		IBody* Hook_GetBodyInterface(void) const;
	};
	CBaseNPC_Entity::CBaseNPC* GetNPC(void);
	void BotUpdateOnRemove(void);
	void BotThink(void);
	void BotSpawn(void);
	int OnTakeDamage(const CTakeDamageInfo& info);
	int OnTakeDamage_Alive(const CTakeDamageInfo& info);

	ILocomotion* GetLocomotionInterface() const;
	IBody* GetBodyInterface() const;

	static void** vtable;
	static MCall<void> mOriginalSpawn;
	static MCall<void> mOriginalUpdateOnRemove;
	static MCall<int, const CTakeDamageInfo&> mOriginalOnTakeDamage;
	static MCall<int, const CTakeDamageInfo&> mOriginalOnTakeDamage_Alive;
};

class CBaseNPCFactory : CustomFactory
{
public:
	CBaseNPCFactory();
	virtual size_t GetEntitySize() override final;
	virtual void Create_Extra(CBaseEntityHack* ent) override final;
	virtual void Create_PostConstructor(CBaseEntityHack* ent) override final;
};

extern CBaseNPCFactory* g_pBaseNPCFactory;

#endif