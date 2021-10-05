
#include "sourcesdk/NextBot/NextBotLocomotionInterface.h"

VCall<bool, const Vector&, const Vector&, const CBaseEntity*> ILocomotion::vClimbUpToLedge;
VCall<void, const Vector&, const Vector&> ILocomotion::vJumpAcrossGap;
VCall<bool> ILocomotion::vIsClimbingUpToLedge;
VCall<bool> ILocomotion::vIsJumpingAcrossGap;
VCall<bool> ILocomotion::vIsAbleToJumpAcrossGaps;
VCall<bool> ILocomotion::vIsAbleToClimb;
VCall<float> ILocomotion::vGetStepHeight;
VCall<float> ILocomotion::vGetMaxJumpHeight;
VCall<float> ILocomotion::vGetDeathDropHeight;
VCall<float> ILocomotion::vGetRunSpeed;
VCall<float> ILocomotion::vGetWalkSpeed;
VCall<float> ILocomotion::vGetMaxAcceleration;
VCall<bool, CBaseEntity*, ILocomotion::TraverseWhenType> ILocomotion::vIsEntityTraversable;
VCall<bool, const CBaseEntity*> ILocomotion::vShouldCollideWith;

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