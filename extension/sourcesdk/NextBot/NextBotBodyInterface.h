#ifndef _NEXT_BOT_BODY_INTERFACE_H_
#define _NEXT_BOT_BODY_INTERFACE_H_

#include "NextBotComponentInterface.h"
#include <ai_activity.h>
#include <vector.h>
#include <animation.h>

class INextBot;
class INextBotReply;

class IBody : public INextBotComponent
{
public:
	IBody(INextBot* bot) : INextBotComponent(bot) {};
	virtual ~IBody() { };

	virtual void Reset(void) { INextBotComponent::Reset(); };
	virtual void Update(void) { };
	virtual bool SetPosition(const Vector& pos);

	virtual const Vector& GetEyePosition(void) const;
	virtual const Vector& GetViewVector(void) const;

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
		INextBotReply* replyWhenAimed = nullptr,
		const char* reason = nullptr);
	virtual void AimHeadTowards(CBaseEntity* subject,
		LookAtPriorityType priority = BORING,
		float duration = 0.0f,
		INextBotReply* replyWhenAimed = nullptr,
		const char* reason = nullptr);

	virtual bool IsHeadAimingOnTarget(void) const;
	virtual bool IsHeadSteady(void) const { return true; };
	virtual float GetHeadSteadyDuration(void) const { return 0.0f; };
	virtual float GetHeadAimSubjectLeadTime(void) { return 0.0f; };
	virtual float GetHeadAimTrackingInterval(void) const { return 0.0f; };
	virtual void ClearPendingAimReply(void) { }

	virtual float GetMaxHeadAngularVelocity(void) const { return 1000.0f; };

	enum ActivityType
	{
		MOTION_CONTROLLED_XY = 0x0001,
		MOTION_CONTROLLED_Z = 0x0002,
		ACTIVITY_UNINTERRUPTIBLE = 0x0004,
		ACTIVITY_TRANSITORY = 0x0008,
		ENTINDEX_PLAYBACK_RATE = 0x0010,
	};
	virtual bool StartActivity(Activity act, unsigned int flags = 0) { return false; };
	virtual int SelectAnimationSequence(Activity act) const { return 0; };

	virtual Activity GetActivity(void) const { return ACT_INVALID; };
	virtual bool IsActivity(Activity act) const { return false; };
	virtual bool HasActivityType(unsigned int flags) const { return false; };

	enum PostureType
	{
		STAND,
		CROUCH,
		SIT,
		CRAWL,
		LIE
	};
	virtual void SetDesiredPosture(PostureType posture) { };
	virtual PostureType GetDesiredPosture(void) const { return IBody::STAND; };
	virtual bool IsDesiredPosture(PostureType posture) const { return true; };
	virtual bool IsInDesiredPosture(void) const { return true; };

	virtual PostureType GetActualPosture(void) const { return IBody::STAND; };
	virtual bool IsActualPosture(PostureType posture) const { return true; };

	virtual bool IsPostureMobile(void) const { return true; };
	virtual bool IsPostureChanging(void) const { return false; };

	enum ArousalType
	{
		NEUTRAL,
		ALERT,
		INTENSE
	};
	virtual void SetArousal(ArousalType arousal) { };
	virtual ArousalType GetArousal(void) const { return IBody::NEUTRAL; };
	virtual bool IsArousal(ArousalType arousal) const { return true; };


	virtual float GetHullWidth(void) const { return 26.0f; };
	virtual float GetHullHeight(void) const
	{
		switch(GetActualPosture())
		{
		case LIE:
			return 16.0f;

		case SIT:
		case CROUCH:
			return GetCrouchHullHeight();

		case STAND:
		default:
			return GetStandHullHeight();
		}
	};
	virtual float GetStandHullHeight(void) const { return 68.0f; };
	virtual float GetCrouchHullHeight(void) const { return 32.0f; };
	virtual const Vector& GetHullMins(void) const
	{
		static Vector hullMins;

		hullMins.x = -GetHullWidth()/2.0f;
		hullMins.y = hullMins.x;
		hullMins.z = 0.0f;

		return hullMins;
	};
	virtual const Vector& GetHullMaxs(void) const
	{
		static Vector hullMaxs;

		hullMaxs.x = GetHullWidth()/2.0f;
		hullMaxs.y = hullMaxs.x;
		hullMaxs.z = GetHullHeight();

		return hullMaxs;
	};

	virtual unsigned int GetSolidMask(void) const { return MASK_NPCSOLID; };
	virtual unsigned int GetCollisionGroup(void) const { return COLLISION_GROUP_NONE; };
};



#endif // _NEXT_BOT_BODY_INTERFACE_H_
