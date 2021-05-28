#ifndef H_COMBATCHARACTER_CBASENPC_
#define H_COMBATCHARACTER_CBASENPC_
#ifdef _WIN32
#pragma once
#endif

#include "sourcesdk/baseanimatingoverlay.h"
#include <ehandle.h>
#include <isaverestore.h>
#include <takedamageinfo.h>

class CNavArea;
class CBaseCombatCharacterHack : public CBaseAnimatingOverlayHack
{
public:
	DECLARE_CLASS_NOBASE(CBaseCombatCharacterHack);
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	static VCall<void> vUpdateLastKnownArea;
	void UpdateLastKnownArea(void);

	static VCall<CNavArea*> vGetLastKnownArea;
	CNavArea* GetLastKnownArea(void);

	static VCall<int, const CTakeDamageInfo&> vOnTakeDamage_Alive;
	int OnTakeDamage_Alive(const CTakeDamageInfo& info);
};

#endif // H_COMBATCHARACTER_CBASENPC_