#ifndef NATIVE_CBASEANIM_H_
#define NATIVE_CBASEANIM_H_
#pragma once

#include "sourcesdk/baseanimating.h"

#define CBASEANIMNATIVE(name) \
	cell_t CBaseAnimating_##name(IPluginContext* pContext, const cell_t *params) \
	{ \
		CBaseAnimatingHack* anim = (CBaseAnimatingHack* )gamehelpers->ReferenceToEntity(params[1]); \
		if (!anim) \
		{ \
			return pContext->ThrowNativeError("Invalid entity %x", params[1]); \
		}

#define CANIMLAYERNATIVE(name) \
	cell_t CAnimationLayer_##name(IPluginContext* pContext, const cell_t *params) \
	{ \
		CAnimationLayer *pLayer = (CAnimationLayer*)params[1]; \
		if ( !pLayer ) return pContext->ThrowNativeError("Invalid CAnimationLayer pointer");

cell_t CBaseAnimating_iHandleAnimEvent(IPluginContext * pContext, const cell_t * params)
{
	return  CBaseAnimatingHack::offset_HandleAnimEvent;
}

CBASEANIMNATIVE(LookupAttachment)
	char* name = nullptr;
	pContext->LocalToString(params[2], &name);
	return anim->LookupAttachment(name);
}

CBASEANIMNATIVE(GetAttachment)
	Vector origin;
	QAngle angles;
	cell_t* originAdd, * anglesAdd;
	pContext->LocalToPhysAddr(params[3], &originAdd);
	pContext->LocalToPhysAddr(params[4], &anglesAdd);

	cell_t result = anim->GetAttachment(params[2], origin, angles) ? 1 : 0;
	VectorToPawnVector(originAdd, origin);
	VectorToPawnVector(anglesAdd, angles);
	return result;
}

CBASEANIMNATIVE(GetAttachmentMatrix)
	matrix3x4_t matrix;

	int iAttachment = params[2];

	cell_t * pawnMat;
	pContext->LocalToPhysAddr( params[3], &pawnMat );

	cell_t result = anim->GetAttachment( iAttachment, matrix );
	if ( result )
	{
		MatrixToPawnMatrix( pawnMat, matrix );
	}

	return result;
}

CBASEANIMNATIVE(StudioFrameAdvance)
	anim->StudioFrameAdvance();
	return 0;
}

CBASEANIMNATIVE(DispatchAnimEvents)

	CBaseAnimatingHack* anim2 = (CBaseAnimatingHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!anim2)
	{
		return pContext->ThrowNativeError("Invalid entity %x", params[2]);
	}
	anim->DispatchAnimEvents(anim2);
	return 0;
}

CBASEANIMNATIVE(LookupSequence)
	char* name = nullptr;
	pContext->LocalToString(params[2], &name);
	return anim->LookupSequence(name);
}

CBASEANIMNATIVE(SelectWeightedSequence)
	return anim->SelectWeightedSequence((Activity)params[2]);
}

CBASEANIMNATIVE(ResetSequence)
	anim->ResetSequence(params[2]);
	return 0;
}

CBASEANIMNATIVE(SequenceDuration)
	return sp_ftoc(anim->SequenceDuration(params[2]));
}

CBASEANIMNATIVE(GetModelPtr)
	return (cell_t)anim->GetModelPtr();
}

CBASEANIMNATIVE(LookupPoseParameter)
	char* name = nullptr;
	pContext->LocalToString(params[2], &name);
	return anim->LookupPoseParameter(name);
}

CBASEANIMNATIVE(SetPoseParameter);
	return sp_ftoc(anim->SetPoseParameter(params[2], sp_ctof(params[3])));
}

CBASEANIMNATIVE(GetPoseParameter);
	return sp_ftoc(anim->GetPoseParameter(params[2]));
}

CANIMLAYERNATIVE(m_fFlagsGet)
	return pLayer->m_fFlags;
}

CANIMLAYERNATIVE(m_fFlagsSet)
	pLayer->m_fFlags = params[2];
	return 0;
}

CANIMLAYERNATIVE(m_bSequenceFinishedGet)
	return pLayer->m_bSequenceFinished;
}

CANIMLAYERNATIVE(m_bSequenceFinishedSet)
	pLayer->m_bSequenceFinished = (params[2] != 0);
	return 0;
}

