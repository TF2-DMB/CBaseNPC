#ifndef NATIVE_CBASEANIMOVERLAY_H_
#define NATIVE_CBASEANIMOVERLAY_H_
#pragma once

#include "sourcesdk/baseanimatingoverlay.h"

#define CBASEOVERLAYNATIVE(name) \
	cell_t CBaseAnimatingOverlay_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CBaseAnimatingOverlayHack* anim = (CBaseAnimatingOverlayHack *)gamehelpers->ReferenceToEntity(params[1]); \
		if (!anim) \
		{ \
			return pContext->ThrowNativeError("Invalid entity %x", params[1]); \
		}

CBASEOVERLAYNATIVE(AddGestureSequence)
	return anim->AddGestureSequence(params[2], sp_ctof(params[3]), (params[4]) ? true : false);
};

CBASEOVERLAYNATIVE(AddGesture)
	return anim->AddGesture((Activity)params[2], sp_ctof(params[3]), (params[4]) ? true : false);
};

CBASEOVERLAYNATIVE(IsPlayingGesture)
	return (anim->IsPlayingGesture((Activity)params[2])) ? 1 : 0;
};

CBASEOVERLAYNATIVE(RestartGesture)
	anim->RestartGesture((Activity)params[2], (params[3]) ? true : false, (params[4]) ? true : false);
	return 0;
};

CBASEOVERLAYNATIVE(RemoveAllGestures)
	anim->RemoveAllGestures();
	return 0;
};

CBASEOVERLAYNATIVE(AddLayeredSequence)
	return anim->AddLayeredSequence(params[2], params[3]);
};

CBASEOVERLAYNATIVE(SetLayerPriority)
	anim->SetLayerPriority(params[2], params[3]);
	return 0;
};

CBASEOVERLAYNATIVE(IsValidLayer)
	return (anim->IsValidLayer(params[2])) ? 1 : 0;
};

CBASEOVERLAYNATIVE(SetLayerDuration)
	anim->SetLayerDuration(params[2], sp_ctof(params[3]));
	return 0;
};

CBASEOVERLAYNATIVE(GetLayerDuration)
	return sp_ftoc(anim->GetLayerDuration(params[2]));
};

CBASEOVERLAYNATIVE(SetLayerCycle)
	anim->SetLayerCycle(params[2], sp_ctof(params[3]));
	return 0;
};

CBASEOVERLAYNATIVE(GetLayerCycle)
	return sp_ftoc(anim->GetLayerCycle(params[2]));
};

CBASEOVERLAYNATIVE(SetLayerPlaybackRate)
	anim->SetLayerPlaybackRate(params[2], sp_ctof(params[3]));
	return 0;
};

CBASEOVERLAYNATIVE(SetLayerWeight)
	anim->SetLayerWeight(params[2], sp_ctof(params[3]));
	return 0;
};

CBASEOVERLAYNATIVE(GetLayerWeight)
	return sp_ftoc(anim->GetLayerWeight(params[2]));
};

CBASEOVERLAYNATIVE(SetLayerBlendIn)
	anim->SetLayerBlendIn(params[2], sp_ctof(params[3]));
	return 0;
};

CBASEOVERLAYNATIVE(SetLayerBlendOut)
	anim->SetLayerBlendOut(params[2], sp_ctof(params[3]));
	return 0;
};

CBASEOVERLAYNATIVE(SetLayerAutokill)
	anim->SetLayerAutokill(params[2], (params[3]) ? true : false);
	return 0;
};

CBASEOVERLAYNATIVE(SetLayerLooping)
	anim->SetLayerLooping(params[2], (params[3]) ? true : false);
	return 0;
};

CBASEOVERLAYNATIVE(SetLayerNoRestore)
	anim->SetLayerNoRestore(params[2], (params[3]) ? true : false);
	return 0;
};

CBASEOVERLAYNATIVE(GetLayerActivity)
	return anim->GetLayerActivity(params[2]);
};

CBASEOVERLAYNATIVE(GetLayerSequence)
	return anim->GetLayerSequence(params[2]);
};

CBASEOVERLAYNATIVE(FindGestureLayer)
	return anim->FindGestureLayer((Activity)params[2]);
};

CBASEOVERLAYNATIVE(RemoveLayer)
	anim->RemoveLayer(params[2], sp_ctof(params[3]), sp_ctof(params[4]));
	return 0;
};

CBASEOVERLAYNATIVE(FastRemoveLayer)
	anim->FastRemoveLayer(params[2]);
	return 0;
};

CBASEOVERLAYNATIVE(GetAnimOverlay)
	return (cell_t)anim->GetAnimOverlay(params[2]);
};

CBASEOVERLAYNATIVE(GetNumAnimOverlays)
	return anim->GetNumAnimOverlays();
};

CBASEOVERLAYNATIVE(SetNumAnimOverlays)
	anim->SetNumAnimOverlays(params[2]);
	return 0;
};

CBASEOVERLAYNATIVE(HasActiveLayer)
	return anim->HasActiveLayer();
};

#endif // NATIVE_CBASEANIMOVERLAY_H_
