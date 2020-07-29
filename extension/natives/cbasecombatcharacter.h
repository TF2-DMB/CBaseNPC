#ifndef NATIVES_BCC_H_INCLUDED_
#define NATIVES_BCC_H_INCLUDED_

#include "sourcesdk/basecombatcharacter.h"

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}
	
#define BASECOMBATCHARACTERNATIVE(name) \
	cell_t CBaseCombatCharacter_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CBaseEntity *pEnt; \
		ENTINDEX_TO_CBASEENTITY(params[1], pEnt) \
		CBaseCombatCharacterHack *pBCC = (CBaseCombatCharacterHack *)(pEnt); \
		if(!pBCC) \
		{ \
			return pContext->ThrowNativeError("Invalid entity %i", params[1]); \
		} \
		
BASECOMBATCHARACTERNATIVE(GetLastKnownArea)
	return (cell_t)pBCC->GetLastKnownArea();
}

BASECOMBATCHARACTERNATIVE(UpdateLastKnownArea)
	pBCC->UpdateLastKnownArea();
	return 0;
}
#endif