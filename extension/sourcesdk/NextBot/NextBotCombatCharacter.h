#ifndef _NEXT_BOT_COMBAT_CHARACTER_INTERFACE_H_
#define _NEXT_BOT_COMBAT_CHARACTER_INTERFACE_H_

#include "sourcesdk/basecombatcharacter.h"

class NextBotCombatCharacter : public CBaseCombatCharacterHack
{
public:
    static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);
    static MCall<void> NextBotCombatCharacter_Ctor;
    static int size_of;
    static int vtable_entries;
};

#endif