
#include "sourcesdk/NextBot/NextBotLocomotionInterface.h"
#include "tier0/vprof.h"

VCall<bool, const Vector&, const Vector&, const CBaseEntity*> ILocomotion::vClimbUpToLedge;
VCall<void, const Vector&, const Vector&> ILocomotion::vJumpAcrossGap;
VCall<bool> ILocomotion::vIsClimbingUpToLedge;
VCall<bool> ILocomotion::vIsJumpingAcrossGap;
VCall<bool> ILocomotion::vIsAbleToJumpAcrossGaps;
VCall<bool> ILocomotion::vIsAbleToClimb;
VCall<float> ILocomotion::vGetStepHeight;
VCall<float> ILocomotion::vGetMaxJumpHeight;
VCall<float> ILocomotion::vGetDeathDropHeight;
VCall<float> ILocomotion::vGetRunSpeed;
VCall<float> ILocomotion::vGetWalkSpeed;
VCall<float> ILocomotion::vGetMaxAcceleration;
VCall<bool, CBaseEntity*, ILocomotion::TraverseWhenType> ILocomotion::vIsEntityTraversable;
VCall<bool, const CBaseEntity*> ILocomotion::vShouldCollideWith;

bool ILocomotion::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	try
	{
		vClimbUpToLedge.Init(&ILocomotion::ClimbUpToLedge);
		vJumpAcrossGap.Init(&ILocomotion::JumpAcrossGap);
		vIsClimbingUpToLedge.Init(&ILocomotion::IsClimbingUpToLedge);
		vIsJumpingAcrossGap.Init(&ILocomotion::IsJumpingAcrossGap);
		vIsAbleToJumpAcrossGaps.Init(&ILocomotion::IsAbleToJumpAcrossGaps);
		vIsAbleToClimb.Init(&ILocomotion::IsAbleToClimb);
		vGetStepHeight.Init(&ILocomotion::GetStepHeight);
		vGetMaxJumpHeight.Init(&ILocomotion::GetMaxJumpHeight);
		vGetDeathDropHeight.Init(&ILocomotion::GetDeathDropHeight);
		vGetRunSpeed.Init(&ILocomotion::GetRunSpeed);
		vGetWalkSpeed.Init(&ILocomotion::GetWalkSpeed);
		vGetMaxAcceleration.Init(&ILocomotion::GetMaxAcceleration);
		vIsEntityTraversable.Init(&ILocomotion::IsEntityTraversable);
		vShouldCollideWith.Init(&ILocomotion::ShouldCollideWith);
	}
	catch (const std::exception& e)
	{
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}

	return true;
}

ILocomotion::ILocomotion( INextBot *bot ) : INextBotComponent( bot )
{
	Reset();
}

ILocomotion::~ILocomotion()
{
}

void ILocomotion::Reset( void )
{
	INextBotComponent::Reset();

	m_motionVector = Vector( 1.0f, 0.0f, 0.0f );
	m_speed = 0.0f;
	m_groundMotionVector = m_motionVector;
	m_groundSpeed = m_speed;

	m_moveRequestTimer.Invalidate();

	m_isStuck = false;
	m_stuckTimer.Invalidate();
	m_stuckPos = vec3_origin;
}

void ILocomotion::Update( void )
{
	StuckMonitor();

	// maintain motion vector and speed values
	const Vector &vel = GetVelocity();
	m_speed = vel.Length();
	m_groundSpeed = vel.AsVector2D().Length();

	const float velocityThreshold = 10.0f;
	if ( m_speed > velocityThreshold )
	{
		m_motionVector = vel / m_speed;
	}

	if ( m_groundSpeed > velocityThreshold )
	{
		m_groundMotionVector.x = vel.x / m_groundSpeed;
		m_groundMotionVector.y = vel.y / m_groundSpeed;
		m_groundMotionVector.z = 0.0f;
	}
}

