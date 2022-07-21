#include "toolsnextbot.h"

ConVar* NextBotPlayerStop = nullptr;

bool ToolsNextBot::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	return (NextBotManager::Init(config, error, maxlength)
		&& INextBotComponent::Init(config, error, maxlength)
		&& ILocomotion::Init(config, error, maxlength)
		&& NextBotCombatCharacter::Init(config, error, maxlength)
		&& NextBotGroundLocomotion::Init(config, error, maxlength));
}

ToolsNextBot::ToolsNextBot(CBaseCombatCharacterHack* link) :
	INextBot(),
	m_linkedEntity(link)
{
}

ToolsNextBotPlayer::ToolsNextBotPlayer(CBaseCombatCharacterHack* link) :
	ToolsNextBot(link)
{
	m_burningTimer.Invalidate();
}

void ToolsNextBotPlayer::Update()
{
	if ((GetEntity()->IsAlive() || !IsDormantWhenDead()) && !NextBotPlayerStop->GetBool())
	{
		ToolsNextBot::Update();	
	}
}

int ToolsNextBotPlayer::Hook_OnTakeDamage_Alive(const CTakeDamageInfo& info)
{
	if (info.GetDamageType() & DMG_BURN)
	{
		if (!m_burningTimer.HasStarted() || m_burningTimer.IsGreaterThen(1.0f))
		{
			OnIgnite();
			m_burningTimer.Start();
		}
	}

	OnInjured(info);
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

int ToolsNextBotPlayer::Hook_OnTakeDamage_Dying(const CTakeDamageInfo& info)
{
	if (info.GetDamageType() & DMG_BURN)
	{
		if (!m_burningTimer.HasStarted() || m_burningTimer.IsGreaterThen(1.0f))
		{
			OnIgnite();
			m_burningTimer.Start();
		}
	}

	OnInjured(info);
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void ToolsNextBotPlayer::Hook_Event_Killed(const CTakeDamageInfo& info)
{
	OnKilled(info);
	RETURN_META(MRES_IGNORED);
}

void ToolsNextBotPlayer::Hook_HandleAnimEvent(animevent_t* event)
{
	OnAnimationEvent(event);
	RETURN_META(MRES_IGNORED);
}

void ToolsNextBotPlayer::Hook_OnNavAreaChanged(CNavArea* enteredArea, CNavArea* leftArea)
{
	OnNavAreaChanged(enteredArea, leftArea);
	RETURN_META(MRES_IGNORED);
}

void ToolsNextBotPlayer::Hook_Touch(CBaseEntityHack* other)
{
	if (ShouldTouch(other))
	{
		trace_t result;
		result = GetEntity()->GetTouchTrace();
		OnContact(other, &result);
	}
	RETURN_META(MRES_IGNORED);
}

void ToolsNextBotPlayer::Hook_Weapon_Equip(CBaseEntityHack* weapon)
{
// TO-DO: L4D2 support
// OnPickUp(weapon, weapon->GetDroppingPlayer());
	OnPickUp(weapon, nullptr);
	RETURN_META(MRES_IGNORED);
}

void ToolsNextBotPlayer::Hook_Weapon_Drop(CBaseEntityHack* weapon, const Vector* target, const Vector* velocity)
{
	OnDrop(weapon);
	RETURN_META(MRES_IGNORED);
}

bool ToolsNextBotPlayer::IsDormantWhenDead(void) const
{
	return true;
}