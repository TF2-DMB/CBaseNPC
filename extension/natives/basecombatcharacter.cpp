#include "sourcesdk/basecombatcharacter.h"
#include "basecombatcharacter.hpp"

namespace natives::basecombatcharacter {

CBaseCombatCharacter* Get(IPluginContext* context, const cell_t param) {
	CBaseCombatCharacter* entity = (CBaseCombatCharacter*)gamehelpers->ReferenceToEntity(param);
	if (!entity) {
		context->ThrowNativeError("Invalid entity %d", param);
		return nullptr;
	}

	if (!entity->MyCombatCharacterPointer()) {
		context->ThrowNativeError("Entity %d isn't a CBaseCombatCharacter!", param);
		return nullptr;
	}

	return entity;
}

cell_t UpdateLastKnownArea(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	entity->UpdateLastKnownArea();
	return 0;
};

cell_t GetLastKnownArea(IPluginContext* context, const cell_t* params) {
	auto entity = Get(context, params[1]);
	if (!entity) {
		return 0;
	}

	return PtrToPawnAddress(entity->GetLastKnownArea(), context);
};

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CBaseCombatCharacter.UpdateLastKnownArea", UpdateLastKnownArea},
		{"CBaseCombatCharacter.GetLastKnownArea", GetLastKnownArea},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}