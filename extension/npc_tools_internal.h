#ifndef NPC_TOOLS_INTERNAL_H
#define NPC_TOOLS_INTERNAL_H

#include "shared/npctools.h"

class BaseNPC_Tools_API : public IBaseNPC_Tools
{
public:
	virtual const char* GetInterfaceName() override final;
	virtual unsigned int GetInterfaceVersion() override final;
	virtual int GrantID(CBaseEntity* ent, CExtNPC* npc) override final;
	virtual CExtNPC* DeleteNPC(CExtNPC* npc) override final;
	virtual CExtNPC* DeleteNPCByEntIndex(int index) override final;
	virtual INextBot* GetNextBotOfEntity(CBaseEntity* pEntity) override final;
};

extern CExtNPC* g_objNPC[MAX_NPCS];
extern CExtNPC* g_objNPC2[2049];

#endif // NPC_TOOLS_INTERNAL_H