#include "path.hpp"

namespace natives::nextbot::component {

inline INextBotComponent* Get(IPluginContext* context, const cell_t param) {	
	INextBotComponent* component = (INextBotComponent*)param;
	if (!component) {
		context->ThrowNativeError("Component is a null ptr!");
		return nullptr;
	}

	return component;
}

cell_t Reset(IPluginContext* context, const cell_t* params) {
	auto component = Get(context, params[1]);
	if (!component) {
		return 0;
	}

	component->Reset();
	return 0;
}

cell_t Update(IPluginContext* context, const cell_t* params) {
	auto component = Get(context, params[1]);
	if (!component) {
		return 0;
	}

	component->Update();
	return 0;
}

cell_t Upkeep(IPluginContext* context, const cell_t* params) {
	auto component = Get(context, params[1]);
	if (!component) {
		return 0;
	}

	component->Upkeep();
	return 0;
}

cell_t GetBot(IPluginContext* context, const cell_t* params) {
	auto component = Get(context, params[1]);
	if (!component) {
		return 0;
	}

	return (cell_t)(component->GetBot());
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"INextBotComponent.Reset", Reset},
		{"INextBotComponent.Update", Update},
		{"INextBotComponent.Upkeep", Upkeep},
		{"INextBotComponent.GetBot", GetBot},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}