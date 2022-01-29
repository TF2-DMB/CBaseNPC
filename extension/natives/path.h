#ifndef NATIVES_PATH_H_INCLUDED_
#define NATIVES_PATH_H_INCLUDED_

#pragma once

#include "NextBotPath.h"
#include "NextBotPathFollow.h"
#include "NextBotChasePath.h"

class SMBotPathCost : public IPathCost
{
public:
	SMBotPathCost( INextBot *pBot, IPluginFunction *pFunc = NULL )
	{
		m_pFunc = pFunc;
		m_bot = pBot;
	}
	virtual float operator()( CNavArea *area, CNavArea *fromArea, const CNavLadder *ladder, const CFuncElevator *elevator, float length ) const override
	{
		if ( fromArea == NULL )
		{
			return 0.0f;
		}
		else
		{
			if (!m_pFunc)
			{
				ILocomotion *mover = m_bot->GetLocomotionInterface();
				
				if (!mover->IsAreaTraversable(area)) return -1.0f;
				
				float dist;
				if (length > 0.0f) {
					dist = length;
				} else {
					dist = (area->GetCenter() - fromArea->GetCenter()).Length();
				}
				
				/* account for step height, max jump height, death drop height */
				float delta_z = fromArea->ComputeAdjacentConnectionHeightChange(area);
				if (delta_z >= mover->GetStepHeight()) {
					if (delta_z >= mover->GetMaxJumpHeight()) return -1.0f;
					
					/* cost penalty for going up steps */
					dist *= 2.0f;
				} else {
					if (delta_z < -mover->GetDeathDropHeight()) return -1.0f;
				}
				
				return dist + fromArea->GetCostSoFar();
			}
		
			cell_t cost = sp_ftoc(0.0);
			m_pFunc->PushCell((cell_t)m_bot);
			m_pFunc->PushCell((cell_t)area);
			m_pFunc->PushCell((cell_t)fromArea);
			m_pFunc->PushCell((cell_t)ladder);
			m_pFunc->PushCell(gamehelpers->EntityToBCompatRef((CBaseEntity *)elevator));
			m_pFunc->PushFloat(length);
			m_pFunc->Execute(&cost);
			
			return sp_ctof(cost);
		}
	};
private:
	IPluginFunction *m_pFunc;
	INextBot *m_bot;
};

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}
	
#define SEGMENTNATIVE(name) \
	cell_t Segment_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		Path::Segment *seg = (Path::Segment *)(params[1]); \
		if(!seg) { \
			return pContext->ThrowNativeError("Invalid segment %x", params[1]); \
		}
		
#define CURSORDATANATIVE(name) \
	cell_t CursorData_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		Path::Data *cursor = (Path::Data *)(params[1]); \
		if(!cursor) { \
			return pContext->ThrowNativeError("Invalid cursor data %x", params[1]); \
		}

#define PATHNATIVE(name) \
	cell_t Path_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		Path *pPath = (Path *)(params[1]); \
		if(!pPath) { \
			return pContext->ThrowNativeError("Invalid Path %x", params[1]); \
		} \

#define PATHFOLLOWNATIVE(name) \
	cell_t PathFollower_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		PathFollower *pPathFollow = (PathFollower *)(params[1]); \
		if(!pPathFollow) { \
			return pContext->ThrowNativeError("Invalid PathFollower %x", params[1]); \
		} \

#define CHASEPATHNATIVE(name) \
	cell_t ChasePath_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		ChasePath *pChasePath = (ChasePath *)(params[1]); \
		if(!pChasePath) { \
			return pContext->ThrowNativeError("Invalid ChasePath %x", params[1]); \
		} \
		
#define DIRECTCHASEPATHNATIVE(name) \
	cell_t DirectChasePath_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		DirectChasePath *pChasePath = (DirectChasePath *)(params[1]); \
		if(!pChasePath) { \
			return pContext->ThrowNativeError("Invalid DirectChasePath %x", params[1]); \
		} \

cell_t Path_Path(IPluginContext *pContext, const cell_t *params)
{
	Path *pNewPath = new Path;
	IPluginFunction *pCostCallback = pContext->GetFunctionById(params[1]);
	IPluginFunction *pTraceFilter = pContext->GetFunctionById(params[2]);
	IPluginFunction *pTraceFilter2 = pContext->GetFunctionById(params[3]);

	pNewPath->pCostFunction = pCostCallback;
	pNewPath->pTraceFilterIgnoreActors = pTraceFilter;
	pNewPath->pTraceFilterOnlyActors = pTraceFilter2;
	
	return (cell_t)pNewPath;
}

