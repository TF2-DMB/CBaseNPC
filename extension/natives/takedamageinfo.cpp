#include <ehandle.h>
#include <isaverestore.h>

#define protected_old protected
#undef protected
#define protected public
#include <takedamageinfo.h>
#undef protected
#define protected protected_old

#include "takedamageinfo.hpp"

#include <IGameHelpers.h>
extern SourceMod::IGameHelpers* gamehelpers;

namespace natives::takedamageinfo {

CTakeDamageInfo g_GlobalDamageInfo;

inline CTakeDamageInfo* Get(IPluginContext* context, const cell_t param) {
	CTakeDamageInfo* info = (CTakeDamageInfo*)PawnAddressToPtr(param, context);
	if (!info) {
		context->ThrowNativeError("CTakeDamageInfo is a null ptr!");
		return nullptr;
	}
	return info;
}

cell_t CTakeDamageInfo_Ctor(IPluginContext* context, const cell_t* params) {
	return PtrToPawnAddress(PawnAddressToPtr(params[1], context), context);
}

cell_t GetGlobalDamageInfo(IPluginContext* context, const cell_t* params) {
	static Handle_t globalDmg = BAD_HANDLE;
	if (globalDmg == BAD_HANDLE) {
		globalDmg = PtrToPawnAddress(&g_GlobalDamageInfo, nullptr, true);
	}
	return globalDmg;
}

cell_t Init(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	CBaseEntity* inflictor = gamehelpers->ReferenceToEntity(params[2]);
	if (!inflictor && params[2] != -1) {
		return context->ThrowNativeError("Invalid inflictor index!");
	}

	CBaseEntity* attacker = gamehelpers->ReferenceToEntity(params[3]);
	if (!attacker && params[3] != -1) {
		return context->ThrowNativeError("Invalid attacker index!");
	}

	CBaseEntity* weapon = gamehelpers->ReferenceToEntity(params[4]);
	if (!attacker && params[4] != -1) {
		return context->ThrowNativeError("Invalid weapon index!");
	}

	cell_t* damageForceAddr;
	context->LocalToPhysAddr(params[5], &damageForceAddr);
	Vector damageForce;
	if (context->GetNullRef(SP_NULL_VECTOR) == damageForceAddr) {
		damageForce = vec3_origin;
	}
	else {
		PawnVectorToVector(damageForceAddr, &damageForce);
	}
	
	cell_t* damagePositionAddr;
	context->LocalToPhysAddr(params[6], &damagePositionAddr);
	Vector damagePosition;
	if (context->GetNullRef(SP_NULL_VECTOR) == damagePositionAddr) {
		damagePosition = vec3_origin;
	}
	else {
		PawnVectorToVector(damagePositionAddr, &damagePosition);
	}

	float damage = sp_ctof(params[7]);
	int bitsDamageType = params[8];
	int customDamage = params[9];

	cell_t* reportedPositionAddr;
	context->LocalToPhysAddr(params[10], &reportedPositionAddr);
	Vector reportedPosition;
	if (context->GetNullRef(SP_NULL_VECTOR) == reportedPositionAddr) {
		reportedPosition = vec3_origin;
	}
	else {
		PawnVectorToVector(reportedPositionAddr, &reportedPosition);
	}

	info->Set(inflictor, attacker, weapon, damageForce, damagePosition, damage, bitsDamageType, customDamage, &reportedPosition);	
	return 0;
}

cell_t GetInflictor(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->m_hInflictor.IsValid() ? info->m_hInflictor.GetEntryIndex() : -1;
}

cell_t SetInflictor(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	CBaseEntity* inflictor = gamehelpers->ReferenceToEntity(params[2]);
	if (!inflictor && params[2] != -1) {
		return context->ThrowNativeError("Invalid inflictor index!");
	}
	info->m_hInflictor = inflictor;
	return 0;
}

cell_t GetWeapon(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->m_hWeapon.IsValid() ? info->m_hWeapon.GetEntryIndex() : -1;
}

cell_t SetWeapon(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	CBaseEntity* weapon = gamehelpers->ReferenceToEntity(params[2]);
	if (!weapon && params[2] != -1) {
		return context->ThrowNativeError("Invalid weapon index!");
	}
	info->m_hWeapon = weapon;
	return 0;
}

cell_t GetAttacker(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);

	return info->m_hAttacker.IsValid() ? info->m_hAttacker.GetEntryIndex() : -1;
}

