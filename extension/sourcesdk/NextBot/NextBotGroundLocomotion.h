#ifndef NEXT_BOT_GROUND_LOCOMOTION_H
#define NEXT_BOT_GROUND_LOCOMOTION_H

#include "NextBotLocomotionInterface.h"
#include "sourcesdk/nav_mesh.h"


class NextBotCombatCharacter;

class NextBotGroundLocomotion : public ILocomotion
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);
	static MCall<void, INextBot*> NextBotGroundLocomotion_Ctor;

	virtual ~NextBotGroundLocomotion() = 0;

	virtual void Reset(void) = 0;
	virtual void Update(void) = 0;

	virtual void Approach(const Vector& pos, float goalWeight = 1.0f) = 0;
	virtual void DriveTo(const Vector& pos) = 0;

	virtual bool ClimbUpToLedge(const Vector& landingGoal, const Vector& landingForward, const CBaseEntity* obstacle) = 0;
	virtual void JumpAcrossGap(const Vector& landingGoal, const Vector& landingForward) = 0;
	virtual void Jump(void) = 0;
	virtual bool IsClimbingOrJumping(void) const = 0;
	virtual bool IsClimbingUpToLedge(void) const = 0;
	virtual bool IsJumpingAcrossGap(void) const = 0;

	virtual void Run(void) = 0;
	virtual void Walk(void) = 0;
	virtual void Stop(void) = 0;
	virtual bool IsRunning(void) const = 0;
	virtual void SetDesiredSpeed(float speed) = 0;
	virtual float GetDesiredSpeed(void) const = 0;

	virtual float GetSpeedLimit(void) const = 0;

	virtual bool IsOnGround(void) const = 0;
	virtual void OnLeaveGround(CBaseEntity* ground) = 0;
	virtual void OnLandOnGround(CBaseEntity* ground) = 0;
	virtual CBaseEntity* GetGround(void) const = 0;
	virtual const Vector& GetGroundNormal(void) const = 0;

	virtual void ClimbLadder(const CNavLadder* ladder, const CNavArea* dismountGoal) = 0;
	virtual void DescendLadder(const CNavLadder* ladder, const CNavArea* dismountGoal) = 0;
	virtual bool IsUsingLadder(void) const = 0;
	virtual bool IsAscendingOrDescendingLadder(void) const = 0;

	virtual void FaceTowards(const Vector& target) = 0;

	virtual void SetDesiredLean(const QAngle& lean) = 0;
	virtual const QAngle& GetDesiredLean(void) const = 0;

	virtual const Vector& GetFeet(void) const = 0;

	virtual float GetStepHeight(void) const = 0;
	virtual float GetMaxJumpHeight(void) const = 0;
	virtual float GetDeathDropHeight(void) const = 0;

	virtual float GetRunSpeed(void) const = 0;
	virtual float GetWalkSpeed(void) const = 0;

	virtual float GetMaxAcceleration(void) const = 0;
	virtual float GetMaxDeceleration(void) const = 0;

	virtual const Vector& GetAcceleration(void) const = 0;
	virtual void SetAcceleration(const Vector& accel) = 0;

	virtual const Vector& GetVelocity(void) const = 0;
	virtual void SetVelocity(const Vector& vel) = 0;

	virtual void OnMoveToSuccess(const Path* path) = 0;
	virtual void OnMoveToFailure(const Path* path, MoveToFailureType reason) = 0;

	//private: expose the functions to our natives
	virtual float GetGravity(void) const = 0;
	virtual float GetFrictionForward(void) const = 0;
	virtual float GetFrictionSideways(void) const = 0;
	virtual float GetMaxYawRate(void) const = 0;

	//This function does no exist it's there to make sure we can update our velocity
	int TryNextBotMove(Vector* pFirstDest = NULL, trace_t* pFirstTrace = NULL);
	void StepMove(Vector& vecDestination, trace_t& trace);

	// Re-implement of the game's funcs without VPROF
	/*void NonVirtualUpdate(void);
	void UpdateGroundConstraint(void);
	void ApplyAccumulatedApproach(void);
	void UpdatePosition(const Vector&);
	bool TraverseLadder(void);
	bool DidJustJump(void) const;

	static MCall<Vector, const Vector&, const Vector&, int> mResolveCollision;*/


protected:
	NextBotCombatCharacter* m_nextBot;

	Vector m_priorPos;										// last update's position
	Vector m_lastValidPos;									// last valid position (not interpenetrating)

	Vector m_acceleration;
	Vector m_velocity;

	float m_desiredSpeed;									// speed bot wants to be moving
	float m_actualSpeed;									// actual speed bot is moving

	float m_maxRunSpeed;

	float m_forwardLean;
	float m_sideLean;
	QAngle m_desiredLean;

	bool m_isJumping;										// if true, we have jumped and have not yet hit the ground
	bool m_isJumpingAcrossGap;								// if true, we have jumped across a gap and have not yet hit the ground
	EHANDLE m_ground;										// have to manage this ourselves, since MOVETYPE_CUSTOM always NULLs out GetGroundEntity()
	Vector m_groundNormal;									// surface normal of the ground we are in contact with
	bool m_isClimbingUpToLedge;									// true if we are jumping up to an adjacent ledge
	Vector m_ledgeJumpGoalPos;
	bool m_isUsingFullFeetTrace;							// true if we're in the air and tracing the lowest StepHeight in ResolveCollision

	const CNavLadder* m_ladder;								// ladder we are currently climbing/descending
	const CNavArea* m_ladderDismountGoal;					// the area we enter when finished with our ladder move
	bool m_isGoingUpLadder;									// if false, we're going down

	CountdownTimer m_inhibitObstacleAvoidanceTimer;			// when active, turn off path following feelers

	CountdownTimer m_wiggleTimer;							// for wiggling
	NavRelativeDirType m_wiggleDirection;

	mutable Vector m_eyePos;								// for use with GetEyes(), etc.

	Vector m_moveVector;									// the direction of our motion in XY plane
	float m_moveYaw;										// global yaw of movement direction

	Vector m_accumApproachVectors;							// weighted sum of Approach() calls since last update
	float m_accumApproachWeights;
	bool m_bRecomputePostureOnCollision;

	CountdownTimer m_ignorePhysicsPropTimer;				// if active, don't collide with physics props (because we got stuck in one)
	EHANDLE m_ignorePhysicsProp;							// which prop to ignore
};

#endif

