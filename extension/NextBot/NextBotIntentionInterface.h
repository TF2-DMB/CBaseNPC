#ifndef _NEXT_BOT_INTENTION_INTERFACE_H_
#define _NEXT_BOT_INTENTION_INTERFACE_H_

#include "NextBotComponentInterface.h"
#include "NextBotContextualQueryInterface.h"

class INextBot;

class IIntention : public INextBotComponent, public IContextualQuery
{
public:
	virtual ~IIntention() { };
	virtual void Reset( void )  { INextBotComponent::Reset(); };
	virtual void Update( void ) { };

	virtual QueryResultType			ShouldPickUp( const INextBot *me, CBaseEntity *item ) const = 0;
	virtual QueryResultType			ShouldHurry( const INextBot *me ) const = 0;
	virtual QueryResultType			ShouldRetreat( const INextBot *me ) const = 0;
	virtual QueryResultType			ShouldAttack( const INextBot *me, const CKnownEntity *them ) const = 0;
	virtual QueryResultType			IsHindrance( const INextBot *me, CBaseEntity *blocker ) const = 0;
	virtual Vector					SelectTargetPoint( const INextBot *me, const CBaseCombatCharacter *subject ) const = 0;
	virtual QueryResultType			IsPositionAllowed( const INextBot *me, const Vector &pos ) const = 0;
	virtual const CKnownEntity *	SelectMoreDangerousThreat( const INextBot *me, 
															   const CBaseCombatCharacter *subject,
															   const CKnownEntity *threat1, 
															   const CKnownEntity *threat2 ) const = 0;
};

#endif // _NEXT_BOT_INTENTION_INTERFACE_H_
