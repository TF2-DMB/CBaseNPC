
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
		vClimbUpToLedge.Init(config, "ILocomotion::ClimbUpToLedge");
		vJumpAcrossGap.Init(config, "ILocomotion::JumpAcrossGap");
		vIsClimbingUpToLedge.Init(config, "ILocomotion::IsClimbingUpToLedge");
		vIsJumpingAcrossGap.Init(config, "ILocomotion::IsJumpingAcrossGap");
		vIsAbleToJumpAcrossGaps.Init(config, "ILocomotion::IsAbleToJumpAcrossGaps");
		vIsAbleToClimb.Init(config, "ILocomotion::IsAbleToClimb");
		vGetStepHeight.Init(config, "ILocomotion::GetStepHeight");
		vGetMaxJumpHeight.Init(config, "ILocomotion::GetMaxJumpHeight");
		vGetDeathDropHeight.Init(config, "ILocomotion::GetDeathDropHeight");
		vGetRunSpeed.Init(config, "ILocomotion::GetRunSpeed");
		vGetWalkSpeed.Init(config, "ILocomotion::GetWalkSpeed");
		vGetMaxAcceleration.Init(config, "ILocomotion::GetMaxAcceleration");
		vIsEntityTraversable.Init(config, "ILocomotion::IsEntityTraversable");
		vShouldCollideWith.Init(config, "ILocomotion::ShouldCollideWith");
	}
	catch (const std::exception& e)
	{
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}

	return true;
}