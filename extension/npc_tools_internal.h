#ifndef NPC_TOOLS_INTERNAL_H
#define NPC_TOOLS_INTERNAL_H

#include "shared/npctools.h"

class BaseNPC_Tools_API : public IBaseNPC_Tools
{
public:
	virtual const char* GetInterfaceName();
	virtual unsigned int GetInterfaceVersion();
	virtual int GrantID(CBaseEntity* ent, CExtNPC* npc);
	virtual INextBot* GetNextBotOfEntity(CBaseEntity* pEntity);
	virtual ILocomotion_Hook* Hook_ILocomotion(ILocomotion* mover, ILocomotion_Hook* realHook);
	virtual NextBotGroundLocomotion_Hook* Hook_NextBotGroundLocomotion(NextBotGroundLocomotion* mover, NextBotGroundLocomotion_Hook* realHook);
	virtual IBody_Hook* Hook_IBody(IBody* mover, IBody_Hook* realHook);
};

extern CExtNPC* g_objNPC[MAX_NPCS];
extern CExtNPC* g_objNPC2[2049];

#endif // NPC_TOOLS_INTERNAL_H