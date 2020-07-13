#ifndef _NEXT_BOT_LOCOMOTION_INTERFACE_H_
#define _NEXT_BOT_LOCOMOTION_INTERFACE_H_

#include "NextBotComponentInterface.h"

class Path;
class INextBot;
class CNavLadder;

class ILocomotion : public INextBotComponent
{
public:
	virtual ~ILocomotion() {};
	
	virtual void Reset( void ) = 0;
	virtual void Update( void ) = 0;
	virtual void Approach( const Vector &goalPos, float goalWeight = 1.0f ) = 0;
	virtual void DriveTo( const Vector &pos ) = 0;
	virtual bool ClimbUpToLedge( const Vector &landingGoal, const Vector &landingForward, const CBaseEntity *obstacle ) { return true; }
	virtual void JumpAcrossGap( const Vector &landingGoal, const Vector &landingForward ) { }
	virtual void Jump( void ) { };
	virtual bool IsClimbingOrJumping( void ) = 0;	
	virtual bool IsClimbingUpToLedge( void ) = 0;	
	virtual bool IsJumpingAcrossGap( void ) = 0;
	virtual bool IsScrambling( void ) = 0;	
	virtual void Run( void ) { };
	virtual void Walk( void ) { };							
	virtual void Stop( void ) { };
	virtual bool IsRunning( void ) const = 0;
	virtual void SetDesiredSpeed( float speed ) { };
	virtual float GetDesiredSpeed( void ) const = 0;
	virtual void SetSpeedLimit( float speed ) { };
	virtual float GetSpeedLimit( void ) const { return 1000.0f; };

	virtual bool IsOnGround( void ) = 0;
	virtual void OnLeaveGround( CBaseEntity *ground ) { };
	virtual void OnLandOnGround( CBaseEntity *ground ) { };
	virtual CBaseEntity *GetGround( void ) = 0;	
	virtual const Vector &GetGroundNormal( void ) = 0;	
	virtual float GetGroundSpeed( void ) = 0;	
	virtual const Vector &GetGroundMotionVector( void ) = 0;

	virtual void ClimbLadder( const CNavLadder *ladder, const CNavArea *dismountGoal ) { };
	virtual void DescendLadder( const CNavLadder *ladder, const CNavArea *dismountGoal ) { };
	virtual bool IsUsingLadder( void ) = 0;
	virtual bool IsAscendingOrDescendingLadder( void ) = 0;
	virtual bool IsAbleToAutoCenterOnLadder( void ) const { return false; };

	virtual void FaceTowards( const Vector &target ) { };

	virtual void SetDesiredLean( const QAngle &lean ) { };
	virtual const QAngle &GetDesiredLean( void ) = 0;	
	virtual bool IsAbleToJumpAcrossGaps( void ) = 0;
	virtual bool IsAbleToClimb( void ) = 0;	

	virtual const Vector &GetFeet( void ) const = 0;

	virtual float GetStepHeight( void ) = 0;
	virtual float GetMaxJumpHeight( void ) = 0;
	virtual float GetDeathDropHeight( void ) = 0;

	virtual float GetRunSpeed( void ) = 0;
	virtual float GetWalkSpeed( void ) = 0;

	virtual float GetMaxAcceleration( void ) = 0;
	virtual float GetMaxDeceleration( void ) = 0;

	virtual const Vector &GetVelocity( void ) = 0;
	virtual float GetSpeed( void ) const = 0;
	virtual const Vector &GetMotionVector( void ) = 0;

	virtual bool IsAreaTraversable( const CNavArea *baseArea ) = 0;

	virtual float GetTraversableSlopeLimit( void ) = 0;
	enum TraverseWhenType 
	{ 
		IMMEDIATELY,
		EVENTUALLY
	};
	virtual bool IsPotentiallyTraversable( const Vector &from, const Vector &to, TraverseWhenType when = EVENTUALLY, float *fraction = NULL ) = 0;
	virtual bool HasPotentialGap( const Vector &from, const Vector &to, float *fraction = NULL ) = 0;
	virtual bool IsGap( const Vector &pos, const Vector &forward ) = 0;
	virtual bool IsEntityTraversable( CBaseEntity *obstacle, TraverseWhenType when = EVENTUALLY ) = 0;
	virtual bool IsStuck( void ) = 0;
	virtual float GetStuckDuration( void ) = 0;
	virtual void ClearStuckStatus( const char *reason = "" ) = 0;

	virtual bool IsAttemptingToMove( void ) = 0;

	void TraceHull(const Vector& start, const Vector& end, const Vector &mins, const Vector &maxs, unsigned int fMask, ITraceFilter *pFilter, trace_t *pTrace) const;
	
	virtual bool ShouldCollideWith( const CBaseEntity *object ) = 0;


//protected: they should be protected but we need to expose them to our natives
	virtual void AdjustPosture( const Vector &moveGoal ) = 0;
	virtual void StuckMonitor( void ) = 0;

private:
	Vector m_motionVector;
	Vector m_groundMotionVector;
	float m_speed;
	float m_groundSpeed;

	bool m_isStuck;									
	IntervalTimer m_stuckTimer;						
	CountdownTimer m_stillStuckTimer;				
	Vector m_stuckPos;
	IntervalTimer m_moveRequestTimer;
};

inline void ILocomotion::TraceHull( const Vector& start, const Vector& end, const Vector &mins, const Vector &maxs, unsigned int fMask, ITraceFilter *pFilter, trace_t *pTrace ) const
{			
	Ray_t ray;
	ray.Init( start, end, mins, maxs );
	enginetrace->TraceRay( ray, fMask, pFilter, pTrace );
}

#endif

