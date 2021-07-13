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
	ResetDebugHistory();
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
	for ( int i=0; i<m_debugHistory.Count(); ++i )
	{
		delete m_debugHistory[i];
	}

	m_debugHistory.RemoveAll();
}

void INextBot::DebugConColorMsg( NextBotDebugType debugType, const Color &color, const char *fmt, ... )
{
	bool isDataFormatted = false;

	va_list argptr;
	char data[ MAX_NEXTBOT_DEBUG_LINE_LENGTH ];

	
	if ( g_cvDeveloper->GetBool() && IsDebugging( debugType ) )
	{
		va_start(argptr, fmt);
		Q_vsnprintf(data, sizeof( data ), fmt, argptr);
		va_end(argptr);
		isDataFormatted = true;

		ConColorMsg( color, "%s", data );
	}

	if ( !NextBotDebugHistory->GetBool() )
	{
		if ( m_debugHistory.Count() )
		{
			ResetDebugHistory();
		}
		return;
	}

	// Don't bother with event data - it's spammy enough to overshadow everything else.
	if ( debugType == NEXTBOT_EVENTS )
		return;

	if ( !isDataFormatted )
	{
		va_start(argptr, fmt);
		Q_vsnprintf(data, sizeof( data ), fmt, argptr);
		va_end(argptr);
		isDataFormatted = true;
	}

	int lastLine = m_debugHistory.Count() - 1;
	if ( lastLine >= 0 )
	{
		NextBotDebugLineType *line = m_debugHistory[lastLine];
		if ( line->debugType == debugType && V_strstr( line->data, "\n" ) == NULL )
		{
			// append onto previous line
			V_strncat( line->data, data, MAX_NEXTBOT_DEBUG_LINE_LENGTH );
			return;
		}
	}

	// Prune out an old line if needed, keeping a pointer to re-use the memory
	NextBotDebugLineType *line = NULL;
	if ( m_debugHistory.Count() == MAX_NEXTBOT_DEBUG_HISTORY )
	{
		line = m_debugHistory[0];
		m_debugHistory.Remove( 0 );
	}

	// Add to debug history
	if ( !line )
	{
		line = new NextBotDebugLineType;
	}
	line->debugType = debugType;
	V_strncpy( line->data, data, MAX_NEXTBOT_DEBUG_LINE_LENGTH );
	m_debugHistory.AddToTail( line );
}