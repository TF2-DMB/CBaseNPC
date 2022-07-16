#pragma once

#include "sourcesdk/basecombatcharacter.h"
#include "sourcesdk/NextBot/NextBotInterface.h"

#include "cbasenpc_behavior.h"

#include <vector>

class ToolsNextBot : public INextBot
{
public:
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

	virtual int Hook_OnTakeDamage_Alive(const CTakeDamageInfo& info);
	virtual int Hook_OnTakeDamage_Dying(const CTakeDamageInfo& info);
	virtual void Hook_Event_Killed(const CTakeDamageInfo& info);
	virtual void Hook_HandleAnimEvent(animevent_t* event);
	virtual void Hook_OnNavAreaChanged(CNavArea* enteredArea, CNavArea* leftArea);
	virtual void Hook_Touch(CBaseEntityHack* other);
	virtual void Hook_Weapon_Equip(CBaseEntityHack* weapon);
	virtual	void Hook_Weapon_Drop(CBaseEntityHack* weapon, const Vector* target, const Vector* velocity);

protected:
	std::vector<int> m_hooks;
};