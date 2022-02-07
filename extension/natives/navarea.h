#ifndef NATIVES_AREA_H
#define NATIVES_AREA_H

#include "sourcesdk/nav_area.h"

#if SOURCE_ENGINE == SE_TF2
#include "sourcesdk/tf_nav_area.h"
#endif

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}

#define HIDINGSPOT_NATIVE(name) \
	cell_t HidingSpot_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		HidingSpot *pSpot = (HidingSpot *)(params[1]); \
		if(!pSpot) { \
			return pContext->ThrowNativeError("Invalid hiding spot %x", params[1]); \
		} \

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

HIDINGSPOT_NATIVE(GetID)
	return pSpot->GetID();
}

HIDINGSPOT_NATIVE(GetFlags)
	return pSpot->GetFlags();
}

HIDINGSPOT_NATIVE(GetPosition)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	VectorToPawnVector(posAddr, pSpot->GetPosition());
	return 0;
}

HIDINGSPOT_NATIVE(GetArea)
	return reinterpret_cast<cell_t>(pSpot->GetArea());
}

NAVAREA_NATIVE(UpdateBlocked)
	pArea->UpdateBlocked(params[2], params[3]);
	return 1;
}

NAVAREA_NATIVE(IsBlocked)
	return pArea->IsBlocked(params[2], params[3]);
}

NAVAREA_NATIVE(GetID)
	return pArea->GetID();
}

NAVAREA_NATIVE(SetParent)
	pArea->SetParent((CNavArea *)params[2], (NavTraverseType)params[3]);
	return 1;
}

NAVAREA_NATIVE(GetParent)
	return (cell_t)pArea->GetParent();
}

NAVAREA_NATIVE(GetParentHow)
	return pArea->GetParentHow();
}

NAVAREA_NATIVE(SetCostSoFar)
	pArea->SetCostSoFar(sp_ctof(params[2]));
	return 1;
}

NAVAREA_NATIVE(GetCostSoFar)
	return sp_ftoc(pArea->GetCostSoFar());
}

NAVAREA_NATIVE(SetTotalCost)
	pArea->SetTotalCost(sp_ctof(params[2]));
	return 1;
}

NAVAREA_NATIVE(GetTotalCost)
	return sp_ftoc(pArea->GetTotalCost());
}

NAVAREA_NATIVE(SetPathLengthSoFar)
	pArea->SetPathLengthSoFar(sp_ctof(params[2]));
	return 1;
}

NAVAREA_NATIVE(GetPathLengthSoFar)
	return sp_ftoc(pArea->GetPathLengthSoFar());
}

NAVAREA_NATIVE(ComputePortal)
	CNavArea *pOther = (CNavArea *)params[2];
	if (!pOther) 
	{
		return pContext->ThrowNativeError("Invalid nav area %x", params[2]);
	}

	NavDirType dir = (NavDirType)params[3];
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[4], &posAddr);
	Vector pos;
	cell_t *halfWidthAddr;
	float halfWidth;
	pContext->LocalToPhysAddr(params[5], &halfWidthAddr);
	pArea->ComputePortal(pOther, dir, &pos, &halfWidth);
	VectorToPawnVector(posAddr, pos);
	*halfWidthAddr = sp_ftoc(halfWidth);
	return 1;
}

NAVAREA_NATIVE(ComputeClosestPointInPortal)
	CNavArea *pOther = (CNavArea *)params[2];
	if (!pOther) 
	{
		return pContext->ThrowNativeError("Invalid nav area %x", params[2]);
	}

	NavDirType dir = (NavDirType)params[3];
	cell_t *fromPosAddr;
	pContext->LocalToPhysAddr(params[4], &fromPosAddr);
	Vector fromPos;
	PawnVectorToVector(fromPosAddr, &fromPos);
	cell_t *closePosAddr;
	Vector closePos;
	pContext->LocalToPhysAddr(params[5], &closePosAddr);
	pArea->ComputeClosestPointInPortal(pOther, dir, fromPos, &closePos);
	VectorToPawnVector(closePosAddr, &closePos);
	return 1;
}

NAVAREA_NATIVE(GetClosestPointOnArea)
	cell_t *fromPosAddr;
	pContext->LocalToPhysAddr(params[2], &fromPosAddr);
	Vector fromPos;
	PawnVectorToVector(fromPosAddr, &fromPos);
	cell_t *closePosAddr;
	Vector closePos;
	pContext->LocalToPhysAddr(params[3], &closePosAddr);
	pArea->GetClosestPointOnArea(fromPos, &closePos);
	VectorToPawnVector(closePosAddr, &closePos);
	return 1;
}

NAVAREA_NATIVE(GetDistanceSquaredToPoint)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, &pos);
	return sp_ftoc(pArea->GetDistanceSquaredToPoint(pos));
}