cell_t SetAttacker(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	CBaseEntity* attacker = gamehelpers->ReferenceToEntity(params[2]);
	if (!attacker && params[2] != -1) {
		return context->ThrowNativeError("Invalid attacker index!");
	}
	info->m_hAttacker = attacker;
	return 0;
}

cell_t GetDamage(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return sp_ftoc(info->GetDamage());
}

cell_t SetDamage(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetDamage(sp_ctof(params[2]));
	return 0;
}

cell_t GetMaxDamage(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return sp_ftoc(info->GetMaxDamage());
}

cell_t SetMaxDamage(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetMaxDamage(sp_ctof(params[2]));
	return 0;
}

cell_t ScaleDamage(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->ScaleDamage(sp_ctof(params[2]));
	return 0;
}

cell_t SubtractDamage(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SubtractDamage(sp_ctof(params[2]));
	return 0;
}

cell_t AddDamage(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->AddDamage(sp_ctof(params[2]));
	return 0;
}

cell_t GetDamageBonus(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return sp_ftoc(info->GetDamageBonus());
}

cell_t GetDamageBonusProvider(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->m_hDamageBonusProvider.IsValid() ? info->m_hDamageBonusProvider.GetEntryIndex() : -1;
}

cell_t SetDamageBonus(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}
	CBaseEntity* bonus = gamehelpers->ReferenceToEntity(params[3]);

	info->SetDamageBonus(sp_ctof(params[2]), bonus);
	return 0;
}

cell_t GetBaseDamage(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return sp_ftoc(info->GetBaseDamage());
}

cell_t BaseDamageIsValid(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->BaseDamageIsValid();
}

cell_t GetDamageForce(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	cell_t *vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, info->GetDamageForce());
	return 0;
}

cell_t SetDamageForce(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	cell_t *vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	Vector vec;
	PawnVectorToVector(vecAddr, vec);

	info->SetDamageForce(vec);
	return 0;
}

cell_t ScaleDamageForce(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->ScaleDamageForce(sp_ctof(params[2]));
	return 0;
}

cell_t GetDamageForForceCalc(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return sp_ftoc(info->GetDamageForForceCalc());
}

cell_t SetDamageForForceCalc(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetDamageForForceCalc(sp_ctof(params[2]));
	return 0;
}

cell_t GetDamagePosition(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	cell_t *vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, info->GetDamagePosition());
	return 0;
}

cell_t SetDamagePosition(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	cell_t *vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	Vector vec;
	PawnVectorToVector(vecAddr, vec);

	info->SetDamagePosition(vec);
	return 0;
}

cell_t GetReportedPosition(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	cell_t *vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	VectorToPawnVector(vecAddr, info->GetReportedPosition());
	return 0;
}

cell_t SetReportedPosition(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	cell_t *vecAddr;
	context->LocalToPhysAddr(params[2], &vecAddr);
	Vector vec;
	PawnVectorToVector(vecAddr, vec);

	info->SetReportedPosition(vec);
	return 0;
}

cell_t GetDamageType(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->GetDamageType();
}

cell_t SetDamageType(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetDamageType(params[2]);
	return 0;
}

cell_t AddDamageType(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->AddDamageType(params[2]);
	return 0;
}

cell_t GetDamageCustom(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->GetDamageCustom();
}

cell_t SetDamageCustom(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetDamageCustom(params[2]);
	return 0;
}

cell_t GetDamageStats(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->GetDamageStats();
}

cell_t SetDamageStats(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetDamageStats(params[2]);
	return 0;
}

cell_t IsForceFriendlyFire(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->IsForceFriendlyFire() ? 1 : 0;
}

cell_t SetForceFriendlyFire(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetForceFriendlyFire(!(params[2] == 0));
	return 0;
}

cell_t GetAmmoType(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->GetAmmoType();
}

cell_t SetAmmoType(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetAmmoType(params[2]);
	return 0;
}

/*cell_t GetAmmoName(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	context->StringToLocal(params[2], params[3], info->GetAmmoName());
	return 0;
}*/

cell_t GetPlayerPenetrationCount(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->GetPlayerPenetrationCount();
}

cell_t SetPlayerPenetrationCount(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetPlayerPenetrationCount(params[2]);
	return 0;
}

cell_t GetDamagedOtherPlayers(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	return info->GetDamagedOtherPlayers();
}

cell_t SetDamagedOtherPlayers(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}

	info->SetDamagedOtherPlayers(params[2]);
	return 0;
}

