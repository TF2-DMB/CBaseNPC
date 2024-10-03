#include "nav.hpp"
#include "nav/area.hpp"
#include "tf/nav.hpp"

#include "sourcesdk/nav_mesh.h"
#include "toolsnav_mesh.h"

namespace natives::nav {

namespace collector {

inline CUtlVector<CNavArea*>* Get(IPluginContext* context, const cell_t param) {	
	HandleSecurity security;
	security.pOwner = nullptr;
	security.pIdentity = myself->GetIdentity();
	Handle_t hndlObject = static_cast<Handle_t>(param);
	CUtlVector<CNavArea*>* collector = nullptr;
	READHANDLE(hndlObject, AreasCollector, collector) 
	return collector;
}

cell_t GetCount(IPluginContext* context, const cell_t* params) {
	auto collector = Get(context, params[1]);
	return collector->Count();
}

cell_t GetElement(IPluginContext* context, const cell_t* params) {
	auto collector = Get(context, params[1]);

	int ID = params[2];
	if (ID > collector->Count()) {
		return 0;
	}
	return PtrToPawnAddress(collector->Element(ID), context);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"AreasCollector.Count", GetCount},
		{"AreasCollector.Get", GetElement},
		{"SurroundingAreasCollector.Count", GetCount},
		{"SurroundingAreasCollector.Get", GetElement},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}

cell_t GetAddress(IPluginContext* context, const cell_t* params) {
	static Handle_t hndlNavMesh = BAD_HANDLE;
	if (hndlNavMesh == BAD_HANDLE) {
		hndlNavMesh = PtrToPawnAddress(TheNavMesh, nullptr, true);
	}
	return hndlNavMesh;
}

cell_t IsLoaded(IPluginContext* context, const cell_t* params) {
	return ToolsNavMesh->IsLoaded();
}

cell_t IsAnalyzed(IPluginContext* context, const cell_t* params) {
	return TheNavMesh->IsAnalyzed();
}

cell_t IsOutOfDate(IPluginContext* context, const cell_t* params) {
	return TheNavMesh->IsOutOfDate();
}

cell_t GetNavAreaCount(IPluginContext* context, const cell_t* params) {
	return ToolsNavMesh->GetNavAreaCount();
}

cell_t GetNavArea(IPluginContext* context, const cell_t* params) {
	cell_t* vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	Vector vecPos;
	PawnVectorToVector(vecAddr, vecPos);
	return PtrToPawnAddress(ToolsNavMesh->GetNavArea(vecPos, sp_ctof(params[3])), context);
}

cell_t GetNavAreaEntity(IPluginContext* context, const cell_t* params) {
	CBaseEntity* ent = gamehelpers->ReferenceToEntity(params[2]);
	if (!ent) {
		return context->ThrowNativeError("Invalid Entity index/reference %d", params[1]);
	}
	return PtrToPawnAddress(ToolsNavMesh->GetNavArea(ent, params[3], sp_ctof(params[4])), context);
}

cell_t GetNearestNavArea(IPluginContext* context, const cell_t* params) {
	cell_t* vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	Vector vecPos;
	PawnVectorToVector(vecAddr, vecPos);
	return PtrToPawnAddress(ToolsNavMesh->GetNearestNavArea(vecPos, params[3], sp_ctof(params[4]), params[5], params[6], params[7]), context);
}

cell_t GetNavAreaByID(IPluginContext* context, const cell_t* params) {
	unsigned int id = (unsigned int)params[2];
	return PtrToPawnAddress(ToolsNavMesh->GetNavAreaByID(id), context);
}

class CCollectorAddToTail {
public:
	CCollectorAddToTail( CUtlVector<CNavArea*>* vec ) : m_vec( vec ) {}
	bool operator() ( CNavArea *area ) { m_vec->AddToTail(area); return true; }

private:
	CUtlVector<CNavArea*>* m_vec;
};

cell_t CollectSurroundingAreas(IPluginContext* context, const cell_t* params) {
	CUtlVector<CNavArea*> *pCollector = new CUtlVector<CNavArea*>;
	CollectSurroundingAreas( pCollector, (CNavArea*)PawnAddressToPtr(params[2], context), sp_ctof(params[3]), sp_ctof(params[4]), sp_ctof(params[5]));
	return CREATEHANDLE(AreasCollector, pCollector);
}

cell_t CollectAreasOverlappingExtent(IPluginContext* context, const cell_t* params) {
	Vector lo; Vector hi; cell_t* addr;
	context->LocalToPhysAddr(params[2], &addr);
	PawnVectorToVector(addr, lo);
	context->LocalToPhysAddr(params[3], &addr);
	PawnVectorToVector(addr, hi);

	Extent extent;
	extent.Init();
	extent.lo = lo;
	extent.hi = hi;

	CUtlVector<CNavArea*> *pCollector = new CUtlVector<CNavArea*>;
	CCollectorAddToTail addToTail(pCollector);

	ToolsNavMesh->ForAllAreasOverlappingExtent(addToTail, extent);
	return CREATEHANDLE(AreasCollector, pCollector);
}

cell_t CollectAreasInRadius(IPluginContext* context, const cell_t* params) {
	cell_t* posAddr; Vector pos;
	context->LocalToPhysAddr(params[2], &posAddr);
	PawnVectorToVector(posAddr, pos);

	float radius = sp_ctof(params[3]);

	CUtlVector<CNavArea*> *pCollector = new CUtlVector<CNavArea*>;
	CCollectorAddToTail addToTail(pCollector);

	ToolsNavMesh->ForAllAreasInRadius(addToTail, pos, radius);
	return CREATEHANDLE(AreasCollector, pCollector);
}

cell_t CollectAreasAlongLine(IPluginContext* context, const cell_t* params) {
	CNavArea* startArea = (CNavArea*)PawnAddressToPtr(params[2], context);
	CNavArea* endArea = (CNavArea*)PawnAddressToPtr(params[3], context);
	cell_t* reachedEndAddr;
	context->LocalToPhysAddr(params[4], &reachedEndAddr);

	CUtlVector<CNavArea*> *pCollector = new CUtlVector<CNavArea*>;
	CCollectorAddToTail addToTail(pCollector);

	*reachedEndAddr = ToolsNavMesh->ForAllAreasAlongLine(addToTail, startArea, endArea) ? 1 : 0;
	return CREATEHANDLE(AreasCollector, pCollector);
}

class SMPathCost : public IPathCost
{
public:
	SMPathCost(IPluginFunction* pFunc) : m_pFunc(pFunc) {}

