#include "sourcesdk/baseentity.h"

#include "baseentity.hpp"

namespace natives::baseentity {

inline CBaseEntity* Get(IPluginContext* context, const cell_t param) {	
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(param);
	if (!entity) {
		context->ThrowNativeError("Invalid entity %d", param);
		return nullptr;
	}
	return entity;
}

cell_t iUpdateOnRemove(IPluginContext* context, const cell_t* params) {
	return CBaseEntity::offset_UpdateOnRemove;
}

cell_t NetworkProp(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return PtrToPawnAddress(entity->NetworkProp(), context);
}

cell_t CollisionProp(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return PtrToPawnAddress(entity->CollisionProp(), context);
}

cell_t DispatchUpdateTransmitState(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->DispatchUpdateTransmitState();
	return 0;
}

cell_t GetEFlags(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->GetEFlags();
}

cell_t IsEFlagSet(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->IsEFlagSet(params[2]) ? 1 : 0;
}

cell_t SetEFlags(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetEFlags(params[2]);
	return 0;
}

cell_t AddEFlags(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->AddEFlags(params[2]);
	return 0;
}

cell_t RemoveEFlags(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->RemoveEFlags(params[2]);
	return 0;
}

cell_t SetModelName(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* model = nullptr;
	context->LocalToString(params[2], &model);
	entity->SetModelName(AllocPooledString(model));
	return 0;
}

cell_t GetModelName(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	string_t m = entity->GetModelName();
	const char* model = (m == NULL_STRING) ? "" : STRING(m);
	context->StringToLocal(params[2], params[3], model);
	return 0;
}

cell_t RegisterThinkContext(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* str = nullptr;
	context->LocalToStringNULL(params[2], &str);
	return entity->RegisterThinkContext(str);
}

cell_t SetNextThink(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* str = nullptr;
	context->LocalToStringNULL(params[3], &str);
	entity->SetNextThink(sp_ctof(params[2]), str);
	return 0;
}

cell_t GetNextThink(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* str = nullptr;
	context->LocalToStringNULL(params[2], &str);
	return sp_ftoc(entity->GetNextThink(str));
}

cell_t GetLastThink(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* str = nullptr;
	context->LocalToStringNULL(params[2], &str);
	return sp_ftoc(entity->GetLastThink(str));
}

cell_t GetNextThinkTick(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* str = nullptr;
	context->LocalToStringNULL(params[2], &str);
	return entity->GetNextThinkTick(str);
}

cell_t GetLastThinkTick(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* str = nullptr;
	context->LocalToStringNULL(params[2], &str);
	return entity->GetLastThinkTick(str);
}

cell_t WillThink(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->WillThink() ? 1 : 0;
}

cell_t CheckHasThinkFunction(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->CheckHasThinkFunction((params[2]) ? true : false);
	return 0;
}

cell_t GetAbsOrigin(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec = entity->GetAbsOrigin();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

cell_t GetAbsAngles(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	QAngle vec = entity->GetAbsAngles();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

cell_t GetAbsVelocity(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec = entity->GetAbsVelocity();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

cell_t SetAbsOrigin(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec;
	PawnVectorToVector(vecAdd, vec);
	entity->SetAbsOrigin(vec);
	return 0;
}

cell_t SetAbsAngles(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	QAngle vec;
	PawnVectorToVector(vecAdd, vec);
	entity->SetAbsAngles(vec);
	return 0;
}

cell_t SetAbsVelocity(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec;
	PawnVectorToVector(vecAdd, vec);
	entity->SetAbsVelocity(vec);
	return 0;
}

cell_t NetworkStateChanged(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->NetworkStateChanged();
	return 0;
}

cell_t NetworkStateChangedVar(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->NetworkStateChanged(PawnAddressToPtr(params[2], context));
	return 0;
}

cell_t GetSimulationTime(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return sp_ftoc(entity->GetSimulationTime());
}

cell_t SetSimulationTime(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->SetSimulationTime(sp_ctof(params[2]));
	return 0;
}

cell_t GetLocalAngles(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	QAngle vec = entity->GetLocalAngles();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

cell_t GetLocalOrigin(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec = entity->GetLocalOrigin();
	VectorToPawnVector(vecAdd, vec);
	return 0;
}

cell_t SetLocalAngles(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	QAngle vec;
	PawnVectorToVector(vecAdd, vec);
	entity->SetLocalAngles(vec);
	return 0;
}

cell_t SetLocalOrigin(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec;
	PawnVectorToVector(vecAdd, vec);
	entity->SetLocalOrigin(vec);
	return 0;
}

cell_t GetVectors(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* nullAdd = context->GetNullRef(SP_NULL_VECTOR);
	cell_t* upAdd, * rigthAdd, * forwardAdd;

	context->LocalToPhysAddr(params[2], &forwardAdd);
	context->LocalToPhysAddr(params[3], &rigthAdd);
	context->LocalToPhysAddr(params[4], &upAdd);

	Vector forward, right, up;
	PawnVectorToVector(forwardAdd, forward);
	PawnVectorToVector(rigthAdd, right);
	PawnVectorToVector(upAdd, up);

	entity->GetVectors((forwardAdd == nullAdd) ? nullptr : &forward, (rigthAdd == nullAdd) ? nullptr : &right, (upAdd == nullAdd) ? nullptr : &up);

	VectorToPawnVector(forwardAdd, forward);
	VectorToPawnVector(rigthAdd, right);
	VectorToPawnVector(upAdd, up);
	return 0;
}

cell_t WorldSpaceCenter(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	Vector vec = entity->WorldSpaceCenter();
	VectorToPawnVector(vecAddr, vec);
	return 0;
}

cell_t Spawn(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	servertools->DispatchSpawn(entity);
	return 0;
}

cell_t Teleport(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* nullAdd = context->GetNullRef(SP_NULL_VECTOR);
	cell_t* originAdd, * angAdd, * velAdd;
	context->LocalToPhysAddr(params[2], &originAdd);
	context->LocalToPhysAddr(params[3], &angAdd);
	context->LocalToPhysAddr(params[4], &velAdd);

	Vector origin = vec3_origin;
	Vector vel = vec3_origin;
	QAngle ang = QAngle(0, 0, 0);
	PawnVectorToVector(originAdd, origin);
	PawnVectorToVector(angAdd, ang);
	PawnVectorToVector(velAdd, vel);

	entity->Teleport((originAdd == nullAdd) ? nullptr : &origin, (angAdd == nullAdd) ? nullptr : &ang, (velAdd == nullAdd) ? nullptr : &vel);
	return 0;
}

cell_t SetModel(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	char* model = nullptr;
	context->LocalToString(params[2], &model);
	entity->SetModel(model);
	return 0;
}

cell_t EntityToWorldTransform(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	cell_t* pawnMat = nullptr;
	context->LocalToPhysAddr(params[2], &pawnMat);

	MatrixToPawnMatrix(context, pawnMat, entity->EntityToWorldTransform());
	return 0;
}

cell_t MyNextBotPointer(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return PtrToPawnAddress(entity->MyNextBotPointer(), context);
}

cell_t GetBaseAnimating(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->GetBaseAnimating() ? params[1] : -1;
}

cell_t MyCombatCharacterPointer(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->MyCombatCharacterPointer() ? params[1] : -1;
}

cell_t IsCombatCharacter(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return entity->MyCombatCharacterPointer() ? 1 : 0;
}

cell_t TakeDamage(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	CTakeDamageInfo* inputInfo = (CTakeDamageInfo*)PawnAddressToPtr(params[2], context);
	if (!inputInfo) {
		context->ThrowNativeError("CTakeDamageInfo is a null ptr!");
		return 0;
	}

	entity->TakeDamage(*inputInfo);
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CBaseEntity.iUpdateOnRemove", iUpdateOnRemove},
		{"CBaseEntity.NetworkProp", NetworkProp},
		{"CBaseEntity.CollisionProp", CollisionProp},
		{"CBaseEntity.DispatchUpdateTransmitState", DispatchUpdateTransmitState},
		{"CBaseEntity.GetEFlags", GetEFlags},
		{"CBaseEntity.IsEFlagSet", IsEFlagSet},
		{"CBaseEntity.SetEFlags", SetEFlags},
		{"CBaseEntity.AddEFlags", AddEFlags},
		{"CBaseEntity.RemoveEFlags", RemoveEFlags},
		{"CBaseEntity.SetModelName", SetModelName},
		{"CBaseEntity.GetModelName", GetModelName},
		{"CBaseEntity.RegisterThinkContext", RegisterThinkContext},
		{"CBaseEntity.SetNextThink", SetNextThink},
		{"CBaseEntity.GetNextThink", GetNextThink},
		{"CBaseEntity.GetLastThink", GetLastThink},
		{"CBaseEntity.GetNextThinkTick", GetNextThinkTick},
		{"CBaseEntity.GetLastThinkTick", GetLastThinkTick},
		{"CBaseEntity.WillThink", WillThink},
		{"CBaseEntity.CheckHasThinkFunction", CheckHasThinkFunction},
		{"CBaseEntity.GetAbsOrigin", GetAbsOrigin},
		{"CBaseEntity.GetAbsAngles", GetAbsAngles},
		{"CBaseEntity.GetAbsVelocity", GetAbsVelocity},
		{"CBaseEntity.SetAbsVelocity", SetAbsVelocity},
		{"CBaseEntity.SetAbsAngles", SetAbsAngles},
		{"CBaseEntity.SetAbsOrigin", SetAbsOrigin},
		{"CBaseEntity.NetworkStateChanged", NetworkStateChanged},
		{"CBaseEntity.NetworkStateChangedVar", NetworkStateChangedVar},
		{"CBaseEntity.GetSimulationTime", GetSimulationTime},
		{"CBaseEntity.SetSimulationTime", SetSimulationTime},
		{"CBaseEntity.GetLocalAngles", GetLocalAngles},
		{"CBaseEntity.SetLocalAngles", SetLocalAngles},
		{"CBaseEntity.GetLocalOrigin", GetLocalOrigin},
		{"CBaseEntity.SetLocalOrigin", SetLocalOrigin},
		{"CBaseEntity.Spawn", Spawn},
		{"CBaseEntity.Teleport", Teleport},
		{"CBaseEntity.SetModel", SetModel},
		{"CBaseEntity.GetVectors", GetVectors},
		{"CBaseEntity.WorldSpaceCenter", WorldSpaceCenter},
		{"CBaseEntity.EntityToWorldTransform", EntityToWorldTransform},
		{"CBaseEntity.MyNextBotPointer", MyNextBotPointer},
		{"CBaseEntity.GetBaseAnimating", GetBaseAnimating},
		{"CBaseEntity.MyCombatCharacterPointer", MyCombatCharacterPointer},
		{"CBaseEntity.IsCombatCharacter", IsCombatCharacter},
		{"CBaseEntity.TakeDamage", TakeDamage}
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}