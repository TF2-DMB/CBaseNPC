#include "intention.hpp"

#include "sourcesdk/basecombatcharacter.h"
#include "sourcesdk/NextBot/NextBotIntentionInterface.h"

namespace natives::nextbot::intention {

inline IIntention* Get(IPluginContext* context, const cell_t param) {
	IIntention* intention = (IIntention*)PawnAddressToPtr(param);
	if (!intention) {
		context->ThrowNativeError("Intention ptr is null!");
		return nullptr;
	}
	return intention;
}

cell_t ShouldPickUp(IPluginContext* context, const cell_t* params) {
	auto intention = Get(context, params[1]);
	if (!intention) {
		return 0;
	}

	CBaseEntity* item = gamehelpers->ReferenceToEntity( params[2] );
	if (!item) {
		return context->ThrowNativeError( "item is an invalid entity" );
	}

	INextBot *me = intention->GetBot();
	if (!me) {
		return context->ThrowNativeError("GetBot returned null!");
	}

	return intention->ShouldPickUp(me, item);
}

cell_t ShouldHurry(IPluginContext* context, const cell_t* params) {
	auto intention = Get(context, params[1]);
	if (!intention) {
		return 0;
	}

	INextBot *me = intention->GetBot();
	if (!me) {
		return context->ThrowNativeError("GetBot returned null!");
	}

	return intention->ShouldHurry(me);
}

cell_t ShouldRetreat(IPluginContext* context, const cell_t* params) {
	auto intention = Get(context, params[1]);
	if (!intention) {
		return 0;
	}

	INextBot *me = intention->GetBot();
	if (!me) {
		return context->ThrowNativeError("GetBot returned null!");
	}
	
	return intention->ShouldRetreat(me);
}

cell_t ShouldAttack(IPluginContext* context, const cell_t* params) {
	auto intention = Get(context, params[1]);
	if (!intention) {
		return 0;
	}
	
	CKnownEntity* them = reinterpret_cast< CKnownEntity* >( PawnAddressToPtr(params[2]) );
	if (!them) {
		return context->ThrowNativeError("them entity is a null ptr!");
	}

	INextBot *me = intention->GetBot();
	if (!me) {
		return context->ThrowNativeError("GetBot returned null!");
	}

	return intention->ShouldAttack( me, them );
}

cell_t IsHindrance(IPluginContext* context, const cell_t* params) {
	auto intention = Get(context, params[1]);
	if (!intention) {
		return 0;
	}
	
	CBaseEntity* blocker = gamehelpers->ReferenceToEntity( params[2] );
	if (!blocker) {
		return context->ThrowNativeError("Blocker entity %d is invalid !", params[2]);
	}

	INextBot *me = intention->GetBot();
	if (!me) {
		return context->ThrowNativeError("GetBot returned null!");
	}

	return intention->IsHindrance(me, blocker);
}

cell_t SelectTargetPoint(IPluginContext* context, const cell_t* params) {
	auto intention = Get(context, params[1]);
	if (!intention) {
		return 0;
	}
	
	CBaseEntity* subjectEntity = gamehelpers->ReferenceToEntity( params[2] );
	if (!subjectEntity) {
		return context->ThrowNativeError("Subject entity %d is invalid !", params[2]);
	}

	CBaseCombatCharacter* subject = subjectEntity->MyCombatCharacterPointer();
	if (!subject) {
		return context->ThrowNativeError( "Subject entity is not a CBaseCombatCharacter.");
	}

	cell_t* buffer = nullptr;
	context->LocalToPhysAddr( params[3], &buffer );
	if (!buffer) {
		return context->ThrowNativeError("The buffer cannot be null !");
	}

	INextBot *me = intention->GetBot();
	if (!me) {
		return context->ThrowNativeError("GetBot returned null!");
	}

	Vector targetPoint = intention->SelectTargetPoint(me, subject);
	VectorToPawnVector(buffer, targetPoint);

	return 0;
}

cell_t IsPositionAllowed(IPluginContext* context, const cell_t* params) {
	auto intention = Get(context, params[1]);
	if (!intention) {
		return 0;
	}
	
	cell_t* pos = nullptr;
	context->LocalToPhysAddr(params[2], &pos);
	if (!pos) {
		return context->ThrowNativeError("Position cannot be null !");
	}

	INextBot *me = intention->GetBot();
	if (!me) {
		return context->ThrowNativeError("GetBot returned null!");
	}

	Vector vecPos;
	PawnVectorToVector(pos, vecPos);

	return intention->IsPositionAllowed(me, vecPos);
}

cell_t SelectMoreDangerousThreat(IPluginContext* context, const cell_t* params) {
	auto intention = Get(context, params[1]);
	if (!intention) {
		return 0;
	}
	
	CBaseEntity* subjectEntity = gamehelpers->ReferenceToEntity( params[2] );
	if (!subjectEntity) {
		return context->ThrowNativeError("Subject entity %d is invalid !", params[2]);
	}

	CBaseCombatCharacter* subject = subjectEntity->MyCombatCharacterPointer();
	if (!subject) {
		return context->ThrowNativeError( "Subject entity is not a CBaseCombatCharacter.");
	}

	CKnownEntity* threat1 = reinterpret_cast< CKnownEntity* >( PawnAddressToPtr(params[3]) );
	if (!threat1) {
		return context->ThrowNativeError("threat1 entity cannot be a null ptr!");
	}

	CKnownEntity* threat2 = reinterpret_cast< CKnownEntity* >( PawnAddressToPtr(params[4]) );
	if (!threat2) {
		return context->ThrowNativeError("threat2 entity cannot be a null ptr!");
	}

	INextBot *me = intention->GetBot();
	if (!me) {
		return context->ThrowNativeError("GetBot returned null!");
	}

	return PtrToPawnAddress(intention->SelectMoreDangerousThreat(me, subject, threat1, threat2));
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"IIntention.ShouldPickUp", ShouldPickUp},
		{"IIntention.ShouldHurry", ShouldHurry},
		{"IIntention.ShouldRetreat", ShouldRetreat},
		{"IIntention.ShouldAttack", ShouldAttack},
		{"IIntention.IsHindrance", IsHindrance},
		{"IIntention.SelectTargetPoint", SelectTargetPoint},
		{"IIntention.IsPositionAllowed", IsPositionAllowed},
		{"IIntention.SelectMoreDangerousThreat", SelectMoreDangerousThreat},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}