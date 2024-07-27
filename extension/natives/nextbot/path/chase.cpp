#include "NextBotChasePath.h"

#include "chase.hpp"

namespace natives::nextbot::path::chase {

template<typename T>
inline T* Get(IPluginContext* context, const cell_t param) {	
	T* bot = (T*)PawnAddressToPtr(param, context);
	if (!bot) {
		context->ThrowNativeError("Object is a null ptr!");
		return nullptr;
	}
	return bot;
}

cell_t ChasePathCtor(IPluginContext* context, const cell_t* params) {
	ChasePath::SubjectChaseType how = (ChasePath::SubjectChaseType)params[1];
	ChasePath* path = new ChasePath(how);
	
	IPluginFunction *pCostCallback = context->GetFunctionById(params[2]);
	IPluginFunction *pTraceFilter = context->GetFunctionById(params[3]);
	IPluginFunction *pTraceFilter2 = context->GetFunctionById(params[4]);
	
	path->pCostFunction = pCostCallback;
	path->pTraceFilterIgnoreActors = pTraceFilter;
	path->pTraceFilterOnlyActors = pTraceFilter2;
	
	return PtrToPawnAddress(path, context);
}

cell_t Update(IPluginContext* context, const cell_t* params) {
	auto path = Get<ChasePath>(context, params[1]);
	if (!path) {
		return 0;
	}

	auto bot = Get<INextBot>(context, params[2]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[3]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[3]);
	}

	SMPathFollowerCost func(bot, path->pCostFunction);
	
	cell_t* predictPos;
	context->LocalToPhysAddr(params[4], &predictPos);
	
	Vector vpredictPos;
	PawnVectorToVector(predictPos, vpredictPos);
	
	if (context->GetNullRef(SP_NULL_VECTOR) == predictPos) {
		path->Update(bot, entity, func, nullptr);
	}
	else {
		path->Update(bot, entity, func, &vpredictPos);
		VectorToPawnVector(predictPos, vpredictPos);
	}
	return 0;
}

cell_t GetLeadRadius(IPluginContext* context, const cell_t* params) {
	auto path = Get<ChasePath>(context, params[1]);
	if (!path) {
		return 0;
	}

	return sp_ftoc(path->GetLeadRadius());
}

cell_t GetMaxPathLength(IPluginContext* context, const cell_t* params) {
	auto path = Get<ChasePath>(context, params[1]);
	if (!path) {
		return 0;
	}

	return sp_ftoc(path->GetMaxPathLength());
}

cell_t PredictSubjectPosition(IPluginContext* context, const cell_t* params) {
	auto path = Get<ChasePath>(context, params[1]);
	if (!path) {
		return 0;
	}

	auto bot = Get<INextBot>(context, params[2]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[3]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[3]);
	}

	cell_t* posAddr;
	context->LocalToPhysAddr(params[4], &posAddr);
	Vector pos = path->PredictSubjectPosition(bot, entity);
	VectorToPawnVector(posAddr, pos);
	return 0;
}

cell_t IsRepathNeeded(IPluginContext* context, const cell_t* params) {
	auto path = Get<ChasePath>(context, params[1]);
	if (!path) {
		return 0;
	}

	auto bot = Get<INextBot>(context, params[2]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[3]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[3]);
	}

	return path->IsRepathNeeded(bot, entity) ? 1 : 0;
}

cell_t GetLifetime(IPluginContext* context, const cell_t* params) {
	auto path = Get<ChasePath>(context, params[1]);
	if (!path) {
		return 0;
	}

	return sp_ftoc(path->GetLifetime());
}

cell_t DirectChasePathCtor(IPluginContext* context, const cell_t* params) {
	ChasePath::SubjectChaseType how = (ChasePath::SubjectChaseType)params[1];
	DirectChasePath* path = new DirectChasePath(how);
	
	IPluginFunction* pCostCallback = context->GetFunctionById(params[2]);
	IPluginFunction* pTraceFilter = context->GetFunctionById(params[3]);
	IPluginFunction* pTraceFilter2 = context->GetFunctionById(params[4]);
	
	path->pCostFunction = pCostCallback;
	path->pTraceFilterIgnoreActors = pTraceFilter;
	path->pTraceFilterOnlyActors = pTraceFilter2;
	
	return PtrToPawnAddress(path, context);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"ChasePath.ChasePath", ChasePathCtor},
		{"ChasePath.Update", Update},
		{"ChasePath.GetLeadRadius", GetLeadRadius},
		{"ChasePath.GetMaxPathLength", GetMaxPathLength},
		{"ChasePath.PredictSubjectPosition", PredictSubjectPosition},
		{"ChasePath.IsRepathNeeded", IsRepathNeeded},
		{"ChasePath.GetLifetime", GetLifetime},
		{"DirectChasePath.DirectChasePath", DirectChasePathCtor}
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}
}