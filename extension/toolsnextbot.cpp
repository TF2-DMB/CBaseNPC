#include "toolsnextbot.h"

ConVar* NextBotPlayerStop = nullptr;

SH_DECL_MANUALHOOK0_void(ToolsNextBot_Spawn, 0, 0, 0);
SH_DECL_MANUALHOOK0_void(ToolsNextBot_PhysicsSimulate, 0, 0, 0);
SH_DECL_MANUALHOOK1(ToolsNextBot_OnTakeDamage_Alive, 0, 0, 0, int, const CTakeDamageInfo&);
SH_DECL_MANUALHOOK1(ToolsNextBot_OnTakeDamage_Dying, 0, 0, 0, int, const CTakeDamageInfo&);
SH_DECL_MANUALHOOK1_void(ToolsNextBot_Event_Killed, 0, 0, 0, const CTakeDamageInfo&);
SH_DECL_MANUALHOOK1_void(ToolsNextBot_HandleAnimEvent, 0, 0, 0, animevent_t*);
SH_DECL_MANUALHOOK2_void(ToolsNextBot_OnNavAreaChanged, 0, 0, 0, CNavArea*, CNavArea*);
SH_DECL_MANUALHOOK1_void(ToolsNextBot_Touch, 0, 0, 0, CBaseEntityHack*);
SH_DECL_MANUALHOOK1_void(ToolsNextBot_Weapon_Equip, 0, 0, 0, CBaseEntityHack*);
SH_DECL_MANUALHOOK3_void(ToolsNextBot_Weapon_Drop, 0, 0, 0, CBaseEntityHack*, const Vector*, const Vector*);

#define RECONFIGURE(name, sh) \
	if (config->GetOffset(name, &offset)) \
	{ \
		SH_MANUALHOOK_RECONFIGURE(sh, offset, 0, 0); \
	} \
	else \
	{ \
		snprintf(error, maxlength, "Couldn't find " name " offset!"); \
		return false; \
	}

bool ToolsNextBot::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	int offset;
	RECONFIGURE("CBaseEntity::Spawn", ToolsNextBot_Spawn)
	RECONFIGURE("CBaseEntity::PhysicsSimulate", ToolsNextBot_PhysicsSimulate)
	RECONFIGURE("CBaseCombatCharacter::OnTakeDamage_Alive", ToolsNextBot_OnTakeDamage_Alive)
	RECONFIGURE("CBaseCombatCharacter::OnTakeDamage_Dying", ToolsNextBot_OnTakeDamage_Dying)
	RECONFIGURE("CBaseEntity::Event_Killed", ToolsNextBot_Event_Killed)
	RECONFIGURE("CBaseAnimating::HandleAnimEvent", ToolsNextBot_HandleAnimEvent)
	RECONFIGURE("CBaseCombatCharacter::OnNavAreaChanged", ToolsNextBot_OnNavAreaChanged)
	RECONFIGURE("CBaseEntity::Touch", ToolsNextBot_Touch)
	RECONFIGURE("CBaseCombatCharacter::Weapon_Equip", ToolsNextBot_Weapon_Equip)
	RECONFIGURE("CBaseCombatCharacter::Weapon_Drop", ToolsNextBot_Weapon_Drop)

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

	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_Spawn, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_Spawn), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_PhysicsSimulate, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_PhysicsSimulate), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_OnTakeDamage_Alive, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_OnTakeDamage_Alive), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_OnTakeDamage_Dying, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_OnTakeDamage_Dying), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_Event_Killed, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_Event_Killed), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_HandleAnimEvent, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_HandleAnimEvent), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_OnNavAreaChanged, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_OnNavAreaChanged), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_Touch, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_Touch), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_Weapon_Equip, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_Weapon_Equip), false));
	m_hooks.push_back(SH_ADD_MANUALHOOK(ToolsNextBot_Weapon_Drop, link, SH_MEMBER(this, &ToolsNextBotPlayer::Hook_Weapon_Drop), false));
}

ToolsNextBotPlayer::~ToolsNextBotPlayer()
{
	for (auto it = m_hooks.begin(); it != m_hooks.end(); it++)
	{
		SH_REMOVE_HOOK_ID((*it));
	}
}

void ToolsNextBotPlayer::Update()
{
	if ((GetEntity()->IsAlive() || !IsDormantWhenDead()) && !NextBotPlayerStop->GetBool())
	{
		ToolsNextBot::Update();	
	}
}

void ToolsNextBotPlayer::Hook_Spawn(void)
{
	m_burningTimer.Invalidate();

	ToolsNextBot::Reset();
	RETURN_META(MRES_IGNORED);
}

void ToolsNextBotPlayer::Hook_PhysicsSimulate(void)
{
	VPROF("ToolsNextBotPlayer::PhysicsSimulate");

	if ((IsDormantWhenDead() && GetEntity()->GetLifeState() == LIFE_DEAD) || NextBotStop->GetBool())
	{
		RETURN_META(MRES_IGNORED);
	}

	if (BeginUpdate())
	{
		Update();
		EndUpdate();
	}
	else
	{
		GetBodyInterface()->Update();
	}

	RETURN_META(MRES_IGNORED);
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