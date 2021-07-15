#ifndef NEXTBOT_NEXTBOTEVENTRESPONDERINTERFACE_H
#define NEXTBOT_NEXTBOTEVENTRESPONDERINTERFACE_H

#pragma once

#include "sourcesdk/nav.h"
#include <npcevent.h>
#include <enginecallback.h>
#include <shareddefs.h>
#include <util_shared.h>
#include <isaverestore.h>
#include <AI_Criteria.h>
#include <utlvector.h>

class CBaseCombatWeapon;
class CBaseCombatCharacterHack;
class Path;
class CTakeDamageInfo;

enum class MoveToFailureType
{
	FAIL_NO_PATH_EXISTS = 0,
	FAIL_STUCK        = 1,
	FAIL_FELL_OFF     = 2,
};

typedef const char* AIConcept_t;
	
class INextBotEventResponder
{
public:
	virtual ~INextBotEventResponder() {}
	
	virtual INextBotEventResponder *FirstContainedResponder() const { return nullptr; };
	virtual INextBotEventResponder *NextContainedResponder(INextBotEventResponder *prev) const { return nullptr; };

	virtual void OnLeaveGround(CBaseEntity*);
	virtual void OnLandOnGround(CBaseEntity*);
	
	virtual void OnContact(CBaseEntity*, CGameTrace*);
	
	virtual void OnMoveToSuccess(const Path*);
	virtual void OnMoveToFailure(const Path*, MoveToFailureType);
	
	virtual void OnStuck(void);
	virtual void OnUnStuck(void);
	
	virtual void OnPostureChanged(void);
	virtual void OnAnimationActivityComplete(int);
	virtual void OnAnimationActivityInterrupted(int);
	virtual void OnAnimationEvent(animevent_t*);
	
	virtual void OnIgnite(void);
	virtual void OnInjured(const CTakeDamageInfo&);
	virtual void OnKilled(const CTakeDamageInfo&);
	virtual void OnOtherKilled(CBaseCombatCharacterHack*, const CTakeDamageInfo&);
	
	virtual void OnSight(CBaseEntity*);
	virtual void OnLostSight(CBaseEntity*);
	virtual void OnSound(CBaseEntity*, const Vector&, KeyValues *);
	virtual void OnSpokeConcept(CBaseCombatCharacterHack*, AIConcept_t, AI_Response *);
	virtual void OnWeaponFired(CBaseCombatCharacterHack*, CBaseEntity* );
	
	virtual void OnNavAreaChanged(CNavArea*, CNavArea*);
	virtual void OnModelChanged(void);
	virtual void OnPickUp(CBaseEntity*, CBaseCombatCharacterHack*);
	virtual void OnDrop(CBaseEntity*);

	virtual void OnActorEmoted(CBaseCombatCharacterHack*, int);
	
	virtual void OnCommandAttack(CBaseEntity*);
	virtual void OnCommandApproach(const Vector&, float);
	virtual void OnCommandApproach(CBaseEntity*);
	virtual void OnCommandRetreat(CBaseEntity*, float);
	virtual void OnCommandPause(float);
	virtual void OnCommandResume(void);
	virtual void OnCommandString(const char*);
	
	virtual void OnShoved(CBaseEntity*);
	virtual void OnBlinded(CBaseEntity*);
	
	virtual void OnTerritoryContested(int);
	virtual void OnTerritoryCaptured(int);
	virtual void OnTerritoryLost(int);
	
	virtual void OnWin(void);
	virtual void OnLose(void);
};

inline void INextBotEventResponder::OnLeaveGround(CBaseEntity *ground)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnLeaveGround(ground);
	}
}

inline void INextBotEventResponder::OnLandOnGround(CBaseEntity *ground)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnLandOnGround(ground);
	}
}

inline void INextBotEventResponder::OnContact(CBaseEntity *other, CGameTrace *result)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnContact(other, result);
	}
}

inline void INextBotEventResponder::OnMoveToSuccess(const Path *path)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnMoveToSuccess(path);
	}
}

inline void INextBotEventResponder::OnMoveToFailure(const Path *path, MoveToFailureType reason)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnMoveToFailure(path, reason);
	}
}

inline void INextBotEventResponder::OnStuck(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnStuck();
	}
}

inline void INextBotEventResponder::OnUnStuck(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnUnStuck();
	}
}

inline void INextBotEventResponder::OnPostureChanged(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnPostureChanged();
	}
}

inline void INextBotEventResponder::OnAnimationActivityComplete(int activity)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnAnimationActivityComplete(activity);
	}
}

inline void INextBotEventResponder::OnAnimationActivityInterrupted(int activity)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnAnimationActivityInterrupted(activity);
	}
}

inline void INextBotEventResponder::OnAnimationEvent(animevent_t *event)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnAnimationEvent(event);
	}
}

inline void INextBotEventResponder::OnIgnite(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnIgnite();
	}
}

inline void INextBotEventResponder::OnInjured(const CTakeDamageInfo& info)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnInjured(info);
	}
}

inline void INextBotEventResponder::OnKilled(const CTakeDamageInfo& info)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnKilled(info);
	}
}

inline void INextBotEventResponder::OnOtherKilled(CBaseCombatCharacterHack* victim, const CTakeDamageInfo& info)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnOtherKilled(victim, info);
	}
}

