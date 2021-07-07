
#include "baseentityoutput.h"

CUtlMemoryPool * g_pEntityListPool = nullptr;
ISaveRestoreOps *eventFuncs = nullptr;

bool CBaseEntityOutputHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	SourceMod::IGameConfig* configCore;
	if (!gameconfs->LoadGameConfigFile("core.games", &configCore, error, maxlength))
	{
		return false;
	}

    try
    {
        //mFireOutput.Init(configCore, "FireOutput");
    }
    catch (const std::exception & e)
	{
		// Could use strncpy, but compiler complains
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}

	uint8_t* addr;

	// g_EntityListPool
	if (config->GetMemSig("g_EntityListPool", (void**)&addr) && addr)
	{
#ifndef WIN32
		g_pEntityListPool = reinterpret_cast<CUtlMemoryPool*>(addr);
#else
		int offset;
		if (!config->GetOffset("g_EntityListPool", &offset) || !offset)
		{
			snprintf(error, maxlength, "Couldn't find offset for g_EntityListPool ptr!");
			return false;
		}
		g_pEntityListPool = *reinterpret_cast<CUtlMemoryPool**>(addr + offset);
#endif
	}
	else
	{
		snprintf(error, maxlength, "Couldn't find g_EntityListPool!");
		return false;
	}

	// eventFuncs
	if (config->GetMemSig("eventFuncs", (void**)&addr) && addr)
	{
#ifndef WIN32
		eventFuncs = *reinterpret_cast<ISaveRestoreOps**>(addr);
#else
		int offset;
		if (!config->GetOffset("eventFuncs", &offset) || !offset)
		{
			snprintf(error, maxlength, "Couldn't find offset for eventFuncs ptr!");
			return false;
		}
		eventFuncs = **reinterpret_cast<ISaveRestoreOps***>(addr + offset);
#endif
	}
	else
	{
		snprintf(error, maxlength, "Couldn't find eventFuncs!");
		return false;
	}

	gameconfs->CloseGameConfigFile(configCore);

    return true;
}

void CBaseEntityOutputHack::Init()
{
	m_ActionList = nullptr;

	m_Value.fieldType = FIELD_VOID;
	m_Value.iVal = 0;
}

void CBaseEntityOutputHack::Destroy()
{
    DeleteAllElements();
}

void CBaseEntityOutputHack::DeleteAllElements( void ) 
{
	CEventActionHack* pNext = m_ActionList;
	m_ActionList = NULL;
	while (pNext)
	{
		CEventActionHack *strikeThis = pNext;
		pNext = pNext->m_pNext;

		g_pEntityListPool->Free(strikeThis);
	}
}

int CBaseEntityOutputHack::NumberOfElements( void )
{
	int count = 0;
	for ( CEventActionHack *ev = m_ActionList; ev != NULL; ev = ev->m_pNext )
	{
		count++;
	}
	return count;
}

float CBaseEntityOutputHack::GetMaxDelay(void)
{
	float flMaxDelay = 0;
	CEventActionHack *ev = m_ActionList;

	while (ev != NULL)
	{
		if (ev->m_flDelay > flMaxDelay)
		{
			flMaxDelay = ev->m_flDelay;
		}
		ev = ev->m_pNext;
	}

	return flMaxDelay;
}