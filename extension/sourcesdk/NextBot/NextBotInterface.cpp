#include "NextBotInterface.h"
#include "NextBotComponentInterface.h"
#include "sourcesdk/baseentity.h"
#include "sourcesdk/basecombatcharacter.h"

ConVar* NextBotDebugHistory = nullptr;

bool IgnoreActorsTraceFilterFunction(IHandleEntity *pServerEntity, int contentsMask)
{
	CBaseEntityHack* entity = (CBaseEntityHack *)EntityFromEntityHandle(pServerEntity);
	return (entity->MyCombatCharacterPointer() == NULL);
}

void INextBot::Destroy()
{
	// ResetDebugHistory();
}

void INextBot::RegisterComponent(INextBotComponent *comp)
{
	comp->m_nextComponent = m_componentList;
	m_componentList = comp;
}

const char *INextBot::GetDebugIdentifier( void ) const
{
	const int nameSize = 256;
	static char name[ nameSize ];
	
	Q_snprintf( name, nameSize, "%s(#%d)", const_cast< INextBot * >( this )->GetEntity()->GetClassname(), GetEntity()->entindex() );

	return name;
}

void INextBot::ResetDebugHistory( void )
{
}

void INextBot::DebugConColorMsg( NextBotDebugType debugType, const Color &color, const char *fmt, ... )
{
}