#ifndef NATIVES_NAVMESH_H_INCLUDED_
#define NATIVES_NAVMESH_H_INCLUDED_

#include "sourcesdk/nav_mesh.h"

CUtlVector< CNavArea * > areaCollector;

#define NAVMESHNATIVE(name) \
	cell_t CNavMesh_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		if(!TheNavMesh) { \
			return pContext->ThrowNativeError("TheNavMesh isn't initialized!"); \
		} \
		

#define COLLECTORNATIVE(name) \
	cell_t SurroundingAreasCollector_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		if(!TheNavMesh) { \
			return pContext->ThrowNativeError("TheNavMesh isn't initialized!"); \
		} \
	
NAVMESHNATIVE(GetNearestNavArea)
	cell_t *vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	Vector vecPos;
	PawnVectorToVector(vecAddr, vecPos);
	return (cell_t)TheNavMesh->GetNearestNavArea(vecPos, params[3], sp_ctof(params[4]), params[5], params[6], params[7]);
}

NAVMESHNATIVE(CollectSurroundingAreas)
	
	CollectSurroundingAreas( &areaCollector, (CNavArea *)params[2], sp_ctof(params[3]), sp_ctof(params[4]), sp_ctof(params[5]));
	return 1;
}

COLLECTORNATIVE(Count)
	return areaCollector.Count();
}


COLLECTORNATIVE(Get)
	int ID = params[2];
	if ( ID > areaCollector.Count()) return NULL;
	return (cell_t)areaCollector[ID];
}

#endif