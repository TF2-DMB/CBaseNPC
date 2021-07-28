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

// Deprecated
CBASEANIMNATIVE(FindAttachment)
	char* name = nullptr;
	pContext->LocalToString(params[2], &name);
	return anim->LookupAttachment(name) - 1;
}

#endif // NATIVE_CBASEANIM_H_