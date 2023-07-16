#include "area.hpp"

#include "sourcesdk/nav_area.h"

namespace natives::nav::area {

namespace hidingspot {

namespace theptr {

cell_t GetCount(IPluginContext* pContext, const cell_t* params)
{
	return TheHidingSpots.Count();
}

inline cell_t Get(IPluginContext* context, const cell_t* params)
{
	int i = params[2];
	if (i < 0 || i >= TheHidingSpots.Count()) {
		return context->ThrowNativeError("Index is out of bounds!");
	}
	return PtrToPawnAddress(TheHidingSpots[i]);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"TheHidingSpotsVector.Get", Get},
		{"TheHidingSpotsVector.Count.get", GetCount},

		// Deprecated
		{"TheHidingSpotsVector.Length.get", GetCount},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}


}

inline HidingSpot* Get(IPluginContext* context, const cell_t param) {
	HidingSpot* spot = (HidingSpot*)PawnAddressToPtr(param);
	if (!spot) {
		context->ThrowNativeError("Hiding spot ptr is null!");
		return nullptr;
	}
	return spot;
}

cell_t GetID(IPluginContext* context, const cell_t* params) {
	auto spot = Get(context, params[1]);
	if (!spot) {
		return 0;
	}

	return spot->GetID();
}

cell_t GetFlags(IPluginContext* context, const cell_t* params) {
	auto spot = Get(context, params[1]);
	if (!spot) {
		return 0;
	}

	return spot->GetFlags();
}

cell_t GetPosition(IPluginContext* context, const cell_t* params) {
	auto spot = Get(context, params[1]);
	if (!spot) {
		return 0;
	}

	cell_t* posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	VectorToPawnVector(posAddr, spot->GetPosition());
	return 0;
}

cell_t GetArea(IPluginContext* context, const cell_t* params) {
	auto spot = Get(context, params[1]);
	if (!spot) {
		return 0;
	}

	return PtrToPawnAddress(spot->GetArea());
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	theptr::setup(natives);

	sp_nativeinfo_t list[] = {
		{"HidingSpot.GetID", GetID},
		{"HidingSpot.GetFlags", GetFlags},
		{"HidingSpot.GetPosition", GetPosition},
		{"HidingSpot.GetArea", GetArea},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}

namespace ladder {

inline CNavLadder* Get(IPluginContext* context, const cell_t param) {
	CNavLadder* spot = (CNavLadder*)PawnAddressToPtr(param);
	if (!spot) {
		context->ThrowNativeError("Nav ladder ptr is null!");
		return nullptr;
	}
	return spot;
}

cell_t GetLength(IPluginContext* context, const cell_t* params) {
	auto ladder = Get(context, params[1]);
	if (!ladder) {
		return 0;
	}

	return sp_ftoc(ladder->m_length);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CNavLadder.Length.get", GetLength},

		// Deprecated
		{"CNavLadder.length.get", GetLength},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}

namespace theptr {

cell_t GetCount(IPluginContext* context, const cell_t* params)
{
	return TheNavAreas.Count();
}

inline cell_t Get(IPluginContext* context, const cell_t* params)
{
	int i = params[2];
	if (i < 0 || i >= TheNavAreas.Count()) {
		return context->ThrowNativeError("Index is out of bounds!");
	}
	return PtrToPawnAddress(TheNavAreas[i]);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"TheNavAreasVector.Get", Get},
		{"TheNavAreasVector.Count.get", GetCount},

		// Deprecated
		{"TheNavAreasVector.Length.get", GetCount},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}

cell_t ClearSearchLists(IPluginContext* context, const cell_t* params) {
	CNavArea::ClearSearchLists();
	return 0;
}

cell_t IsOpenListEmpty(IPluginContext* context, const cell_t* params) {
	return CNavArea::IsOpenListEmpty();
}

cell_t PopOpenList(IPluginContext* context, const cell_t* params) {
	return reinterpret_cast<cell_t>(CNavArea::PopOpenList());
}

inline CNavArea* Get(IPluginContext* context, const cell_t param) {
	CNavArea* area = (CNavArea*)PawnAddressToPtr(param);
	if (!area) {
		context->ThrowNativeError("Nav area ptr is null!");
		return nullptr;
	}
	return area;
}

cell_t UpdateBlocked(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}

	area->UpdateBlocked(params[2], params[3]);
	return 1;
}

cell_t IsBlocked(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}

	return area->IsBlocked(params[2], params[3]);
}

cell_t GetID(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return area->GetID();
}

cell_t SetParent(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	area->SetParent((CNavArea*)PawnAddressToPtr(params[2]), (NavTraverseType)params[3]);
	return 1;
}

cell_t GetParent(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return (cell_t)area->GetParent();
}

cell_t GetParentHow(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return area->GetParentHow();
}

cell_t SetCostSoFar(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	area->SetCostSoFar(sp_ctof(params[2]));
	return 1;
}

cell_t GetCostSoFar(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return sp_ftoc(area->GetCostSoFar());
}

cell_t SetTotalCost(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	area->SetTotalCost(sp_ctof(params[2]));
	return 1;
}

cell_t GetTotalCost(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return sp_ftoc(area->GetTotalCost());
}

cell_t SetPathLengthSoFar(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	area->SetPathLengthSoFar(sp_ctof(params[2]));
	return 1;
}

cell_t GetPathLengthSoFar(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return sp_ftoc(area->GetPathLengthSoFar());
}

cell_t ComputePortal(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	CNavArea* other = (CNavArea*)PawnAddressToPtr(params[2]);
	if (!other) {
		return context->ThrowNativeError("Nav area is null !");
	}

	NavDirType dir = (NavDirType)params[3];
	cell_t* posAddr;
	context->LocalToPhysAddr(params[4], &posAddr);
	Vector pos;
	cell_t* halfWidthAddr;
	float halfWidth;
	context->LocalToPhysAddr(params[5], &halfWidthAddr);
	area->ComputePortal(other, dir, &pos, &halfWidth);
	VectorToPawnVector(posAddr, pos);
	*halfWidthAddr = sp_ftoc(halfWidth);
	return 1;
}

cell_t ComputeClosestPointInPortal(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	CNavArea* other = (CNavArea*)PawnAddressToPtr(params[2]);
	if (!other) {
		return context->ThrowNativeError("Nav area is null !");
	}

	NavDirType dir = (NavDirType)params[3];
	cell_t* fromPosAddr;
	context->LocalToPhysAddr(params[4], &fromPosAddr);
	Vector fromPos;
	PawnVectorToVector(fromPosAddr, &fromPos);
	cell_t* closePosAddr;
	Vector closePos;
	context->LocalToPhysAddr(params[5], &closePosAddr);
	area->ComputeClosestPointInPortal(other, dir, fromPos, &closePos);
	VectorToPawnVector(closePosAddr, &closePos);
	return 1;
}

cell_t GetClosestPointOnArea(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* fromPosAddr;
	context->LocalToPhysAddr(params[2], &fromPosAddr);
	Vector fromPos;
	PawnVectorToVector(fromPosAddr, &fromPos);
	cell_t *closePosAddr;
	Vector closePos;
	context->LocalToPhysAddr(params[3], &closePosAddr);
	area->GetClosestPointOnArea(fromPos, &closePos);
	VectorToPawnVector(closePosAddr, &closePos);
	return 1;
}

cell_t GetDistanceSquaredToPoint(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t *posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, &pos);
	return sp_ftoc(area->GetDistanceSquaredToPoint(pos));
}

