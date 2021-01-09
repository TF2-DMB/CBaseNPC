#ifndef _NEXT_BOT_BODY_INTERFACE_H_
#define _NEXT_BOT_BODY_INTERFACE_H_

#include "animation.h"
#include "NextBotComponentInterface.h"
#include <ai_activity.h>

class INextBot;
class INextBotReply;

class IBody : public INextBotComponent
{
public:
	virtual ~IBody() { };

	virtual void Reset(void) { INextBotComponent::Reset(); };
	virtual void Update(void) { };
	virtual bool SetPosition(const Vector& pos) = 0;

	virtual const Vector& GetEyePosition(void) const = 0;
	virtual const Vector& GetViewVector(void) const = 0;

	enum LookAtPriorityType
	{
		BORING,
		INTERESTING,
		IMPORTANT,
		CRITICAL,
		MANDATORY
	};
	virtual void AimHeadTowards(const Vector& lookAtPos,
		LookAtPriorityType priority = BORING,
		float duration = 0.0f,
		INextBotReply* replyWhenAimed = NULL,
		const char* reason = NULL) = 0;
	virtual void AimHeadTowards(CBaseEntity* subject,
		LookAtPriorityType priority = BORING,
		float duration = 0.0f,
		INextBotReply* replyWhenAimed = NULL,
		const char* reason = NULL) = 0;

	virtual bool IsHeadAimingOnTarget(void) const = 0;
	virtual bool IsHeadSteady(void) const = 0;
	virtual float GetHeadSteadyDuration(void) const = 0;
	virtual float GetHeadAimSubjectLeadTime(void) const = 0;
	virtual float GetHeadAimTrackingInterval(void) const = 0;
	virtual void ClearPendingAimReply(void) { }

	virtual float GetMaxHeadAngularVelocity(void) const = 0;

	enum ActivityType
	{
		MOTION_CONTROLLED_XY = 0x0001,
		MOTION_CONTROLLED_Z = 0x0002,
		ACTIVITY_UNINTERRUPTIBLE = 0x0004,
		ACTIVITY_TRANSITORY = 0x0008,
		ENTINDEX_PLAYBACK_RATE = 0x0010,
	};
	virtual bool StartActivity(Activity act, unsigned int flags = 0) = 0;
	virtual int SelectAnimationSequence(Activity act) const = 0;

	virtual Activity GetActivity(void) const = 0;
	virtual bool IsActivity(Activity act) const = 0;
	virtual bool HasActivityType(unsigned int flags) const = 0;

	enum PostureType
	{
		STAND,
		CROUCH,
		SIT,
		CRAWL,
		LIE
	};
	virtual void SetDesiredPosture(PostureType posture) { };
	virtual PostureType GetDesiredPosture(void) const = 0;
	virtual bool IsDesiredPosture(PostureType posture) const = 0;
	virtual bool IsInDesiredPosture(void) const = 0;

	virtual PostureType GetActualPosture(void) const = 0;
	virtual bool IsActualPosture(PostureType posture) const = 0;

	virtual bool IsPostureMobile(void) const = 0;
	virtual bool IsPostureChanging(void) const = 0;

	enum ArousalType
	{
		NEUTRAL,
		ALERT,
		INTENSE
	};
	virtual void SetArousal(ArousalType arousal) { };
	virtual ArousalType GetArousal(void) const = 0;
	virtual bool IsArousal(ArousalType arousal) const = 0;


	virtual float GetHullWidth(void) const = 0;
	virtual float GetHullHeight(void) const = 0;
	virtual float GetStandHullHeight(void) const = 0;
	virtual float GetCrouchHullHeight(void) const = 0;
	virtual const Vector& GetHullMins(void) const = 0;
	virtual const Vector& GetHullMaxs(void) const = 0;

	virtual unsigned int GetSolidMask(void) const = 0;
	virtual unsigned int GetCollisionGroup(void) const = 0;
};



#endif // _NEXT_BOT_BODY_INTERFACE_H_