SEGMENTNATIVE(areaGet)
	return (cell_t)seg->area;
}

SEGMENTNATIVE(howGet)
	return seg->how;
}

SEGMENTNATIVE(GetPos)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	VectorToPawnVector(posAddr, &seg->pos);
	return 0;
}

SEGMENTNATIVE(ladderGet)
	return (cell_t)seg->ladder;
}

SEGMENTNATIVE(typeGet)
	return seg->type;
}

SEGMENTNATIVE(GetForward)
	cell_t *vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, &seg->forward);
	return 0;
}

SEGMENTNATIVE(lengthGet)
	return sp_ftoc(seg->length);
}

SEGMENTNATIVE(distanceFromStartGet)
	return sp_ftoc(seg->distanceFromStart);
}

SEGMENTNATIVE(curvatureGet)
	return sp_ftoc(seg->curvature);
}

SEGMENTNATIVE(GetPortalCenter)
	cell_t *vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, &seg->m_portalCenter);
	return 0;
}

SEGMENTNATIVE(m_portalHalfWidthGet)
	return sp_ftoc(seg->m_portalHalfWidth);
}

CURSORDATANATIVE(GetPos)
	cell_t *vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, &cursor->pos);
	return 0;
}

CURSORDATANATIVE(GetForward)
	cell_t *vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, &cursor->forward);
	return 0;
}

CURSORDATANATIVE(curvatureGet)
	return sp_ftoc(cursor->curvature);
}

CURSORDATANATIVE(segmentPriorGet)
	return (cell_t)cursor->segmentPrior;
}

PATHNATIVE(GetLength)
	return sp_ftoc(pPath->GetLength());
}

PATHNATIVE(GetPosition)
	float dist = sp_ctof(params[2]);
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[3], &posAddr);
	Path::Segment *pSegment = (Path::Segment *)(params[4]);

	Vector pos = pPath->GetPosition(dist, pSegment);
	VectorToPawnVector(posAddr, pos);
	return 0;
}

PATHNATIVE(Copy)
	INextBot *pNextBot = (INextBot *)(params[2]);
	Path *pPath2 = (Path *)(params[3]);
	
	pPath->Copy(pNextBot, *pPath2);
	return 0;
}

PATHNATIVE(GetClosestPosition)
	cell_t *nearAddr;
	pContext->LocalToPhysAddr(params[2], &nearAddr);
	Vector vecNear;
	PawnVectorToVector(nearAddr, vecNear);
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[3], &posAddr);
	Path::Segment *pSegment = (Path::Segment *)(params[4]);
	float alongLimit = sp_ctof(params[5]);

	Vector pos = pPath->GetClosestPosition(vecNear, pSegment, alongLimit);
	VectorToPawnVector(posAddr, pos);
	return 0;
}

PATHNATIVE(GetStartPosition)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos = pPath->GetStartPosition();
	VectorToPawnVector(posAddr, pos);
	return 0;
}

PATHNATIVE(GetEndPosition)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos = pPath->GetEndPosition();
	VectorToPawnVector(posAddr, pos);
	return 0;
}

PATHNATIVE(GetSubject)
	CBaseEntity *pEntity = (CBaseEntity*)(pPath->GetSubject());
	return gamehelpers->EntityToBCompatRef(pEntity);
}

PATHNATIVE(GetCurrentGoal)
	return (cell_t)(pPath->GetCurrentGoal());
}

PATHNATIVE(GetAge)
	return sp_ftoc(pPath->GetAge());
}

PATHNATIVE(MoveCursorToClosestPosition)
	cell_t *nearAddr;
	pContext->LocalToPhysAddr(params[2], &nearAddr);
	Vector vecNear;
	PawnVectorToVector(nearAddr, vecNear);
	pPath->MoveCursorToClosestPosition(vecNear, (Path::SeekType)params[3], sp_ctof(params[4]));
	return 0;
}

PATHNATIVE(MoveCursorToStart)
	pPath->MoveCursorToStart();
	return 0;
}

PATHNATIVE(MoveCursorToEnd)
	pPath->MoveCursorToEnd();
	return 0;
}

