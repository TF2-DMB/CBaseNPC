#ifndef NATIVE_CBASEENT_H_
#define NATIVE_CBASEENT_H_
#pragma once

#include "sourcesdk/baseentity.h"

#define CBASEENTNATIVE(name) \
	cell_t CBaseEntity_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		CBaseEntityHack* ent = (CBaseEntityHack *)gamehelpers->ReferenceToEntity(params[1]); \
		if (!ent) \
		{ \
			return pContext->ThrowNativeError("Invalid entity %x", params[1]); \
		}

cell_t CBaseEntity_iUpdateOnRemove(IPluginContext * pContext, const cell_t * params)
{
	return CBaseEntityHack::offset_UpdateOnRemove;
}

CBASEENTNATIVE(GetVectors)
	cell_t* nullAdd = pContext->GetNullRef(SP_NULL_VECTOR);
	cell_t* upAdd, * rigthAdd, * forwardAdd;

	pContext->LocalToPhysAddr(params[2], &forwardAdd);
	pContext->LocalToPhysAddr(params[3], &rigthAdd);
	pContext->LocalToPhysAddr(params[4], &upAdd);

	Vector forward, right, up;
	PawnVectorToVector(forwardAdd, forward);
	PawnVectorToVector(rigthAdd, right);
	PawnVectorToVector(upAdd, up);

	ent->GetVectors((forwardAdd == nullAdd) ? nullptr : &forward, (rigthAdd == nullAdd) ? nullptr : &right, (upAdd == nullAdd) ? nullptr : &up);

	VectorToPawnVector(forwardAdd, forward);
	VectorToPawnVector(rigthAdd, right);
	VectorToPawnVector(upAdd, up);
	return 0;
};

CBASEENTNATIVE(WorldSpaceCenter)
	cell_t* vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	Vector vec = ent->WorldSpaceCenter();
	VectorToPawnVector(vecAddr, vec);
	return 0;
};

#endif // NATIVE_CBASEENT_H_