#include "sourcesdk/NextBot/NextBotInterface.h"
#include "sourcesdk/NextBot/NextBotBodyInterface.h"
#include "sourcesdk/NextBot/NextBotIntentionInterface.h"
#include "sourcesdk/NextBot/NextBotGroundLocomotion.h"

MCall<void, INextBot*> NextBotGroundLocomotion::NextBotGroundLocomotion_Ctor;
//MCall<Vector, const Vector&, const Vector&, int> NextBotGroundLocomotion::mResolveCollision;
int NextBotGroundLocomotion::vtable_entries = 0;

VCALL_DEFINE_MEMBER(NextBotGroundLocomotion, GetGravity, float)
VCALL_DEFINE_MEMBER(NextBotGroundLocomotion, GetFrictionForward, float)
VCALL_DEFINE_MEMBER(NextBotGroundLocomotion, GetFrictionSideways, float)
VCALL_DEFINE_MEMBER(NextBotGroundLocomotion, GetMaxYawRate, float)

ConVar base_npc_freeze("base_npc_freeze", "0", FCVAR_CHEAT);

bool NextBotGroundLocomotion::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	if (!config->GetOffset("NextBotGroundLocomotion::vtable_entries", &vtable_entries))
	{
		snprintf(error, maxlength, "Could not retrieve NextBotGroundLocomotion::vtable_entries offset");
		return false;
	}

	try
	{
		NextBotGroundLocomotion_Ctor.Init(config, "NextBotGroundLocomotion::NextBotGroundLocomotion");
		vGetGravity.Init(&NextBotGroundLocomotion::GetGravity);
		vGetFrictionForward.Init(&NextBotGroundLocomotion::GetFrictionForward);
		vGetFrictionSideways.Init(&NextBotGroundLocomotion::GetFrictionSideways);
		vGetMaxYawRate.Init(&NextBotGroundLocomotion::GetMaxYawRate);
	}
	catch (const std::exception& e)
	{
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}
	
	return true;
}

