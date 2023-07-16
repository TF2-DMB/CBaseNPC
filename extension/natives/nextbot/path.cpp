#include "path.hpp"
#include "path/chase.hpp"
#include "path/follower.hpp"

namespace natives::nextbot::path {

SMPathFollowerCost::SMPathFollowerCost(INextBot* bot, IPluginFunction* func) : m_pFunc(func), m_bot(bot)
{}

float SMPathFollowerCost::operator()(CNavArea* area, CNavArea* fromArea, const CNavLadder* ladder, const CFuncElevator* elevator, float length) const {
	if (fromArea == nullptr) {
		return 0.0f;
	} else {
		if (!m_pFunc) {
			ILocomotion* mover = m_bot->GetLocomotionInterface();
			
			if (!mover->IsAreaTraversable(area)) {
				return -1.0f;
			}
			
			float dist;
			if (length > 0.0f) {
				dist = length;
			} else {
				dist = (area->GetCenter() - fromArea->GetCenter()).Length();
			}
			
			/* account for step height, max jump height, death drop height */
			float delta_z = fromArea->ComputeAdjacentConnectionHeightChange(area);
			if (delta_z >= mover->GetStepHeight()) {
				if (delta_z >= mover->GetMaxJumpHeight()) {
					return -1.0f;
				}
				
				/* cost penalty for going up steps */
				dist *= 2.0f;
			} else {
				if (delta_z < -mover->GetDeathDropHeight()) {
					return -1.0f;
				}
			}
			
			return dist + fromArea->GetCostSoFar();
		}
	
		cell_t cost = sp_ftoc(0.0);
		m_pFunc->PushCell(PtrToPawnAddress(m_bot));
		m_pFunc->PushCell(PtrToPawnAddress(area));
		m_pFunc->PushCell(PtrToPawnAddress(fromArea));
		m_pFunc->PushCell(PtrToPawnAddress(ladder));
		m_pFunc->PushCell(gamehelpers->EntityToBCompatRef((CBaseEntity *)elevator));
		m_pFunc->PushFloat(length);
		m_pFunc->Execute(&cost);
		
		return sp_ctof(cost);
	}
}

template<typename T>
inline T* Get(IPluginContext* context, const cell_t param) {
	T* bot = (T*)PawnAddressToPtr(param);
	if (!bot) {
		context->ThrowNativeError("Object is a null ptr!");
		return nullptr;
	}
	return bot;
}

namespace segment {

cell_t GetArea(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	return PtrToPawnAddress(seg->area);
}

cell_t GetHow(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	return seg->how;
}

cell_t GetPosition(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	cell_t* posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	VectorToPawnVector(posAddr, &seg->pos);
	return 0;
}

cell_t GetLadder(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	return PtrToPawnAddress(seg->ladder);
}

cell_t GetType(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	return seg->type;
}

cell_t GetForward(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	cell_t *vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, &seg->forward);
	return 0;
}

cell_t GetLength(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	return sp_ftoc(seg->length);
}

cell_t GetDistanceFromStart(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	return sp_ftoc(seg->distanceFromStart);
}

cell_t GetCurvature(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	return sp_ftoc(seg->curvature);
}

cell_t GetPortalCenter(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	cell_t* vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, &seg->m_portalCenter);
	return 0;
}

cell_t GetPortalHalfWidth(IPluginContext* context, const cell_t* params) {
	auto seg = Get<Path::Segment>(context, params[1]);
	if (!seg) {
		return 0;
	}

	return sp_ftoc(seg->m_portalHalfWidth);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"Segment.Area.get", GetArea},
		{"Segment.How.get", GetHow},
		{"Segment.GetPosition", GetPosition},
		{"Segment.Ladder.get", GetLadder},
		{"Segment.Type.get", GetType},
		{"Segment.GetForward", GetForward},
		{"Segment.Length.get", GetLength},
		{"Segment.DistanceFromStart.get", GetDistanceFromStart},
		{"Segment.Curvature.get", GetCurvature},
		{"Segment.GetPortalCenter", GetPortalCenter},
		{"Segment.PortalHalfWidth.get", GetPortalHalfWidth},

		// TO-DO: Remove in 2.0.0
		{"Segment.area.get", GetArea},
		{"Segment.how.get", GetHow},
		{"Segment.GetPos", GetPosition},
		{"Segment.ladder.get", GetLadder},
		{"Segment.type.get", GetType},
		{"Segment.distanceFromStart.get", GetDistanceFromStart},
		{"Segment.curvature.get", GetCurvature},
		{"Segment.m_portalHalfWidth.get", GetPortalHalfWidth},
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}
}

