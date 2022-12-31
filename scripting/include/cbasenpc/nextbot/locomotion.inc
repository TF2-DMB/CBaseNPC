#if defined _CBASENPC_NEXTBOT_LOCOMOTION_INC_
 #endinput
#endif
#define _CBASENPC_NEXTBOT_LOCOMOTION_INC_


enum TraverseWhenType 
{ 
	IMMEDIATELY,
	EVENTUALLY
};

/**
 * Component that manages the movement state of the NextBot.
 */
methodmap ILocomotion < INextBotComponent
{
	/**
	 * Move the bot towards the given position.
	 *
	 * @param dst            Position to move towards
	 * @param goalWeight     Weight. Multiple Approach() calls within a single
	 *                       frame will be combined into a single, weighted
	 *                       average goal.
	 */
	public native void Approach(const float dst[3], float goalWeight = 1.0);

	/**
	 * Move the bot towards the given position precisely.
	 *
	 * @param dst      Position to move towards
	 */
	public native void DriveTo(const float dst[3]);

	/**
	 * Traverses to an adjacent ledge.
	 *
	 * @param dst            Destination vector
	 * @param dir            Direction of movement
	 * @param obstacle       Entity to climb
	 * @return               True if was able to start climbing, false otherwise
	 */
	public native bool ClimbUpToLedge(const float dst[3], const float dir[3], int obstacle);

	/**
	 * Jumps towards the given destination.
	 *
	 * @param dst      Destination vector
	 * @param dir      Direction of movement
	 */
	public native void JumpAcrossGap(const float dst[3], const float dir[3]);

	/**
	 * Performs a simple jump
	 */
	public native void Jump();

	/**
	 * Whether the bot is currently climbing or jumping, or not.
	 *
	 * @return     True if jumping or climbing, false otherwise
	 */
	public native bool IsClimbingOrJumping();

	/**
	 * Whether the bot is currently climbing or not.
	 *
	 * @return     True if climbing, false otherwise
	 */
	public native bool IsClimbingUpToLedge();

	/**
	 * Whether the bot is currently jumping or not.
	 *
	 * @return     True if jumping, false otherwise
	 */
	public native bool IsJumpingAcrossGap();

	/**
	 * Whether the bot is in the middle of a complex action (climbing a ladder,
	 * climbing a ledge, jumping, etc) that shouldn't be interrupted.
	 *
	 * @return     True if scrambling, false otherwise
	 */
	public native bool IsScrambling();

	/**
	 * Sets movement to running.
	 */
	public native void Run();

	/**
	 * Sets movement to walking.
	 */
	public native void Walk();

	/**
	 * Stops all movement.
	 */
	public native void Stop();

	/**
	 * Whether the bot is in a running movement state or not.
	 *
	 * @return     True if running, false otherwise
	 */
	public native bool IsRunning();

	/**
	 * Sets the movement speed of the bot to the given value.
	 *
	 * @param speed     Speed to move at
	 */
	public native void SetDesiredSpeed(float speed);

	/**
	 * Returns the current desired movement speed of the bot.
	 *
	 * @return     Speed
	 */
	public native float GetDesiredSpeed();

	/**
	 * Sets the maximum speed of the bot.
	 *
	 * @param limit     Speed limit
	 */
	public native void SetSpeedLimit(float limit);

	/**
	 * Returns the maximum speed the bot can move at.
	 *
	 * @return     Speed limit
	 */
	public native float GetSpeedLimit();

	/**
	 * Returns whether the bot is on the ground or not.
	 *
	 * @return     True if on ground, false otherwise
	 */
	public native bool IsOnGround();

	/**
	 * Gets the entity that the bot is standing on.
	 *
	 * @return     Entity index of ground entity
	 */
	public native int GetGround();

	/**
	 * Retrieves the normal vector of the plane the bot is standing on.
	 *
	 * @param normal      Directional vector
	 */
	public native void GetGroundNormal(float normal[3]);

	/**
	 * The current speed the bot is moving at on the ground.
	 *
	 * @return     Ground speed
	 */
	public native float GetGroundSpeed();

	/**
	 * Retrieves the direction of ground movement of the bot.
	 *
	 * @param motion      Directional vector
	 */
	public native void GetGroundMotionVector(float motion[3]);

	/**
	 * Starts climbing up the given ladder to dismount to a given area.
	 *
	 * @param ladder             Ladder to climb
	 * @param dismountGoal       Area to dismount ladder from
	 */
	public native void ClimbLadder(CNavLadder ladder, CNavArea dismountGoal);

	/**
	 * Starts climbing down the ladder to dismount to a given area.
	 *
	 * @param ladder             Ladder to descend
	 * @param dismountGoal       Area to dismount ladder from
	 */
	public native void DescendLadder(CNavLadder ladder, CNavArea dismountGoal);

	/**
	 * Whether the bot is currently using a ladder or not.
	 *
	 * @return     True if using a ladder, false otherwise
	 */
	public native bool IsUsingLadder();

	/**
	 * Whether the bot is currently climbing up or down a ladder.
	 *
	 * @return     True if climbing a ladder, false otherwise
	 */
	public native bool IsAscendingOrDescendingLadder();

	public native bool IsAbleToAutoCenterOnLadder();

	/**
	 * Sets the look goal of the bot to the given position.
	 *
	 * @param pos      Position to look at
	 */
	public native void FaceTowards(const float pos[3]);

	public native void SetDesiredLean(const float lean[3]);
	public native void GetDesiredLean(float lean[3]);

	/**
	 * Whether the bot is allowed to jump or not.
	 *
	 * @return     True if allowed, false otherwise
	 */
	public native bool IsAbleToJumpAcrossGaps();

	/**
	 * Whether the bot is allowed to climb or not.
	 *
	 * @return     True if allowed, false otherwise
	 */
	public native bool IsAbleToClimb();

	/**
	 * Retrieves the location of the bot's feet.
	 *
	 * @param feet      Position vector
	 */
	public native void GetFeet(float feet[3]);

	/**
	 * Returns the maximum height the bot can traverse without jumping or
	 * dropping down.
	 *
	 * @return     Step height
	 */
	public native float GetStepHeight();

	/**
	 * Returns the maximum height of an obstacle the bot can traverse by
	 * jumping.
	 *
	 * @return     Jump height
	 */
	public native float GetMaxJumpHeight();

	/**
	 * Returns the maximum height the bot can safely drop down from a cliff.
	 *
	 * @return     Drop down height
	 */
	public native float GetDeathDropHeight();

	/**
	 * The run speed of the bot.
	 *
	 * @return     Speed
	 */
	public native float GetRunSpeed();

	/**
	 * The walk speed of the bot.
	 *
	 * @return     Speed
	 */
	public native float GetWalkSpeed();

	/**
	 * The acceleration of the bot.
	 *
	 * @return     Acceleration
	 */
	public native float GetMaxAcceleration();

	/**
	 * The deceleration of the bot.
	 *
	 * @return     Acceleration
	 */
	public native float GetMaxDeceleration();

	/**
	 * The current world velocity of the bot.
	 *
	 * @param vec      Velocity vector
	 */
	public native void GetVelocity(float vel[3]);

	/**
	 * The current speed of the bot.
	 *
	 * @return     Speed
	 */
	public native float GetSpeed();

	/**
	 * The current desired world movement direction of the bot.
	 *
	 * @param vec      Direction vector
	 */
	public native void GetMotionVector(float motion[3]);

	/**
	 * Whether or not the area can be used for navigation.
	 *
	 * @param area     CNavArea to test
	 * @return         True if traversable, false otherwise
	 */
	public native bool IsAreaTraversable(CNavArea area);

	/**
	 * The Z component of the steepest ground normal vector the bot can traverse.
	 *
	 * @return         Slope limit
	 */
	public native float GetTraversableSlopeLimit();

	/**
	 * Checks if the bot can safely move across a line specified by the given
	 * positions (no gaps, no obstacles). If false, fraction will be filled
	 * with fraction of walkable ray.
	 *
	 * @param from           Start position
	 * @param to             End position
	 * @param when           Types of entities to ignore
	 * @param fraction       Walkable trace fraction
	 * @return               True if traversable, false otherwise
	 */
	public native bool IsPotentiallyTraversable(const float from[3], const float to[3], TraverseWhenType when = EVENTUALLY, float &fraction = 0.0);

	/**
	 * Checks if there's a gap between the two positions that will need to be
	 * jumped over. If true, fraction will be filled with fraction of walkable
	 * ray.
	 *
	 * @param from      Start position
	 * @param to        End position
	 * @param fraction  Walkable trace fraction
	 * @return          True if there's a gap, false otherwise.
	 */
	public native bool HasPotentialGap(const float from[3], const float to[3], float &fraction = 0.0);

	/**
	 * Checks if the given position is a gap while moving in the
	 * specified direction.
	 *
	 * @param pos      Position vector
	 * @param to       Param description
	 * @return         True if position is a gap, false otherwise
	 */
	public native bool IsGap(const float pos[3], const float fwd[3]);

	/**
	 * Checks if the entity is traversable.
	 *
	 * @param entity     Entity index to check
	 * @param when       Type to check
	 * @return           True if traversable, false otherwise
	 * @error            Locomotion is invalid or entity is invalid
	 */
	public native bool IsEntityTraversable(int entity, TraverseWhenType when);

	/**
	 * Checks if the bot is currently stuck.
	 *
	 * @return     True if stuck, false otherwise
	 */
	public native bool IsStuck();

	/**
	 * Returns how long the bot has been stuck for.
	 *
	 * @return     Stuck duration
	 */
	public native float GetStuckDuration();

	/**
	 * Clears the bot of being stuck with a given reason.
	 *
	 * @param reason     Reason for clearing the stuck status.
	 */
	public native void ClearStuckStatus(const char[] reason = "");

	/**
	 * Whether the bot is attempting to move or not.
	 *
	 * @return     True if trying to move, false otherwise.
	 */
	public native bool IsAttemptingToMove();

	/**
	 * Whether the bot should collide with the given entity or not.
	 *
	 * @param entity     Entity to collide
	 * @return           True to collide, false otherwise
	 */
	public native bool ShouldCollideWith(int entity);

	public native void AdjustPosture(const float lean[3]);
	public native void StuckMonitor();
};

methodmap NextBotGroundLocomotion < ILocomotion
{
	/**
	 * Retrieves the acceleration vector.
	 *
	 * @param acel      Acceleration vector
	 */
	public native void GetAcceleration(float acel[3]);

	/**
	 * Sets the acceleration vector.
	 *
	 * @param acel      Acceleration vector
	 */
	public native void SetAcceleration(const float acel[3]);

	/**
	 * Sets the world velocity.
	 *
	 * @param vel       Velocity vector
	 */
	public native void SetVelocity(const float vel[3]);

	/**
	 * Gets the current gravity.
	 *
	 * @return     Gravity
	 */
	public native float GetGravity();

	/**
	 * Returns friction for forward movement.
	 *
	 * @return     Friction
	 */
	public native float GetFrictionForward();

	/**
	 * Returns friction for sideways movement.
	 *
	 * @return     Friction
	 */
	public native float GetFrictionSideways();

	/**
	 * Returns rate of change of yaw while looking towards a position.
	 *
	 * @return     Yaw rate
	 */
	public native float GetMaxYawRate();
}