/*
void NextBotGroundLocomotion::NonVirtualUpdate(void)
{
	StuckMonitor();

	// maintain motion vector and speed values
	const Vector &vel = GetVelocity();
	m_speed = vel.Length();
	m_groundSpeed = vel.AsVector2D().Length();

	const float velocityThreshold = 10.0f;
	if (m_speed > velocityThreshold)
	{
		m_motionVector = vel / m_speed;
	}

	if (m_groundSpeed > velocityThreshold)
	{
		m_groundMotionVector.x = vel.x / m_groundSpeed;
		m_groundMotionVector.y = vel.y / m_groundSpeed;
		m_groundMotionVector.z = 0.0f;
	}

	const float deltaT = GetUpdateInterval();

	// apply accumulated position changes
	ApplyAccumulatedApproach();

	// need to do this first thing, because ground constraints, etc, can change it
	Vector origPos = GetFeet();

	INextBot* bot = GetBot();
	IBody* body = bot->GetBodyInterface();

	if (TraverseLadder())
	{
		// bot is climbing a ladder
		return;
	}

	if (!body->IsPostureMobile())
	{
		// sitting/lying on the ground - no slip
		m_acceleration.x = 0.0f;
		m_acceleration.y = 0.0f;
		m_velocity.x = 0.0f;
		m_velocity.y = 0.0f;
	}

	bool wasOnGround = IsOnGround();

	if (!body->HasActivityType(IBody::MOTION_CONTROLLED_Z))
	{
		// fall if in the air
		if (!IsOnGround())
		{
			// no ground below us - fall
			m_acceleration.z -= GetGravity();
		}

		if (!IsClimbingOrJumping() || m_velocity.z <= 0.0f)
		{
			// keep us on the ground
			UpdateGroundConstraint();
		}
	}

	Vector newPos = GetFeet();

	//
	// Update position physics
	//
	Vector right( m_moveVector.y, -m_moveVector.x, 0.0f );

	if ( IsOnGround() ) // || m_isClimbingUpToLedge )
	{
		if ( IsAttemptingToMove() )
		{
			float forwardSpeed = DotProduct( m_velocity, m_moveVector );
			Vector forwardVelocity = forwardSpeed * m_moveVector;
			Vector sideVelocity = DotProduct( m_velocity, right ) * right;

			Vector frictionAccel = vec3_origin;

			// only apply friction along forward direction if we are sliding backwards
			if ( forwardSpeed < 0.0f )
			{
				frictionAccel = -GetFrictionForward() * forwardVelocity;
			}

			// always apply lateral friction to counteract sideslip		
			frictionAccel += -GetFrictionSideways() * sideVelocity;

			m_acceleration.x += frictionAccel.x;
			m_acceleration.y += frictionAccel.y;
		}
		else
		{
			// come to a stop if we haven't been told to move
			m_acceleration = vec3_origin;
			m_velocity = vec3_origin;
		}
	}

	// compute new position, taking into account MOTION_CONTROLLED animations in progress
	if ( body->HasActivityType( IBody::MOTION_CONTROLLED_XY ) )
	{
		m_acceleration.x = 0.0f;
		m_acceleration.y = 0.0f;
		m_velocity.x = GetBot()->GetEntity()->GetAbsVelocity().x;
		m_velocity.y = GetBot()->GetEntity()->GetAbsVelocity().y;
	}
	else
	{
		// euler integration
		m_velocity.x += m_acceleration.x * deltaT;
		m_velocity.y += m_acceleration.y * deltaT;

		// euler integration		
		newPos.x += m_velocity.x * deltaT;
		newPos.y += m_velocity.y * deltaT;
	}

	if ( body->HasActivityType( IBody::MOTION_CONTROLLED_Z ) )
	{
		m_acceleration.z = 0.0f;
		m_velocity.z = GetBot()->GetEntity()->GetAbsVelocity().z;
	}
	else
	{
		// euler integration
		m_velocity.z += m_acceleration.z * deltaT;

		// euler integration		
		newPos.z += m_velocity.z * deltaT;
	}
	
	// move bot to new position, resolving collisions along the way
	UpdatePosition(newPos);


	// set actual velocity based on position change after collision resolution step
	Vector adjustedVelocity = ( GetFeet() - origPos ) / deltaT;

	if ( !body->HasActivityType( IBody::MOTION_CONTROLLED_XY ) )
	{
		m_velocity.x = adjustedVelocity.x;
		m_velocity.y = adjustedVelocity.y;
	}

	if ( !body->HasActivityType( IBody::MOTION_CONTROLLED_Z ) )
	{
		m_velocity.z = adjustedVelocity.z;
	}


	// collision resolution may create very high instantaneous velocities, limit it
	Vector2D groundVel = m_velocity.AsVector2D();
	m_actualSpeed = groundVel.NormalizeInPlace();

	if (IsOnGround())
	{
		if (m_actualSpeed > GetRunSpeed())
		{
			m_actualSpeed = GetRunSpeed();
			m_velocity.x = m_actualSpeed * groundVel.x;
			m_velocity.y = m_actualSpeed * groundVel.y;
		}

		// remove downward velocity when landing on the ground
		if (!wasOnGround)
		{
			m_velocity.z = 0.0f;
			m_acceleration.z = 0.0f;
		}
	}
	else
	{
		// we're falling. if our velocity has become zero for any reason, shove it forward
		const float epsilon = 1.0f;
		if ( m_velocity.IsLengthLessThan(epsilon))
		{
			m_velocity = GetRunSpeed() * GetGroundMotionVector();
		}
	}

	// update entity velocity to that of locomotor
	bot->GetEntity()->SetAbsVelocity(m_velocity);


#ifdef LEANING
	// lean sideways proportional to lateral acceleration
	QAngle lean = GetDesiredLean();
	
	float sideAccel = DotProduct( right, m_acceleration );
	float slide = sideAccel / GetMaxAcceleration();

	// max lean depends on how fast we're actually moving
	float maxLeanAngle = NextBotLeanMaxAngle.GetFloat() * m_actualSpeed / GetRunSpeed();

	// actual lean angle is proportional to lateral acceleration (sliding)
	float desiredSideLean = -maxLeanAngle * slide;
	
	lean.y += ( desiredSideLean - lean.y ) * NextBotLeanRate.GetFloat() * deltaT;

	SetDesiredLean(lean);
#endif // _DEBUG


	// reset acceleration accumulation
	m_acceleration = vec3_origin;
}

void NextBotGroundLocomotion::ApplyAccumulatedApproach(void)
{
	Vector rawPos = GetFeet();

	const float deltaT = GetUpdateInterval();

	if ( deltaT <= 0.0f )
		return;

	if ( m_accumApproachWeights > 0.0f )
	{
		Vector approachDelta = m_accumApproachVectors / m_accumApproachWeights;

		// limit total movement to our max speed
		float maxMove = GetRunSpeed() * deltaT;

		float desiredMove = approachDelta.NormalizeInPlace();
		if ( desiredMove > maxMove )
		{
			desiredMove = maxMove;
		}

		rawPos += desiredMove * approachDelta;

		m_accumApproachVectors = vec3_origin;
		m_accumApproachWeights = 0.0f;
	}

	// can only move in 2D - geometry moves us up and down
	Vector pos( rawPos.x, rawPos.y, GetFeet().z );
	
	INextBot* bot = GetBot();
	if (!bot->GetBodyInterface()->IsPostureMobile())
	{
		// body is not in a movable state right now
		return;
	}

	Vector currentPos = bot->GetPosition();

	// compute unit vector to goal position
	m_moveVector = pos - currentPos;
	m_moveVector.z = 0.0f;
	float change = m_moveVector.NormalizeInPlace();

	const float epsilon = 0.001f;
	if ( change < epsilon )
	{
		// no motion
		m_forwardLean = 0.0f;
		m_sideLean = 0.0f;
		return;
	}

	Vector newPos;

	// if we just started a jump, don't snap to the ground - let us get in the air first
	if (DidJustJump() || !IsOnGround())
	{
		if (false && m_isClimbingUpToLedge)	// causes bots to hang in air stuck against edges
		{
			// drive towards the approach position in XY to help reach ledge
			m_moveVector = m_ledgeJumpGoalPos - currentPos;
			m_moveVector.z = 0.0f;
			m_moveVector.NormalizeInPlace();
			
			m_acceleration += GetMaxAcceleration() * m_moveVector;
		}
	}
	else if (IsOnGround())
	{
		// on the ground - move towards the approach position
		m_isClimbingUpToLedge = false;
		
		// snap forward movement vector along floor
		const Vector &groundNormal = GetGroundNormal();
		
		Vector left(-m_moveVector.y, m_moveVector.x, 0.0f);
		m_moveVector = CrossProduct(left, groundNormal);
		m_moveVector.NormalizeInPlace();
		
		// limit maximum forward speed from self-acceleration
		float forwardSpeed = DotProduct(m_velocity, m_moveVector);
		
		float maxSpeed = MIN(m_desiredSpeed, GetSpeedLimit());
		
		if (forwardSpeed < maxSpeed)
		{
			float ratio = (forwardSpeed <= 0.0f) ? 0.0f : (forwardSpeed / maxSpeed);
			float governor = 1.0f - (ratio * ratio * ratio * ratio);
			
			// accelerate towards goal
			m_acceleration += governor * GetMaxAcceleration() * m_moveVector;
		}
	}
}

void NextBotGroundLocomotion::UpdatePosition(const Vector &newPos)
{
	INextBot* bot = GetBot();
	if (base_npc_freeze.GetBool() || (bot->GetEntity()->GetFlags() & FL_FROZEN) != 0 || newPos == bot->GetPosition())
	{
		return;
	}

	// avoid very nearby Actors to simulate "mushy" collisions between actors in contact with each other
	//Vector adjustedNewPos = ResolveZombieCollisions( newPos );
	Vector adjustedNewPos = newPos;

	// check for collisions during move and resolve them	
	const int recursionLimit = 3;
	Vector safePos = mResolveCollision(this, bot->GetPosition(), adjustedNewPos, recursionLimit);

	// set the bot's position
	if (bot->GetIntentionInterface()->IsPositionAllowed(bot, safePos) != ANSWER_NO)
	{
		bot->SetPosition(safePos);
	}
}

bool NextBotGroundLocomotion::TraverseLadder(void)
{
	// not climbing a ladder right now
	return false;
}

bool NextBotGroundLocomotion::DidJustJump(void) const
{
	return IsClimbingOrJumping() && (GetBot()->GetEntity()->GetAbsVelocity().z > 0.0f);
}

void NextBotGroundLocomotion::UpdateGroundConstraint(void)
{
	// if we're up on the upward arc of our jump, don't interfere by snapping to ground
	// don't do ground constraint if we're climbing a ladder
	if (DidJustJump() || IsAscendingOrDescendingLadder())
	{
		m_isUsingFullFeetTrace = false;
		return;
	}
		
	INextBot* bot = GetBot();
	IBody* body = bot->GetBodyInterface();
	if ( body == NULL )
	{
		return;
	}

	float halfWidth = body->GetHullWidth()/2.0f;
	
	// since we only care about ground collisions, keep hull short to avoid issues with low ceilings
	/// @TODO: We need to also check actual hull height to avoid interpenetrating the world
	float hullHeight = GetStepHeight();
	
	// always need tolerance even when jumping/falling to make sure we detect ground penetration
	// must be at least step height to avoid 'falling' down stairs
	const float stickToGroundTolerance = GetStepHeight() + 0.01f;

	trace_t ground;
	NextBotTraceFilterIgnoreActors filter(bot->GetEntity(), body->GetCollisionGroup());

	TraceHull(bot->GetPosition() + Vector( 0, 0, GetStepHeight() + 0.001f ),
					bot->GetPosition() + Vector( 0, 0, -stickToGroundTolerance ), 
					Vector( -halfWidth, -halfWidth, 0 ), 
					Vector( halfWidth, halfWidth, hullHeight ), 
					body->GetSolidMask(), &filter, &ground );

	if ( ground.startsolid )
	{
		return;
	}

	if (ground.fraction < 1.0f)
	{
		// there is ground below us
		m_groundNormal = ground.plane.normal;

		m_isUsingFullFeetTrace = false;
		
		// zero velocity normal to the ground
		float normalVel = DotProduct( m_groundNormal, m_velocity );
		m_velocity -= normalVel * m_groundNormal;
		
		// check slope limit
		if (ground.plane.normal.z < GetTraversableSlopeLimit())
		{
			// too steep to stand here

			// too steep to be ground - treat it like a wall hit
			if ((m_velocity.x * ground.plane.normal.x + m_velocity.y * ground.plane.normal.y ) <= 0.0f)
			{
				bot->OnContact(ground.m_pEnt, &ground);			
			}
			
			// we're contacting some kind of ground
			// zero accelerations normal to the ground

			float normalAccel = DotProduct(m_groundNormal, m_acceleration);
			m_acceleration -= normalAccel * m_groundNormal;

			// clear out upward velocity so we don't walk up lightpoles
			m_velocity.z = MIN(0, m_velocity.z);
			m_acceleration.z = MIN(0, m_acceleration.z);

			return;
		}
		
		// inform other components of collision if we didn't land on the 'world'
		if (ground.m_pEnt && !(((CBaseEntityHack *)ground.m_pEnt)->IsWorld()))
		{
			bot->OnContact(ground.m_pEnt, &ground);
		}

		// snap us to the ground 
		bot->SetPosition(ground.endpos);

		if (!IsOnGround())
		{
			// just landed
			bot->GetEntity()->SetGroundEntity(ground.m_pEnt);
			m_ground = ground.m_pEnt;

			// landing stops any jump in progress
			m_isJumping = false;
			m_isJumpingAcrossGap = false;

			bot->OnLandOnGround(ground.m_pEnt);
		}
	}
	else
	{
		// not on the ground
		if (IsOnGround())
		{
			bot->OnLeaveGround(bot->GetEntity()->GetGroundEntity());
			if (!IsClimbingUpToLedge() && !IsJumpingAcrossGap())
			{
				m_isUsingFullFeetTrace = true; // We're in the air and there's space below us, so use the full trace
				m_acceleration.z -= GetGravity(); // start our gravity now
			}
		}		
	}
}
*/