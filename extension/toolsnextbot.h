#pragma once

#include "sourcesdk/basecombatcharacter.h"
#include "sourcesdk/NextBot/NextBotInterface.h"
#include "sourcesdk/NextBot/NextBotManager.h"
#include "cbasenpc_behavior.h"

#include <vector>

class ToolsNextBot : public INextBot
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	ToolsNextBot(CBaseCombatCharacterHack* link);

	virtual CBaseCombatCharacterHack* GetEntity() const override { return m_linkedEntity; }
	virtual NextBotCombatCharacter* GetNextBotCombatCharacter() const override { return nullptr; };
	virtual ILocomotion *GetLocomotionInterface() const override { return nullptr; };
	virtual IVision *GetVisionInterface() const override { return nullptr; };
	virtual IIntention *GetIntentionInterface() const override { return nullptr; };

protected:
	CBaseCombatCharacterHack* m_linkedEntity;
};

class ToolsNextBotPlayer : public ToolsNextBot
{
public:
	ToolsNextBotPlayer(CBaseCombatCharacterHack* link);

// INextbot
	virtual void Update() override;

// Source events propagation
	int  Hook_OnTakeDamage_Alive(const CTakeDamageInfo& info);
	int  Hook_OnTakeDamage_Dying(const CTakeDamageInfo& info);
	void Hook_Event_Killed(const CTakeDamageInfo& info);
	void Hook_HandleAnimEvent(animevent_t* event);
	void Hook_OnNavAreaChanged(CNavArea* enteredArea, CNavArea* leftArea);
	void Hook_Touch(CBaseEntityHack* other);
	void Hook_Weapon_Equip(CBaseEntityHack* weapon);
	void Hook_Weapon_Drop(CBaseEntityHack* weapon, const Vector* target, const Vector* velocity);

// Our interface
	virtual bool IsDormantWhenDead(void) const;

protected:
	IntervalTimer m_burningTimer;
	std::vector<int> m_hooks;
};