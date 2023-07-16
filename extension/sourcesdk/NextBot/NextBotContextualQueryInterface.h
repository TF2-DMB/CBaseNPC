#ifndef _NEXT_BOT_CONTEXTUAL_QUERY_H_
#define _NEXT_BOT_CONTEXTUAL_QUERY_H_

class INextBot;
class CBaseEntity;
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

	virtual QueryResultType			ShouldPickUp( const INextBot *me, CBaseEntity *item ) const;
	virtual QueryResultType			ShouldHurry( const INextBot *me ) const;
	virtual QueryResultType			ShouldRetreat( const INextBot *me ) const;
	virtual QueryResultType			ShouldAttack( const INextBot *me, const CKnownEntity *them ) const;
	virtual QueryResultType			IsHindrance( const INextBot *me, CBaseEntity *blocker ) const;

	virtual Vector					SelectTargetPoint( const INextBot *me, const CBaseCombatCharacter* subject ) const;
	virtual QueryResultType IsPositionAllowed( const INextBot *me, const Vector &pos ) const;

	virtual const CKnownEntity *	SelectMoreDangerousThreat( const INextBot *me, 
															   const CBaseCombatCharacter* subject,
															   const CKnownEntity *threat1, 
															   const CKnownEntity *threat2 ) const;
};

inline QueryResultType IContextualQuery::ShouldPickUp( const INextBot *me, CBaseEntity *item ) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldHurry( const INextBot *me ) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldRetreat( const INextBot *me ) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::ShouldAttack( const INextBot *me, const CKnownEntity *them ) const
{
	return ANSWER_UNDEFINED;
}

inline QueryResultType IContextualQuery::IsHindrance( const INextBot *me, CBaseEntity *blocker ) const
{
	return ANSWER_UNDEFINED;
}

inline Vector IContextualQuery::SelectTargetPoint( const INextBot *me, const CBaseCombatCharacter* subject ) const
{
	return vec3_origin;
}

inline QueryResultType IContextualQuery::IsPositionAllowed( const INextBot *me, const Vector &pos ) const
{
	return ANSWER_UNDEFINED;
}

inline const CKnownEntity *IContextualQuery::SelectMoreDangerousThreat( const INextBot *me, const CBaseCombatCharacter* subject, const CKnownEntity *threat1, const CKnownEntity *threat2 ) const
{
	return NULL;
}

#endif // _NEXT_BOT_CONTEXTUAL_QUERY_H_