void ILocomotion::AdjustPosture( const Vector &moveGoal )
{
	IBody *body = GetBot()->GetBodyInterface();
	if ( !body->IsActualPosture( IBody::STAND ) && !body->IsActualPosture( IBody::CROUCH ) )
		return;

	const Vector &mins = body->GetHullMins() + Vector( 0, 0, GetStepHeight() );

	const float halfSize = body->GetHullWidth()/2.0f;
	Vector standMaxs( halfSize, halfSize, body->GetStandHullHeight() );

	trace_t trace;
	NextBotTraversableTraceFilter filter( GetBot(), ILocomotion::IMMEDIATELY );

	// snap forward movement vector along floor
	const Vector &groundNormal = GetGroundNormal();
	const Vector &feet = GetFeet();
	Vector moveDir = moveGoal - feet;
	float moveLength = moveDir.NormalizeInPlace();
	Vector left( -moveDir.y, moveDir.x, 0.0f );
	Vector goal = feet + moveLength * CrossProduct( left, groundNormal ).Normalized();

	TraceHull( feet, goal, mins, standMaxs, body->GetSolidMask(), &filter, &trace );

	if ( trace.fraction >= 1.0f && !trace.startsolid )
	{
		// no collision while standing
		if ( body->IsActualPosture( IBody::CROUCH ) )
		{
			body->SetDesiredPosture( IBody::STAND );
		}
		return;
	}

	if ( body->IsActualPosture( IBody::CROUCH ) )
		return;

	// crouch hull check
	Vector crouchMaxs( halfSize, halfSize, body->GetCrouchHullHeight() );

	TraceHull( feet, goal, mins, crouchMaxs, body->GetSolidMask(), &filter, &trace );

	if ( trace.fraction >= 1.0f && !trace.startsolid )
	{
		// no collision while crouching
		body->SetDesiredPosture( IBody::CROUCH );
	}
}

void ILocomotion::Approach( const Vector &goalPos, float goalWeight )
{
	m_moveRequestTimer.Start();
}

void ILocomotion::DriveTo( const Vector &pos )
{
	m_moveRequestTimer.Start();
}

bool ILocomotion::IsPotentiallyTraversable( const Vector &from, const Vector &to, TraverseWhenType when, float *fraction ) const
{
	VPROF_BUDGET( "Locomotion::IsPotentiallyTraversable", "NextBotExpensive" );

	if ( ( to.z - from.z ) > GetMaxJumpHeight() + 0.1f )
	{
		Vector along = to - from;
		along.NormalizeInPlace();
		if ( along.z > GetTraversableSlopeLimit() )
		{
			if ( fraction )
			{
				*fraction = 0.0f;
			}
			return false;
		}
	}

	trace_t result;
	NextBotTraversableTraceFilter filter( GetBot(), when );

	const float probeSize = 0.25f * GetBot()->GetBodyInterface()->GetHullWidth();
	const float probeZ = GetStepHeight();

	Vector hullMin( -probeSize, -probeSize, probeZ );
	Vector hullMax( probeSize, probeSize, GetBot()->GetBodyInterface()->GetCrouchHullHeight() );
	TraceHull( from, to, hullMin, hullMax, GetBot()->GetBodyInterface()->GetSolidMask(), &filter, &result );

	if ( fraction )
	{
		*fraction = result.fraction;
	}

	return ( result.fraction >= 1.0f ) && ( !result.startsolid );
}

bool ILocomotion::HasPotentialGap( const Vector &from, const Vector &desiredTo, float *fraction ) const
{
	VPROF_BUDGET( "Locomotion::HasPotentialGap", "NextBot" );

	float traversableFraction;
	IsPotentiallyTraversable( from, desiredTo, IMMEDIATELY, &traversableFraction );

	// compute end of traversable ray
	Vector to = from + ( desiredTo - from ) * traversableFraction;

	Vector forward = to - from;
	float length = forward.NormalizeInPlace();

	IBody *body = GetBot()->GetBodyInterface();

	float step = body->GetHullWidth()/2.0f;

	Vector pos = from;
	Vector delta = step * forward;
	for( float t = 0.0f; t < (length + step); t += step )
	{
		if ( IsGap( pos, forward ) )
		{
			if ( fraction )
			{
				*fraction = ( t - step ) / ( length + step );
			}
			
			return true;
		}

		pos += delta;		
	}

	if ( fraction )
	{
		*fraction = 1.0f;
	}

	return false;
}

