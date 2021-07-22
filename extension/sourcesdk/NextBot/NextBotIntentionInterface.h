#ifndef _NEXT_BOT_INTENTION_INTERFACE_H_
#define _NEXT_BOT_INTENTION_INTERFACE_H_

#include "NextBotComponentInterface.h"
#include "NextBotContextualQueryInterface.h"

class INextBot;

class IIntention : public INextBotComponent, public IContextualQuery
{
public:
	IIntention( INextBot *bot ) : INextBotComponent( bot ) {}
	virtual ~IIntention() { };
	virtual void Reset( void )  { INextBotComponent::Reset(); };
	virtual void Update( void ) { };

	virtual QueryResultType			ShouldPickUp( const INextBot *me, CBaseEntity *item ) const;
	virtual QueryResultType			ShouldHurry( const INextBot *me ) const;
	virtual QueryResultType			ShouldRetreat( const INextBot *me ) const;
	virtual QueryResultType			ShouldAttack( const INextBot *me, const CKnownEntity *them ) const;
	virtual QueryResultType			IsHindrance( const INextBot *me, CBaseEntity *blocker ) const;
	virtual Vector					SelectTargetPoint( const INextBot *me, const CBaseCombatCharacterHack *subject ) const;
	virtual QueryResultType			IsPositionAllowed( const INextBot *me, const Vector &pos ) const;
	virtual const CKnownEntity *	SelectMoreDangerousThreat( const INextBot *me, 
															   const CBaseCombatCharacterHack *subject,
															   const CKnownEntity *threat1, 
															   const CKnownEntity *threat2 ) const;
};

inline QueryResultType IIntention::ShouldPickUp( const INextBot *me, CBaseEntity *item ) const
{
	for ( INextBotEventResponder *sub = FirstContainedResponder(); sub; sub = NextContainedResponder( sub ) )
	{
		const IContextualQuery *query = dynamic_cast< const IContextualQuery * >( sub );
		if ( query )
		{
			// return the response of the first responder that gives a definitive answer
			QueryResultType result = query->ShouldPickUp( me, item );
			if ( result != ANSWER_UNDEFINED )
			{
				return result;
			}
		}
	}	
	return ANSWER_UNDEFINED;
}


inline QueryResultType IIntention::ShouldHurry( const INextBot *me ) const
{
	for ( INextBotEventResponder *sub = FirstContainedResponder(); sub; sub = NextContainedResponder( sub ) )
	{
		const IContextualQuery *query = dynamic_cast< const IContextualQuery * >( sub );
		if ( query )
		{
			// return the response of the first responder that gives a definitive answer
			QueryResultType result = query->ShouldHurry( me );
			if ( result != ANSWER_UNDEFINED )
			{
				return result;
			}
		}
	}	
	return ANSWER_UNDEFINED;
}


inline QueryResultType IIntention::ShouldRetreat( const INextBot *me ) const
{
	for ( INextBotEventResponder *sub = FirstContainedResponder(); sub; sub = NextContainedResponder( sub ) )
	{
		const IContextualQuery *query = dynamic_cast< const IContextualQuery * >( sub );
		if ( query )
		{
			// return the response of the first responder that gives a definitive answer
			QueryResultType result = query->ShouldRetreat( me );
			if ( result != ANSWER_UNDEFINED )
			{
				return result;
			}
		}
	}	
	return ANSWER_UNDEFINED;
}


inline QueryResultType IIntention::ShouldAttack( const INextBot *me, const CKnownEntity *them ) const
{
	for ( INextBotEventResponder *sub = FirstContainedResponder(); sub; sub = NextContainedResponder( sub ) )
	{
		const IContextualQuery *query = dynamic_cast< const IContextualQuery * >( sub );
		if ( query )
		{
			// return the response of the first responder that gives a definitive answer
			QueryResultType result = query->ShouldAttack( me, them );
			if ( result != ANSWER_UNDEFINED )
			{
				return result;
			}
		}
	}	
	return ANSWER_UNDEFINED;
}


inline QueryResultType IIntention::IsHindrance( const INextBot *me, CBaseEntity *blocker ) const
{
	for ( INextBotEventResponder *sub = FirstContainedResponder(); sub; sub = NextContainedResponder( sub ) )
	{
		const IContextualQuery *query = dynamic_cast< const IContextualQuery * >( sub );
		if ( query )
		{
			// return the response of the first responder that gives a definitive answer
			QueryResultType result = query->IsHindrance( me, blocker );
			if ( result != ANSWER_UNDEFINED )
			{
				return result;
			}
		}
	}	
	return ANSWER_UNDEFINED;
}


inline QueryResultType IIntention::IsPositionAllowed( const INextBot *me, const Vector &pos ) const
{
	for ( INextBotEventResponder *sub = FirstContainedResponder(); sub; sub = NextContainedResponder( sub ) )
	{
		const IContextualQuery *query = dynamic_cast< const IContextualQuery * >( sub );
		if ( query )
		{
			// return the response of the first responder that gives a definitive answer
			QueryResultType result = query->IsPositionAllowed( me, pos );
			if ( result != ANSWER_UNDEFINED )
			{
				return result;
			}
		}
	}	
	return ANSWER_UNDEFINED;
}

#endif // _NEXT_BOT_INTENTION_INTERFACE_H_
