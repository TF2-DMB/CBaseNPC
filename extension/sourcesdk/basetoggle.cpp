#include "sourcesdk/basetoggle.h"

DEFINEVAR(CBaseToggleHack, m_toggle_state);

bool CBaseToggleHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	BEGIN_VAR("func_door");
	OFFSETVAR_DATA(CBaseToggle, m_toggle_state);
	END_VAR;
	return true;
}