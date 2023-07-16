#include "eventresponder.hpp"

#include "NextBotEventResponderInterface.h"
#include "sourcesdk/baseentity.h"

namespace natives::nextbot::eventresponder  {

inline INextBotEventResponder* Get(IPluginContext* context, const cell_t param) {	
	INextBotEventResponder* responder = (INextBotEventResponder*)PawnAddressToPtr(param);
	if (!responder) {
		context->ThrowNativeError("Event responder ptr is null!");
		return nullptr;
	}
	return responder;
}

cell_t FirstContainedResponder(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	return PtrToPawnAddress(responder->FirstContainedResponder());
}

cell_t NextContainedResponder(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	auto responder2 = Get(context, params[2]);
	if (!responder2) {
		return 0;
	}
	return PtrToPawnAddress(responder->NextContainedResponder(responder2));
}

cell_t OnLeaveGround(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnLeaveGround(entity);
	return 0;
}

cell_t OnLandOnGround(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnLandOnGround(entity);
	return 0;
}

// TO-DO: OnContact

cell_t OnMoveToSuccess(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	Path* path = (Path*)PawnAddressToPtr(params[2]);
	if (!path) {
		return context->ThrowNativeError("Path is null!");
	}

	responder->OnMoveToSuccess(path);
	return 0;
}

cell_t OnMoveToFailure(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	Path* path = (Path*)PawnAddressToPtr(params[2]);
	if (!path) {
		return context->ThrowNativeError("Path is null!");
	}

	responder->OnMoveToFailure(path, (MoveToFailureType)params[3]);
	return 0;
}

cell_t OnStuck(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnStuck();
	return 0;
}

cell_t OnUnStuck(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnUnStuck();
	return 0;
}

cell_t OnPostureChanged(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnPostureChanged();
	return 0;
}

cell_t OnAnimationActivityComplete(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnAnimationActivityComplete(params[2]);
	return 0;
}

cell_t OnAnimationActivityInterrupted(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnAnimationActivityInterrupted(params[2]);
	return 0;
}

// TO-DO: OnAnimationEvent

cell_t OnIgnite(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnIgnite();
	return 0;
}

// TO-DO: OnInjured

// TO-DO: OnKilled

// TO-DO: OnOtherKilled

cell_t OnSight(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnSight(entity);
	return 0;
}

cell_t OnLostSight(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnLostSight(entity);
	return 0;
}

// TO-DO: OnSound

// TO-DO: OnSpokeConcept

cell_t OnWeaponFired(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	auto bcc = entity->MyCombatCharacterPointer();
	if (!bcc) {
		return context->ThrowNativeError("Entity %d is not a CBaseCombatCharacter!", params[2]);
	}

	CBaseEntity* weapon = gamehelpers->ReferenceToEntity(params[3]);
	if (!weapon && params[2] != -1) {
		return context->ThrowNativeError("Weapon entity %d is invalid!", params[3]);
	}

	responder->OnWeaponFired(bcc, weapon);
	return 0;
}

cell_t OnNavAreaChanged(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnNavAreaChanged((CNavArea*)PawnAddressToPtr(params[2]), (CNavArea*)PawnAddressToPtr(params[3]));
	return 0;
}

cell_t OnModelChanged(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnModelChanged();
	return 0;
}

cell_t OnPickUp(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* item = gamehelpers->ReferenceToEntity(params[2]);
	if (!item && params[2] != -1) {
		return context->ThrowNativeError("Item entity %d is invalid!", params[2]);
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[3]);
	if (!entity && params[3] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[3]);
	}

	auto bcc = entity->MyCombatCharacterPointer();
	if (!bcc) {
		return context->ThrowNativeError("Entity %d is not a CBaseCombatCharacter!", params[3]);
	}

	responder->OnPickUp(item, bcc);
	return 0;
}

cell_t OnDrop(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnDrop(entity);
	return 0;
}

cell_t OnActorEmoted(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	auto bcc = entity->MyCombatCharacterPointer();
	if (!bcc) {
		return context->ThrowNativeError("Entity %d is not a CBaseCombatCharacter!", params[2]);
	}

	responder->OnActorEmoted(bcc, params[3]);
	return 0;
}

cell_t OnCommandAttack(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnCommandAttack(entity);
	return 0;
}

cell_t OnCommandApproach(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	cell_t* vecAdd;
	context->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec;
	PawnVectorToVector(vecAdd, vec);
	responder->OnCommandApproach(vec, sp_ctof(params[3]));
	return 0;
}

