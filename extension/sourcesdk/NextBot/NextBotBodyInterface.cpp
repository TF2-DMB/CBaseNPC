#include "sourcesdk/NextBot/NextBotBodyInterface.h"
#include "sourcesdk/NextBot/NextBotInterface.h"
#include "sourcesdk/basecombatcharacter.h"

bool IBody::SetPosition(const Vector &pos)
{
	GetBot()->GetEntity()->SetAbsOrigin(pos);
	return true;
}

const Vector &IBody::GetEyePosition(void) const
{
	static Vector eye;
	eye = GetBot()->GetEntity()->WorldSpaceCenter();
	return eye;
}

const Vector &IBody::GetViewVector(void) const
{
	static Vector view;
	AngleVectors(GetBot()->GetEntity()->EyeAngles(), &view);
	return view;
}

void IBody::AimHeadTowards(const Vector &lookAtPos, LookAtPriorityType priority, float duration, INextBotReply* replyWhenAimed, const char *reason)
{
	if (replyWhenAimed)
	{
		replyWhenAimed->OnFail(GetBot(), INextBotReply::FAILED);
	}
}

void IBody::AimHeadTowards(CBaseEntity* subject, LookAtPriorityType priority, float duration, INextBotReply* replyWhenAimed, const char *reason)
{
	if (replyWhenAimed)
	{
		replyWhenAimed->OnFail(GetBot(), INextBotReply::FAILED);
	}
}

bool IBody::IsHeadAimingOnTarget(void) const
{
	return false;
}