inline void INextBotEventResponder::OnSight(CBaseEntity* subject)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnSight(subject);
	}
}

inline void INextBotEventResponder::OnLostSight(CBaseEntity* subject)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnLostSight(subject);
	}
}

inline void INextBotEventResponder::OnSound(CBaseEntity* source, const Vector& pos, KeyValues* keys)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnSound(source, pos, keys);
	}
}

inline void INextBotEventResponder::OnSpokeConcept(CBaseCombatCharacterHack* who, AIConcept_t concept, AI_Response* response)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnSpokeConcept(who, concept, response);
	}
}

inline void INextBotEventResponder::OnWeaponFired(CBaseCombatCharacterHack* whoFired, CBaseEntity* weapon)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnWeaponFired(whoFired, weapon);
	}
}

inline void INextBotEventResponder::OnNavAreaChanged(CNavArea* newArea, CNavArea* oldArea)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnNavAreaChanged(newArea, oldArea);
	}
}

inline void INextBotEventResponder::OnModelChanged(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnModelChanged();
	}
}

inline void INextBotEventResponder::OnPickUp(CBaseEntity* item, CBaseCombatCharacterHack* giver)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnPickUp(item, giver);
	}
}

inline void INextBotEventResponder::OnDrop(CBaseEntity* item)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnDrop(item);
	}
}

inline void INextBotEventResponder::OnActorEmoted(CBaseCombatCharacterHack* ent, int emote)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnActorEmoted(ent, emote);
	}
}

inline void INextBotEventResponder::OnShoved(CBaseEntity *pusher)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnShoved(pusher);
	}
}

inline void INextBotEventResponder::OnBlinded(CBaseEntity* blinder)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnBlinded(blinder);
	}
}

inline void INextBotEventResponder::OnCommandAttack(CBaseEntity *victim)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandAttack(victim);
	}
}

inline void INextBotEventResponder::OnCommandApproach(const Vector &pos, float range)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandApproach(pos, range);
	}
}

inline void INextBotEventResponder::OnCommandApproach(CBaseEntity *goal)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandApproach( goal );
	}	
}

inline void INextBotEventResponder::OnCommandRetreat(CBaseEntity *threat, float range)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandRetreat( threat, range );
	}	
}

inline void INextBotEventResponder::OnCommandPause(float duration)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandPause( duration );
	}	
}

inline void INextBotEventResponder::OnCommandResume(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandResume();
	}	
}

inline void INextBotEventResponder::OnCommandString(const char *command)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandString( command );
	}	
}

inline void INextBotEventResponder::OnTerritoryContested(int territoryID)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnTerritoryContested( territoryID );
	}
}

inline void INextBotEventResponder::OnTerritoryCaptured(int territoryID)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnTerritoryCaptured( territoryID );
	}
}

inline void INextBotEventResponder::OnTerritoryLost(int territoryID)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnTerritoryLost(territoryID);
	}
}

inline void INextBotEventResponder::OnWin(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnWin();
	}	
}

inline void INextBotEventResponder::OnLose(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnLose();
	}	
}

#ifdef DOTA_SERVER_DLL
inline void INextBotEventResponder::OnCommandMoveTo(const Vector &pos)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandMoveTo( pos );
	}
}

inline void INextBotEventResponder::OnCommandMoveToAggressive(const Vector &pos)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandMoveToAggressive(pos);
	}
}

inline void INextBotEventResponder::OnCommandAttack(CBaseEntity *victim, bool bDeny)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCommandAttack(victim, bDeny);
	}
}

inline void INextBotEventResponder::OnCastAbilityNoTarget(CDOTABaseAbility *ability)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCastAbilityNoTarget(ability);
	}
}

inline void INextBotEventResponder::OnCastAbilityOnPosition(CDOTABaseAbility *ability, const Vector &pos)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCastAbilityOnPosition(ability, pos);
	}
}

inline void INextBotEventResponder::OnCastAbilityOnTarget(CDOTABaseAbility *ability, CBaseEntity *target)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnCastAbilityOnTarget(ability, target);
	}
}

inline void INextBotEventResponder::OnDropItem(const Vector &pos, CBaseEntity *item)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnDropItem(pos, item);
	}
}

inline void INextBotEventResponder::OnPickupItem(CBaseEntity *item)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnPickupItem(item);
	}
}

inline void INextBotEventResponder::OnPickupRune(CBaseEntity *item)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnPickupRune(item);
	}
}

inline void INextBotEventResponder::OnStop(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnStop();
	}
}

inline void INextBotEventResponder::OnFriendThreatened(CBaseEntity *friendly, CBaseEntity *threat)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnFriendThreatened(friendly, threat);
	}
}

inline void INextBotEventResponder::OnCancelAttack(CBaseEntity *pTarget)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		sub->OnCancelAttack( pTarget );
	}
}

inline void INextBotEventResponder::OnDominated(void)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnDominated();
	}
}

inline void INextBotEventResponder::OnWarped(Vector vStartPos)
{
	for (INextBotEventResponder *it = FirstContainedResponder(); it; it = NextContainedResponder(it))
	{
		it->OnWarped(vStartPos);
	}
}
#endif

#endif