	virtual float operator()(CNavArea* area, CNavArea* fromArea, const CNavLadder* ladder, const CFuncElevator* elevator, float length) const override final
	{
		if (m_pFunc && m_pFunc->IsRunnable())
		{
			Handle_t hndlArea = PtrToPawnAddress(area, nullptr);
			Handle_t hndlFromArea = PtrToPawnAddress(fromArea, nullptr);
			Handle_t hndlLadder = PtrToPawnAddress(ladder, nullptr);

			cell_t result = sp_ftoc(0.0);
			m_pFunc->PushCell(hndlArea);
			m_pFunc->PushCell(hndlFromArea);
			m_pFunc->PushCell(hndlLadder);
			m_pFunc->PushCell(gamehelpers->EntityToBCompatRef((CBaseEntity*)elevator));
			m_pFunc->PushFloat(length);
			m_pFunc->Execute(&result);

			ReleasePawnAddress(hndlArea, nullptr);
			ReleasePawnAddress(hndlFromArea, nullptr);
			ReleasePawnAddress(hndlLadder, nullptr);

			return sp_ctof(result);
		}
		else {
			if ( fromArea == nullptr ) {
				// first area in path, no cost
				return 0.0f;
			} else {
				// compute distance traveled along path so far
				float dist;

				if ( ladder ) {
					dist = ladder->m_length;
				} else if ( length > 0.0 ) {
					dist = length;
				} else {
					dist = ( area->GetCenter() - fromArea->GetCenter() ).Length();
				}

				float cost = dist + fromArea->GetCostSoFar();

				// if this is a "crouch" area, add penalty
				if ( area->GetAttributes() & NAV_MESH_CROUCH ) {
					const float crouchPenalty = 20.0f;		// 10
					cost += crouchPenalty * dist;
				}

				// if this is a "jump" area, add penalty
				if ( area->GetAttributes() & NAV_MESH_JUMP ) {
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

cell_t BuildPath(IPluginContext* context, const cell_t* params) {
	CNavArea *startArea = (CNavArea *)PawnAddressToPtr(params[2], context);
	if (!startArea) {
		return context->ThrowNativeError("Starting area is null!");
	}

	CNavArea* goalArea = (CNavArea *)PawnAddressToPtr(params[3], context);
	cell_t* goalPosAddr;
	context->LocalToPhysAddr(params[4], &goalPosAddr);
	Vector goalPos;
	if (context->GetNullRef(SP_NULL_VECTOR) != goalPosAddr) {
		PawnVectorToVector(goalPosAddr, &goalPos);
	}

	IPluginFunction *pFunc = context->GetFunctionById(params[5]);
	SMPathCost pathCost(pFunc);

	cell_t* closestAreaAddr;
	context->LocalToPhysAddr(params[6], &closestAreaAddr);
	CNavArea* closestArea = nullptr;
	float maxPathLength = sp_ctof(params[7]);
	int teamID = params[8];
	bool ignoreNavBlockers = params[9] != 0;

	bool result = NavAreaBuildPath(startArea, 
		goalArea,
		context->GetNullRef(SP_NULL_VECTOR) == goalPosAddr ? nullptr : &goalPos, 
		pathCost, 
		&closestArea,
		maxPathLength,
		teamID,
		ignoreNavBlockers);

	if (closestAreaAddr) {
		*closestAreaAddr = PtrToPawnAddress(closestArea, context);
	}

	return result;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	area::setup(natives);
	collector::setup(natives);
	
	tf::nav::setup(natives);

	sp_nativeinfo_t list[] = {
		{"CNavMesh.Address.get", GetAddress},
		{"CNavMesh.IsLoaded", IsLoaded},
		{"CNavMesh.IsAnalyzed", IsAnalyzed},
		{"CNavMesh.IsOutOfDate", IsOutOfDate},
		{"CNavMesh.GetNavAreaCount", GetNavAreaCount},
		{"CNavMesh.CollectSurroundingAreas", CollectSurroundingAreas},
		{"CNavMesh.CollectAreasOverlappingExtent", CollectAreasOverlappingExtent},
		{"CNavMesh.CollectAreasInRadius", CollectAreasInRadius},
		{"CNavMesh.CollectAreasAlongLine", CollectAreasAlongLine},
		{"CNavMesh.GetNavAreaByID", GetNavAreaByID},
		{"CNavMesh.GetNearestNavArea", GetNearestNavArea},
		{"CNavMesh.BuildPath", BuildPath},
		{"CNavMesh.GetNavArea", GetNavArea},
		{"CNavMesh.GetNavAreaEntity", GetNavAreaEntity},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}