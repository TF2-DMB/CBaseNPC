#include "toolsnextbot.h"

ToolsNextBot::ToolsNextBot(CBaseCombatCharacterHack* link, CBaseNPCPluginActionFactory* factory) :
	m_linkedEntity(link),
	m_IntentionInterface(this, factory)
{
}