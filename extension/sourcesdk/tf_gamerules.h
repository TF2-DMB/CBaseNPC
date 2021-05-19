#ifndef H_TF_GAMERULES_CBASENPC_
#define H_TF_GAMERULES_CBASENPC_
#ifdef _WIN32
#pragma once
#endif

class CBaseEntity;

#include <IGameConfigs.h>
#include <ehandle.h>
#include <isaverestore.h>
#include <takedamageinfo.h>
#include "helpers.h"

class CTFGameRules
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	static MCall<bool, CTakeDamageInfo&, CBaseEntity*, bool> mApplyOnDamageModifyRules;
	bool ApplyOnDamageModifyRules(CTakeDamageInfo& info, CBaseEntity* pVictimBaseEntity, bool bAllowDamage);
	struct DamageModifyExtras_t
	{
		bool bIgniting;
		bool bSelfBlastDmg;
		bool bSendPreFeignDamage;
		bool bPlayDamageReductionSound;
	};
	static MCall<float, const CTakeDamageInfo&, CBaseEntity*, DamageModifyExtras_t&> mApplyOnDamageAliveModifyRules;
	float ApplyOnDamageAliveModifyRules(const CTakeDamageInfo& info, CBaseEntity* pVictimBaseEntity, DamageModifyExtras_t& outParams);
};

CTFGameRules* TFGameRules();

#endif // H_TF_GAMERULES_CBASENPC_