cell_t OnCommandApproachEntity(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnCommandApproach(entity);
	return 0;
}

cell_t OnCommandRetreat(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnCommandRetreat(entity, sp_ctof(params[3]));
	return 0;
}

cell_t OnCommandPause(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnCommandPause(sp_ctof(params[2]));
	return 0;
}

cell_t OnCommandResume(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnCommandResume();
	return 0;
}

cell_t OnCommandString(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	char* cmd = nullptr;
	context->LocalToStringNULL(params[2], &cmd);
	responder->OnCommandString(cmd);
	return 0;
}

cell_t OnShoved(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnShoved(entity);
	return 0;
}

cell_t OnBlinded(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1) {
		return context->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	responder->OnBlinded(entity);
	return 0;
}

cell_t OnTerritoryContested(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnTerritoryContested(params[2]);
	return 0;
}

cell_t OnTerritoryCaptured(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnTerritoryCaptured(params[2]);
	return 0;
}

cell_t OnTerritoryLost(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnTerritoryLost(params[2]);
	return 0;
}

cell_t OnWin(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnWin();
	return 0;
}

cell_t OnLose(IPluginContext* context, const cell_t* params) {
	auto responder = Get(context, params[1]);
	if (!responder) {
		return 0;
	}

	responder->OnLose();
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {

	sp_nativeinfo_t list[] = {
		{"INextBotEventResponder.FirstContainedResponder", FirstContainedResponder},
		{"INextBotEventResponder.NextContainedResponder", NextContainedResponder},
		{"INextBotEventResponder.OnLeaveGround", OnLeaveGround},
		{"INextBotEventResponder.OnLandOnGround", OnLandOnGround},
		{"INextBotEventResponder.OnMoveToSuccess", OnMoveToSuccess},
		{"INextBotEventResponder.OnMoveToFailure", OnMoveToFailure},
		{"INextBotEventResponder.OnStuck", OnStuck},
		{"INextBotEventResponder.OnUnStuck", OnUnStuck},
		{"INextBotEventResponder.OnPostureChanged", OnPostureChanged},
		{"INextBotEventResponder.OnAnimationActivityComplete", OnAnimationActivityComplete},
		{"INextBotEventResponder.OnAnimationActivityInterrupted", OnAnimationActivityInterrupted},
		//{"INextBotEventResponder.OnAnimationEvent", OnAnimationEvent},
		{"INextBotEventResponder.OnIgnite", OnIgnite},
		//{"INextBotEventResponder.OnInjured", OnInjured},
		//{"INextBotEventResponder.OnKilled", OnKilled},
		//{"INextBotEventResponder.OnOtherKilled", OnOtherKilled},
		{"INextBotEventResponder.OnSight", OnSight},
		{"INextBotEventResponder.OnLostSight", OnLostSight},
		//{"INextBotEventResponder.OnSound", OnSound},
		//{"INextBotEventResponder.OnSpokeConcept", OnSpokeConcept},
		{"INextBotEventResponder.OnWeaponFired", OnWeaponFired},
		{"INextBotEventResponder.OnNavAreaChanged", OnNavAreaChanged},
		{"INextBotEventResponder.OnModelChanged", OnModelChanged},
		{"INextBotEventResponder.OnPickUp", OnPickUp},
		{"INextBotEventResponder.OnDrop", OnDrop},
		{"INextBotEventResponder.OnActorEmoted", OnActorEmoted},
		{"INextBotEventResponder.OnCommandAttack", OnCommandAttack},
		{"INextBotEventResponder.OnCommandApproach", OnCommandApproach},
		{"INextBotEventResponder.OnCommandApproachEntity", OnCommandApproachEntity},
		{"INextBotEventResponder.OnCommandRetreat", OnCommandRetreat},
		{"INextBotEventResponder.OnCommandPause", OnCommandPause},
		{"INextBotEventResponder.OnCommandResume", OnCommandResume},
		{"INextBotEventResponder.OnCommandString", OnCommandString},
		{"INextBotEventResponder.OnShoved", OnShoved},
		{"INextBotEventResponder.OnBlinded", OnBlinded},
		{"INextBotEventResponder.OnTerritoryContested", OnTerritoryContested},
		{"INextBotEventResponder.OnTerritoryCaptured", OnTerritoryCaptured},
		{"INextBotEventResponder.OnTerritoryLost", OnTerritoryLost},
		{"INextBotEventResponder.OnWin", OnWin},
		{"INextBotEventResponder.OnLose", OnLose},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}