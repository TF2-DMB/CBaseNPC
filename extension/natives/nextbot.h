#ifndef NATIVES_NEXTBOT_H_INCLUDED_
#define NATIVES_NEXTBOT_H_INCLUDED_

#pragma once

#include "NextBotInterface.h"
#include "helpers.h"

#define NEXTBOTNATIVE(name) \
	cell_t INextBot_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		INextBot *pNextBot = (INextBot *)(params[1]); \
		if(!pNextBot) \
		{ \
			return pContext->ThrowNativeError("Invalid INextBot %x", params[1]); \
		} \


NEXTBOTNATIVE(Reset)
	pNextBot->Reset();
	return 0;
}

NEXTBOTNATIVE(Update)
	pNextBot->Update();
	return 0;
}

NEXTBOTNATIVE(Upkeep)
	pNextBot->Upkeep();
	return 0;
}

NEXTBOTNATIVE(IsRemovedOnReset)
	return (cell_t)(pNextBot->IsRemovedOnReset());
}

NEXTBOTNATIVE(GetEntity)
	return gamehelpers->EntityToBCompatRef((CBaseEntity *)pNextBot->GetEntity());
}

NEXTBOTNATIVE(GetNextBotCombatCharacter)
	return gamehelpers->EntityToBCompatRef((CBaseEntity *)pNextBot->GetNextBotCombatCharacter());
}

NEXTBOTNATIVE(GetLocomotionInterface)
	return (cell_t)(pNextBot->GetLocomotionInterface());
}

NEXTBOTNATIVE(GetBodyInterface)
	return (cell_t)(pNextBot->GetBodyInterface());
}

NEXTBOTNATIVE(GetIntentionInterface)
	return (cell_t)(pNextBot->GetIntentionInterface());
}

NEXTBOTNATIVE(GetVisionInterface)
	return (cell_t)(pNextBot->GetVisionInterface());
}

NEXTBOTNATIVE(SetPosition)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);
	return (cell_t)(pNextBot->SetPosition(pos));
}

NEXTBOTNATIVE(GetPosition)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos = pNextBot->GetPosition();
	VectorToPawnVector(posAddr, pos);
	return 0;
}

NEXTBOTNATIVE(IsEnemy)
	CBaseEntityHack *pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pNextBot->IsEnemy(pEntity));
}

NEXTBOTNATIVE(IsFriend)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pNextBot->IsFriend(pEntity));
}

NEXTBOTNATIVE(IsSelf)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pNextBot->IsSelf(pEntity));
}

NEXTBOTNATIVE(IsAbleToClimbOnto)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pNextBot->IsAbleToClimbOnto(pEntity));
}

NEXTBOTNATIVE(IsAbleToBreak)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pNextBot->IsAbleToBreak(pEntity));
}

NEXTBOTNATIVE(IsAbleToBlockMovementOf)
	INextBot *pOtherNextBot = (INextBot *)(params[2]);
	if(!pOtherNextBot)
	{
		return pContext->ThrowNativeError("Invalid INextBot %x", params[2]);
	}
	return (cell_t)(pNextBot->IsAbleToBlockMovementOf(pOtherNextBot));
}

NEXTBOTNATIVE(ShouldTouch)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pNextBot->ShouldTouch(pEntity));
}

NEXTBOTNATIVE(IsImmobile)
	return (cell_t)(pNextBot->IsImmobile());
}

NEXTBOTNATIVE(GetImmobileDuration)
	return sp_ftoc(pNextBot->GetImmobileDuration());
}

NEXTBOTNATIVE(ClearImmobileStatus)
	pNextBot->ClearImmobileStatus();
	return 0;
}

NEXTBOTNATIVE(GetImmobileSpeedThreshold)
	return sp_ftoc(pNextBot->GetImmobileSpeedThreshold());
}

NEXTBOTNATIVE(GetCurrentPath)
	return (cell_t)(pNextBot->GetCurrentPath());
}

NEXTBOTNATIVE(SetCurrentPath)
	PathFollower *pPathFollow = (PathFollower *)(params[2]);
	if(!pPathFollow)
	{
		return pContext->ThrowNativeError("Invalid PathFollower %x", params[2]);
	}
	pNextBot->SetCurrentPath(pPathFollow);
	return 0;
}

NEXTBOTNATIVE(NotifyPathDestruction)
	PathFollower *pPathFollow = (PathFollower *)(params[2]);
	if(!pPathFollow)
	{
		return pContext->ThrowNativeError("Invalid PathFollower %x", params[2]);
	}
	pNextBot->NotifyPathDestruction(pPathFollow);
	return 0;
}

NEXTBOTNATIVE(IsRangeLessThan)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pNextBot->IsRangeLessThan(pEntity, sp_ctof(params[3])));
}

NEXTBOTNATIVE(IsRangeLessThanEx)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);
	return (cell_t)(pNextBot->IsRangeLessThan(pos, sp_ctof(params[3])));
}

NEXTBOTNATIVE(IsRangeGreaterThan)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return (cell_t)(pNextBot->IsRangeGreaterThan(pEntity, sp_ctof(params[3])));
}

NEXTBOTNATIVE(IsRangeGreaterThanEx)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);
	return (cell_t)(pNextBot->IsRangeGreaterThan(pos, sp_ctof(params[3])));
}

NEXTBOTNATIVE(GetRangeTo)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return sp_ftoc(pNextBot->GetRangeTo(pEntity));
}

NEXTBOTNATIVE(GetRangeToEx)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);
	return sp_ftoc(pNextBot->GetRangeTo(pos));
}

NEXTBOTNATIVE(GetRangeSquaredTo)
	CBaseEntityHack* pEntity = (CBaseEntityHack*)gamehelpers->ReferenceToEntity(params[2]);
	if(!pEntity)
	{
		return pContext->ThrowNativeError("Invalid Entity Reference/Index %i", params[2]);
	}
	return sp_ftoc(pNextBot->GetRangeSquaredTo(pEntity));
}

NEXTBOTNATIVE(GetRangeSquaredToEx)
	cell_t *posAddr;
	pContext->LocalToPhysAddr(params[2], &posAddr);
	Vector pos;
	PawnVectorToVector(posAddr, pos);
	return sp_ftoc(pNextBot->GetRangeSquaredTo(pos));
}

NEXTBOTNATIVE(IsDebugging)
	return (cell_t)(pNextBot->IsDebugging(params[2]));
}

NEXTBOTNATIVE(GetDebugIdentifier)
	pContext->StringToLocal(params[2], static_cast<size_t>(params[3]), pNextBot->GetDebugIdentifier());
	return 0;
}

NEXTBOTNATIVE(IsDebugFilterMatch)
	char *name;
	pContext->LocalToString(params[2], &name);
	return (cell_t)(pNextBot->IsDebugFilterMatch(name));
}

NEXTBOTNATIVE(DisplayDebugText)
	char *name;
	pContext->LocalToString(params[2], &name);
	pNextBot->DisplayDebugText(name);
	return 0;
}

#endif