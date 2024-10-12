#ifndef _NEXT_BOT_LOCOMOTION_INTERFACE_H_
#define _NEXT_BOT_LOCOMOTION_INTERFACE_H_

#include "NextBotComponentInterface.h"

class Path;
class INextBot;
class CNavLadder;

class ILocomotion : public INextBotComponent
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	ILocomotion(INextBot* bot);
	virtual ~ILocomotion();

	virtual void Reset(void);
	virtual void Update(void);
	virtual void Approach(const Vector& goalPos, float goalWeight = 1.0f);
	virtual void DriveTo(const Vector& pos);
	virtual bool ClimbUpToLedge(const Vector& landingGoal, const Vector& landingForward, const CBaseEntity* obstacle) { return true; }
	virtual void JumpAcrossGap(const Vector& landingGoal, const Vector& landingForward) { }
	virtual void Jump(void) { };
	virtual bool IsClimbingOrJumping(void) const;
	virtual bool IsClimbingUpToLedge(void) const;
	virtual bool IsJumpingAcrossGap(void) const;
	virtual bool IsScrambling(void) const;
	virtual void Run(void) { };
	virtual void Walk(void) { };
	virtual void Stop(void) { };
	virtual bool IsRunning(void) const;
	virtual void SetDesiredSpeed(float speed) { };
	virtual float GetDesiredSpeed(void) const;
	virtual void SetSpeedLimit(float speed) { };
	virtual float GetSpeedLimit(void) const { return 1000.0f; };

	virtual bool IsOnGround(void) const;
	virtual void OnLeaveGround(CBaseEntity* ground) { };
	virtual void OnLandOnGround(CBaseEntity* ground) { };
	virtual CBaseEntity* GetGround(void) const;
	virtual const Vector& GetGroundNormal(void) const;
	virtual float GetGroundSpeed(void) const;
	virtual const Vector& GetGroundMotionVector(void) const;

	virtual void ClimbLadder(const CNavLadder* ladder, const CNavArea* dismountGoal) { };
	virtual void DescendLadder(const CNavLadder* ladder, const CNavArea* dismountGoal) { };
	virtual bool IsUsingLadder(void) const;
	virtual bool IsAscendingOrDescendingLadder(void) const;
	virtual bool IsAbleToAutoCenterOnLadder(void) const { return false; };

	virtual void FaceTowards(const Vector& target) { };

	virtual void SetDesiredLean(const QAngle& lean) { };
	virtual const QAngle& GetDesiredLean(void) const;
	virtual bool IsAbleToJumpAcrossGaps(void) const;
	virtual bool IsAbleToClimb(void) const;

	virtual const Vector& GetFeet(void) const;

	virtual float GetStepHeight(void) const;
	virtual float GetMaxJumpHeight(void) const;
	virtual float GetDeathDropHeight(void) const;

	virtual float GetRunSpeed(void) const;
	virtual float GetWalkSpeed(void) const;

	virtual float GetMaxAcceleration(void) const;
	virtual float GetMaxDeceleration(void) const;

	virtual const Vector& GetVelocity(void) const;
	virtual float GetSpeed(void) const;
	virtual const Vector& GetMotionVector(void) const;

	virtual bool IsAreaTraversable(const CNavArea* baseArea) const;

	virtual float GetTraversableSlopeLimit(void) const;
	enum TraverseWhenType
	{
		IMMEDIATELY,
		EVENTUALLY
	};
	virtual bool IsPotentiallyTraversable(const Vector& from, const Vector& to, TraverseWhenType when = EVENTUALLY, float* fraction = NULL) const;
	virtual bool HasPotentialGap(const Vector& from, const Vector& to, float* fraction = NULL) const;
	virtual bool IsGap(const Vector& pos, const Vector& forward) const;
	virtual bool IsEntityTraversable(CBaseEntity* obstacle, TraverseWhenType when = EVENTUALLY) const;
	virtual bool IsStuck(void) const;
	virtual float GetStuckDuration(void) const;
	virtual void ClearStuckStatus(const char* reason = "");

	virtual bool IsAttemptingToMove(void) const;

	void TraceHull(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, unsigned int fMask, ITraceFilter* pFilter, trace_t* pTrace) const;

	virtual bool ShouldCollideWith(const CBaseEntity* object) const { return true; };

	virtual void AdjustPosture(const Vector& moveGoal);
	virtual void StuckMonitor(void);

