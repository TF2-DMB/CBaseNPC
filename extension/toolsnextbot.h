#pragma once

#include "sourcesdk/basecombatcharacter.h"
#include "sourcesdk/NextBot/NextBotInterface.h"

class ToolsNextBot : public INextBot
{
	virtual CBaseCombatCharacterHack* GetEntity() const { return m_linkedEntity; }
	virtual NextBotCombatCharacter* GetNextBotCombatCharacter() const { return nullptr; };
	virtual ILocomotion *GetLocomotionInterface() const { return nullptr; };
	virtual IVision *GetVisionInterface() const { return nullptr; };
protected:
	CBaseCombatCharacterHack* m_linkedEntity;
};