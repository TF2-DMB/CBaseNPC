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

CBASEENTNATIVE(Spawn)
	servertools->DispatchSpawn((CBaseEntity *)ent);
	return 0;
}

CBASEENTNATIVE(Teleport)
	cell_t* nullAdd = pContext->GetNullRef(SP_NULL_VECTOR);
	cell_t* originAdd, * angAdd, * velAdd;
	pContext->LocalToPhysAddr(params[2], &originAdd);
	pContext->LocalToPhysAddr(params[3], &angAdd);
	pContext->LocalToPhysAddr(params[4], &velAdd);

	Vector origin = vec3_origin;
	Vector vel = vec3_origin;
	QAngle ang = QAngle(0, 0, 0);
	PawnVectorToVector(originAdd, origin);
	PawnVectorToVector(originAdd, ang);
	PawnVectorToVector(originAdd, vel);

	ent->Teleport((originAdd == nullAdd) ? NULL : &origin, (angAdd == nullAdd) ? NULL : &ang, (velAdd == nullAdd) ? NULL : &vel);

	return 0;
}

CBASEENTNATIVE(SetModel)
	char* model = nullptr;
	pContext->LocalToString(params[2], &model);
	ent->SetModel(model);
	return 0;
}

CBASEENTNATIVE(EntityToWorldTransform)
	cell_t * pawnMat = nullptr;
	pContext->LocalToPhysAddr( params[2], &pawnMat );

	MatrixToPawnMatrix( pawnMat, ent->EntityToWorldTransform() );
	return 0;
}

#endif // NATIVE_CBASEENT_H_