CANIMLAYERNATIVE(m_bLoopingGet)
	return pLayer->m_bLooping;
}

CANIMLAYERNATIVE(m_bLoopingSet)
	pLayer->m_bLooping = (params[2] != 0);
	return 0;
}

CANIMLAYERNATIVE(m_nSequenceGet)
	return pLayer->m_nSequence;
}

CANIMLAYERNATIVE(m_nSequenceSet)
	pLayer->m_nSequence = params[2];
	return 0;
}

CANIMLAYERNATIVE(m_flCycleGet)
	return sp_ftoc(pLayer->m_flCycle);
}

CANIMLAYERNATIVE(m_flCycleSet)
	pLayer->m_flCycle = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flPrevCycleGet)
	return sp_ftoc(pLayer->m_flPrevCycle);
}

CANIMLAYERNATIVE(m_flPrevCycleSet)
	pLayer->m_flPrevCycle = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flWeightGet)
	return sp_ftoc(pLayer->m_flWeight);
}

CANIMLAYERNATIVE(m_flWeightSet)
	pLayer->m_flWeight = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flPlaybackRateGet)
	return sp_ftoc(pLayer->m_flPlaybackRate);
}

CANIMLAYERNATIVE(m_flPlaybackRateSet)
	pLayer->m_flPlaybackRate = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flBlendInGet)
	return sp_ftoc(pLayer->m_flBlendIn);
}

CANIMLAYERNATIVE(m_flBlendInSet)
	pLayer->m_flBlendIn = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flBlendOutGet)
	return sp_ftoc(pLayer->m_flBlendOut);
}

CANIMLAYERNATIVE(m_flBlendOutSet)
	pLayer->m_flBlendOut = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flKillRateGet)
	return sp_ftoc(pLayer->m_flKillRate);
}

CANIMLAYERNATIVE(m_flKillRateSet)
	pLayer->m_flKillRate = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flKillDelayGet)
	return sp_ftoc(pLayer->m_flKillDelay);
}

CANIMLAYERNATIVE(m_flKillDelaySet)
	pLayer->m_flKillDelay = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flLayerAnimtimeGet)
	return sp_ftoc(pLayer->m_flLayerAnimtime);
}

CANIMLAYERNATIVE(m_flLayerAnimtimeSet)
	pLayer->m_flLayerAnimtime = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flLayerFadeOuttimeGet)
	return sp_ftoc(pLayer->m_flLayerFadeOuttime);
}

CANIMLAYERNATIVE(m_flLayerFadeOuttimeSet)
	pLayer->m_flLayerFadeOuttime = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_nActivityGet)
	return pLayer->m_nActivity;
}

CANIMLAYERNATIVE(m_nActivitySet)
	pLayer->m_nActivity = (Activity)params[2];
	return 0;
}

CANIMLAYERNATIVE(m_nPriorityGet)
	return pLayer->m_nPriority;
}

CANIMLAYERNATIVE(m_nPrioritySet)
	pLayer->m_nPriority = params[2];
	return 0;
}

CANIMLAYERNATIVE(m_nOrderGet)
	return pLayer->m_nOrder;
}

CANIMLAYERNATIVE(m_nOrderSet)
	pLayer->m_nOrder = params[2];
	return 0;
}

CANIMLAYERNATIVE(m_flLastEventCheckGet)
	return sp_ftoc(pLayer->m_flLastEventCheck);
}

CANIMLAYERNATIVE(m_flLastEventCheckSet)
	pLayer->m_flLastEventCheck = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_flLastAccessGet)
	return sp_ftoc(pLayer->m_flLastAccess);
}

CANIMLAYERNATIVE(m_flLastAccessSet)
	pLayer->m_flLastAccess = sp_ctof(params[2]);
	return 0;
}

CANIMLAYERNATIVE(m_pOwnerEntityGet)
	return gamehelpers->EntityToBCompatRef(pLayer->m_pOwnerEntity);
}

CANIMLAYERNATIVE(m_pOwnerEntitySet)
	pLayer->m_pOwnerEntity = reinterpret_cast<CBaseAnimatingOverlayHack*>(gamehelpers->ReferenceToEntity(params[2]));
	return 0;
}

#endif // NATIVE_CBASEANIM_H_