cell_t ComputeAdjacentConnectionHeightChange(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	CNavArea *pOther = (CNavArea *)params[2];
	if (!pOther) {
		return context->ThrowNativeError("Invalid nav area %x", params[2]);
	}

	return sp_ftoc(area->ComputeAdjacentConnectionHeightChange(pOther));
}

cell_t GetAttributes(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return area->GetAttributes();
}

cell_t GetCorner(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	NavCornerType corner = (NavCornerType)params[2];
	if (corner < 0 || corner >= NUM_CORNERS)
		return context->ThrowNativeError("Invalid corner type %d", corner);

	cell_t* posAddr;
	context->LocalToPhysAddr(params[3], &posAddr);
	Vector pos = area->GetCorner(corner);
	VectorToPawnVector(posAddr, pos);
	return 0;
}

cell_t GetCenter(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos = area->GetCenter();
	VectorToPawnVector(posAddr, pos);
	return 1;
}

cell_t IsConnected(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return area->IsConnected((CNavArea *)params[2], (NavDirType)params[3]);
}

cell_t IsOverlappingPoint(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, &pos);
	float tolerance = sp_ctof(params[3]);
	return area->IsOverlapping(pos, tolerance);
}

cell_t IsOverlappingX(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	CNavArea *pOther = (CNavArea *)params[2];
	if (!pOther) {
		return context->ThrowNativeError("Invalid nav area %x", params[2]);
	}
	return area->IsOverlappingX(pOther);
}

