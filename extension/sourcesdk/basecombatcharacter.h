#ifndef H_COMBATCHARACTER_CBASENPC_
#define H_COMBATCHARACTER_CBASENPC_
#ifdef _WIN32
#pragma once
#endif

#include "sourcesdk/baseanimatingoverlay.h"

class CNavArea;
class CBaseCombatCharacterHack : public CBaseAnimatingOverlayHack
{
public:
	DECLARE_CLASS_NOBASE(CBaseCombatCharacterHack);
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	DECLAREFUNCTION_virtual(UpdateLastKnownArea, void, (void));
	DECLAREFUNCTION_virtual(GetLastKnownArea, CNavArea*, (void));
};

#endif // H_COMBATCHARACTER_CBASENPC_