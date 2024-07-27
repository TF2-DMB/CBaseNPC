#include "vision.hpp"
#include "knownentity.hpp"

#include "NextBotVisionInterface.h"

namespace natives::nextbot::vision  {

inline IVision* Get(IPluginContext* context, const cell_t param) {
	IVision* vision = (IVision*)PawnAddressToPtr(param, context);
	if (!vision) {
		context->ThrowNativeError("Vision ptr is null!");
		return nullptr;
	}
	return vision;
}

cell_t GetPrimaryKnownThreat(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}

	return PtrToPawnAddress(vision->GetPrimaryKnownThreat((params[2]) ? true : false), context);
}

cell_t GetTimeSinceVisible(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}

	return sp_ftoc(vision->GetTimeSinceVisible(params[2]));
}

cell_t GetClosestKnown(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}

	return PtrToPawnAddress(vision->GetClosestKnown(params[2]), context);
}

cell_t GetKnownCount(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}

	return vision->GetKnownCount(params[2], (params[3]) ? true : false, sp_ctof(params[4]));
}

cell_t GetKnown(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return PtrToPawnAddress(vision->GetKnown(entity), context);
}

cell_t AddKnownEntity(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	vision->AddKnownEntity(entity);
	return 0;
}

cell_t ForgetEntity(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	vision->ForgetEntity(entity);
	return 0;
}

cell_t ForgetAllKnownEntities(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	vision->ForgetAllKnownEntities();
	return 0;
}

cell_t GetMaxVisionRange(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	return sp_ftoc(vision->GetMaxVisionRange());
}

cell_t GetMinRecognizeTime(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	return sp_ftoc(vision->GetMinRecognizeTime());
}

cell_t IsAbleToSee(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}

	cell_t* pos;
	context->LocalToPhysAddr(params[2], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	
	return vision->IsAbleToSee(vPos, (IVision::FieldOfViewCheckType)params[3]);
}

cell_t IsAbleToSeeTarget(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	cell_t *pos;
	context->LocalToPhysAddr(params[4], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	
	bool bNULL = (context->GetNullRef(SP_NULL_VECTOR) == pos);
	
	bool bClear = vision->IsAbleToSee(entity, (IVision::FieldOfViewCheckType)params[3], (bNULL) ? NULL : &vPos);
	if (!bNULL)
		VectorToPawnVector(pos, vPos);
	
	return bClear;
}

cell_t IsIgnored(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return vision->IsIgnored(entity);
}

cell_t IsVisibleEntityNoticed(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	return vision->IsVisibleEntityNoticed(entity);
}

cell_t IsInFieldOfView(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	cell_t *pos;
	context->LocalToPhysAddr(params[2], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	
	return vision->IsInFieldOfView(vPos);
}

cell_t IsInFieldOfViewTarget(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	return vision->IsInFieldOfView(entity);
}

cell_t GetDefaultFieldOfView(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	return sp_ftoc(vision->GetDefaultFieldOfView());
}

cell_t GetFieldOfView(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	return sp_ftoc(vision->GetFieldOfView());
}

cell_t SetFieldOfView(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	vision->SetFieldOfView(sp_ctof(params[2]));
	return 0;
}

cell_t IsLineOfSightClear(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	cell_t *pos;
	context->LocalToPhysAddr(params[2], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	return vision->IsLineOfSightClear(vPos);
}

cell_t IsLineOfSightClearToEntity(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	cell_t *pos;
	context->LocalToPhysAddr(params[3], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	
	bool bNULL = (context->GetNullRef(SP_NULL_VECTOR) == pos);
	
	bool bClear = vision->IsLineOfSightClearToEntity(entity, (bNULL) ? NULL : &vPos);
	if (!bNULL)
		VectorToPawnVector(pos, vPos);
	return bClear;
}

cell_t IsLookingAtTarget(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}

	auto cbb = entity->MyCombatCharacterPointer();
	if (!cbb) {
		return context->ThrowNativeError("Entity isn't a CBaseCombatCharacter!");
	}
	
	return (vision->IsLookingAt(const_cast<CBaseCombatCharacter*>(cbb), sp_ctof(params[3])) == true) ? 1 : 0;
}

cell_t IsLookingAt(IPluginContext* context, const cell_t* params) {
	auto vision = Get(context, params[1]);
	if (!vision) {
		return 0;
	}
	
	cell_t *pos;
	context->LocalToPhysAddr(params[2], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);

	return vision->IsLookingAt(vPos, sp_ctof(params[3]));
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	knownentity::setup(natives);

	sp_nativeinfo_t list[] = {
		{"IVision.GetPrimaryKnownThreat", GetPrimaryKnownThreat},
		{"IVision.GetTimeSinceVisible", GetTimeSinceVisible},
		{"IVision.GetClosestKnown", GetClosestKnown},
		{"IVision.GetKnownCount", GetKnownCount},
		{"IVision.GetKnown", GetKnown},
		{"IVision.AddKnownEntity", AddKnownEntity},
		{"IVision.ForgetEntity", ForgetEntity},
		{"IVision.ForgetAllKnownEntities", ForgetAllKnownEntities},
		{"IVision.GetMaxVisionRange", GetMaxVisionRange},
		{"IVision.GetMinRecognizeTime", GetMinRecognizeTime},
		{"IVision.IsAbleToSee", IsAbleToSee},
		{"IVision.IsAbleToSeeTarget", IsAbleToSeeTarget},
		{"IVision.IsIgnored", IsIgnored},
		{"IVision.IsVisibleEntityNoticed", IsVisibleEntityNoticed},
		{"IVision.IsInFieldOfView", IsInFieldOfView},
		{"IVision.IsInFieldOfViewTarget", IsInFieldOfViewTarget},
		{"IVision.GetDefaultFieldOfView", GetDefaultFieldOfView},
		{"IVision.GetFieldOfView", GetFieldOfView},
		{"IVision.SetFieldOfView", SetFieldOfView},
		{"IVision.IsLineOfSightClear", IsLineOfSightClear},
		{"IVision.IsLineOfSightClearToEntity", IsLineOfSightClearToEntity},
		{"IVision.IsLookingAt", IsLookingAt},
		{"IVision.IsLookingAtTarget", IsLookingAtTarget},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}