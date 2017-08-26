#ifndef NATIVES_AREA_H
#define NATIVES_AREA_H

#include "sourcesdk/nav_area.h"

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}

#define NAVAREA_NATIVE(name) \
	cell_t CNavArea_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CNavArea *pArea = (CNavArea *)(params[1]); \
		if(!pArea) { \
			return pContext->ThrowNativeError("Invalid nav area %x", params[1]); \
		} \

#define NAVLADDER_NATIVE(name) \
	cell_t CNavLadder_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CNavLadder *pArea = (CNavLadder *)(params[1]); \
		if(!pArea) { \
			return pContext->ThrowNativeError("Invalid nav area %x", params[1]); \
		} \
	
	
NAVAREA_NATIVE(GetCostSoFar)
	return sp_ftoc(pArea->GetCostSoFar());
}

NAVAREA_NATIVE(GetAttributes)
	return pArea->GetAttributes();
}

NAVAREA_NATIVE(GetCenter)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos = pArea->GetCenter();
	VectorToPawnVector(posAddr, pos);
	return 1;
}

NAVLADDER_NATIVE(length)
	return sp_ftoc(pArea->m_length);
}

#endif