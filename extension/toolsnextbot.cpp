#include "toolsnextbot.h"

ToolsNextBot::ToolsNextBot(CBaseCombatCharacterHack* link) :
	INextBot(),
	m_linkedEntity(link)
{
}

int ToolsNextBotPlayer::Hook_OnTakeDamage_Alive(const CTakeDamageInfo& info)
{
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

int ToolsNextBotPlayer::Hook_OnTakeDamage_Dying(const CTakeDamageInfo& info)
{
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

void ToolsNextBotPlayer::Hook_Event_Killed( const CTakeDamageInfo& info)
{
	OnKilled(info);
	RETURN_META(MRES_IGNORED);
}

void ToolsNextBotPlayer::Hook_HandleAnimEvent(animevent_t* event )
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