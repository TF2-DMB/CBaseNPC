#ifndef NPC_TOOLS_H
#define NPC_TOOLS_H

#define SMINTERFACE_NPCTOOLS_NAME		"NPCTools"
#define SMINTERFACE_NPCTOOLS_VERSION	1
#define MAX_NPCS						200
#define INVALID_NPC_ID					-1

class CExtNPC;
class INextBot;
class IBody;
class ILocomotion;
class CBaseEntity;
class NextBotGroundLocomotion;
class INextBotEventResponder_Hook;
class ILocomotion_Hook;
class NextBotGroundLocomotion_Hook;
class IBody_Hook;

#include <IShareSys.h>

class IBaseNPC_Tools : public SourceMod::SMInterface
{
public:
	virtual const char* GetInterfaceName() = 0;
	virtual unsigned int GetInterfaceVersion() = 0;
public:
	/**
	 * @brief Grants an index to an npc entity, this index will be used by plugins to store information
	 *
	 * @return			NPC index, -1 otherwise
	 */
	virtual int GrantID(CBaseEntity* ent, CExtNPC* npc) = 0;

	/**
	 * @brief Retrieves the nextbot interface from a given entity;
	 *
	 * @return			Returns nextbot interface, NULL otherwise
	 */
	virtual INextBot* GetNextBotOfEntity(CBaseEntity* pEntity) = 0;

	/**
	 * @brief Hooks and build a ILocomotion_Hook - You shouldn't have to call this directly
	 *
	 * @return			Internal ILocomotion_Hook pointer
	 */
	virtual ILocomotion_Hook* Hook_ILocomotion(ILocomotion* mover, ILocomotion_Hook* realHook) = 0;

	/**
	 * @brief Hooks and build a NextBotGroundLocomotion_Hook - You shouldn't have to call this directly
	 *
	 * @return			Internal NextBotGroundLocomotion_Hook pointer
	 */
	virtual NextBotGroundLocomotion_Hook* Hook_NextBotGroundLocomotion(NextBotGroundLocomotion* mover, NextBotGroundLocomotion_Hook* realHook) = 0;

	/**
	 * @brief Hooks and build a NextBotGroundLocomotion_Hook - You shouldn't have to call this directly
	 *
	 * @return			Internal NextBotGroundLocomotion_Hook pointer
	 */
	virtual IBody_Hook* Hook_IBody(IBody* mover, IBody_Hook* realHook) = 0;
};

extern IBaseNPC_Tools* g_pBaseNPCTools;

#endif // NPC_TOOLS_H