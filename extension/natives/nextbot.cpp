#include "NextBotInterface.h"
#include "toolsnextbot.h"

#include "nextbot.hpp"
#include "nextbot/component.hpp"
#include "nextbot/eventresponder.hpp"
#include "nextbot/path.hpp"
#include "nextbot/body.hpp"
#include "nextbot/locomotion.hpp"
#include "nextbot/intention.hpp"
#include "nextbot/vision.hpp"
#include "nextbot/behavior.hpp"

namespace natives::nextbot {

template<typename T>
inline T* Get(IPluginContext* context, const cell_t param) {	
	T* bot = (T*)PawnAddressToPtr(param, context);
	if (!bot) {
		context->ThrowNativeError("Bot is a null ptr!");
		return nullptr;
	}
	return bot;
}

cell_t Reset(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	bot->Reset();
	return 0;
}

cell_t Update(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	bot->Update();
	return 0;
}

cell_t Upkeep(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	bot->Upkeep();
	return 0;
}

cell_t IsRemovedOnReset(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	return bot->IsRemovedOnReset() ? 1 : 0;
}

cell_t GetEntity(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	return gamehelpers->EntityToBCompatRef(bot->GetEntity());
}

cell_t GetNextBotCombatCharacter(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	return gamehelpers->EntityToBCompatRef(bot->GetNextBotCombatCharacter());
}

cell_t GetLocomotionInterface(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	return PtrToPawnAddress(bot->GetLocomotionInterface(), context);
}

cell_t GetBodyInterface(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	return PtrToPawnAddress(bot->GetBodyInterface(), context);
}

cell_t GetIntentionInterface(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	return PtrToPawnAddress(bot->GetIntentionInterface(), context);
}

cell_t GetVisionInterface(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	return PtrToPawnAddress(bot->GetVisionInterface(), context);
}

cell_t SetPosition(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	cell_t *posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);

	return bot->SetPosition(pos) ? 1 : 0;
}

cell_t GetPosition(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	cell_t *posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos = bot->GetPosition();
	VectorToPawnVector(posAddr, pos);

	return 0;
}

cell_t IsEnemy(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[2]);
	}

	return bot->IsEnemy(entity) ? 1 : 0;
}

cell_t IsFriend(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[2]);
	}

	return bot->IsFriend(entity) ? 1 : 0;
}

cell_t IsSelf(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[2]);
	}

	return bot->IsSelf(entity) ? 1 : 0;
}

cell_t IsAbleToClimbOnto(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[2]);
	}

	return bot->IsAbleToClimbOnto(entity) ? 1 : 0;
}

cell_t IsAbleToBreak(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[2]);
	}

	return bot->IsAbleToBreak(entity) ? 1 : 0;
}

cell_t IsAbleToBlockMovementOf(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	auto target = Get<INextBot>(context, params[2]);
	if (!target) {
		return 0;
	}

	return bot->IsAbleToBlockMovementOf(target) ? 1 : 0;
}

cell_t ShouldTouch(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %d", params[2]);
	}

	return bot->ShouldTouch(entity) ? 1 : 0;
}

cell_t IsImmobile(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	return bot->IsImmobile() ? 1 : 0;
}

cell_t GetImmobileDuration(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	return sp_ftoc(bot->GetImmobileDuration());
}

cell_t ClearImmobileStatus(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}
	bot->ClearImmobileStatus();
	return 0;
}

cell_t GetImmobileSpeedThreshold(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	return sp_ftoc(bot->GetImmobileSpeedThreshold());
}

cell_t GetCurrentPath(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	return PtrToPawnAddress(bot->GetCurrentPath(), context);
}

cell_t SetCurrentPath(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	bot->SetCurrentPath((PathFollower*)PawnAddressToPtr(params[2], context));
	return 0;
}

cell_t NotifyPathDestruction(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	bot->NotifyPathDestruction((PathFollower*)PawnAddressToPtr(params[2], context));
	return 0;
}

cell_t IsRangeLessThan(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}

	return bot->IsRangeLessThan(entity, sp_ctof(params[3])) ? 1 : 0;
}

cell_t IsRangeLessThanEx(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	cell_t *posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);

	return bot->IsRangeLessThan(pos, sp_ctof(params[3])) ? 1 : 0;
}

cell_t IsRangeGreaterThan(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}

	return bot->IsRangeGreaterThan(entity, sp_ctof(params[3])) ? 1 : 0;
}

cell_t IsRangeGreaterThanEx(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	cell_t *posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);

	return bot->IsRangeGreaterThan(pos, sp_ctof(params[3])) ? 1 : 0;
}

cell_t GetRangeTo(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}

	return sp_ftoc(bot->GetRangeTo(entity));
}

