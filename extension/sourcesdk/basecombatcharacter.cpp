#include "sourcesdk/basecombatcharacter.h"

VCall<void> CBaseCombatCharacter::vUpdateLastKnownArea;
VCall<CNavArea*> CBaseCombatCharacter::vGetLastKnownArea;
VCall<int, const CTakeDamageInfo&> CBaseCombatCharacter::vOnTakeDamage_Alive;

bool CBaseCombatCharacter::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	SourceMod::IGameConfig* configSDKHooks;
	if (!gameconfs->LoadGameConfigFile("sdkhooks.games", &configSDKHooks, error, maxlength))
	{
		return false;
	}

	try
	{
		vUpdateLastKnownArea.Init(config, "CBaseCombatCharacter::UpdateLastKnownArea");
		vGetLastKnownArea.Init(config, "CBaseCombatCharacter::GetLastKnownArea");
		vOnTakeDamage_Alive.Init(configSDKHooks, "OnTakeDamage_Alive");
	}
	catch (const std::exception& e)
	{
		// Could use strncpy, but compiler complains
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}

	gameconfs->CloseGameConfigFile(configSDKHooks);
	return true;
}

void CBaseCombatCharacter::UpdateLastKnownArea(void)
{
	vUpdateLastKnownArea(this);
}

CNavArea* CBaseCombatCharacter::GetLastKnownArea(void)
{
	return vGetLastKnownArea(this);
}

int CBaseCombatCharacter::OnTakeDamage_Alive(const CTakeDamageInfo& info)
{
	return vOnTakeDamage_Alive(this, info);
}