#pragma once

#include "sourcesdk/baseentity.h"

enum TOGGLE_STATE
{
	TS_AT_TOP,
	TS_AT_BOTTOM,
	TS_GOING_UP,
	TS_GOING_DOWN
};

class CBaseToggle : public CBaseEntity
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	TOGGLE_STATE GetToggleState();

private:
	DECLAREVAR(TOGGLE_STATE, m_toggle_state);
};

inline TOGGLE_STATE CBaseToggle::GetToggleState()
{
	return *m_toggle_state();
}