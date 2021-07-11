#include "sourcesdk/NextBot/NextBotCombatCharacter.h"
#include <shareddefs.h>
#include <util.h>

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

	IEntityFactoryDictionary* factoryDictionary = servertools->GetEntityFactoryDictionary();
	{
		IEntityFactory* factory = nullptr;

		int entitySize = 0;
		factory = factoryDictionary->FindFactory("simple_bot");
		
		if (factory) entitySize = factory->GetEntitySize();
		
		NextBotCombatCharacter::size_of = entitySize;
	}

	if (!NextBotCombatCharacter::size_of)
	{
		snprintf(error, maxlength, "Failed to get size of NextBotCombatCharacter");
		return false;
	}

	if (!config->GetOffset("NextBotCombatCharacter::vtable_entries", &NextBotCombatCharacter::vtable_entries))
	{
		snprintf(error, maxlength, "Couldn't find NextBotCombatCharacter::vtable_entries offset!");
		return false;
	}
	return true;
}