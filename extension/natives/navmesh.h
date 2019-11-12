#ifndef NATIVES_NAVMESH_H_INCLUDED_
#define NATIVES_NAVMESH_H_INCLUDED_

#include "sourcesdk/nav_mesh.h"
#include "sourcesdk/nav_threaded.h"

#define NAVMESHNATIVE(name) \
	cell_t CNavMesh_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		if(!TheNavMesh) { \
			return pContext->ThrowNativeError("TheNavMesh isn't initialized!"); \
		} \
		

#define COLLECTORNATIVE(name) \
	cell_t SurroundingAreasCollector_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		HandleSecurity security; \
		security.pOwner = NULL; \
		security.pIdentity = myself->GetIdentity(); \
		Handle_t hndlObject = static_cast<Handle_t>(params[1]); \
		CUtlVector< CNavArea * > *pCollector = nullptr; \
		READHANDLE(hndlObject, SurroundingAreasCollector, pCollector) \
	
NAVMESHNATIVE(GetNearestNavArea)
	cell_t *vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	Vector vecPos;
	PawnVectorToVector(vecAddr, vecPos);
	return (cell_t)TheNavMesh->GetNearestNavArea(vecPos, params[3], sp_ctof(params[4]), params[5], params[6], params[7]);
}

NAVMESHNATIVE(CollectSurroundingAreas)
	
	CUtlVector< CNavArea * > *pCollector = new CUtlVector< CNavArea * >;
	CollectSurroundingAreas( pCollector, (CNavArea *)params[2], sp_ctof(params[3]), sp_ctof(params[4]), sp_ctof(params[5]));
	return CREATEHANDLE(SurroundingAreasCollector, pCollector);
}

NAVMESHNATIVE(TCollectSurroundingAreas)

	CTNavMesh::CollectNavThreadedData *pData = new CTNavMesh::CollectNavThreadedData((CNavArea*)params[2], sp_ctof(params[3]), sp_ctof(params[4]), sp_ctof(params[5]), pContext->GetFunctionById(params[6]), params[7]);
	CTNavMesh::CollectSurroundingAreas(pData);
	return 1;
}

COLLECTORNATIVE(Count)
	return pCollector->Count();
}


COLLECTORNATIVE(Get)
	int ID = params[2];
	if ( ID > pCollector->Count()) return NULL;
	return (cell_t) pCollector->Element(ID);
}

#endif