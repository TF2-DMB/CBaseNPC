#include "sourcesdk/NextBot/NextBotInterface.h"
#include "sourcesdk/NextBot/NextBotComponentInterface.h"
#include "sourcesdk/baseentity.h"

bool IgnoreActorsTraceFilterFunction(IHandleEntity *pServerEntity, int contentsMask)
{
	CBaseEntityHack* entity = (CBaseEntityHack *)EntityFromEntityHandle(pServerEntity);
	return (entity->MyCombatCharacterPointer() == NULL);
}

void INextBot::RegisterComponent(INextBotComponent *comp)
{
	comp->m_nextComponent = m_componentList;
	m_componentList = comp;
}