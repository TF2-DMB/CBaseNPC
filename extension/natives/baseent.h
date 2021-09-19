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

CBASEENTNATIVE(NetworkProp)
	return (cell_t)ent->NetworkProp();
}

CBASEENTNATIVE(CollisionProp)
	return (cell_t)ent->CollisionProp();
}

CBASEENTNATIVE(DispatchUpdateTransmitState)
	ent->DispatchUpdateTransmitState();
	return 0;
}

CBASEENTNATIVE(GetEFlags)
	return ent->GetEFlags();
}

CBASEENTNATIVE(IsEFlagSet)
	return ent->IsEFlagSet(params[2]);
}

CBASEENTNATIVE(SetEFlags)
	ent->SetEFlags(params[2]);
	return 0;
}

CBASEENTNATIVE(AddEFlags)
	ent->AddEFlags(params[2]);
	return 0;
}

CBASEENTNATIVE(RemoveEFlags)
	ent->RemoveEFlags(params[2]);
	return 0;
}

CBASEENTNATIVE(SetModelName)
	char* model = nullptr;
	pContext->LocalToString(params[2], &model);
	ent->SetModelName(AllocPooledString(model));
	return 0;
}

CBASEENTNATIVE(GetModelName)
	string_t m = ent->GetModelName();
	const char* model = (m == NULL_STRING) ? "" : STRING(m);
	pContext->StringToLocal(params[2], params[3], model);
	return 0;
}

CBASEENTNATIVE(RegisterThinkContext)
	char* context = nullptr;
	pContext->LocalToStringNULL(params[2], &context);
	return ent->RegisterThinkContext(context);
}

CBASEENTNATIVE(SetNextThink)
	char* context = nullptr;
	pContext->LocalToStringNULL(params[3], &context);
	ent->SetNextThink(sp_ctof(params[2]), context);
	return 0;
}

CBASEENTNATIVE(GetNextThink)
	char* context = nullptr;
	pContext->LocalToStringNULL(params[2], &context);
	return sp_ftoc(ent->GetNextThink(context));
}

CBASEENTNATIVE(GetLastThink)
	char* context = nullptr;
	pContext->LocalToStringNULL(params[2], &context);
	return sp_ftoc(ent->GetLastThink(context));
}

CBASEENTNATIVE(GetNextThinkTick)
	char* context = nullptr;
	pContext->LocalToStringNULL(params[2], &context);
	return ent->GetNextThinkTick(context);
}

CBASEENTNATIVE(GetLastThinkTick)
	char* context = nullptr;
	pContext->LocalToStringNULL(params[2], &context);
	return ent->GetLastThinkTick(context);
}

CBASEENTNATIVE(WillThink)
	return ent->WillThink();
}

CBASEENTNATIVE(CheckHasThinkFunction)
	ent->CheckHasThinkFunction((params[2]) ? true : false);
	return 0;
}

CBASEENTNATIVE(GetAbsOrigin)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec = ent->GetAbsOrigin();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

CBASEENTNATIVE(GetAbsAngles)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	QAngle vec = ent->GetAbsAngles();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

CBASEENTNATIVE(GetAbsVelocity)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec = ent->GetAbsVelocity();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

CBASEENTNATIVE(SetAbsOrigin)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec;
	PawnVectorToVector(vecAdd, vec);
	ent->SetAbsOrigin(vec);
	return 0;
}

CBASEENTNATIVE(SetAbsAngles)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	QAngle vec;
	PawnVectorToVector(vecAdd, vec);
	ent->SetAbsAngles(vec);
	return 0;
}

CBASEENTNATIVE(SetAbsVelocity)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec;
	PawnVectorToVector(vecAdd, vec);
	ent->SetAbsVelocity(vec);
	return 0;
}

CBASEENTNATIVE(NetworkStateChanged)
	ent->NetworkStateChanged();
	return 0;
}

CBASEENTNATIVE(NetworkStateChangedVar)
	ent->NetworkStateChanged((void*)params[2]);
	return 0;
}

CBASEENTNATIVE(GetSimulationTime)
	return sp_ftoc(ent->GetSimulationTime());
}

CBASEENTNATIVE(SetSimulationTime)
	ent->SetSimulationTime(sp_ctof(params[2]));
	return 0;
}

CBASEENTNATIVE(GetLocalAngles)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	QAngle vec = ent->GetLocalAngles();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

CBASEENTNATIVE(GetLocalOrigin)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec = ent->GetLocalOrigin();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

CBASEENTNATIVE(SetLocalAngles)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	QAngle vec;
	PawnVectorToVector(vecAdd, vec);
	ent->SetLocalAngles(vec);
	return 0;
}

CBASEENTNATIVE(SetLocalOrigin)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec;
	PawnVectorToVector(vecAdd, vec);
	ent->SetLocalOrigin(vec);
	return 0;
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
}

CBASEENTNATIVE(WorldSpaceCenter)
	cell_t* vecAddr;
	pContext->LocalToPhysAddr(params[2], &vecAddr);
	Vector vec = ent->WorldSpaceCenter();
	VectorToPawnVector(vecAddr, vec);
	return 0;
}

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

CBASEENTNATIVE(MyNextBotPointer)
	return (cell_t)ent->MyNextBotPointer();
}

CBASEENTNATIVE(GetBaseAnimating)
	return ent->GetBaseAnimating() ? params[1] : -1;
}

CBASEENTNATIVE(MyCombatCharacterPointer)
	return ent->MyCombatCharacterPointer() ? params[1] : -1;
}

CBASEENTNATIVE(IsCombatCharacter)
	return ent->MyCombatCharacterPointer() ? true : false;
}

#endif // NATIVE_CBASEENT_H_