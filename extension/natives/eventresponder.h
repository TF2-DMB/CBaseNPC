#ifndef NATIVES_EVENTRESPONDER_H_INCLUDED_
#define NATIVES_EVENTRESPONDER_H_INCLUDED_

#pragma once

#include "NextBotEventResponderInterface.h"

#define EVENTRESPONDERNATIVE(name) \
	cell_t INextBotEventResponder_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		INextBotEventResponder *pResponder = (INextBotEventResponder *)(params[1]); \
		if(!pResponder) { \
			return pContext->ThrowNativeError("Invalid INextBotEventResponder %x", params[1]); \
		} \

EVENTRESPONDERNATIVE(FirstContainedResponder)
	return (cell_t)(pResponder->FirstContainedResponder());
}

EVENTRESPONDERNATIVE(NextContainedResponder)
	INextBotEventResponder *pPrevResponder = (INextBotEventResponder *)(params[2]);
	if(!pPrevResponder) {
		return pContext->ThrowNativeError("Invalid INextBotEventResponder %x", params[1]);
	}
	return (cell_t)(pResponder->NextContainedResponder(pPrevResponder));
}

#endif