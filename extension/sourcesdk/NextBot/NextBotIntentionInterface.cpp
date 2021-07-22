
#include "NextBotIntentionInterface.h"
#include "basecombatcharacter.h"
#include "NextBotKnownEntity.h"

Vector IIntention::SelectTargetPoint( const INextBot *me, const CBaseCombatCharacterHack *subject ) const
{
	for ( INextBotEventResponder *sub = FirstContainedResponder(); sub; sub = NextContainedResponder( sub ) )
	{
		const IContextualQuery *query = dynamic_cast< const IContextualQuery * >( sub );
		if ( query )
		{
			// return the response of the first responder that gives a definitive answer
			Vector result = query->SelectTargetPoint( me, subject );
			if ( result != vec3_origin )
			{
				return result;
			}
		}
	}

	// no answer, use a reasonable position
	Vector threatMins, threatMaxs;
	subject->CollisionProp()->WorldSpaceAABB( &threatMins, &threatMaxs );
	Vector targetPoint = subject->GetAbsOrigin();
	targetPoint.z += 0.7f * ( threatMaxs.z - threatMins.z );

	return targetPoint;
}

const CKnownEntity *IIntention::SelectMoreDangerousThreat( const INextBot *me, const CBaseCombatCharacterHack *subject, const CKnownEntity *threat1, const CKnownEntity *threat2 ) const
{
	if ( !threat1 || threat1->IsObsolete() )
	{
		if ( threat2 && !threat2->IsObsolete() )
			return threat2;

		return NULL;
	}
	else if ( !threat2 || threat2->IsObsolete() )
	{
		return threat1;
	}

	for ( INextBotEventResponder *sub = FirstContainedResponder(); sub; sub = NextContainedResponder( sub ) )
	{
		const IContextualQuery *query = dynamic_cast< const IContextualQuery * >( sub );
		if ( query )
		{
			// return the response of the first responder that gives a definitive answer
			const CKnownEntity *result = query->SelectMoreDangerousThreat( me, subject, threat1, threat2 );
			if ( result )
			{
				return result;
			}
		}
	}

	// no specific decision was made - return closest threat as most dangerous
	float range1 = ( subject->GetAbsOrigin() - threat1->GetLastKnownPosition() ).LengthSqr();
	float range2 = ( subject->GetAbsOrigin() - threat2->GetLastKnownPosition() ).LengthSqr();

	if ( range1 < range2 )
	{
		return threat1;
	}

	return threat2;
}