cell_t IsOverlappingY(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	CNavArea *pOther = (CNavArea *)params[2];
	if (!pOther) {
		return context->ThrowNativeError("Invalid nav area %x", params[2]);
	}
	return area->IsOverlappingY(pOther);
}

cell_t IsOverlappingArea(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	CNavArea *pOther = (CNavArea *)params[2];
	if (!pOther) {
		return context->ThrowNativeError("Invalid nav area %x", params[2]);
	}

	return area->IsOverlapping(pOther);
}

cell_t IsOverlappingExtent(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t *minAddr;
	context->LocalToPhysAddr(params[2], &minAddr);
	Vector min;
	PawnVectorToVector(minAddr, &min);
	cell_t *maxAddr;
	context->LocalToPhysAddr(params[3], &maxAddr);
	Vector max;
	PawnVectorToVector(maxAddr, &max);

	Extent extent;
	extent.Init();
	extent.lo = min;
	extent.hi = max;

	return area->IsOverlapping(extent);
}

cell_t GetExtent(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* minAddr;
	context->LocalToPhysAddr(params[2], &minAddr);
	Vector min;
	PawnVectorToVector(minAddr, &min);
	cell_t* maxAddr;
	context->LocalToPhysAddr(params[3], &maxAddr);
	Vector max;
	PawnVectorToVector(maxAddr, &max);

	Extent extent;
	extent.Init();
	extent.lo = min;
	extent.hi = max;

	area->GetExtent(&extent);

	VectorToPawnVector(minAddr, extent.lo);
	VectorToPawnVector(maxAddr, extent.hi);
	return 1;
}

cell_t GetAdjacentCount(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	NavDirType dir = (NavDirType)params[2];
	if (dir < 0 || dir >= NUM_DIRECTIONS) {
		return context->ThrowNativeError("Invalid direction %d", dir);
	}
	return area->GetAdjacentCount(dir);
}

cell_t GetAdjacentArea(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	NavDirType dir = (NavDirType)params[2];
	if (dir < 0 || dir >= NUM_DIRECTIONS) {
		return context->ThrowNativeError("Invalid direction %d", dir);
	}
	return PtrToPawnAddress(area->GetAdjacentArea(dir, params[3]));
}

cell_t GetIncomingConnectionsCount(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	NavDirType dir = (NavDirType)params[2];
	if (dir < 0 || dir >= NUM_DIRECTIONS)
		return context->ThrowNativeError("Invalid direction %d", dir);
	return area->GetIncomingConnections(dir)->Count();
}

cell_t GetIncomingConnections(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	NavDirType dir = (NavDirType)params[2];
	if (dir < 0 || dir >= NUM_DIRECTIONS) {
		return context->ThrowNativeError("Invalid direction %d", dir);
	}

	int i = params[3];
	const NavConnectVector *incoming = area->GetIncomingConnections(dir);
	if (i < 0 || i >= incoming->Count()) {
		return 0;
	}
	return PtrToPawnAddress(incoming->Element(i).area);
}

