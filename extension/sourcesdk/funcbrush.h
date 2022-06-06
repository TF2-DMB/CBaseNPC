#pragma once

#include "sourcesdk/baseentity.h"

class CFuncBrushHack : CBaseEntityHack
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	enum BrushSolidities_e
	{
		BRUSHSOLID_TOGGLE = 0,
		BRUSHSOLID_NEVER  = 1,
		BRUSHSOLID_ALWAYS = 2,
	};

	BrushSolidities_e GetSolidity();

private:

	DECLAREVAR(BrushSolidities_e, m_iSolidity);
};

inline CFuncBrushHack::BrushSolidities_e CFuncBrushHack::GetSolidity()
{
	return *m_iSolidity();
}