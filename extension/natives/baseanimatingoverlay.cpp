#include "sourcesdk/baseanimatingoverlay.h"
#include "baseanimatingoverlay.hpp"

namespace natives::baseanimatingoverlay {

namespace layer {

inline CAnimationLayer* Get(IPluginContext* context, const cell_t param) {	
	CAnimationLayer* layer = (CAnimationLayer*)PawnAddressToPtr(param, context);
	if (!layer) {
		context->ThrowNativeError("Layer is a null ptr!");
		return nullptr;
	}

	return layer;
}

cell_t Getm_fFlags(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return layer->m_fFlags;
}

cell_t Setm_fFlags(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_fFlags = params[2];
	return 0;
}

cell_t Getm_bSequenceFinished(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return layer->m_bSequenceFinished;
}

cell_t Setm_bSequenceFinished(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_bSequenceFinished = (params[2] != 0);
	return 0;
}

cell_t Getm_bLooping(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return layer->m_bLooping;
}

cell_t Setm_bLooping(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_bLooping = (params[2] != 0);
	return 0;
}

cell_t Getm_nSequence(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return layer->m_nSequence;
}

cell_t Setm_nSequence(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_nSequence = params[2];
	return 0;
}

cell_t Getm_flCycle(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flCycle);
}

cell_t Setm_flCycle(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flCycle = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flPrevCycle(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}
	
	return sp_ftoc(layer->m_flPrevCycle);
}

cell_t Setm_flPrevCycle(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flPrevCycle = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flWeight(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flWeight);
}

cell_t Setm_flWeight(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flWeight = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flPlaybackRate(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flPlaybackRate);
}

cell_t Setm_flPlaybackRate(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flPlaybackRate = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flBlendIn(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flBlendIn);
}

cell_t Setm_flBlendIn(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flBlendIn = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flBlendOut(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flBlendOut);
}

cell_t Setm_flBlendOut(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flBlendOut = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flKillRate(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flKillRate);
}

cell_t Setm_flKillRate(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flKillRate = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flKillDelay(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flKillDelay);
}

cell_t Setm_flKillDelay(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flKillDelay = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flLayerAnimtime(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flLayerAnimtime);
}

cell_t Setm_flLayerAnimtime(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flLayerAnimtime = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flLayerFadeOuttime(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flLayerFadeOuttime);
}

cell_t Setm_flLayerFadeOuttime(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flLayerFadeOuttime = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_nActivity(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return layer->m_nActivity;
}

cell_t Setm_nActivity(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_nActivity = (Activity)params[2];
	return 0;
}

cell_t Getm_nPriority(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return layer->m_nPriority;
}

cell_t Setm_nPriority(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_nPriority = params[2];
	return 0;
}

cell_t Getm_nOrder(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return layer->m_nOrder;
}

cell_t Setm_nOrder(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_nOrder = params[2];
	return 0;
}

cell_t Getm_flLastEventCheck(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flLastEventCheck);
}

cell_t Setm_flLastEventCheck(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flLastEventCheck = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_flLastAccess(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return sp_ftoc(layer->m_flLastAccess);
}

cell_t Setm_flLastAccess(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_flLastAccess = sp_ctof(params[2]);
	return 0;
}

cell_t Getm_pOwnerEntity(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	return gamehelpers->EntityToBCompatRef(layer->m_pOwnerEntity);
}

cell_t Setm_pOwnerEntity(IPluginContext* context, const cell_t* params) {
	auto layer = Get(context, params[1]);
	if (!layer) {
		return 0;
	}

	layer->m_pOwnerEntity = reinterpret_cast<CBaseAnimatingOverlay*>(gamehelpers->ReferenceToEntity(params[2]));
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CAnimationLayer.m_fFlags.get", Getm_fFlags},
		{"CAnimationLayer.m_fFlags.set", Setm_fFlags},
		{"CAnimationLayer.m_bSequenceFinished.get", Getm_bSequenceFinished},
		{"CAnimationLayer.m_bSequenceFinished.set", Setm_bSequenceFinished},
		{"CAnimationLayer.m_bLooping.get", Getm_bLooping},
		{"CAnimationLayer.m_bLooping.set", Setm_bLooping},
		{"CAnimationLayer.m_nSequence.get", Getm_nSequence},
		{"CAnimationLayer.m_nSequence.set", Setm_nSequence},
		{"CAnimationLayer.m_flCycle.get", Getm_flCycle},
		{"CAnimationLayer.m_flCycle.set", Setm_flCycle},
		{"CAnimationLayer.m_flPrevCycle.get", Getm_flPrevCycle},
		{"CAnimationLayer.m_flPrevCycle.set", Setm_flPrevCycle},
		{"CAnimationLayer.m_flWeight.get", Getm_flWeight},
		{"CAnimationLayer.m_flWeight.set", Setm_flWeight},
		{"CAnimationLayer.m_flPlaybackRate.get", Getm_flPlaybackRate},
		{"CAnimationLayer.m_flPlaybackRate.set", Setm_flPlaybackRate},
		{"CAnimationLayer.m_flBlendIn.get", Getm_flBlendIn},
		{"CAnimationLayer.m_flBlendIn.set", Setm_flBlendIn},
		{"CAnimationLayer.m_flBlendOut.get", Getm_flBlendOut},
		{"CAnimationLayer.m_flBlendOut.set", Setm_flBlendOut},
		{"CAnimationLayer.m_flKillRate.get", Getm_flKillRate},
		{"CAnimationLayer.m_flKillRate.set", Setm_flKillRate},
		{"CAnimationLayer.m_flKillDelay.get", Getm_flKillDelay},
		{"CAnimationLayer.m_flKillDelay.set", Setm_flKillDelay},
		{"CAnimationLayer.m_flLayerAnimtime.get", Getm_flLayerAnimtime},
		{"CAnimationLayer.m_flLayerAnimtime.set", Setm_flLayerAnimtime},
		{"CAnimationLayer.m_flLayerFadeOuttime.get", Getm_flLayerFadeOuttime},
		{"CAnimationLayer.m_flLayerFadeOuttime.set", Setm_flLayerFadeOuttime},
		{"CAnimationLayer.m_nActivity.get", Getm_nActivity},
		{"CAnimationLayer.m_nActivity.set", Setm_nActivity},
		{"CAnimationLayer.m_nPriority.get", Getm_nPriority},
		{"CAnimationLayer.m_nPriority.set", Setm_nPriority},
		{"CAnimationLayer.m_nOrder.get", Getm_nOrder},
		{"CAnimationLayer.m_nOrder.set", Setm_nOrder},
		{"CAnimationLayer.m_flLastEventCheck.get", Getm_flLastEventCheck},
		{"CAnimationLayer.m_flLastEventCheck.set", Setm_flLastEventCheck},
		{"CAnimationLayer.m_flLastAccess.get", Getm_flLastAccess},
		{"CAnimationLayer.m_flLastAccess.set", Setm_flLastAccess},
		{"CAnimationLayer.m_pOwnerEntity.get", Getm_pOwnerEntity},
		{"CAnimationLayer.m_pOwnerEntity.set", Setm_pOwnerEntity},
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}

CBaseAnimatingOverlay* Get(IPluginContext* context, const cell_t param) {
	CBaseAnimatingOverlay* entity = (CBaseAnimatingOverlay*)gamehelpers->ReferenceToEntity(param);
	if (!entity) {
		context->ThrowNativeError("Invalid entity %d", param);
		return nullptr;
	}
	return entity;
}

cell_t AddGestureSequence(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->AddGestureSequence(params[2], sp_ctof(params[3]), (params[4]) ? true : false);
}

cell_t AddGesture(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->AddGesture((Activity)params[2], sp_ctof(params[3]), (params[4]) ? true : false);
}

cell_t IsPlayingGesture(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return (entity->IsPlayingGesture((Activity)params[2])) ? 1 : 0;
}

cell_t RestartGesture(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->RestartGesture((Activity)params[2], (params[3]) ? true : false, (params[4]) ? true : false);
	return 0;
}

cell_t RemoveAllGestures(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->RemoveAllGestures();
	return 0;
}

cell_t AddLayeredSequence(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->AddLayeredSequence(params[2], params[3]);
}

cell_t SetLayerPriority(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerPriority(params[2], params[3]);
	return 0;
}

cell_t IsValidLayer(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return (entity->IsValidLayer(params[2])) ? 1 : 0;
}

cell_t SetLayerDuration(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerDuration(params[2], sp_ctof(params[3]));
	return 0;
}

cell_t GetLayerDuration(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetLayerDuration(params[2]));
}

cell_t SetLayerCycle(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerCycle(params[2], sp_ctof(params[3]));
	return 0;
}

cell_t GetLayerCycle(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetLayerCycle(params[2]));
}

