#ifndef _NATIVES_VISION_H_INCLUDED_
#define _NATIVES_VISION_H_INCLUDED_

#pragma once

#include "NextBotVisionInterface.h"
#include "NextBotKnownEntity.h"

#define KNOWNNATIVE(name) \
	cell_t CKnownEntity_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CKnownEntity *pKnown = (CKnownEntity *)(params[1]); \
		if(!pKnown) { \
			return pContext->ThrowNativeError("Invalid CKnownEntity %x", params[1]); \
		} \



KNOWNNATIVE(Destroy)
	pKnown->Destroy();
	return 0;
}

KNOWNNATIVE(UpdatePosition)
	pKnown->UpdatePosition();
	return 0;
}

KNOWNNATIVE(GetEntity)
	CBaseEntity *pEntity = pKnown->GetEntity();
	return gamehelpers->EntityToBCompatRef(pEntity);
}

KNOWNNATIVE(GetLastKnownPosition)
	Vector vPos = pKnown->GetLastKnownPosition();
	
	cell_t *pos;
	pContext->LocalToPhysAddr(params[2], &pos);
	VectorToPawnVector(pos, vPos);
	return 0;
}
KNOWNNATIVE(HasLastKnownPositionBeenSeen)
	return pKnown->HasLastKnownPositionBeenSeen();
}
KNOWNNATIVE(MarkLastKnownPositionAsSeen)
	pKnown->MarkLastKnownPositionAsSeen();
	return 0;
}
KNOWNNATIVE(GetLastKnownArea)
	return (cell_t)pKnown->GetLastKnownArea();
}
KNOWNNATIVE(GetTimeSinceLastKnown)
	return sp_ftoc(pKnown->GetTimeSinceLastKnown());
}
KNOWNNATIVE(GetTimeSinceBecameKnown)
	return sp_ftoc(pKnown->GetTimeSinceBecameKnown());
}
KNOWNNATIVE(UpdateVisibilityStatus)
	pKnown->UpdateVisibilityStatus((params[2]) ? true : false);
	return 0;
}
KNOWNNATIVE(IsVisibleInFOVNow)
	return pKnown->IsVisibleInFOVNow();
}
KNOWNNATIVE(IsVisibleRecently)
	return pKnown->IsVisibleRecently();
}
KNOWNNATIVE(GetTimeSinceBecameVisible)
	return sp_ftoc(pKnown->GetTimeSinceBecameVisible());
}
KNOWNNATIVE(GetTimeWhenBecameVisible)
	return sp_ftoc(pKnown->GetTimeWhenBecameVisible());
}
KNOWNNATIVE(GetTimeSinceLastSeen)
	return sp_ftoc(pKnown->GetTimeSinceLastSeen());
}
KNOWNNATIVE(WasEverVisible)
	return pKnown->WasEverVisible();
}
KNOWNNATIVE(IsObsolete)
	return pKnown->IsObsolete();
}
KNOWNNATIVE(Is)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return pKnown->Is(pEntity);
}

#define VISIONNATIVE(name) \
	cell_t IVision_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		IVision *pVision = (IVision *)(params[1]); \
		if(!pVision) { \
			return pContext->ThrowNativeError("Invalid IVision %x", params[1]); \
		} \



VISIONNATIVE(GetPrimaryKnownThreat)
	return (cell_t)pVision->GetPrimaryKnownThreat((params[2]) ? true : false);
}

VISIONNATIVE(GetTimeSinceVisible)
	return sp_ftoc(pVision->GetTimeSinceVisible(params[2]));
}

VISIONNATIVE(GetClosestKnown)
	return (cell_t)pVision->GetClosestKnown(params[2]);
}

VISIONNATIVE(GetKnownCount)
	return pVision->GetKnownCount(params[2], (params[3]) ? true : false, sp_ctof(params[4]));
}

VISIONNATIVE(GetKnown)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)pVision->GetKnown(pEntity);
}

VISIONNATIVE(AddKnownEntity)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if (!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	pVision->AddKnownEntity(pEntity);
	return 0;
}

VISIONNATIVE(ForgetEntity)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	pVision->ForgetEntity(pEntity);
	return 0;
}

VISIONNATIVE(ForgetAllKnownEntities)
	pVision->ForgetAllKnownEntities();
	return 0;
}

VISIONNATIVE(GetMaxVisionRange)
	return sp_ftoc(pVision->GetMaxVisionRange());
}

VISIONNATIVE(GetMinRecognizeTime)
	return sp_ftoc(pVision->GetMinRecognizeTime());
}

VISIONNATIVE(IsAbleToSee)
	cell_t *pos;
	pContext->LocalToPhysAddr(params[2], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	
	return pVision->IsAbleToSee(vPos, (IVision::FieldOfViewCheckType)params[3]);
}

VISIONNATIVE(IsAbleToSeeTarget)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	cell_t *pos;
	pContext->LocalToPhysAddr(params[4], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	
	bool bNULL = (pContext->GetNullRef(SP_NULL_VECTOR) == pos);
	
	bool bClear = pVision->IsAbleToSee(pEntity, (IVision::FieldOfViewCheckType)params[3], (bNULL) ? NULL : &vPos);
	if (!bNULL)
		VectorToPawnVector(pos, vPos);
	
	return bClear;
}

VISIONNATIVE(IsIgnored)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if (!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return pVision->IsIgnored(pEntity);
}

VISIONNATIVE(IsVisibleEntityNoticed)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	return pVision->IsVisibleEntityNoticed(pEntity);
}

VISIONNATIVE(IsInFieldOfView)
	cell_t *pos;
	pContext->LocalToPhysAddr(params[2], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	
	return pVision->IsInFieldOfView(vPos);
}

VISIONNATIVE(IsInFieldOfViewTarget)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	return pVision->IsInFieldOfView(pEntity);
}

VISIONNATIVE(GetDefaultFieldOfView)
	return sp_ftoc(pVision->GetDefaultFieldOfView());
}

VISIONNATIVE(GetFieldOfView)
	return sp_ftoc(pVision->GetFieldOfView());
}

VISIONNATIVE(SetFieldOfView)
	pVision->SetFieldOfView(sp_ctof(params[2]));
	return 0;
}

VISIONNATIVE(IsLineOfSightClear)
	cell_t *pos;
	pContext->LocalToPhysAddr(params[2], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	return pVision->IsLineOfSightClear(vPos);
}

VISIONNATIVE(IsLineOfSightClearToEntity)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	cell_t *pos;
	pContext->LocalToPhysAddr(params[3], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);
	
	bool bNULL = (pContext->GetNullRef(SP_NULL_VECTOR) == pos);
	
	bool bClear = pVision->IsLineOfSightClearToEntity(pEntity, (bNULL) ? NULL : &vPos);
	if (!bNULL)
		VectorToPawnVector(pos, vPos);
	return bClear;
}

VISIONNATIVE(IsLookingAtTarget)
	CBaseEntity *pEntity = gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity) {
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	
	return pVision->IsLookingAt((const CBaseCombatCharacterHack *)pEntity, sp_ctof(params[3]));
}

VISIONNATIVE(IsLookingAt)
	cell_t *pos;
	pContext->LocalToPhysAddr(params[2], &pos);
	
	Vector vPos;
	PawnVectorToVector(pos, vPos);

	return pVision->IsLookingAt(vPos, sp_ctof(params[3]));
}

#endif