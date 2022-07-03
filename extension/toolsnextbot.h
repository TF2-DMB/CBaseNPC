#pragma once

#include "sourcesdk/basecombatcharacter.h"
#include "sourcesdk/NextBot/NextBotInterface.h"

#include "cbasenpc_behavior.h"

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