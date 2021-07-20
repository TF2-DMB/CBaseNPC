#ifndef NATIVE_INTENTION_H_
#define NATIVE_INTENTION_H_

#include "sourcesdk/basecombatcharacter.h"
#include "sourcesdk/NextBot/NextBotIntentionInterface.h"

#define INTENTIONNATIVE(name) \
cell_t IIntention_##name(IPluginContext *pContext, const cell_t *params) { \
    IIntention * pIntention = reinterpret_cast< IIntention * >( params[1] ); \
    if (!pIntention) return pContext->ThrowNativeError( "IIntention is NULL!" );

#define INTENTIONQUERYNATIVE(name) \
INTENTIONNATIVE(name) \
    INextBot *me = pIntention->GetBot(); \
    if (!me) return pContext->ThrowNativeError( "INextBot is NULL!" );

INTENTIONNATIVE(Reset)
    pIntention->Reset();
    return 0;
}

INTENTIONNATIVE(Update)
    pIntention->Update();
    return 0;
}

INTENTIONQUERYNATIVE(ShouldPickUp)
    CBaseEntity* item = gamehelpers->ReferenceToEntity( params[2] );
    if (!item) return pContext->ThrowNativeError( "item is an invalid entity" );

    return pIntention->ShouldPickUp( me, item );
}

INTENTIONQUERYNATIVE(ShouldHurry)
    return pIntention->ShouldHurry( me );
}

INTENTIONQUERYNATIVE(ShouldRetreat)
    return pIntention->ShouldRetreat( me );
}

INTENTIONQUERYNATIVE(ShouldAttack)
    CKnownEntity* them = reinterpret_cast< CKnownEntity* >( params[2] );
    if (!them) return pContext->ThrowNativeError( "them is an invalid entity" );

    return pIntention->ShouldAttack( me, them );
}

INTENTIONQUERYNATIVE(IsHindrance)
    CBaseEntity* blocker = gamehelpers->ReferenceToEntity( params[2] );
    if (!blocker) return pContext->ThrowNativeError( "blocker is an invalid entity" );

    return pIntention->IsHindrance( me, blocker );
}

INTENTIONQUERYNATIVE(SelectTargetPoint)
    CBaseEntity* subjectEntity = gamehelpers->ReferenceToEntity( params[2] );
    if (!subjectEntity) return pContext->ThrowNativeError( "subject is an invalid entity" );

    CBaseCombatCharacterHack* subject = reinterpret_cast< CBaseEntityHack* >( subjectEntity )->MyCombatCharacterPointer();
    if (!subject) return pContext->ThrowNativeError( "subject is not a CBaseCombatCharacter" );

    cell_t* pBuffer = nullptr;
    pContext->LocalToPhysAddr( params[3], &pBuffer );
    if (!pBuffer) return pContext->ThrowNativeError( "buffer cannot be NULL" );

    Vector targetPoint = pIntention->SelectTargetPoint( me, subject );
    pBuffer[0] = sp_ftoc( targetPoint[0] );
    pBuffer[1] = sp_ftoc( targetPoint[1] );
    pBuffer[2] = sp_ftoc( targetPoint[2] );

    return 0;
}

INTENTIONQUERYNATIVE(IsPositionAllowed)
    cell_t* pPos = nullptr;
    pContext->LocalToPhysAddr( params[2], &pPos );
    if (!pPos) return pContext->ThrowNativeError( "pos cannot be NULL" );

    Vector pos( sp_ctof( pPos[0] ), sp_ctof( pPos[1] ), sp_ctof( pPos[2] ) );

    return pIntention->IsPositionAllowed( me, pos );
}

INTENTIONQUERYNATIVE(SelectMoreDangerousThreat)
    CBaseEntity* subjectEntity = gamehelpers->ReferenceToEntity( params[2] );
    if (!subjectEntity) return pContext->ThrowNativeError( "subject is an invalid entity" );

    CBaseCombatCharacterHack* subject = reinterpret_cast< CBaseEntityHack* >( subjectEntity )->MyCombatCharacterPointer();
    if (!subject) return pContext->ThrowNativeError( "subject is not a CBaseCombatCharacter" );

    CKnownEntity* threat1 = reinterpret_cast< CKnownEntity* >( params[3] );
    if (!threat1) return pContext->ThrowNativeError( "threat1 cannot be NULL" );

    CKnownEntity* threat2 = reinterpret_cast< CKnownEntity* >( params[4] );
    if (!threat2) return pContext->ThrowNativeError( "threat2 cannot be NULL" );

    return reinterpret_cast< cell_t >( pIntention->SelectMoreDangerousThreat( me, subject, threat1, threat2 ) );
}

#endif