NAVAREA_NATIVE(ComputeAdjacentConnectionHeightChange)
	CNavArea *pOther = (CNavArea *)params[2];
	if (!pOther) 
	{
		return pContext->ThrowNativeError("Invalid nav area %x", params[2]);
	}

	return sp_ftoc(pArea->ComputeAdjacentConnectionHeightChange(pOther));
}

NAVAREA_NATIVE(GetAttributes)
	return pArea->GetAttributes();
}

NAVAREA_NATIVE(GetCorner)
	NavCornerType corner = (NavCornerType)params[2];
	if (corner < 0 || corner >= NUM_CORNERS)
		return pContext->ThrowNativeError("Invalid corner type %d", corner);

	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[3], &posAddr);
	Vector pos = pArea->GetCorner(corner);
	VectorToPawnVector(posAddr, pos);
	return 0;
}

NAVAREA_NATIVE(GetCenter)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos = pArea->GetCenter();
	VectorToPawnVector(posAddr, pos);
	return 1;
}

NAVAREA_NATIVE(IsConnected)
	return pArea->IsConnected((CNavArea *)params[2], (NavDirType)params[3]);
}

NAVAREA_NATIVE(IsOverlappingPoint)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, &pos);
	float flTolerance = sp_ctof(params[3]);
	return pArea->IsOverlapping(pos, flTolerance);
}

NAVAREA_NATIVE(IsOverlappingArea)
	CNavArea *pOther = (CNavArea *)params[2];
	if (!pOther) 
	{
		return pContext->ThrowNativeError("Invalid nav area %x", params[2]);
	}

	return pArea->IsOverlapping(pOther);
}

NAVAREA_NATIVE(IsOverlappingExtent)
	cell_t *minAddr;
	pContext->LocalToPhysAddr(params[2], &minAddr);
	Vector min;
	PawnVectorToVector(minAddr, &min);
	cell_t *maxAddr;
	pContext->LocalToPhysAddr(params[3], &maxAddr);
	Vector max;
	PawnVectorToVector(maxAddr, &max);

	Extent extent;
	extent.Init();
	extent.lo = min;
	extent.hi = max;

	return pArea->IsOverlapping(extent);
}

NAVAREA_NATIVE(GetAdjacentCount)
	NavDirType dir = (NavDirType)params[2];
	if (dir < 0 || dir >= NUM_DIRECTIONS)
		return pContext->ThrowNativeError("Invalid direction %d", dir);
	return pArea->GetAdjacentCount(dir);
}

NAVAREA_NATIVE(GetAdjacentArea)
	NavDirType dir = (NavDirType)params[2];
	if (dir < 0 || dir >= NUM_DIRECTIONS)
		return pContext->ThrowNativeError("Invalid direction %d", dir);
	return reinterpret_cast<cell_t>(pArea->GetAdjacentArea(dir, params[3]));
}

NAVAREA_NATIVE(GetIncomingConnectionCount)
	NavDirType dir = (NavDirType)params[2];
	if (dir < 0 || dir >= NUM_DIRECTIONS)
		return pContext->ThrowNativeError("Invalid direction %d", dir);
	return pArea->GetIncomingConnections(dir)->Count();
}

NAVAREA_NATIVE(GetIncomingConnection)
	NavDirType dir = (NavDirType)params[2];
	if (dir < 0 || dir >= NUM_DIRECTIONS)
		return pContext->ThrowNativeError("Invalid direction %d", dir);
	int i = params[3];
	const NavConnectVector *incoming = pArea->GetIncomingConnections(dir);
	if ((i < 0) || (i >= incoming->Count()))
		return NULL;
	return reinterpret_cast<cell_t>(incoming->Element(i).area);
}

NAVAREA_NATIVE(IsEdge)
	return pArea->IsEdge((NavDirType)params[2]);
}

NAVAREA_NATIVE(Contains)
	return pArea->Contains((CNavArea *)params[2]);
}

NAVAREA_NATIVE(GetSizeX)
	return sp_ftoc(pArea->GetSizeX());
}

NAVAREA_NATIVE(GetSizeY)
	return sp_ftoc(pArea->GetSizeY());
}

NAVAREA_NATIVE(GetZ)
	return sp_ftoc(pArea->GetZ(sp_ctof(params[2]), sp_ctof(params[3])));
}

NAVAREA_NATIVE(GetZVector)
	cell_t *dstAddr;
	pContext->LocalToPhysAddr(params[2], &dstAddr);
	Vector dst;
	PawnVectorToVector(dstAddr, dst);
	return sp_ftoc(pArea->GetZ(dst));
}

NAVAREA_NATIVE(ComputeNormal)
	cell_t *norAddr;
	pContext->LocalToPhysAddr(params[2], &norAddr);
	Vector nor;
	pArea->ComputeNormal(&nor, params[3]);
	VectorToPawnVector(norAddr, nor);
	return 1;
}