PATHNATIVE(MoveCursor)
	pPath->MoveCursor(sp_ctof(params[2]), (Path::MoveCursorType)params[3]);
	return 0;
}

PATHNATIVE(GetCursorPosition)
	return sp_ftoc(pPath->GetCursorPosition());
}

PATHNATIVE(GetCursorData)
	return (cell_t)(&pPath->GetCursorData());
}

PATHNATIVE(IsValid)
	return (cell_t)(pPath->IsValid());
}

PATHNATIVE(Invalidate)
	pPath->Invalidate();
	return 0;
}

PATHNATIVE(Draw)
	Path::Segment *pSegment = (Path::Segment *)(params[2]);
	pPath->Draw(pSegment);
	return 0;
}

PATHNATIVE(DrawInterpolated)
	pPath->DrawInterpolated(sp_ctof(params[2]), sp_ctof(params[3]));
	return 0;
}

PATHNATIVE(FirstSegment)
	return (cell_t)(pPath->FirstSegment());
}

PATHNATIVE(NextSegment)
	Path::Segment *pSegment = (Path::Segment *)(params[2]);
	if(!pSegment) {
		return pContext->ThrowNativeError("Invalid Segment %x", params[2]);
	}
	return (cell_t)(pPath->NextSegment(pSegment));
}

PATHNATIVE(PriorSegment)
	Path::Segment *pSegment = (Path::Segment *)(params[2]);
	if(!pSegment) {
		return pContext->ThrowNativeError("Invalid Segment %x", params[2]);
	}
	return (cell_t)(pPath->PriorSegment(pSegment));
}

PATHNATIVE(LastSegment)
	return (cell_t)(pPath->LastSegment());
}

PATHNATIVE(ComputeToPos)
	INextBot *pBot = (INextBot *)params[2];
	cell_t *vec;
	pContext->LocalToPhysAddr(params[3], &vec);
	Vector vecGoal;
	PawnVectorToVector(vec, vecGoal);
	float maxPathLength = sp_ctof(params[4]);
	bool includePathIfGoalFails = params[5];
	
	SMBotPathCost pCostFunc(pBot, pPath->pCostFunction);
	
	return pPath->Compute(pBot, vecGoal, pCostFunc, maxPathLength, includePathIfGoalFails);
}

PATHNATIVE(ComputeToTarget)
	INextBot *pBot = (INextBot *)params[2];
	CBaseEntity *pTarget;
	ENTINDEX_TO_CBASEENTITY(params[3], pTarget);

	SMBotPathCost pCostFunc(pBot, pPath->pCostFunction);
	
	return pPath->Compute(pBot, (CBaseCombatCharacterHack *)pTarget, pCostFunc, sp_ctof(params[4]), (params[5]) ? true : false);
}

PATHNATIVE(Destroy)
	delete pPath;
	return 1;
}

cell_t PathFollower_PathFollower(IPluginContext *pContext, const cell_t *params)
{
	PathFollower *pNewPath = new PathFollower;
	IPluginFunction *pCostCallback = pContext->GetFunctionById(params[1]);
	IPluginFunction *pTraceFilter = pContext->GetFunctionById(params[2]);
	IPluginFunction *pTraceFilter2 = pContext->GetFunctionById(params[3]);
	
	pNewPath->pCostFunction = pCostCallback;
	pNewPath->pTraceFilterIgnoreActors = pTraceFilter;
	pNewPath->pTraceFilterOnlyActors = pTraceFilter2;
	
	return (cell_t)pNewPath;
}


PATHFOLLOWNATIVE(Update)
	INextBot *pNextBot = (INextBot *)(params[2]);
	if(!pNextBot) {
		return pContext->ThrowNativeError("Invalid INextBot %x", params[2]);
	}
	pPathFollow->Update(pNextBot);
	return 1;
}

PATHFOLLOWNATIVE(SetMinLookAheadDistance)
	pPathFollow->SetMinLookAheadDistance(sp_ctof(params[2]));
	return 1;
}

PATHFOLLOWNATIVE(GetHindrance)
	CBaseEntity *pEntity = pPathFollow->GetHindrance();
	return gamehelpers->EntityToBCompatRef(pEntity);
}

