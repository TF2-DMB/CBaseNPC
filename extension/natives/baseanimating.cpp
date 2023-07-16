#include "sourcesdk/baseanimating.h"
#include "baseanimating.hpp"

namespace natives::baseanimating {

inline CBaseAnimating* Get(IPluginContext* context, const cell_t param) {	
	CBaseAnimating* entity = (CBaseAnimating*)gamehelpers->ReferenceToEntity(param);
	if (!entity) {
		context->ThrowNativeError("Invalid entity %d", param);
		return nullptr;
	}
	return entity;
}

cell_t OffsetHandleAnimEvent(IPluginContext* context, const cell_t* params) {
	return CBaseAnimating::offset_HandleAnimEvent;
}

cell_t LookupAttachment(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* name = nullptr;
	context->LocalToString(params[2], &name);
	return entity->LookupAttachment(name);
}

cell_t GetAttachment(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	Vector origin;
	QAngle angles;
	cell_t* originAdd, *anglesAdd;
	context->LocalToPhysAddr(params[3], &originAdd);
	context->LocalToPhysAddr(params[4], &anglesAdd);

	cell_t result = entity->GetAttachment(params[2], origin, angles) ? 1 : 0;
	VectorToPawnVector(originAdd, origin);
	VectorToPawnVector(anglesAdd, angles);
	return result;
}

cell_t GetAttachmentMatrix(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	matrix3x4_t matrix;
	int attachment = params[2];
	cell_t * pawnMat;
	context->LocalToPhysAddr(params[3], &pawnMat);

	cell_t result = entity->GetAttachment(attachment, matrix);
	if (result) {
		MatrixToPawnMatrix(context, pawnMat, matrix);
	}

	return result;
}

cell_t StudioFrameAdvance(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->StudioFrameAdvance();
	return 0;
}

cell_t DispatchAnimEvents(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	CBaseAnimating* anim2 = (CBaseAnimating*)gamehelpers->ReferenceToEntity(params[2]);
	if (!anim2) {
		return context->ThrowNativeError("Invalid entity %d", params[2]);
	}
	entity->DispatchAnimEvents(anim2);
	return 0;
}

cell_t LookupSequence(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* name = nullptr;
	context->LocalToString(params[2], &name);
	return entity->LookupSequence(name);
}

cell_t SelectWeightedSequence(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->SelectWeightedSequence((Activity)params[2]);
}

cell_t ResetSequence(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->ResetSequence(params[2]);
	return 0;
}

cell_t SequenceDuration(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->SequenceDuration(params[2]));
}

cell_t GetModelPtr(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return PtrToPawnAddress(entity->GetModelPtr());
}

cell_t LookupPoseParameter(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* name = nullptr;
	context->LocalToString(params[2], &name);
	return entity->LookupPoseParameter(name);
}

cell_t SetPoseParameter(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->SetPoseParameter(params[2], sp_ctof(params[3])));
}

cell_t GetPoseParameter(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetPoseParameter(params[2]));
}

void setup(std::vector<sp_nativeinfo_t>& natives) {	
	sp_nativeinfo_t list[] = {
		{"CBaseAnimating.OffsetHandleAnimEvent.get", OffsetHandleAnimEvent},
		{"CBaseAnimating.LookupAttachment", LookupAttachment},
		{"CBaseAnimating.GetAttachment", GetAttachment},
		{"CBaseAnimating.GetAttachmentMatrix", GetAttachmentMatrix},
		{"CBaseAnimating.StudioFrameAdvance", StudioFrameAdvance},
		{"CBaseAnimating.DispatchAnimEvents", DispatchAnimEvents},
		{"CBaseAnimating.LookupSequence", LookupSequence},
		{"CBaseAnimating.SelectWeightedSequence", SelectWeightedSequence},
		{"CBaseAnimating.ResetSequence", ResetSequence},
		{"CBaseAnimating.SequenceDuration", SequenceDuration},
		{"CBaseAnimating.GetModelPtr", GetModelPtr},
		{"CBaseAnimating.LookupPoseParameter", LookupPoseParameter},
		{"CBaseAnimating.SetPoseParameter", SetPoseParameter},
		{"CBaseAnimating.GetPoseParameter", GetPoseParameter},
		
		// To-Do: remove this in 2.0.0
		{"CBaseAnimating.iHandleAnimEvent", OffsetHandleAnimEvent}
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}