cell_t GetCritType(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}
#if SOURCE_ENGINE == SE_TF2
	return (cell_t)info->m_eCritType;
#else
	return context->ThrowNativeError("No mod support.");
#endif
}

cell_t SetCritType(IPluginContext* context, const cell_t* params) {
	CTakeDamageInfo* info = Get(context, params[1]);
	if (!info) {
		return 0;
	}
#if SOURCE_ENGINE == SE_TF2
	info->SetCritType((CTakeDamageInfo::ECritType)params[2]);
#else
	context->ThrowNativeError("No mod support.");
#endif
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CTakeDamageInfo.CTakeDamageInfo", CTakeDamageInfo_Ctor},

		{"GetGlobalDamageInfo", GetGlobalDamageInfo},

		{"CTakeDamageInfo.Init", Init},

		{"CTakeDamageInfo.GetInflictor", GetInflictor},
		{"CTakeDamageInfo.SetInflictor", SetInflictor},

		{"CTakeDamageInfo.GetWeapon", GetWeapon},
		{"CTakeDamageInfo.SetWeapon", SetWeapon},

		{"CTakeDamageInfo.GetAttacker", GetAttacker},
		{"CTakeDamageInfo.SetAttacker", SetAttacker},

		{"CTakeDamageInfo.GetDamage", GetDamage},
		{"CTakeDamageInfo.SetDamage", SetDamage},

		{"CTakeDamageInfo.GetMaxDamage", GetMaxDamage},
		{"CTakeDamageInfo.SetMaxDamage", SetMaxDamage},

		{"CTakeDamageInfo.ScaleDamage", ScaleDamage},
		{"CTakeDamageInfo.AddDamage", AddDamage},

		{"CTakeDamageInfo.SubtractDamage", SubtractDamage},

		{"CTakeDamageInfo.GetDamageBonus", GetDamageBonus},
		{"CTakeDamageInfo.SetDamageBonus", SetDamageBonus},
		{"CTakeDamageInfo.GetDamageBonusProvider", GetDamageBonusProvider},

		{"CTakeDamageInfo.GetBaseDamage", GetBaseDamage},
		{"CTakeDamageInfo.BaseDamageIsValid", BaseDamageIsValid},

		{"CTakeDamageInfo.GetDamageForce", GetDamageForce},
		{"CTakeDamageInfo.SetDamageForce", SetDamageForce},

		{"CTakeDamageInfo.ScaleDamageForce", ScaleDamageForce},
		{"CTakeDamageInfo.GetDamageForForceCalc", GetDamageForForceCalc},
		{"CTakeDamageInfo.SetDamageForForceCalc", SetDamageForForceCalc},

		{"CTakeDamageInfo.GetDamagePosition", GetDamagePosition},
		{"CTakeDamageInfo.SetDamagePosition", SetDamagePosition},

		{"CTakeDamageInfo.GetReportedPosition", GetReportedPosition},
		{"CTakeDamageInfo.SetReportedPosition", SetReportedPosition},

		{"CTakeDamageInfo.GetDamageType", GetDamageType},
		{"CTakeDamageInfo.SetDamageType", SetDamageType},
		{"CTakeDamageInfo.AddDamageType", AddDamageType},

		{"CTakeDamageInfo.GetDamageCustom", GetDamageCustom},
		{"CTakeDamageInfo.SetDamageCustom", SetDamageCustom},

		{"CTakeDamageInfo.GetDamageStats", GetDamageStats},
		{"CTakeDamageInfo.SetDamageStats", SetDamageStats},

		{"CTakeDamageInfo.SetForceFriendlyFire", SetForceFriendlyFire},
		{"CTakeDamageInfo.IsForceFriendlyFire", IsForceFriendlyFire},

		{"CTakeDamageInfo.GetAmmoType", GetAmmoType},
		{"CTakeDamageInfo.SetAmmoType", SetAmmoType},
		//{"CTakeDamageInfo.GetAmmoName", GetAmmoName},

		{"CTakeDamageInfo.GetPlayerPenetrationCount", GetPlayerPenetrationCount},
		{"CTakeDamageInfo.SetPlayerPenetrationCount", SetPlayerPenetrationCount},

		{"CTakeDamageInfo.GetDamagedOtherPlayers", GetDamagedOtherPlayers},
		{"CTakeDamageInfo.SetDamagedOtherPlayers", SetDamagedOtherPlayers},

		{"CTakeDamageInfo.SetCritType", SetCritType},
		{"CTakeDamageInfo.GetCritType", GetCritType},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}