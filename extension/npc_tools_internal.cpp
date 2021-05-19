#include "extension.h"
#include "npc_tools_internal.h"
#include "cbasenpc_internal.h"
#include "ehandle.h"
#include "sourcesdk/baseentity.h"

EHANDLE g_registeredNPCS[MAX_NPCS];
CExtNPC* g_objNPC[MAX_NPCS];
CExtNPC* g_objNPC2[2049];

const char* BaseNPC_Tools_API::GetInterfaceName()
{
	return SMINTERFACE_NPCTOOLS_NAME;
}

unsigned int BaseNPC_Tools_API::GetInterfaceVersion()
{
	return SMINTERFACE_NPCTOOLS_VERSION;
}

int BaseNPC_Tools_API::GrantID(CBaseEntity* ent, CExtNPC* npc)
{
	if (!ent)
	{
		return INVALID_NPC_ID;
	}

	int searchIndex = gamehelpers->EntityToBCompatRef(ent);
	if (searchIndex <= 0 || searchIndex > 2048)
	{
		return INVALID_NPC_ID;
	}

	int freeID = INVALID_NPC_ID;
	for (int ID = 0; ID < MAX_NPCS; ID++)
	{
		int entIndex = g_registeredNPCS[ID].GetEntryIndex();
		if (entIndex == searchIndex)
		{
			g_objNPC[ID] = npc;
			g_objNPC2[entIndex] = npc;
			return ID;
		}
		else if (freeID == INVALID_NPC_ID && (entIndex <= 32 || entIndex > 2048))
		{
			freeID = ID;
		}
	}

	if (freeID != INVALID_NPC_ID)
	{
		g_registeredNPCS[freeID] = ent;
		g_objNPC[freeID] = npc;
		g_objNPC2[searchIndex] = npc;
		return freeID;
	}
	return INVALID_NPC_ID;
}

CExtNPC* BaseNPC_Tools_API::DeleteNPC(CExtNPC* npc)
{
	if (npc->GetID() == INVALID_NPC_ID)
	{
		return nullptr;
	}
	
	if (npc->GetEntity() != g_registeredNPCS[npc->GetID()].Get())
	{
		return nullptr;
	}

	g_objNPC[npc->GetID()] = nullptr;
	g_objNPC2[gamehelpers->EntityToBCompatRef(npc->GetEntity())] = nullptr;
	g_registeredNPCS[npc->GetID()] = nullptr;
	return npc;
}

CExtNPC* BaseNPC_Tools_API::DeleteNPCByEntIndex(int index)
{
	if (index <= 0 || index > 2048)
	{
		return nullptr;
	}

	CExtNPC* npc = g_objNPC2[index];
	g_objNPC2[index] = nullptr;
	if (npc && npc->GetID() != INVALID_NPC_ID)
	{
		g_objNPC[npc->GetID()] = nullptr;
		g_registeredNPCS[npc->GetID()] = nullptr;
	}
	return npc;
}

INextBot* BaseNPC_Tools_API::GetNextBotOfEntity(CBaseEntity* pEntity)
{
	return ((CBaseEntityHack *)pEntity)->MyNextBotPointer();
}