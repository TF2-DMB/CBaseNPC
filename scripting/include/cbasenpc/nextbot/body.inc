#if defined _CBASENPC_NEXTBOT_BODY_INC_
 #endinput
#endif
#define _CBASENPC_NEXTBOT_BODY_INC_

enum LookAtPriorityType
{
	BORING,
	INTERESTING,
	IMPORTANT,
	CRITICAL,
	MANDATORY
};

enum ActivityType 
{ 
	MOTION_CONTROLLED_XY	= 0x0001,
	MOTION_CONTROLLED_Z		= 0x0002,
	ACTIVITY_UNINTERRUPTIBLE= 0x0004,
	ACTIVITY_TRANSITORY		= 0x0008,
	ENTINDEX_PLAYBACK_RATE	= 0x0010,
};
	
enum PostureType
{
	STAND,
	CROUCH,
	SIT,
	CRAWL,
	LIE
};
	
enum ArousalType
{
	NEUTRAL,
	ALERT,
	INTENSE
};

/**
 * Component that acts as an interface to the class that contains the NextBot.
 */
methodmap IBody < INextBotComponent
{
	/**
	 * Sets the world position of the bot.
	 *
	 * @param pos      Position vector
	 * @return         True if successful, false otherwise
	 */
	public native bool SetPosition(const float pos[3]);

	/**
	 * Gets the eye world position.
	 *
	 * @param pos      Position vector
	 */
	public native void GetEyePosition(float pos[3]);

	/**
	 * Gets the world directional vector of where the bot is looking.
	 *
	 * @param view      Directional vector
	 */
	public native void GetViewVector(float view[3]);

	//public native void AimHeadTowards(float pos[3], LookAtPriorityType priority, float duration, PluginBotReply reply, const char[] reason);
	//public native void AimHeadTowardsEx(int entity, LookAtPriorityType priority, float duration, PluginBotReply reply, const char[] reason);

	public native bool IsHeadAimingOnTarget();
	public native bool IsHeadSteady();
	public native float GetHeadSteadyDuration();
	public native float GetHeadAimSubjectLeadTime();
	public native float GetHeadAimTrackingInterval();
	public native void ClearPendingAimReply();
	public native float GetMaxHeadAngularVelocity();

	//public native bool StartActivity(Activity act, int unknown);
	//public native int SelectAnimationSequence(Activity act);
	//public native Activity GetActivity();
	//public native bool IsActivity(Activity act);
	//public native bool HasActivityType(int unknown);

	public native void SetDesiredPosture(PostureType posture);
	public native PostureType GetDesiredPosture();
	public native bool IsDesiredPosture(PostureType posture);
	public native bool IsInDesiredPosture();
	public native PostureType GetActualPosture();
	public native bool IsActualPosture(PostureType posture);
	public native bool IsPostureMobile();
	public native bool IsPostureChanging();

	public native void SetArousal(ArousalType arousal);
	public native ArousalType GetArousal();
	public native bool IsArousal(ArousalType arousal);

	public native float GetHullWidth();
	public native float GetHullHeight();
	public native float GetStandHullHeight();
	public native float GetCrouchHullHeight();
	public native void GetHullMins(float mins[3]);
	public native void GetHullMaxs(float maxs[3]);

	public native int GetSolidMask();
	public native int GetCollisionGroup();
};