namespace cursor {

cell_t GetPosition(IPluginContext* context, const cell_t* params) {
	auto cursor = Get<Path::Data>(context, params[1]);
	if (!cursor) {
		return 0;
	}

	cell_t* vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, &cursor->pos);
	return 0;
}

cell_t GetForward(IPluginContext* context, const cell_t* params) {
	auto cursor = Get<Path::Data>(context, params[1]);
	if (!cursor) {
		return 0;
	}

	cell_t* vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, &cursor->forward);
	return 0;
}

cell_t GetCurvature(IPluginContext* context, const cell_t* params) {
	auto cursor = Get<Path::Data>(context, params[1]);
	if (!cursor) {
		return 0;
	}

	return sp_ftoc(cursor->curvature);
}

cell_t GetSegmentPrior(IPluginContext* context, const cell_t* params) {
	auto cursor = Get<Path::Data>(context, params[1]);
	if (!cursor) {
		return 0;
	}

	PtrToPawnAddress(cursor->segmentPrior);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CursorData.GetPosition", GetPosition},
		{"CursorData.GetForward", GetForward},
		{"CursorData.Curvature.get", GetCurvature},
		{"CursorData.SegmentPrior.get", GetSegmentPrior},

		// TO-DO: Remove in 2.0.0
		{"CursorData.GetPos", GetPosition},
		{"CursorData.curvature.get", GetCurvature},
		{"CursorData.segmentPrior.get", GetSegmentPrior}
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}
}

cell_t PathCtor(IPluginContext* context, const cell_t* params) {
	Path* path = new Path;

	IPluginFunction* pCostCallback = context->GetFunctionById(params[1]);
	IPluginFunction* pTraceFilter = context->GetFunctionById(params[2]);
	IPluginFunction* pTraceFilter2 = context->GetFunctionById(params[3]);

	path->pCostFunction = pCostCallback;
	path->pTraceFilterIgnoreActors = pTraceFilter;
	path->pTraceFilterOnlyActors = pTraceFilter2;
	
	return PtrToPawnAddress(path);
}

cell_t GetLength(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	return sp_ftoc(path->GetLength());
}

cell_t GetPosition(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	float dist = sp_ctof(params[2]);
	cell_t* addr;
	context->LocalToPhysAddr(params[3], &addr);
	Path::Segment* segment = (Path::Segment*)(params[4]);

	Vector pos = path->GetPosition(dist, segment);
	VectorToPawnVector(addr, pos);
	return 0;
}

cell_t Copy(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	INextBot* bot = (INextBot*)(params[2]);
	Path* path2 = (Path*)(params[3]);
	
	path->Copy(bot, *path2);
	return 0;
}

cell_t GetClosestPosition(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	cell_t* nearAddr;
	context->LocalToPhysAddr(params[2], &nearAddr);
	Vector vecNear;
	PawnVectorToVector(nearAddr, vecNear);
	cell_t* posAddr;
	context->LocalToPhysAddr(params[3], &posAddr);
	Path::Segment* segment = (Path::Segment*)(params[4]);
	float alongLimit = sp_ctof(params[5]);

	Vector pos = path->GetClosestPosition(vecNear, segment, alongLimit);
	VectorToPawnVector(posAddr, pos);
	return 0;
}

cell_t GetStartPosition(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	cell_t* addr;
	context->LocalToPhysAddr(params[2], &addr);
	Vector pos = path->GetStartPosition();
	VectorToPawnVector(addr, pos);
	return 0;
}

cell_t GetEndPosition(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	cell_t* addr;
	context->LocalToPhysAddr(params[2], &addr);
	Vector pos = path->GetEndPosition();
	VectorToPawnVector(addr, pos);
	return 0;
}

cell_t GetSubject(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	CBaseEntity* entity = path->GetSubject();
	return gamehelpers->EntityToBCompatRef(entity);
}

cell_t GetCurrentGoal(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	return PtrToPawnAddress(path->GetCurrentGoal());
}

cell_t GetAge(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	return sp_ftoc(path->GetAge());
}

cell_t MoveCursorToClosestPosition(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	cell_t* nearAddr;
	context->LocalToPhysAddr(params[2], &nearAddr);
	Vector vecNear;
	PawnVectorToVector(nearAddr, vecNear);
	path->MoveCursorToClosestPosition(vecNear, (Path::SeekType)params[3], sp_ctof(params[4]));
	return 0;
}

cell_t MoveCursorToStart(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	path->MoveCursorToStart();
	return 0;
}

cell_t MoveCursorToEnd(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	path->MoveCursorToEnd();
	return 0;
}