cell_t IsEdge(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return area->IsEdge((NavDirType)params[2]);
}

cell_t Contains(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return area->Contains((CNavArea *)params[2]);
}

cell_t ContainsPoint(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, &pos);
	return area->Contains(pos);
}

cell_t GetSizeX(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return sp_ftoc(area->GetSizeX());
}

cell_t GetSizeY(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return sp_ftoc(area->GetSizeY());
}

cell_t GetZ(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return sp_ftoc(area->GetZ(sp_ctof(params[2]), sp_ctof(params[3])));
}

cell_t GetZVector(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* dstAddr;
	context->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	return sp_ftoc(area->GetZ(dst));
}

cell_t ComputeNormal(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* norAddr;
	context->LocalToPhysAddr(params[2], &norAddr);
	Vector nor;
	area->ComputeNormal(&nor, params[3]);
	VectorToPawnVector(norAddr, nor);
	return 1;
}

cell_t GetLightIntensity(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return sp_ftoc(area->GetLightIntensity());
}

cell_t GetPositionLightIntensity(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector vecPos;
	PawnVectorToVector(posAddr, &vecPos);
	return sp_ftoc(area->GetLightIntensity(vecPos));
}

cell_t GetHidingSpotCount(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return area->GetHidingSpots()->Count();
}

cell_t GetHidingSpot(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	int i = params[2];
	const HidingSpotVector* pSpots = area->GetHidingSpots();
	if ((i < 0) || (i >= pSpots->Count())) {
		return 0;
	}
	return PtrToPawnAddress(pSpots->Element(i));
}

cell_t IsOpen(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}

	return area->IsOpen();
}

cell_t AddToOpenList(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	area->AddToOpenList();
	return 0;
}

cell_t AddToOpenListTail(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	area->AddToOpenListTail();
	return 0;
}

cell_t UpdateOnOpenList(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	area->UpdateOnOpenList();
	return 0;
}

cell_t IsClosed(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	return area->IsClosed();
}

cell_t AddToClosedList(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	area->AddToClosedList();
	return 0;
}

cell_t IsEntirelyVisible(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* eyeAddr;
	context->LocalToPhysAddr(params[2], &eyeAddr);
	Vector eye;
	PawnVectorToVector(eyeAddr, &eye);
	CBaseEntity* ignore = gamehelpers->ReferenceToEntity(params[3]);
	return area->IsEntirelyVisible(eye, ignore);
}

cell_t IsPartiallyVisible(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	cell_t* eyeAddr;
	context->LocalToPhysAddr(params[2], &eyeAddr);
	Vector eye;
	PawnVectorToVector(eyeAddr, &eye);
	CBaseEntity* ignore = gamehelpers->ReferenceToEntity(params[3]);
	return area->IsPartiallyVisible(eye, ignore);
}

cell_t IsPotentiallyVisible(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	CNavArea* viewedArea = (CNavArea*)PawnAddressToPtr(params[2]);
	if (!viewedArea) {
		return context->ThrowNativeError("Nav area is null !");
	}
	return area->IsPotentiallyVisible(viewedArea);
}

cell_t IsCompletelyVisible(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}
	
	CNavArea* viewedArea = (CNavArea*)PawnAddressToPtr(params[2]);
	if (!viewedArea) {
		return context->ThrowNativeError("Nav area is null !");
	}
	return area->IsCompletelyVisible(viewedArea);
}

