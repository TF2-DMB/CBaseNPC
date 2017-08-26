#ifndef _NATIVES_COMPONENT_H_INCLUDED_
#define _NATIVES_COMPONENT_H_INCLUDED_

#pragma once

#include "NextBotComponentInterface.h"

#define COMPONENTNATIVE(name) \
	cell_t INextBotComponent_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		INextBotComponent *pComponent = (INextBotComponent *)(params[1]); \
		if(!pComponent) { \
			return pContext->ThrowNativeError("Invalid INextBotComponent %x", params[1]); \
		} \



COMPONENTNATIVE(Reset)
	pComponent->Reset();
	return 0;
}

COMPONENTNATIVE(Update)
	pComponent->Update();
	return 0;
}

COMPONENTNATIVE(Upkeep)
	pComponent->Upkeep();
	return 0;
}

COMPONENTNATIVE(GetBot)
	return (cell_t)(pComponent->GetBot());
}

#endif