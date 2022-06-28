
#include "collisionproperty.h"
#include <mathlib/mathlib.h>

//-----------------------------------------------------------------------------
// Transforms an AABB measured in entity space to a box that surrounds it in world space
//-----------------------------------------------------------------------------
void CCollisionPropertyHack::CollisionAABBToWorldAABB( const Vector &entityMins, 
	const Vector &entityMaxs, Vector *pWorldMins, Vector *pWorldMaxs ) const
{
	if ( !IsBoundsDefinedInEntitySpace() || (GetCollisionAngles() == vec3_angle) )
	{
		VectorAdd( entityMins, GetCollisionOrigin(), *pWorldMins );
		VectorAdd( entityMaxs, GetCollisionOrigin(), *pWorldMaxs );
	}
	else
	{
		TransformAABB( CollisionToWorldTransform(), entityMins, entityMaxs, *pWorldMins, *pWorldMaxs );
	}
}

void CCollisionPropertyHack::CalcNearestPoint(const Vector &vecWorldPt, Vector *pVecNearestWorldPt) const
{
	Vector localPt, localClosestPt;
	WorldToCollisionSpace(vecWorldPt, &localPt);
	CalcClosestPointOnAABB(m_vecMins, m_vecMaxs, localPt, localClosestPt);
	CollisionToWorldSpace(localClosestPt, pVecNearestWorldPt);
}

float CCollisionPropertyHack::CalcDistanceFromPoint(const Vector &vecWorldPt) const
{
	Vector localPt, localClosestPt;
	WorldToCollisionSpace(vecWorldPt, &localPt);
	CalcClosestPointOnAABB(m_vecMins, m_vecMaxs, localPt, localClosestPt);
	return localPt.DistTo(localClosestPt);
}