bool ILocomotion::IsGap( const Vector &pos, const Vector &forward ) const
{
	VPROF_BUDGET( "Locomotion::IsGap", "NextBotSpiky" );

	IBody *body = GetBot()->GetBodyInterface();

	const float halfWidth = 1.0f;
	const float hullHeight = 1.0f;

	unsigned int mask = ( body ) ? body->GetSolidMask() : MASK_PLAYERSOLID;

	trace_t ground;

	NextBotTraceFilterIgnoreActors filter( GetBot()->GetEntity(), COLLISION_GROUP_NONE );

	TraceHull( pos + Vector( 0, 0, GetStepHeight() ),	// start up a bit to handle rough terrain
					pos + Vector( 0, 0, -GetMaxJumpHeight() ), 
					Vector( -halfWidth, -halfWidth, 0 ), Vector( halfWidth, halfWidth, hullHeight ), 
					mask, &filter, &ground );
	return ( ground.fraction >= 1.0f && !ground.startsolid );
}

bool ILocomotion::IsEntityTraversable( CBaseEntity *obstacle, TraverseWhenType when ) const
{
	if ( obstacle->IsWorld() )
		return false;

	if ( FClassnameIs( obstacle, "prop_door*" ) || FClassnameIs( obstacle, "func_door*" ) )
	{
		CBasePropDoor *door = dynamic_cast< CBasePropDoor * >( obstacle );

		if ( door && door->IsDoorOpen() )
		{
			// open doors are obstacles
			return false;
		}

		return true;
	}

	if ( FClassnameIs( obstacle, "func_brush" ) )
	{
		CFuncBrush *brush = (CFuncBrush *)obstacle;
		
		switch ( brush->m_iSolidity )
		{
			case CFuncBrush::BRUSHSOLID_ALWAYS:
				return false;
			case CFuncBrush::BRUSHSOLID_NEVER:
				return true;
			case CFuncBrush::BRUSHSOLID_TOGGLE:
				return true;
		}
	}

	if ( when == IMMEDIATELY )
	{
		return false;
	}

	return GetBot()->IsAbleToBreak( obstacle );
}

bool ILocomotion::IsAreaTraversable( const CNavArea *baseArea ) const
{ 
	return !baseArea->IsBlocked( GetBot()->GetEntity()->GetTeamNumber() );
}

void ILocomotion::ClearStuckStatus( const char *reason )
{
	if ( IsStuck() )
	{
		m_isStuck = false;
		GetBot()->OnUnStuck();
	}

	m_stuckPos = GetFeet();
	m_stuckTimer.Start();
}

void ILocomotion::StuckMonitor( void )
{
	const float idleTime = 0.25f;
	if ( m_moveRequestTimer.IsGreaterThen( idleTime ) )
	{
		m_stuckPos = GetFeet();
		m_stuckTimer.Start();

		return;
	}

	if ( IsStuck() )
	{
		if ( GetBot()->IsRangeGreaterThan( m_stuckPos, STUCK_RADIUS ) )
		{
			ClearStuckStatus( "UN-STUCK" );
		}
		else
		{
			if ( m_stillStuckTimer.IsElapsed() )
			{
				m_stillStuckTimer.Start( 1.0f );
				GetBot()->OnStuck();
			}
		}
	}
	else
	{
		if ( GetBot()->IsRangeGreaterThan( m_stuckPos, STUCK_RADIUS ) )
		{
			m_stuckPos = GetFeet();
			m_stuckTimer.Start();
		}
		else
		{
			float minMoveSpeed = 0.1f * GetDesiredSpeed() + 0.1f;
			float escapeTime = STUCK_RADIUS / minMoveSpeed;
			if ( m_stuckTimer.IsGreaterThen( escapeTime ) )
			{
				m_isStuck = true;
				GetBot()->OnStuck();
			}
		}
	}
}

const Vector &ILocomotion::GetFeet( void ) const
{
	return GetBot()->GetEntity()->GetAbsOrigin();
}