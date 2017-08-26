#ifndef _NEXT_BOT_CONTEXTUAL_QUERY_H_
#define _NEXT_BOT_CONTEXTUAL_QUERY_H_

class INextBot;
class CBaseEntity;
class CBaseCombatCharacter;
class Path;
class CKnownEntity;

enum QueryResultType
{
	ANSWER_NO,
	ANSWER_YES,
	ANSWER_UNDEFINED
};

#define IS_ANY_HINDRANCE_POSSIBLE	( (CBaseEntity*)0xFFFFFFFF )
class IContextualQuery
{
public:
	virtual ~IContextualQuery() { };

	virtual QueryResultType			ShouldPickUp( const INextBot *me, CBaseEntity *item ) const = 0;
	virtual QueryResultType			ShouldHurry( const INextBot *me ) const = 0;
	virtual QueryResultType			ShouldRetreat( const INextBot *me ) const = 0;
	virtual QueryResultType			ShouldAttack( const INextBot *me, const CKnownEntity *them ) const = 0;
	virtual QueryResultType			IsHindrance( const INextBot *me, CBaseEntity *blocker ) const = 0;

	virtual Vector					SelectTargetPoint( const INextBot *me, const CBaseCombatCharacter *subject ) const = 0;
	virtual QueryResultType IsPositionAllowed( const INextBot *me, const Vector &pos ) const = 0;

	virtual const CKnownEntity *	SelectMoreDangerousThreat( const INextBot *me, 
															   const CBaseCombatCharacter *subject,
															   const CKnownEntity *threat1, 
															   const CKnownEntity *threat2 ) const = 0;
};
#endif // _NEXT_BOT_CONTEXTUAL_QUERY_H_
