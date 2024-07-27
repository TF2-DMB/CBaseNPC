#include "knownentity.hpp"

#include "NextBotKnownEntity.h"

namespace natives::nextbot::knownentity  {

inline CKnownEntity* Get(IPluginContext* context, const cell_t param) {	
	CKnownEntity* entity = (CKnownEntity*)PawnAddressToPtr(param, context);
	if (!entity) {
		context->ThrowNativeError("Known entity ptr is null!");
		return nullptr;
	}
	return entity;
}

cell_t Destroy(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->Destroy();
	return 0;
}

cell_t UpdatePosition(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->UpdatePosition();
	return 0;
}

cell_t GetEntity(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return gamehelpers->EntityToBCompatRef(entity->GetEntity());
}

cell_t GetLastKnownPosition(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	Vector vPos = entity->GetLastKnownPosition();
	
	cell_t *pos;
	context->LocalToPhysAddr(params[2], &pos);
	VectorToPawnVector(pos, vPos);
	return 0;
}

cell_t HasLastKnownPositionBeenSeen(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->HasLastKnownPositionBeenSeen();
}

cell_t MarkLastKnownPositionAsSeen(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->MarkLastKnownPositionAsSeen();
	return 0;
}

cell_t GetLastKnownArea(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return PtrToPawnAddress(entity->GetLastKnownArea(), context);
}

cell_t GetTimeSinceLastKnown(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}
	return sp_ftoc(entity->GetTimeSinceLastKnown());
}

cell_t GetTimeSinceBecameKnown(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetTimeSinceBecameKnown());
}

cell_t UpdateVisibilityStatus(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->UpdateVisibilityStatus((params[2]) ? true : false);
	return 0;
}

cell_t IsVisibleInFOVNow(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->IsVisibleInFOVNow();
}

cell_t IsVisibleRecently(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->IsVisibleRecently();
}

cell_t GetTimeSinceBecameVisible(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetTimeSinceBecameVisible());
}

cell_t GetTimeWhenBecameVisible(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetTimeWhenBecameVisible());
}

cell_t GetTimeSinceLastSeen(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetTimeSinceLastSeen());
}

cell_t WasEverVisible(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->WasEverVisible();
}

cell_t IsObsolete(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->IsObsolete();
}

cell_t Is(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	CBaseEntity* baseentity = gamehelpers->ReferenceToEntity(params[2]);
	if (!baseentity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return entity->Is(baseentity);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CKnownEntity.Destroy", Destroy},
		{"CKnownEntity.UpdatePosition", UpdatePosition},
		{"CKnownEntity.GetEntity", GetEntity},
		{"CKnownEntity.GetLastKnownPosition", GetLastKnownPosition},
		{"CKnownEntity.HasLastKnownPositionBeenSeen", HasLastKnownPositionBeenSeen},
		{"CKnownEntity.MarkLastKnownPositionAsSeen", MarkLastKnownPositionAsSeen},
		{"CKnownEntity.GetLastKnownArea", GetLastKnownArea},
		{"CKnownEntity.GetTimeSinceLastKnown", GetTimeSinceLastKnown},
		{"CKnownEntity.GetTimeSinceBecameKnown", GetTimeSinceBecameKnown},
		{"CKnownEntity.UpdateVisibilityStatus", UpdateVisibilityStatus},
		{"CKnownEntity.IsVisibleInFOVNow", IsVisibleInFOVNow},
		{"CKnownEntity.IsVisibleRecently", IsVisibleRecently},
		{"CKnownEntity.GetTimeSinceBecameVisible", GetTimeSinceBecameVisible},
		{"CKnownEntity.GetTimeSinceLastSeen", GetTimeSinceLastSeen},
		{"CKnownEntity.WasEverVisible", WasEverVisible},
		{"CKnownEntity.IsObsolete", IsObsolete},
		{"CKnownEntity.Is", Is}
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}