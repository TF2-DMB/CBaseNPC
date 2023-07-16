#include "sourcesdk/basetoggle.h"

DEFINEVAR(CBaseToggle, m_toggle_state);

bool CBaseToggle::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	BEGIN_VAR("func_door");
	OFFSETVAR_DATA(CBaseToggle, m_toggle_state);
	END_VAR;
	return true;
}