PATHFOLLOWNATIVE(IsDiscontinuityAhead)
	INextBot *pNextBot = (INextBot *)(params[2]);
	if(!pNextBot) {
		return pContext->ThrowNativeError("Invalid INextBot %x", params[2]);
	}
	return (cell_t)(pPathFollow->IsDiscontinuityAhead(pNextBot, (Path::SegmentType)params[3], sp_ctof(params[4])));
}

PATHFOLLOWNATIVE(SetGoalTolerance)
	pPathFollow->SetGoalTolerance(sp_ctof(params[2]));
	return 1;
}

cell_t ChasePath_ChasePath(IPluginContext *pContext, const cell_t *params)
{
	ChasePath::SubjectChaseType how = (ChasePath::SubjectChaseType)params[2];
	ChasePath *pNewPath = new ChasePath(how);
	
	IPluginFunction *pCostCallback = pContext->GetFunctionById(params[2]);
	IPluginFunction *pTraceFilter = pContext->GetFunctionById(params[3]);
	IPluginFunction *pTraceFilter2 = pContext->GetFunctionById(params[4]);
	
	pNewPath->pCostFunction = pCostCallback;
	pNewPath->pTraceFilterIgnoreActors = pTraceFilter;
	pNewPath->pTraceFilterOnlyActors = pTraceFilter2;
	
	return (cell_t)pNewPath;
}

CHASEPATHNATIVE(Update)

	INextBot *pBot = (INextBot *)params[2];
	CBaseEntity *pTarget;
	ENTINDEX_TO_CBASEENTITY(params[3], pTarget);

	SMBotPathCost pCostFunc(pBot, pChasePath->pCostFunction);
	
	cell_t *predictPos;
	pContext->LocalToPhysAddr(params[4], &predictPos);
	
	Vector vpredictPos;
	PawnVectorToVector(predictPos, vpredictPos);
	
	if (pContext->GetNullRef(SP_NULL_VECTOR) == predictPos)
		pChasePath->Update(pBot, (CBaseEntityHack *)pTarget, pCostFunc, NULL);
	else
	{
		pChasePath->Update(pBot, (CBaseEntityHack*)pTarget, pCostFunc, &vpredictPos);
		VectorToPawnVector(predictPos, vpredictPos);
	}
	
	return 1;
}

CHASEPATHNATIVE(GetLeadRadius)
	return sp_ftoc(pChasePath->GetLeadRadius());
}

CHASEPATHNATIVE(GetMaxPathLength)
	return sp_ftoc(pChasePath->GetMaxPathLength());
}

CHASEPATHNATIVE(PredictSubjectPosition)
	INextBot *pNextBot = (INextBot *)(params[2]);
	if(!pNextBot)
	{
		return pContext->ThrowNativeError("Invalid INextBot %x", params[2]);
	}
	CBaseEntityHack *pEntity = (CBaseEntityHack *)gamehelpers->ReferenceToEntity(params[3]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[3]);
	}
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[4], &posAddr);
	Vector pos = pChasePath->PredictSubjectPosition(pNextBot, pEntity);
	VectorToPawnVector(posAddr, pos);
	return 0;
}

CHASEPATHNATIVE(IsRepathNeeded)
	INextBot *pNextBot = (INextBot *)(params[2]);
	if (!pNextBot)
	{
		return pContext->ThrowNativeError("Invalid INextBot %x", params[2]);
	}
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[3]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[3]);
	}
	return (cell_t)(pChasePath->IsRepathNeeded(pNextBot, pEntity));
}

CHASEPATHNATIVE(GetLifetime)
	return sp_ftoc(pChasePath->GetLifetime());
}

cell_t DirectChasePath_DirectChasePath(IPluginContext *pContext, const cell_t *params) \
{
	ChasePath::SubjectChaseType how = (ChasePath::SubjectChaseType)params[2];
	DirectChasePath *pNewPath = new DirectChasePath(how);
	
	IPluginFunction *pCostCallback = pContext->GetFunctionById(params[2]);
	IPluginFunction *pTraceFilter = pContext->GetFunctionById(params[3]);
	IPluginFunction *pTraceFilter2 = pContext->GetFunctionById(params[4]);
	
	pNewPath->pCostFunction = pCostCallback;
	pNewPath->pTraceFilterIgnoreActors = pTraceFilter;
	pNewPath->pTraceFilterOnlyActors = pTraceFilter2;
	
	return (cell_t)pNewPath;
}

#endif