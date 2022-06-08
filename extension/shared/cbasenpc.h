#ifndef CBASENPC_H_SHARED
#define CBASENPC_H_SHARED

#include <ehandle.h>
#include <ai_activity.h>
#include "npctools.h"

#include "sourcesdk/NextBot/NextBotInterface.h"
#include "sourcesdk/NextBot/NextBotLocomotionInterface.h"
#include "sourcesdk/NextBot/NextBotGroundLocomotion.h"
#include "sourcesdk/NextBot/NextBotBodyInterface.h"
#include "sourcesdk/NextBot/NextBotCombatCharacter.h"

// Basic NPC object, the extension will give an index so that plugins can coordinate and store information accordingly
// IMPORTANT NOTE: THIS IS NOT A CBASENPC OBJECT AND AS SUCH SHOULDNT BE USED BY PLUGINS TO CALL CBASENPC NATIVES!!!!!
class CustomFactory;

class CExtNPC
{
public:
	CExtNPC() : m_iIndex(INVALID_NPC_ID), m_hEntity(nullptr) {};
	virtual ~CExtNPC() { g_pBaseNPCTools->DeleteNPC(this); };

	inline int GetID()
	{
		return m_iIndex;
	};

	CBaseEntityHack* GetEntity() const
	{
		return (CBaseEntityHack*)m_hEntity.Get();
	};

	void SetEntity(CBaseEntity* ent)
	{
		m_iIndex = g_pBaseNPCTools->GrantID(ent, this);
		m_hEntity = ent;
	};

private:
	int m_iIndex;
	EHANDLE m_hEntity;
};

#endif
