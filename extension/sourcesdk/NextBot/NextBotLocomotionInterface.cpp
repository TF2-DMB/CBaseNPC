
#include "sourcesdk/NextBot/NextBotLocomotionInterface.h"

VCALL_DEFINE_MEMBER(ILocomotion, ClimbUpToLedge, bool, const Vector&, const Vector&, const CBaseEntity*)
VCALL_DEFINE_MEMBER(ILocomotion, JumpAcrossGap, void, const Vector&, const Vector&)
VCALL_DEFINE_MEMBER(ILocomotion, IsClimbingUpToLedge, bool)
VCALL_DEFINE_MEMBER(ILocomotion, IsJumpingAcrossGap, bool)
VCALL_DEFINE_MEMBER(ILocomotion, IsAbleToJumpAcrossGaps, bool)
VCALL_DEFINE_MEMBER(ILocomotion, IsAbleToClimb, bool)
VCALL_DEFINE_MEMBER(ILocomotion, GetStepHeight, float)
VCALL_DEFINE_MEMBER(ILocomotion, GetMaxJumpHeight, float)
VCALL_DEFINE_MEMBER(ILocomotion, GetDeathDropHeight, float)
VCALL_DEFINE_MEMBER(ILocomotion, GetRunSpeed, float)
VCALL_DEFINE_MEMBER(ILocomotion, GetWalkSpeed, float)
VCALL_DEFINE_MEMBER(ILocomotion, GetMaxAcceleration, float)
VCALL_DEFINE_MEMBER(ILocomotion, IsEntityTraversable, bool, CBaseEntity*, ILocomotion::TraverseWhenType)
VCALL_DEFINE_MEMBER(ILocomotion, ShouldCollideWith, bool, const CBaseEntity*)

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