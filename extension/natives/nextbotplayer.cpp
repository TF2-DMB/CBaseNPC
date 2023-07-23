#include "NextBotInterface.h"
#include "toolsnextbot.h"

#include "natives.hpp"

namespace natives::nextbotplayer {

template<typename T>
inline T* Get(IPluginContext* context, const cell_t param) {	
	T* bot = (T*)PawnAddressToPtr(param);
	if (!bot) {
		context->ThrowNativeError("Botplayer is a null ptr!");
		return nullptr;
	}
	return bot;
}

cell_t ToolsNextBotPlayer(IPluginContext* context, const cell_t* params) {
	CBaseCombatCharacter* entity = (CBaseCombatCharacter*)PawnAddressToPtr(params[1]);
	if (!entity) {
		return context->ThrowNativeError("Invalid entity address %x", params[1]);
	}

	return PtrToPawnAddress(new class ToolsNextBotPlayer(entity));
}

cell_t SetIsDormantWhenDead(IPluginContext* context, const cell_t* params) {
	auto bot = Get<class ToolsNextBotPlayer>(context, params[1]);
	if (!bot) {
		return 0;
	}

	bot->SetIsDormantWhenDead(params[2] == 1);
	return 0;
}

cell_t GetIsDormantWhenDead(IPluginContext* context, const cell_t* params) {
	auto bot = Get<class ToolsNextBotPlayer>(context, params[1]);
	if (!bot) {
		return 0;
	}

	return bot->IsDormantWhenDead() ? 1 : 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"ToolsNextBotPlayer.ToolsNextBotPlayer", ToolsNextBotPlayer},
		{"ToolsNextBotPlayer.IsDormantWhenDead.set", SetIsDormantWhenDead},
		{"ToolsNextBotPlayer.IsDormantWhenDead.get", GetIsDormantWhenDead},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}
}