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

	virtual ~ILocomotion() {};

	virtual void Reset(void) = 0;
	virtual void Update(void) = 0;
	virtual void Approach(const Vector& goalPos, float goalWeight = 1.0f) = 0;
	virtual void DriveTo(const Vector& pos) = 0;
	virtual bool ClimbUpToLedge(const Vector& landingGoal, const Vector& landingForward, const CBaseEntity* obstacle) { return true; }
	virtual void JumpAcrossGap(const Vector& landingGoal, const Vector& landingForward) { }
	virtual void Jump(void) { };
	virtual bool IsClimbingOrJumping(void) const = 0;
	virtual bool IsClimbingUpToLedge(void) const = 0;
	virtual bool IsJumpingAcrossGap(void) const = 0;
	virtual bool IsScrambling(void) const = 0;
	virtual void Run(void) { };
	virtual void Walk(void) { };
	virtual void Stop(void) { };
	virtual bool IsRunning(void) const = 0;
	virtual void SetDesiredSpeed(float speed) { };
	virtual float GetDesiredSpeed(void) const = 0;
	virtual void SetSpeedLimit(float speed) { };
	virtual float GetSpeedLimit(void) const { return 1000.0f; };

	virtual bool IsOnGround(void) const = 0;
	virtual void OnLeaveGround(CBaseEntity* ground) { };
	virtual void OnLandOnGround(CBaseEntity* ground) { };
	virtual CBaseEntity* GetGround(void) const = 0;
	virtual const Vector& GetGroundNormal(void) const = 0;
	virtual float GetGroundSpeed(void) const = 0;
	virtual const Vector& GetGroundMotionVector(void) const = 0;

	virtual void ClimbLadder(const CNavLadder* ladder, const CNavArea* dismountGoal) { };
	virtual void DescendLadder(const CNavLadder* ladder, const CNavArea* dismountGoal) { };
	virtual bool IsUsingLadder(void) const = 0;
	virtual bool IsAscendingOrDescendingLadder(void) const = 0;
	virtual bool IsAbleToAutoCenterOnLadder(void) const { return false; };

	virtual void FaceTowards(const Vector& target) { };

	virtual void SetDesiredLean(const QAngle& lean) { };
	virtual const QAngle& GetDesiredLean(void) const = 0;
	virtual bool IsAbleToJumpAcrossGaps(void) const = 0;
	virtual bool IsAbleToClimb(void) const = 0;

	virtual const Vector& GetFeet(void) const = 0;

	virtual float GetStepHeight(void) const = 0;
	virtual float GetMaxJumpHeight(void) const = 0;
	virtual float GetDeathDropHeight(void) const = 0;

	virtual float GetRunSpeed(void) const = 0;
	virtual float GetWalkSpeed(void) const = 0;

	virtual float GetMaxAcceleration(void) const = 0;
	virtual float GetMaxDeceleration(void) const = 0;

	virtual const Vector& GetVelocity(void) const = 0;
	virtual float GetSpeed(void) const = 0;
	virtual const Vector& GetMotionVector(void) const = 0;

	virtual bool IsAreaTraversable(const CNavArea* baseArea) const = 0;

	virtual float GetTraversableSlopeLimit(void) const = 0;
	enum TraverseWhenType
	{
		IMMEDIATELY,
		EVENTUALLY
	};
	virtual bool IsPotentiallyTraversable(const Vector& from, const Vector& to, TraverseWhenType when = EVENTUALLY, float* fraction = NULL) const = 0;
	virtual bool HasPotentialGap(const Vector& from, const Vector& to, float* fraction = NULL) const = 0;
	virtual bool IsGap(const Vector& pos, const Vector& forward) const = 0;
	virtual bool IsEntityTraversable(CBaseEntity* obstacle, TraverseWhenType when = EVENTUALLY) const = 0;
	virtual bool IsStuck(void) const = 0;
	virtual float GetStuckDuration(void) const = 0;
	virtual void ClearStuckStatus(const char* reason = "") = 0;

	virtual bool IsAttemptingToMove(void) const = 0;

	void TraceHull(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, unsigned int fMask, ITraceFilter* pFilter, trace_t* pTrace) const;

	virtual bool ShouldCollideWith(const CBaseEntity* object) const = 0;


	//protected: they should be protected but we need to expose them to our natives
	virtual void AdjustPosture(const Vector& moveGoal) = 0;
	virtual void StuckMonitor(void) = 0;

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
	VCALL_DECLARE_MEMBER(ClimbUpToLedge, bool, const Vector&, const Vector&, const CBaseEntity*)
	VCALL_DECLARE_MEMBER(JumpAcrossGap, void, const Vector&, const Vector&)
	VCALL_DECLARE_MEMBER(IsClimbingUpToLedge, bool)
	VCALL_DECLARE_MEMBER(IsJumpingAcrossGap, bool)
	VCALL_DECLARE_MEMBER(IsAbleToJumpAcrossGaps, bool)
	VCALL_DECLARE_MEMBER(IsAbleToClimb, bool)
	VCALL_DECLARE_MEMBER(GetStepHeight, float)
	VCALL_DECLARE_MEMBER(GetMaxJumpHeight, float)
	VCALL_DECLARE_MEMBER(GetDeathDropHeight, float)
	VCALL_DECLARE_MEMBER(GetRunSpeed, float)
	VCALL_DECLARE_MEMBER(GetWalkSpeed, float)
	VCALL_DECLARE_MEMBER(GetMaxAcceleration, float)
	VCALL_DECLARE_MEMBER(IsEntityTraversable, bool, CBaseEntity*, ILocomotion::TraverseWhenType)
	VCALL_DECLARE_MEMBER(ShouldCollideWith, bool, const CBaseEntity*)
};

inline void ILocomotion::TraceHull(const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, unsigned int fMask, ITraceFilter* pFilter, trace_t* pTrace) const
{
	Ray_t ray;
	ray.Init(start, end, mins, maxs);
	enginetrace->TraceRay(ray, fMask, pFilter, pTrace);
}

#endif

