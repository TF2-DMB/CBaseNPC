#include "sourcesdk/basecombatcharacter.h"

DEFINEFUNCTION_virtual_void(CBaseCombatCharacterHack, UpdateLastKnownArea, (void), ());
DEFINEFUNCTION_virtual(CBaseCombatCharacterHack, GetLastKnownArea, CNavArea*, (void), ());

bool CBaseCombatCharacterHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	FINDVTABLE(config, UpdateLastKnownArea, "CBaseCombatCharacter::UpdateLastKnownArea");
	FINDVTABLE(config, GetLastKnownArea, "CBaseCombatCharacter::GetLastKnownArea");
	return true;
}