#pragma once

#include "sourcesdk/basecombatcharacter.h"

class NextBotCombatCharacter : public CBaseCombatCharacter
{
public:
    static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);
    static MCall<void> NextBotCombatCharacter_Ctor;
    static int size_of;
    static int vtable_entries;
};