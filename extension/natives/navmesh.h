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

class SMPathCost : public IPathCost
{
public:
	SMPathCost(IPluginFunction *pFunc) : m_pFunc(pFunc)
	{
	}

	virtual float operator()(CNavArea *area, CNavArea *fromArea, const CNavLadder *ladder, const CFuncElevator *elevator, float length) const override final
	{
		if (m_pFunc && m_pFunc->IsRunnable())
		{
			cell_t result = sp_ftoc(0.0);
			m_pFunc->PushCell(reinterpret_cast<cell_t>(area));
			m_pFunc->PushCell(reinterpret_cast<cell_t>(fromArea));
			m_pFunc->PushCell(reinterpret_cast<cell_t>(ladder));
			m_pFunc->PushCell(gamehelpers->EntityToBCompatRef((CBaseEntity *)elevator));
			m_pFunc->PushFloat(length);
			m_pFunc->Execute(&result);
			return sp_ctof(result);
		}
		else
		{
			if ( fromArea == nullptr )
			{
				// first area in path, no cost
				return 0.0f;
			}
			else
			{
				// compute distance traveled along path so far
				float dist;

				if ( ladder )
				{
					dist = ladder->m_length;
				}
				else if ( length > 0.0 )
				{
					dist = length;
				}
				else
				{
					dist = ( area->GetCenter() - fromArea->GetCenter() ).Length();
				}

				float cost = dist + fromArea->GetCostSoFar();

				// if this is a "crouch" area, add penalty
				if ( area->GetAttributes() & NAV_MESH_CROUCH )
				{
					const float crouchPenalty = 20.0f;		// 10
					cost += crouchPenalty * dist;
				}

				// if this is a "jump" area, add penalty
				if ( area->GetAttributes() & NAV_MESH_JUMP )
				{
					const float jumpPenalty = 5.0f;
					cost += jumpPenalty * dist;
				}

				return cost;
			}
		}
	}

private:
	IPluginFunction *m_pFunc;
};

NAVMESHNATIVE(BuildPath)
	CNavArea *startArea = (CNavArea *)params[2];
	if (!startArea)
	{
		return pContext->ThrowNativeError("Invalid starting area!");
	}

	CNavArea *goalArea = (CNavArea *)params[3];
	cell_t* goalPosAddr;
	pContext->LocalToPhysAddr(params[4], &goalPosAddr);
	Vector goalPos;
	if (pContext->GetNullRef(SP_NULL_VECTOR) != goalPosAddr)
	{
		PawnVectorToVector(goalPosAddr, &goalPos);
	}

	IPluginFunction *pFunc = pContext->GetFunctionById(params[5]);
	SMPathCost pathCost(pFunc);

	cell_t* closestAreaAddr;
	pContext->LocalToPhysAddr(params[6], &closestAreaAddr);
	CNavArea *closestArea = nullptr;
	float maxPathLength = sp_ctof(params[7]);
	int teamID = params[8];
	bool ignoreNavBlockers = params[9] != 0;

	bool result = NavAreaBuildPath(startArea, 
		goalArea,
		pContext->GetNullRef(SP_NULL_VECTOR) == goalPosAddr ? nullptr : &goalPos, 
		pathCost, 
		&closestArea,
		maxPathLength,
		teamID,
		ignoreNavBlockers);

	if (closestAreaAddr)
	{
		*closestAreaAddr = (cell_t)closestArea;
	}

	return result;
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