cell_t MoveCursor(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	path->MoveCursor(sp_ctof(params[2]), (Path::MoveCursorType)params[3]);
	return 0;
}

cell_t GetCursorPosition(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	return sp_ftoc(path->GetCursorPosition());
}

cell_t GetCursorData(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	return PtrToPawnAddress(&path->GetCursorData());
}

cell_t IsValid(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	return path->IsValid() ? 1 : 0;
}

cell_t Invalidate(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	path->Invalidate();
	return 0;
}

cell_t Draw(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	Path::Segment* segment = (Path::Segment*)(params[2]);
	path->Draw(segment);
	return 0;
}

cell_t DrawInterpolated(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	path->DrawInterpolated(sp_ctof(params[2]), sp_ctof(params[3]));
	return 0;
}

cell_t FirstSegment(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	return PtrToPawnAddress(path->FirstSegment());
}

cell_t NextSegment(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	Path::Segment* segment = Get<Path::Segment>(context, params[2]);
	if (!segment) {
		return 0;
	}

	return PtrToPawnAddress(path->NextSegment(segment));
}

cell_t PriorSegment(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	Path::Segment* segment = Get<Path::Segment>(context, params[2]);
	if (!segment) {
		return 0;
	}

	return PtrToPawnAddress(path->PriorSegment(segment));
}

cell_t LastSegment(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	return PtrToPawnAddress(path->LastSegment());
}

cell_t ComputeToPos(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	auto bot = Get<INextBot>(context, params[2]);
	if (!bot) {
		return 0;
	}

	cell_t* vec;
	context->LocalToPhysAddr(params[3], &vec);
	Vector vecGoal;
	PawnVectorToVector(vec, vecGoal);
	float maxPathLength = sp_ctof(params[4]);
	bool includePathIfGoalFails = params[5];
	
	SMPathFollowerCost func(bot, path->pCostFunction);
	
	return path->Compute(bot, vecGoal, func, maxPathLength, includePathIfGoalFails);
}

cell_t ComputeToTarget(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (!path) {
		return 0;
	}

	auto bot = Get<INextBot>(context, params[2]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[3]);
	if (!entity || !(entity = entity->MyCombatCharacterPointer())) {
		context->ThrowNativeError("Invalid entity %d", params[3]);
		return 0;
	}


	cell_t* vec;
	context->LocalToPhysAddr(params[3], &vec);
	Vector vecGoal;
	PawnVectorToVector(vec, vecGoal);
	float maxPathLength = sp_ctof(params[4]);
	bool includePathIfGoalFails = params[5];
	
	SMPathFollowerCost func(bot, path->pCostFunction);
	
	return path->Compute(bot, (CBaseCombatCharacter*)entity, func, sp_ctof(params[4]), (params[5]) ? true : false);
}

cell_t Destroy(IPluginContext* context, const cell_t* params) {
	auto path = Get<Path>(context, params[1]);
	if (path) {
		return 0;
	}

	delete path;
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	cursor::setup(natives);

	chase::setup(natives);
	follower::setup(natives);

	sp_nativeinfo_t list[] = {
		{"Path.Path", PathCtor},
		{"Path.GetLength", GetLength},
		{"Path.GetPosition", GetPosition},
		{"Path.Copy", Copy},
		{"Path.GetClosestPosition", GetClosestPosition},
		{"Path.GetStartPosition", GetStartPosition},
		{"Path.GetEndPosition", GetEndPosition},
		{"Path.GetSubject", GetSubject},
		{"Path.GetCurrentGoal", GetCurrentGoal},
		{"Path.GetAge", GetAge},
		{"Path.MoveCursorToClosestPosition", MoveCursorToClosestPosition},
		{"Path.MoveCursorToStart", MoveCursorToStart},
		{"Path.MoveCursorToEnd", MoveCursorToEnd},
		{"Path.MoveCursor", MoveCursor},
		{"Path.GetCursorPosition", GetCursorPosition},
		{"Path.GetCursorData", GetCursorData},
		{"Path.IsValid", IsValid},
		{"Path.Invalidate", Invalidate},
		{"Path.Draw", Draw},
		{"Path.DrawInterpolated", DrawInterpolated},
		{"Path.FirstSegment", FirstSegment},
		{"Path.NextSegment", NextSegment},
		{"Path.PriorSegment", PriorSegment},
		{"Path.LastSegment", LastSegment},
		{"Path.ComputeToPos", ComputeToPos},
		{"Path.ComputeToTarget", ComputeToTarget},
		{"Path.Destroy", Destroy},
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}