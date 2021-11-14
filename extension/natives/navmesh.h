#ifndef NATIVES_NAVMESH_H_INCLUDED_
#define NATIVES_NAVMESH_H_INCLUDED_

#include "sourcesdk/nav_mesh.h"

#define NAVMESHNATIVE(name) \
	cell_t CNavMesh_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		if (!TheNavMesh) \
		{ \
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

NAVMESHNATIVE(AddressGet)
	return reinterpret_cast<cell_t>(TheNavMesh);
}

NAVMESHNATIVE(IsLoaded)
	return TheNavMesh->IsLoaded();
}

NAVMESHNATIVE(IsAnalyzed)
	return TheNavMesh->IsAnalyzed();
}

NAVMESHNATIVE(IsOutOfDate)
	return TheNavMesh->IsOutOfDate();
}

NAVMESHNATIVE(GetNavAreaCount)
	return TheNavMesh->GetNavAreaCount();
}

NAVMESHNATIVE(GetNearestNavArea)
	cell_t *vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	Vector vecPos;
	PawnVectorToVector(vecAddr, vecPos);
	return (cell_t)TheNavMesh->GetNearestNavArea(vecPos, params[3], sp_ctof(params[4]), params[5], params[6], params[7]);
}

NAVMESHNATIVE(GetNavAreaByID)
	unsigned int id = (unsigned int)params[2];
	return (cell_t)TheNavMesh->GetNavAreaByID(id);
}

NAVMESHNATIVE(CollectSurroundingAreas)
	
	CUtlVector< CNavArea * > *pCollector = new CUtlVector< CNavArea * >;
	CollectSurroundingAreas( pCollector, (CNavArea *)params[2], sp_ctof(params[3]), sp_ctof(params[4]), sp_ctof(params[5]));
	return CREATEHANDLE(SurroundingAreasCollector, pCollector);
}

cell_t TheHidingSpotsVector_LengthGet(IPluginContext* pContext, const cell_t* params)
{
	return TheHidingSpots.Count();
}

cell_t TheHidingSpotsVector_Get(IPluginContext* pContext, const cell_t* params)
{
	int i = params[2];
	if ((i < 0) || (i >= TheHidingSpots.Count()))
		return NULL;
	return reinterpret_cast<cell_t>(TheHidingSpots[i]);
}

cell_t smn_GetHidingSpotByID(IPluginContext* pContext, const cell_t* params)
{
	unsigned int id = params[1];
	return reinterpret_cast<cell_t>(GetHidingSpotByID(id));
}

COLLECTORNATIVE(Count)
	return pCollector->Count();
}

COLLECTORNATIVE(Get)
	int ID = params[2];
	if (ID > pCollector->Count())
	{
		return 0;
	}
	return (cell_t)pCollector->Element(ID);
}

#endif