NAVAREA_NATIVE(GetLightIntensity)
	return sp_ftoc(pArea->GetLightIntensity());
}

NAVAREA_NATIVE(GetPositionLightIntensity)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector vecPos;
	PawnVectorToVector(posAddr, &vecPos);
	return sp_ftoc(pArea->GetLightIntensity(vecPos));
}

NAVAREA_NATIVE(GetHidingSpotCount)
	return pArea->GetHidingSpots()->Count();
}

NAVAREA_NATIVE(GetHidingSpot)
	int i = params[2];
	const HidingSpotVector* pSpots = pArea->GetHidingSpots();
	if ((i < 0) || (i >= pSpots->Count()))
		return NULL;
	return reinterpret_cast<cell_t>(pSpots->Element(i));
}

cell_t CNavArea_ClearSearchLists(IPluginContext *pContext, const cell_t* params)
{
	CNavArea::ClearSearchLists();
	return 0;
}

cell_t CNavArea_IsOpenListEmpty(IPluginContext *pContext, const cell_t* params)
{
	return CNavArea::IsOpenListEmpty();
}

cell_t CNavArea_PopOpenList(IPluginContext *pContext, const cell_t* params)
{
	return reinterpret_cast<cell_t>(CNavArea::PopOpenList());
}

NAVAREA_NATIVE(IsOpen)
	return pArea->IsOpen();
}

NAVAREA_NATIVE(AddToOpenList)
	pArea->AddToOpenList();
	return 0;
}

NAVAREA_NATIVE(AddToOpenListTail)
	pArea->AddToOpenListTail();
	return 0;
}

NAVAREA_NATIVE(UpdateOnOpenList)
	pArea->UpdateOnOpenList();
	return 0;
}

NAVAREA_NATIVE(IsClosed)
	return pArea->IsClosed();
}

NAVAREA_NATIVE(AddToClosedList)
	pArea->AddToClosedList();
	return 0;
}

NAVAREA_NATIVE(IsEntirelyVisible)
	cell_t *eyeAddr;
	pContext->LocalToPhysAddr(params[2], &eyeAddr);
	Vector eye;
	PawnVectorToVector(eyeAddr, &eye);
	CBaseEntity *ignore = gamehelpers->ReferenceToEntity(params[3]);
	return pArea->IsEntirelyVisible(eye, ignore);
}

NAVAREA_NATIVE(IsPartiallyVisible)
	cell_t *eyeAddr;
	pContext->LocalToPhysAddr(params[2], &eyeAddr);
	Vector eye;
	PawnVectorToVector(eyeAddr, &eye);
	CBaseEntity *ignore = gamehelpers->ReferenceToEntity(params[3]);
	return pArea->IsPartiallyVisible(eye, ignore);
}

NAVAREA_NATIVE(IsPotentiallyVisible)
	CNavArea *viewedArea = (CNavArea *)params[2];
	if (!viewedArea)
		return pContext->ThrowNativeError("Invalid nav area %x", params[2]);
	return pArea->IsPotentiallyVisible(viewedArea);
}

NAVAREA_NATIVE(IsCompletelyVisible)
	CNavArea *viewedArea = (CNavArea *)params[2];
	if (!viewedArea)
		return pContext->ThrowNativeError("Invalid nav area %x", params[2]);
	return pArea->IsCompletelyVisible(viewedArea);
}

NAVLADDER_NATIVE(lengthGet)
	return sp_ftoc(pArea->m_length);
}

cell_t TheNavAreasVector_LengthGet(IPluginContext* pContext, const cell_t* params)
{
	return TheNavAreas.Count();
}

cell_t TheNavAreasVector_Get(IPluginContext* pContext, const cell_t* params)
{
	int i = params[2];
	if ((i < 0) || (i >= TheNavAreas.Count()))
		return NULL;
	return reinterpret_cast<cell_t>(TheNavAreas[i]);
}

#if SOURCE_ENGINE == SE_TF2
#define TFNAVAREA_NATIVE(name) \
	cell_t CTFNavArea_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CTFNavArea *pArea = (CTFNavArea *)(params[1]); \
		if(!pArea) { \
			return pContext->ThrowNativeError("Invalid nav area %x", params[1]); \
		}

TFNAVAREA_NATIVE(GetAttributesTF)
	return pArea->GetAttributesTF();
}

TFNAVAREA_NATIVE(SetAttributeTF)
	pArea->SetAttributeTF(params[2]);
	return 0;
}

TFNAVAREA_NATIVE(ClearAttributeTF)
	pArea->ClearAttributeTF(params[2]);
	return 0;
}

TFNAVAREA_NATIVE(HasAttributeTF)
	return pArea->HasAttributeTF(params[2]);
}
#endif

#endif