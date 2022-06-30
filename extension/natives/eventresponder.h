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

EVENTRESPONDERNATIVE(OnLeaveGround)
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}
	pResponder->OnLeaveGround(entity);
	return 0;
}

EVENTRESPONDERNATIVE(OnLandOnGround)
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}
	pResponder->OnLandOnGround(entity);
	return 0;
}

// TO-DO: OnContact

EVENTRESPONDERNATIVE(OnMoveToSuccess)
	pResponder->OnMoveToSuccess((Path*)params[2]);
	return 0;
}

EVENTRESPONDERNATIVE(OnMoveToFailure)
	pResponder->OnMoveToFailure((Path*)params[2], (MoveToFailureType)params[3]);
	return 0;
}

EVENTRESPONDERNATIVE(OnStuck)
	pResponder->OnStuck();
	return 0;
}

EVENTRESPONDERNATIVE(OnUnStuck)
	pResponder->OnUnStuck();
	return 0;
}

EVENTRESPONDERNATIVE(OnPostureChanged)
	pResponder->OnPostureChanged();
	return 0;
}

EVENTRESPONDERNATIVE(OnAnimationActivityComplete)
	pResponder->OnAnimationActivityComplete(params[2]);
	return 0;
}

EVENTRESPONDERNATIVE(OnAnimationActivityInterrupted)
	pResponder->OnAnimationActivityInterrupted(params[2]);
	return 0;
}

// TO-DO: OnAnimationEvent

EVENTRESPONDERNATIVE(OnIgnite)
	pResponder->OnIgnite();
	return 0;
}

// TO-DO: OnInjured

// TO-DO: OnKilled

// TO-DO: OnOtherKilled

EVENTRESPONDERNATIVE(OnSight)
	CBaseEntityHack* entity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	pResponder->OnSight(entity);
	return 0;
}

EVENTRESPONDERNATIVE(OnLostSight)
	CBaseEntityHack* entity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	pResponder->OnLostSight(entity);
	return 0;
}

// TO-DO: OnSound

// TO-DO: OnSpokeConcept

EVENTRESPONDERNATIVE(OnWeaponFired)
	CBaseEntityHack* bcc = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!bcc && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	if (bcc && !bcc->MyCombatCharacterPointer())
	{
		return pContext->ThrowNativeError("Entity %d is not a CBaseCombatCharacter!", params[2]);
	}

	CBaseEntityHack* entity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[3]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[3]);
	}

	pResponder->OnWeaponFired(bcc->MyCombatCharacterPointer(), entity);
	return 0;
}

EVENTRESPONDERNATIVE(OnNavAreaChanged)
	pResponder->OnNavAreaChanged((CNavArea*)params[2], (CNavArea*)params[3]);
	return 0;
}

EVENTRESPONDERNATIVE(OnModelChanged)
	pResponder->OnModelChanged();
	return 0;
}

EVENTRESPONDERNATIVE(OnPickUp)
	CBaseEntityHack* entity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	CBaseEntityHack* bcc = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[3]);
	if (!bcc && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[3]);
	}

	if (bcc && !bcc->MyCombatCharacterPointer())
	{
		return pContext->ThrowNativeError("Entity %d is not a CBaseCombatCharacter!", params[3]);
	}

	pResponder->OnPickUp(entity, bcc->MyCombatCharacterPointer());
	return 0;
}

EVENTRESPONDERNATIVE(OnDrop)
	CBaseEntityHack* entity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	pResponder->OnDrop(entity);
	return 0;
}

EVENTRESPONDERNATIVE(OnActorEmoted)
	CBaseEntityHack* entity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	if (entity && !entity->MyCombatCharacterPointer())
	{
		return pContext->ThrowNativeError("Entity %d is not a CBaseCombatCharacter!", params[2]);
	}

	pResponder->OnActorEmoted(entity->MyCombatCharacterPointer(), params[3]);
	return 0;
}

EVENTRESPONDERNATIVE(OnCommandAttack)
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	pResponder->OnCommandAttack(entity);
	return 0;
}

EVENTRESPONDERNATIVE(OnCommandApproach)
	cell_t* vecAdd;
	pContext->LocalToPhysAddr(params[2], &vecAdd);
	Vector vec;
	PawnVectorToVector(vecAdd, vec);
	pResponder->OnCommandApproach(vec, sp_ctof(params[3]));
	return 0;
}

EVENTRESPONDERNATIVE(OnCommandApproachEntity)
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	pResponder->OnCommandApproach(entity);
	return 0;
}

EVENTRESPONDERNATIVE(OnCommandRetreat)
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	pResponder->OnCommandRetreat(entity, sp_ctof(params[3]));
	return 0;
}

EVENTRESPONDERNATIVE(OnCommandPause)
	pResponder->OnCommandPause(sp_ctof(params[2]));
	return 0;
}

EVENTRESPONDERNATIVE(OnCommandResume)
	pResponder->OnCommandResume();
	return 0;
}

EVENTRESPONDERNATIVE(OnCommandString)
	char* context = nullptr;
	pContext->LocalToStringNULL(params[2], &context);
	pResponder->OnCommandString(context);
	return 0;
}

EVENTRESPONDERNATIVE(OnShoved)
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	pResponder->OnShoved(entity);
	return 0;
}

EVENTRESPONDERNATIVE(OnBlinded)
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[2]);
	if (!entity && params[2] != -1)
	{
		return pContext->ThrowNativeError("Entity %d is invalid!", params[2]);
	}

	pResponder->OnBlinded(entity);
	return 0;
}

EVENTRESPONDERNATIVE(OnTerritoryContested)
	pResponder->OnTerritoryContested(params[2]);
	return 0;
}

EVENTRESPONDERNATIVE(OnTerritoryCaptured)
	pResponder->OnTerritoryCaptured(params[2]);
	return 0;
}

EVENTRESPONDERNATIVE(OnTerritoryLost)
	pResponder->OnTerritoryLost(params[2]);
	return 0;
}

EVENTRESPONDERNATIVE(OnWin)
	pResponder->OnWin();
	return 0;
}

EVENTRESPONDERNATIVE(OnLose)
	pResponder->OnLose();
	return 0;
}

#endif