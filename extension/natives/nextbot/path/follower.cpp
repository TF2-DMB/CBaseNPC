#include "NextBotPathFollow.h"

#include "follower.hpp"
#include "../path.hpp"

namespace natives::nextbot::path::follower {

template<typename T>
inline T* Get(IPluginContext* context, const cell_t param) {	
	T* bot = (T*)PawnAddressToPtr(param);
	if (!bot) {
		context->ThrowNativeError("Object is a null ptr!");
		return nullptr;
	}
	return bot;
}

cell_t PathFollowerCtor(IPluginContext* context, const cell_t* params) {
	PathFollower* path = new PathFollower;

	IPluginFunction* pCostCallback = context->GetFunctionById(params[1]);
	IPluginFunction* pTraceFilter = context->GetFunctionById(params[2]);
	IPluginFunction* pTraceFilter2 = context->GetFunctionById(params[3]);
	
	path->pCostFunction = pCostCallback;
	path->pTraceFilterIgnoreActors = pTraceFilter;
	path->pTraceFilterOnlyActors = pTraceFilter2;
	
	return PtrToPawnAddress(path);
}

cell_t Update(IPluginContext* context, const cell_t* params) {
	auto path = Get<PathFollower>(context, params[1]);
	if (path) {
		return 0;
	}

	auto bot = Get<INextBot>(context, params[2]);
	if (!bot) {
		return 0;
	}

	path->Update(bot);
	return 0;
}

cell_t SetMinLookAheadDistance(IPluginContext* context, const cell_t* params) {
	auto path = Get<PathFollower>(context, params[1]);
	if (path) {
		return 0;
	}

	path->SetMinLookAheadDistance(sp_ctof(params[2]));
	return 0;
}

cell_t GetHindrance(IPluginContext* context, const cell_t* params) {
	auto path = Get<PathFollower>(context, params[1]);
	if (path) {
		return 0;
	}

	CBaseEntity* entity = path->GetHindrance();
	return gamehelpers->EntityToBCompatRef(entity);
}

cell_t IsDiscontinuityAhead(IPluginContext* context, const cell_t* params) {
	auto path = Get<PathFollower>(context, params[1]);
	if (path) {
		return 0;
	}

	auto bot = Get<INextBot>(context, params[2]);
	if (!bot) {
		return 0;
	}

	return (path->IsDiscontinuityAhead(bot, (Path::SegmentType)params[3], sp_ctof(params[4])) == true) ? 1 : 0;
}

cell_t SetGoalTolerance(IPluginContext* context, const cell_t* params) {
	auto path = Get<PathFollower>(context, params[1]);
	if (path) {
		return 0;
	}

	path->SetGoalTolerance(sp_ctof(params[2]));
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"PathFollower.PathFollower", PathFollowerCtor},
		{"PathFollower.Update", Update},
		{"PathFollower.SetMinLookAheadDistance", SetMinLookAheadDistance},
		{"PathFollower.GetHindrance", GetHindrance},
		{"PathFollower.IsDiscontinuityAhead", IsDiscontinuityAhead},
		{"PathFollower.SetGoalTolerance", SetGoalTolerance},
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}