cell_t GetRangeToEx(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	cell_t *posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);

	return sp_ftoc(bot->GetRangeTo(pos));
}

cell_t GetRangeSquaredTo(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity) {
		return context->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}

	return sp_ftoc(bot->GetRangeSquaredTo(entity));
}

cell_t GetRangeSquaredToEx(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	cell_t *posAddr;
	context->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);

	return sp_ftoc(bot->GetRangeSquaredTo(pos));
}

cell_t IsDebugging(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	return bot->IsDebugging(params[2]) ? 1 : 0;
}

cell_t GetDebugIdentifier(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	context->StringToLocal(params[2], static_cast<size_t>(params[3]), bot->GetDebugIdentifier());
	return 0;
}

cell_t IsDebugFilterMatch(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	char *name;
	context->LocalToString(params[2], &name);

	return bot->IsDebugFilterMatch(name) ? 1 : 0;
}

cell_t DisplayDebugText(IPluginContext* context, const cell_t* params) {
	auto bot = Get<INextBot>(context, params[1]);
	if (!bot) {
		return 0;
	}

	char *name;
	context->LocalToString(params[2], &name);

	bot->DisplayDebugText(name);
	return 0;
}

cell_t ToolsNextBot(IPluginContext* context, const cell_t* params) {
	CBaseCombatCharacter* entity = (CBaseCombatCharacter*)PawnAddressToPtr(params[1], context);
	if (!entity) {
		return context->ThrowNativeError("Invalid entity address %x", params[1]);
	}

	return PtrToPawnAddress(new class ToolsNextBot(entity), context);
}	

void setup(std::vector<sp_nativeinfo_t>& natives) {
	component::setup(natives);
	eventresponder::setup(natives);
	intention::setup(natives);
	vision::setup(natives);
	locomotion::setup(natives);
	path::setup(natives);
	body::setup(natives);
	behavior::setup(natives);

	sp_nativeinfo_t list[] = {
		{"INextBot.Reset", Reset},
		{"INextBot.Update", Update},
		{"INextBot.Upkeep", Upkeep},
		{"INextBot.IsRemovedOnReset", IsRemovedOnReset},
		{"INextBot.GetEntity", GetEntity},
		{"INextBot.GetNextBotCombatCharacter", GetNextBotCombatCharacter},
		{"INextBot.GetLocomotionInterface", GetLocomotionInterface},
		{"INextBot.GetBodyInterface", GetBodyInterface},
		{"INextBot.GetIntentionInterface", GetIntentionInterface},
		{"INextBot.GetVisionInterface", GetVisionInterface},
		{"INextBot.SetPosition", SetPosition},
		{"INextBot.GetPosition", GetPosition},
		{"INextBot.IsEnemy", IsEnemy},
		{"INextBot.IsFriend", IsFriend},
		{"INextBot.IsSelf", IsSelf},
		{"INextBot.IsAbleToClimbOnto", IsAbleToClimbOnto},
		{"INextBot.IsAbleToBreak", IsAbleToBreak},
		{"INextBot.IsAbleToBlockMovementOf", IsAbleToBlockMovementOf},
		{"INextBot.ShouldTouch", ShouldTouch},
		{"INextBot.IsImmobile", IsImmobile},
		{"INextBot.GetImmobileDuration", GetImmobileDuration},
		{"INextBot.ClearImmobileStatus", ClearImmobileStatus},
		{"INextBot.GetImmobileSpeedThreshold", GetImmobileSpeedThreshold},
		{"INextBot.GetCurrentPath", GetCurrentPath},
		{"INextBot.SetCurrentPath", SetCurrentPath},
		{"INextBot.NotifyPathDestruction", NotifyPathDestruction},
		{"INextBot.IsRangeLessThan", IsRangeLessThan},
		{"INextBot.IsRangeLessThanEx", IsRangeLessThanEx},
		{"INextBot.IsRangeGreaterThan", IsRangeGreaterThan},
		{"INextBot.IsRangeGreaterThanEx", IsRangeGreaterThanEx},
		{"INextBot.GetRangeTo", GetRangeTo},
		{"INextBot.GetRangeToEx", GetRangeToEx},
		{"INextBot.GetRangeSquaredTo", GetRangeSquaredTo},
		{"INextBot.GetRangeSquaredToEx", GetRangeSquaredToEx},
		{"INextBot.IsDebugging", IsDebugging},
		{"INextBot.GetDebugIdentifier", GetDebugIdentifier},
		{"INextBot.IsDebugFilterMatch", IsDebugFilterMatch},
		{"INextBot.DisplayDebugText", DisplayDebugText},

		// To-Do: move this
		{"ToolsNextBot.ToolsNextBot", ToolsNextBot},
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}