cell_t SetLayerPlaybackRate(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerPlaybackRate(params[2], sp_ctof(params[3]));
	return 0;
}

cell_t SetLayerWeight(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerWeight(params[2], sp_ctof(params[3]));
	return 0;
}

cell_t GetLayerWeight(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetLayerWeight(params[2]));
}

cell_t SetLayerBlendIn(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerBlendIn(params[2], sp_ctof(params[3]));
	return 0;
};

cell_t SetLayerBlendOut(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerBlendOut(params[2], sp_ctof(params[3]));
	return 0;
};

cell_t SetLayerAutokill(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerAutokill(params[2], (params[3]) ? true : false);
	return 0;
};

cell_t SetLayerLooping(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerLooping(params[2], (params[3]) ? true : false);
	return 0;
};

cell_t SetLayerNoRestore(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetLayerNoRestore(params[2], (params[3]) ? true : false);
	return 0;
};

cell_t GetLayerActivity(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->GetLayerActivity(params[2]);
};

cell_t GetLayerSequence(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->GetLayerSequence(params[2]);
};

cell_t FindGestureLayer(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->FindGestureLayer((Activity)params[2]);
};

cell_t RemoveLayer(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->RemoveLayer(params[2], sp_ctof(params[3]), sp_ctof(params[4]));
	return 0;
};