protected:
	Vector m_motionVector;
	Vector m_groundMotionVector;
	float m_speed;
	float m_groundSpeed;

	bool m_isStuck;
	IntervalTimer m_stuckTimer;
	CountdownTimer m_stillStuckTimer;
	Vector m_stuckPos;
	IntervalTimer m_moveRequestTimer;

public:
	static VCall<bool, const Vector&, const Vector&, const CBaseEntity*> vClimbUpToLedge;
	static VCall<void, const Vector&, const Vector&> vJumpAcrossGap;
	static VCall<bool> vIsClimbingUpToLedge;
	static VCall<bool> vIsJumpingAcrossGap;
	static VCall<bool> vIsAbleToJumpAcrossGaps;
	static VCall<bool> vIsAbleToClimb;
	static VCall<float> vGetStepHeight;
	static VCall<float> vGetMaxJumpHeight;
	static VCall<float> vGetDeathDropHeight;
	static VCall<float> vGetRunSpeed;
	static VCall<float> vGetWalkSpeed;
	static VCall<float> vGetMaxAcceleration;
	static VCall<bool, CBaseEntity*, TraverseWhenType> vIsEntityTraversable;
	static VCall<bool, const CBaseEntity*> vShouldCollideWith;
};

inline bool ILocomotion::IsAbleToJumpAcrossGaps( void ) const
{
	return true;
}

inline bool ILocomotion::IsAbleToClimb( void ) const
{
	return true;
}

inline bool ILocomotion::IsAttemptingToMove( void ) const
{
	return m_moveRequestTimer.HasStarted() && m_moveRequestTimer.GetElapsedTime() < 0.25f;
}

inline bool ILocomotion::IsScrambling( void ) const
{
	return !IsOnGround() || IsClimbingOrJumping() || IsAscendingOrDescendingLadder();
}

inline bool ILocomotion::IsClimbingOrJumping( void ) const
{
	return false;
}

inline bool ILocomotion::IsClimbingUpToLedge( void ) const
{
	return false;
}

inline bool ILocomotion::IsJumpingAcrossGap( void ) const
{
	return false;
}

inline bool ILocomotion::IsRunning( void ) const
{
	return false;
}

inline float ILocomotion::GetDesiredSpeed( void ) const
{
	return 0.0f;
}

inline bool ILocomotion::IsOnGround( void ) const
{
	return false;
}

inline CBaseEntity *ILocomotion::GetGround( void ) const
{
	return NULL;
}

inline const Vector &ILocomotion::GetGroundNormal( void ) const
{
	return vec3_origin;
}

inline float ILocomotion::GetGroundSpeed( void ) const
{
	return m_groundSpeed;
}

inline const Vector & ILocomotion::GetGroundMotionVector( void ) const
{
	return m_groundMotionVector;
}

inline bool ILocomotion::IsUsingLadder( void ) const
{
	return false;
}

inline bool ILocomotion::IsAscendingOrDescendingLadder( void ) const
{
	return false;
}

inline const QAngle &ILocomotion::GetDesiredLean( void ) const
{
	return vec3_angle;
}

inline float ILocomotion::GetStepHeight( void ) const
{
	return 0.0f;
}

inline float ILocomotion::GetMaxJumpHeight( void ) const
{
	return 0.0f;
}

inline float ILocomotion::GetDeathDropHeight( void ) const
{
	return 0.0f;
}

inline float ILocomotion::GetRunSpeed( void ) const
{
	return 0.0f;
}

inline float ILocomotion::GetWalkSpeed( void ) const
{
	return 0.0f;
}

inline float ILocomotion::GetMaxAcceleration( void ) const
{
	return 0.0f;
}

inline float ILocomotion::GetMaxDeceleration( void ) const
{
	return 0.0f;
}

inline const Vector &ILocomotion::GetVelocity( void ) const
{
	return vec3_origin;
}

inline float ILocomotion::GetSpeed( void ) const
{
	return m_speed;
}

inline const Vector & ILocomotion::GetMotionVector( void ) const
{
	return m_motionVector;
}

inline float ILocomotion::GetTraversableSlopeLimit( void ) const	
{ 
	return 0.6; 
}

inline bool ILocomotion::IsStuck( void ) const
{
	return m_isStuck;
}

inline float ILocomotion::GetStuckDuration( void ) const
{
	return ( IsStuck() ) ? m_stuckTimer.GetElapsedTime() : 0.0f;
}

inline void ILocomotion::TraceHull(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, unsigned int fMask, ITraceFilter* pFilter, trace_t* pTrace) const
{
	Ray_t ray;
	ray.Init(start, end, mins, maxs);
	enginetrace->TraceRay(ray, fMask, pFilter, pTrace);
}

#endif

