#include "sourcesdk/NextBot/NextBotCombatCharacter.h"

MCall<void> NextBotCombatCharacter::NextBotCombatCharacter_Ctor;

int NextBotCombatCharacter::size_of = 0;
int NextBotCombatCharacter::vtable_entries = 0;

bool NextBotCombatCharacter::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	try
	{
		NextBotCombatCharacter_Ctor.Init(config, "NextBotCombatCharacter::NextBotCombatCharacter");
	}
	catch (const std::exception& e)
	{
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}
	
	if (!config->GetOffset("sizeof(NextBotCombatCharacter)", &NextBotCombatCharacter::size_of))
	{
		snprintf(error, maxlength, "Couldn't find sizeof(NextBotCombatCharacter) offset!");
		return false;
	}

	if (!config->GetOffset("NextBotCombatCharacter::vtable_entries", &NextBotCombatCharacter::vtable_entries))
	{
		snprintf(error, maxlength, "Couldn't find NextBotCombatCharacter::vtable_entries offset!");
		return false;
	}
	return true;
}