cell_t native_GetHidingSpotByID(IPluginContext* pContext, const cell_t* params) {
	unsigned int id = params[1];
	return PtrToPawnAddress(GetHidingSpotByID(id));
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	hidingspot::setup(natives);
	ladder::setup(natives);
	theptr::setup(natives);

	sp_nativeinfo_t list[] = {
		{"CNavArea.ClearSearchLists", ClearSearchLists},
		{"CNavArea.IsOpenListEmpty", IsOpenListEmpty},
		{"CNavArea.PopOpenList", PopOpenList},	

		{"CNavArea.UpdateBlocked", UpdateBlocked},
		{"CNavArea.IsBlocked", IsBlocked},
		{"CNavArea.GetID", GetID},
		{"CNavArea.SetParent", SetParent},
		{"CNavArea.GetParent", GetParent},
		{"CNavArea.GetParentHow", GetParentHow},
		{"CNavArea.GetCostSoFar", GetCostSoFar},
		{"CNavArea.SetCostSoFar", SetCostSoFar},
		{"CNavArea.GetTotalCost", GetTotalCost},
		{"CNavArea.SetTotalCost", SetTotalCost},
		{"CNavArea.GetPathLengthSoFar", GetPathLengthSoFar},
		{"CNavArea.SetPathLengthSoFar", SetPathLengthSoFar},
		{"CNavArea.ComputePortal", ComputePortal},
		{"CNavArea.ComputeClosestPointInPortal", ComputeClosestPointInPortal},
		{"CNavArea.GetClosestPointOnArea", GetClosestPointOnArea},
		{"CNavArea.GetDistanceSquaredToPoint", GetDistanceSquaredToPoint},
		{"CNavArea.ComputeAdjacentConnectionHeightChange", ComputeAdjacentConnectionHeightChange},
		{"CNavArea.GetAttributes", GetAttributes},
		{"CNavArea.GetCorner", GetCorner},
		{"CNavArea.GetCenter", GetCenter},
		{"CNavArea.GetAdjacentCount", GetAdjacentCount},
		{"CNavArea.GetAdjacentArea", GetAdjacentArea},
		{"CNavArea.IsConnected", IsConnected},
		{"CNavArea.IsOverlappingX", IsOverlappingX},
		{"CNavArea.IsOverlappingY", IsOverlappingY},
		{"CNavArea.IsOverlappingPoint", IsOverlappingPoint},
		{"CNavArea.IsOverlappingArea", IsOverlappingArea},
		{"CNavArea.IsOverlappingExtent", IsOverlappingExtent},
		{"CNavArea.GetExtent", GetExtent},
		{"CNavArea.GetIncomingConnectionCount", GetIncomingConnectionsCount},
		{"CNavArea.GetIncomingConnection", GetIncomingConnections},
		{"CNavArea.IsEdge", IsEdge},
		{"CNavArea.Contains", Contains},
		{"CNavArea.ContainsPoint", ContainsPoint},
		{"CNavArea.GetSizeX", GetSizeX},
		{"CNavArea.GetSizeY", GetSizeY},
		{"CNavArea.GetZ", GetZ},
		{"CNavArea.GetZVector", GetZVector},
		{"CNavArea.ComputeNormal", ComputeNormal},
		{"CNavArea.GetLightIntensity", GetLightIntensity},
		{"CNavArea.GetPositionLightIntensity", GetPositionLightIntensity},
		{"CNavArea.GetHidingSpotCount", GetHidingSpotCount},
		{"CNavArea.GetHidingSpot", GetHidingSpot},
		{"CNavArea.IsOpen", IsOpen},
		{"CNavArea.AddToOpenList", AddToOpenList},
		{"CNavArea.AddToOpenListTail", AddToOpenListTail},
		{"CNavArea.UpdateOnOpenList", UpdateOnOpenList},
		{"CNavArea.IsClosed", IsClosed},
		{"CNavArea.AddToClosedList", AddToClosedList},
		{"CNavArea.IsPartiallyVisible", IsPartiallyVisible},
		{"CNavArea.IsEntirelyVisible", IsEntirelyVisible},
		{"CNavArea.IsPotentiallyVisible", IsPotentiallyVisible},
		{"CNavArea.IsCompletelyVisible", IsCompletelyVisible},

		{"GetHidingSpotByID", native_GetHidingSpotByID},

		// Deprecated
		{"CNavArea.GetIncomingConnectionCount", GetIncomingConnectionsCount},
		{"CNavArea.GetIncomingConnection", GetIncomingConnections},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}