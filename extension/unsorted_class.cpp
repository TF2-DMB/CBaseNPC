#include "extension.h"
#include "unsorted_class.h"

CTraceFilterSimpleHack::CTraceFilterSimpleHack( const IHandleEntity *passedict, int collisionGroup,
									   ShouldHitFunc_t pExtraShouldHitFunc )
{
	m_pPassEnt = passedict;
	m_collisionGroup = collisionGroup;
	m_pExtraShouldHitCheckFunction = pExtraShouldHitFunc;
	m_pFunc = NULL;
}


bool CTraceFilterSimpleHack::ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
{
	bool bResult = (this->*func_ShouldHitEntity)(pHandleEntity, contentsMask);
	if	(bResult)
	{
		if (m_pFunc)
		{
			cell_t action;
			m_pFunc->PushCell(gamehelpers->EntityToBCompatRef(reinterpret_cast<CBaseEntity *>(pHandleEntity)));
			m_pFunc->PushCell(contentsMask);
			m_pFunc->PushCell(m_collisionGroup);
			m_pFunc->Execute(&action);
			return (action) ? true : false;
		}
		return bResult;
	}
	return bResult;
}

void CBaseCombatCharacter::SetAbsOrigin(const Vector &vecAbsOrigin)
{
	sm_datatable_info_t info;
	gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_vecAbsOrigin", &info);
	
	int offset = info.actual_offset;
	Vector *vec = (Vector *)((uint8_t *)this + offset);
	vec->x = vecAbsOrigin.x;
	vec->y = vecAbsOrigin.y;
	vec->z = vecAbsOrigin.z;
	
	gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_vecOrigin", &info);
	
	offset = info.actual_offset;
	Vector *vec2 = (Vector *)((uint8_t *)this + offset);
	vec2->x = vecAbsOrigin.x;
	vec2->y = vecAbsOrigin.y;
	vec2->z = vecAbsOrigin.z;
}

void CBaseCombatCharacter::SetAbsVelocity(const Vector &vecAbsVelocity)
{
	sm_datatable_info_t info;
	gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_vecAbsVelocity", &info);
	
	int offset = info.actual_offset;
	Vector *vec = (Vector *)((uint8_t *)this + offset);
	vec->x = vecAbsVelocity.x;
	vec->y = vecAbsVelocity.y;
	vec->z = vecAbsVelocity.z;
	
	gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap((CBaseEntity *)this), "m_vecVelocity", &info);
	
	offset = info.actual_offset;
	Vector *vec2 = (Vector *)((uint8_t *)this + offset);
	vec2->x = vecAbsVelocity.x;
	vec2->y = vecAbsVelocity.y;
	vec2->z = vecAbsVelocity.z;
}