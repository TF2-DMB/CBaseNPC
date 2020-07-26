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
	if (!ent) return INVALID_NPC_ID; // What the hell

	int searchIndex = gamehelpers->EntityToBCompatRef(ent);
	if (searchIndex <= 0 || searchIndex > 2048) return INVALID_NPC_ID;

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
		else if (entIndex <= 32)
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

INextBot* BaseNPC_Tools_API::GetNextBotOfEntity(CBaseEntity* pEntity)
{
	return ((CBaseEntityHack *)pEntity)->MyNextBotPointer();
}

ILocomotion_Hook* BaseNPC_Tools_API::Hook_ILocomotion(ILocomotion* mover, ILocomotion_Hook* realHook)
{
	return new ILocomotion_Hook_Internal(realHook, mover);
}

NextBotGroundLocomotion_Hook* BaseNPC_Tools_API::Hook_NextBotGroundLocomotion(NextBotGroundLocomotion* mover, NextBotGroundLocomotion_Hook* realHook)
{
	return new NextBotGroundLocomotion_Hook_Internal(realHook, mover);
}

IBody_Hook* BaseNPC_Tools_API::Hook_IBody(IBody* body, IBody_Hook* realHook)
{
	return new IBody_Hook_Internal(realHook, body);
}