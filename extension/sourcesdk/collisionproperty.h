#ifndef H_COLLISIONPROPERTY_CBASENPC_
#define H_COLLISIONPROPERTY_CBASENPC_
#ifdef _WIN32
#pragma once
#endif

#include <tier0/platform.h>
#include <mathlib/vector.h>
#include <mathlib/mathlib.h>
#include <ICollideable.h>
#include "ispatialpartition.h"
#include <const.h>

class CBaseEntity;
class IHandleEntity;
class QAngle;
class Vector;

class CCollisionPropertyHack : public ICollideable
{
public:
	bool IsBoundsDefinedInEntitySpace() const;
	
	void CollisionAABBToWorldAABB( const Vector &entityMins, const Vector &entityMaxs, Vector *pWorldMins, Vector *pWorldMaxs ) const;
	void WorldSpaceAABB( Vector *pWorldMins, Vector *pWorldMaxs ) const;

	CBaseEntity *m_pOuter;

	Vector m_vecMinsPreScaled;
	Vector m_vecMaxsPreScaled;
	Vector m_vecMins;
	Vector m_vecMaxs;
	float m_flRadius;

	unsigned short m_usSolidFlags;

	SpatialPartitionHandle_t m_Partition;
	unsigned char m_nSurroundType;

	unsigned char m_nSolidType;
	unsigned char m_triggerBloat;

	Vector m_vecSpecifiedSurroundingMinsPreScaled;
	Vector m_vecSpecifiedSurroundingMaxsPreScaled;
	Vector m_vecSpecifiedSurroundingMins;
	Vector m_vecSpecifiedSurroundingMaxs;

	Vector	m_vecSurroundingMins;
	Vector	m_vecSurroundingMaxs;
};

inline bool CCollisionPropertyHack::IsBoundsDefinedInEntitySpace() const
{
	return (( m_usSolidFlags & FSOLID_FORCE_WORLD_ALIGNED ) == 0 ) &&
			( m_nSolidType != SOLID_BBOX ) && ( m_nSolidType != SOLID_NONE );
}

//-----------------------------------------------------------------------------
// Computes a bounding box in world space surrounding the collision bounds
//-----------------------------------------------------------------------------
inline void CCollisionPropertyHack::WorldSpaceAABB( Vector *pWorldMins, Vector *pWorldMaxs ) const
{
	CollisionAABBToWorldAABB( m_vecMins, m_vecMaxs, pWorldMins, pWorldMaxs );
}

#endif