#include "sourcesdk/tf_gamerules.h"
#include <ISDKTools.h>

extern ISDKTools* g_pSDKTools;

MCall<bool, CTakeDamageInfo&, CBaseEntity*, bool> CTFGameRules::mApplyOnDamageModifyRules;
MCall<float, const CTakeDamageInfo&, CBaseEntity*, CTFGameRules::DamageModifyExtras_t&> CTFGameRules::mApplyOnDamageAliveModifyRules;

bool CTFGameRules::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	try
	{
		mApplyOnDamageModifyRules.Init(config, "CTFGameRules::ApplyOnDamageModifyRules");
		mApplyOnDamageAliveModifyRules.Init(config, "CTFGameRules::ApplyOnDamageAliveModifyRules");
	}
	catch (const std::exception & e)
	{
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}

	return true;
}

CTFGameRules* TFGameRules()
{
	return ((CTFGameRules*)g_pSDKTools->GetGameRules());
}

bool CTFGameRules::ApplyOnDamageModifyRules(CTakeDamageInfo& info, CBaseEntity* pVictimBaseEntity, bool bAllowDamage)
{
	return mApplyOnDamageModifyRules(this, info, pVictimBaseEntity, bAllowDamage);
}

float CTFGameRules::ApplyOnDamageAliveModifyRules(const CTakeDamageInfo& info, CBaseEntity* pVictimBaseEntity, DamageModifyExtras_t& outParams)
{
	return mApplyOnDamageAliveModifyRules(this, info, pVictimBaseEntity, outParams);
}