
#include "baseentityoutput.h"
#include "baseentity.h"

CUtlMemoryPool * g_pEntityListPool = nullptr;
ISaveRestoreOps *eventFuncs = nullptr;

bool CBaseEntityOutput::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength, datamap_t* dataMap)
{
	SourceMod::IGameConfig* configCore;
	if (!gameconfs->LoadGameConfigFile("core.games", &configCore, error, maxlength))
	{
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

	// All outputs use eventFuncs as pSaveRestoreOps.
	CBaseEntity* pOffsetEnt = nullptr;
	datamap_t* pRootDataMap = dataMap;

	if (!pRootDataMap)
	{
		pOffsetEnt = servertools->CreateEntityByName("info_target");

		if (pOffsetEnt)
		{
			pRootDataMap = gamehelpers->GetDataMap(pOffsetEnt);
		}
	}

	for (datamap_t* pDataMap = pRootDataMap; pDataMap && !eventFuncs; pDataMap = pDataMap->baseMap)
	{
		for (int i = 0; i < pDataMap->dataNumFields; i++)
		{
			typedescription_t* pTypeDesc = &pDataMap->dataDesc[i];
			if (pTypeDesc->fieldType == FIELD_CUSTOM && (pTypeDesc->flags & FTYPEDESC_OUTPUT) && pTypeDesc->pSaveRestoreOps)
			{
				eventFuncs = pTypeDesc->pSaveRestoreOps;
				break;
			}
		}
	}

	if (pOffsetEnt)
	{
		servertools->RemoveEntityImmediate(pOffsetEnt);
	}

	if (!eventFuncs)
	{
		snprintf(error, maxlength, "Couldn't find eventFuncs!");
		return false;
	}

	gameconfs->CloseGameConfigFile(configCore);

    return true;
}

void CBaseEntityOutput::Init()
{
	m_ActionList = nullptr;

	m_Value.fieldType = FIELD_VOID;
	m_Value.iVal = 0;
}

void CBaseEntityOutput::Destroy()
{
    DeleteAllElements();
}

void CBaseEntityOutput::DeleteAllElements( void ) 
{
	CEventAction* pNext = m_ActionList;
	m_ActionList = NULL;
	while (pNext)
	{
		CEventAction* strikeThis = pNext;
		pNext = pNext->m_pNext;

		// This can be directly called like this because CUtlMemoryPool::Free() 
		// doesn't physically free memory, rather it marks the given memory 
		// block as free in its internal structure.
		g_pEntityListPool->Free(strikeThis);
	}
}

int CBaseEntityOutput::NumberOfElements( void )
{
	int count = 0;
	for ( CEventAction *ev = m_ActionList; ev != NULL; ev = ev->m_pNext )
	{
		count++;
	}
	return count;
}

float CBaseEntityOutput::GetMaxDelay(void)
{
	float flMaxDelay = 0;
	CEventAction *ev = m_ActionList;

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