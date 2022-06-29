#pragma once

#include "sourcesdk/basecombatcharacter.h"
#include "sourcesdk/NextBot/NextBotInterface.h"

#include "cbasenpc_behavior.h"

class ToolsNextBot : public INextBot
{
public:
	ToolsNextBot(CBaseCombatCharacterHack* link, CBaseNPCPluginActionFactory* factory);

	virtual CBaseCombatCharacterHack* GetEntity() const override { return m_linkedEntity; }
	virtual NextBotCombatCharacter* GetNextBotCombatCharacter() const override { return nullptr; };
	virtual ILocomotion *GetLocomotionInterface() const override { return nullptr; };
	virtual IVision *GetVisionInterface() const override { return nullptr; };
	virtual IIntention *GetIntentionInterface() const override { return (IIntention*)&m_IntentionInterface; }

protected:
	CBaseCombatCharacterHack* m_linkedEntity;
	CBaseNPCIntention m_IntentionInterface;
};