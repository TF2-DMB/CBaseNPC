#include "body.hpp"

#include "NextBotBodyInterface.h"

namespace natives::nextbot::body {

inline IBody* Get(IPluginContext* context, const cell_t param) {
	IBody* body = (IBody*)PawnAddressToPtr(param, context);
	if (!body) {
		context->ThrowNativeError("Body ptr is null!");
		return nullptr;
	}
	return body;
}

cell_t SetPosition(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	cell_t *posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);
	return (body->SetPosition(pos) == true) ? 1 : 0;
}

cell_t GetEyePosition(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	cell_t *eyePosAddr;
	context->LocalToPhysAddr(params[2], &eyePosAddr);
	Vector eyepos = body->GetEyePosition();
	VectorToPawnVector(eyePosAddr, eyepos);
	return 0;
}

cell_t GetViewVector(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	cell_t *viewAddr;
	context->LocalToPhysAddr(params[2], &viewAddr);
	Vector view = body->GetViewVector();
	VectorToPawnVector(viewAddr, view);
	return 0;
}

cell_t IsHeadAimingOnTarget(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (body->IsHeadAimingOnTarget() == true) ? 1 : 0;
}

cell_t IsHeadSteady(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (cell_t)(body->IsHeadSteady());
}

cell_t GetHeadSteadyDuration(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return sp_ftoc(body->GetHeadSteadyDuration());
}

cell_t GetHeadAimSubjectLeadTime(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return sp_ftoc(body->GetHeadAimSubjectLeadTime());
}

cell_t GetHeadAimTrackingInterval(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return sp_ftoc(body->GetHeadAimTrackingInterval());
}

cell_t ClearPendingAimReply(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	body->ClearPendingAimReply();
	return 0;
}

cell_t GetMaxHeadAngularVelocity(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return sp_ftoc(body->GetMaxHeadAngularVelocity());
}

cell_t SetDesiredPosture(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	body->SetDesiredPosture((IBody::PostureType)params[2]);
	return 0;
}

cell_t GetDesiredPosture(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (body->GetDesiredPosture() == true) ? 1 : 0;
}

cell_t IsDesiredPosture(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (body->IsDesiredPosture((IBody::PostureType)params[2]) == true) ? 1 : 0;
}

cell_t IsInDesiredPosture(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (body->IsInDesiredPosture() == true) ? 1 : 0;
}

cell_t GetActualPosture(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (cell_t)body->GetActualPosture();
}

cell_t IsActualPosture(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (body->IsActualPosture((IBody::PostureType)params[2]) == true) ? 1 : 0;
}

cell_t IsPostureMobile(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (body->IsPostureMobile() == true) ? 1 : 0;
}

cell_t IsPostureChanging(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (body->IsPostureChanging() == true) ? 1 : 0;
}

cell_t SetArousal(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	body->SetArousal((IBody::ArousalType)params[2]);
	return 0;
}

cell_t IsArousal(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (body->IsArousal((IBody::ArousalType)params[2]) == true) ? 1 : 0;
}

cell_t GetArousal(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (cell_t)(body->GetArousal());
}

cell_t GetHullWidth(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return sp_ftoc(body->GetHullWidth());
}

cell_t GetHullHeight(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return sp_ftoc(body->GetHullHeight());
}

cell_t GetStandHullHeight(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return sp_ftoc(body->GetStandHullHeight());
}

cell_t GetCrouchHullHeight(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return sp_ftoc(body->GetCrouchHullHeight());
}

cell_t GetHullMins(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	cell_t *minsAddr;
	context->LocalToPhysAddr(params[2], &minsAddr);
	Vector mins = body->GetHullMins();
	VectorToPawnVector(minsAddr, mins);
	return 0;
}

cell_t GetHullMaxs(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	cell_t *maxsAddr;
	context->LocalToPhysAddr(params[2], &maxsAddr);
	Vector maxs = body->GetHullMaxs();
	VectorToPawnVector(maxsAddr, maxs);
	return 0;
}

cell_t GetSolidMask(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (cell_t)(body->GetSolidMask());
}

cell_t GetCollisionGroup(IPluginContext* context, const cell_t* params) {
	auto body = Get(context, params[1]);
	if (!body) {
		return 0;
	}

	return (cell_t)(body->GetCollisionGroup());
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"IBody.SetPosition", SetPosition},
		{"IBody.GetEyePosition", GetEyePosition},
		{"IBody.GetViewVector", GetViewVector},
		{"IBody.IsHeadAimingOnTarget", IsHeadAimingOnTarget},
		{"IBody.IsHeadSteady", IsHeadSteady},
		{"IBody.GetHeadSteadyDuration", GetHeadSteadyDuration},
		{"IBody.GetHeadAimSubjectLeadTime", GetHeadAimSubjectLeadTime},
		{"IBody.GetHeadAimTrackingInterval", GetHeadAimTrackingInterval},
		{"IBody.ClearPendingAimReply", ClearPendingAimReply},
		{"IBody.GetMaxHeadAngularVelocity", GetMaxHeadAngularVelocity},
		{"IBody.SetDesiredPosture", SetDesiredPosture},
		{"IBody.GetDesiredPosture", GetDesiredPosture},
		{"IBody.IsDesiredPosture", IsDesiredPosture},
		{"IBody.IsInDesiredPosture", IsInDesiredPosture},
		{"IBody.GetActualPosture", GetActualPosture},
		{"IBody.IsActualPosture", IsActualPosture},
		{"IBody.IsPostureMobile", IsPostureMobile},
		{"IBody.IsPostureChanging", IsPostureChanging},
		{"IBody.SetArousal", SetArousal},
		{"IBody.GetArousal", GetArousal},
		{"IBody.IsArousal", IsArousal},
		{"IBody.GetHullWidth", GetHullWidth},
		{"IBody.GetHullHeight", GetHullHeight},
		{"IBody.GetStandHullHeight", GetStandHullHeight},
		{"IBody.GetCrouchHullHeight", GetCrouchHullHeight},
		{"IBody.GetHullMins", GetHullMins},
		{"IBody.GetHullMaxs", GetHullMaxs},
		{"IBody.GetSolidMask", GetSolidMask},
		{"IBody.GetCollisionGroup", GetCollisionGroup},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}