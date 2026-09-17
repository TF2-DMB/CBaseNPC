#include "sourcesdk/funcbrush.h"

DEFINEVAR(CFuncBrush, m_iSolidity);

bool CFuncBrush::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength, datamap_t* dataMap)
{
	BEGIN_VAR("func_brush", dataMap);
	OFFSETVAR_DATA(CFuncBrush, m_iSolidity);
	END_VAR;
	return true;
}