cell_t FastRemoveLayer(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->FastRemoveLayer(params[2]);
	return 0;
};

cell_t GetAnimOverlay(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return PtrToPawnAddress(entity->GetAnimOverlay(params[2]), context);
};

cell_t GetNumAnimOverlays(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->GetNumAnimOverlays();
};

cell_t SetNumAnimOverlays(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetNumAnimOverlays(params[2]);
	return 0;
};

cell_t HasActiveLayer(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->HasActiveLayer();
};

void setup(std::vector<sp_nativeinfo_t>& natives) {
	layer::setup(natives);
	
	sp_nativeinfo_t list[] = {
		{"CBaseAnimatingOverlay.AddGestureSequence", AddGestureSequence},
		{"CBaseAnimatingOverlay.AddGesture", AddGesture},
		{"CBaseAnimatingOverlay.IsPlayingGesture", IsPlayingGesture},
		{"CBaseAnimatingOverlay.RestartGesture", RestartGesture},
		{"CBaseAnimatingOverlay.RemoveAllGestures", RemoveAllGestures},
		{"CBaseAnimatingOverlay.AddLayeredSequence", AddLayeredSequence},
		{"CBaseAnimatingOverlay.SetLayerPriority", SetLayerPriority},
		{"CBaseAnimatingOverlay.IsValidLayer", IsValidLayer},
		{"CBaseAnimatingOverlay.SetLayerDuration", SetLayerDuration},
		{"CBaseAnimatingOverlay.GetLayerDuration", GetLayerDuration},
		{"CBaseAnimatingOverlay.SetLayerCycle", SetLayerCycle},
		{"CBaseAnimatingOverlay.GetLayerCycle", GetLayerCycle},
		{"CBaseAnimatingOverlay.SetLayerPlaybackRate", SetLayerPlaybackRate},
		{"CBaseAnimatingOverlay.SetLayerWeight", SetLayerWeight},
		{"CBaseAnimatingOverlay.GetLayerWeight", GetLayerWeight},
		{"CBaseAnimatingOverlay.SetLayerBlendIn", SetLayerBlendIn},
		{"CBaseAnimatingOverlay.SetLayerBlendOut", SetLayerBlendOut},
		{"CBaseAnimatingOverlay.SetLayerAutokill", SetLayerAutokill},
		{"CBaseAnimatingOverlay.SetLayerLooping", SetLayerLooping},
		{"CBaseAnimatingOverlay.SetLayerNoRestore", SetLayerNoRestore},
		{"CBaseAnimatingOverlay.GetLayerActivity", GetLayerActivity},
		{"CBaseAnimatingOverlay.GetLayerSequence", GetLayerSequence},
		{"CBaseAnimatingOverlay.FindGestureLayer", FindGestureLayer},
		{"CBaseAnimatingOverlay.RemoveLayer", RemoveLayer},
		{"CBaseAnimatingOverlay.FastRemoveLayer", FastRemoveLayer},
		{"CBaseAnimatingOverlay.GetAnimOverlay", GetAnimOverlay},
		{"CBaseAnimatingOverlay.GetNumAnimOverlays", GetNumAnimOverlays},
		{"CBaseAnimatingOverlay.SetNumAnimOverlays", SetNumAnimOverlays},
		{"CBaseAnimatingOverlay.HasActiveLayer", HasActiveLayer},
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}