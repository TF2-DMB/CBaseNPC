#include "sourcesdk/funcbrush.h"

DEFINEVAR(CFuncBrushHack, m_iSolidity);

bool CFuncBrushHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	BEGIN_VAR("func_brush");
	OFFSETVAR_DATA(CFuncBrush, m_iSolidity);
	END_VAR;
	return true;
}