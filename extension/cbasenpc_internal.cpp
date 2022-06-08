#include "extension.h"
#include "cbasenpc_internal.h"
#include "cbasenpc_behavior.h"
#include "pluginentityfactory.h"
#include "sourcesdk/baseentity.h"
#include "sourcesdk/tf_gamerules.h"
#include "NextBot/Path/NextBotPathFollow.h"
#include <bspflags.h>
#include <ai_activity.h>
#include <util.h>

// CBaseEntity/any class with a vtable (npc destruction)
#ifdef __linux__
SH_DECL_MANUALHOOK0_void(CBaseNPC_Dtor, 1, 0, 0);
#else
SH_DECL_MANUALHOOK1_void(CBaseNPC_Dtor, 0, 0, 0, unsigned int);
#endif

// INextBotComponent
SH_DECL_HOOK0_void(INextBotComponent, Update, SH_NOATTRIB, 0);

// INextBot
SH_DECL_HOOK0(INextBot, GetIntentionInterface, const, 0, IIntention*);
SH_DECL_HOOK0(INextBot, GetLocomotionInterface, const, 0, ILocomotion*);
SH_DECL_HOOK0(INextBot, GetBodyInterface, const, 0, IBody*);

// IBody
SH_DECL_HOOK2(IBody, StartActivity, SH_NOATTRIB, 0, bool, Activity, unsigned int);
SH_DECL_HOOK0(IBody, GetHullWidth, const, 0, float);
SH_DECL_HOOK0(IBody, GetHullHeight, const, 0, float);
SH_DECL_HOOK0(IBody, GetStandHullHeight, const, 0, float);
SH_DECL_HOOK0(IBody, GetCrouchHullHeight, const, 0, float);
SH_DECL_HOOK0(IBody, GetHullMins, const, 0, const Vector&);
SH_DECL_HOOK0(IBody, GetHullMaxs, const, 0, const Vector&);
SH_DECL_HOOK0(IBody, GetSolidMask, const, 0, unsigned int);

CBaseNPCFactory* g_pBaseNPCFactory = nullptr;
void** CBaseNPC_Entity::vtable = nullptr;
MCall<void> CBaseNPC_Entity::mOriginalSpawn;
MCall<int, const CTakeDamageInfo&> CBaseNPC_Entity::mOriginalOnTakeDamage;
MCall<int, const CTakeDamageInfo&> CBaseNPC_Entity::mOriginalOnTakeDamage_Alive;
MCall<void> CBaseNPC_Entity::mOriginalUpdateOnRemove;

CBaseNPCFactory::CBaseNPCFactory()
: CustomFactory("base_npc", &NextBotCombatCharacter::NextBotCombatCharacter_Ctor)
{
	m_pInitialActionFactory = nullptr;
}

CBaseNPCFactory::~CBaseNPCFactory()
{
}

void CBaseNPCFactory::Create_Extra(CBaseEntityHack* ent)
{
	// Replace the vtable with ours
	if (CBaseNPC_Entity::vtable == nullptr)
	{
		CBaseNPC_Entity::vtable = vtable_dup(ent, NextBotCombatCharacter::vtable_entries);
		void* original = nullptr;
		original = CBaseEntityHack::vSpawn.Replace(CBaseNPC_Entity::vtable, &CBaseNPC_Entity::BotSpawn);
		CBaseNPC_Entity::mOriginalSpawn.Init(original);
		original = CBaseEntityHack::vOnTakeDamage.Replace(CBaseNPC_Entity::vtable, &CBaseNPC_Entity::OnTakeDamage);
		CBaseNPC_Entity::mOriginalOnTakeDamage.Init(original);
		original = CBaseCombatCharacterHack::vOnTakeDamage_Alive.Replace(CBaseNPC_Entity::vtable, &CBaseNPC_Entity::OnTakeDamage_Alive);
		CBaseNPC_Entity::mOriginalOnTakeDamage_Alive.Init(original);
		original = CBaseEntityHack::vUpdateOnRemove.Replace(CBaseNPC_Entity::vtable, &CBaseNPC_Entity::BotUpdateOnRemove);
		CBaseNPC_Entity::mOriginalUpdateOnRemove.Init(original);
	}
	vtable_replace(ent, CBaseNPC_Entity::vtable);
	new (((CBaseNPC_Entity*)ent)->GetNPC()) CBaseNPC_Entity::CBaseNPC((NextBotCombatCharacter*)ent, m_pInitialActionFactory);
	SetInitialActionFactory(nullptr);
}

void CBaseNPCFactory::Create_PostConstructor(CBaseEntityHack* ent)
{
	((CBaseNPC_Entity*)ent)->GetNPC()->SetEntity(ent);
}

size_t CBaseNPCFactory::GetEntitySize()
{
	return sizeof(CBaseNPC_Entity::CBaseNPC) + NextBotCombatCharacter::size_of;
}

void IBaseNPCComponent::OnPawnEvent(const char* eventName, cell_t eventData) 
{
	INextBotEventResponder* _this = dynamic_cast<INextBotEventResponder*>(this);

	if (_this)
	{
		for (INextBotEventResponder* sub = _this->FirstContainedResponder(); sub; sub = _this->NextContainedResponder(sub))
		{
			IBaseNPCComponent* responder = dynamic_cast<IBaseNPCComponent*>(sub);
			if (_this)
			{
				responder->OnPawnEvent(eventName, eventData);
			}
		}
	}
}

QueryResultType IBaseNPCComponent::OnPawnQuery(INextBot* bot, const char* queryName, cell_t data) const
{
	return ANSWER_UNDEFINED;
}

CBaseNPC_Entity::CBaseNPC::CBaseNPC(NextBotCombatCharacter* ent, CBaseNPCPluginActionFactory* initialActionFactory) : CExtNPC()
{
	INextBot* bot = GetBot();

	m_pIntention = new CBaseNPCIntention(bot, initialActionFactory);
	m_pMover = CBaseNPC_Locomotion::New(bot);
	m_pBody = new CBaseNPC_Body(bot);
	m_type[0] = '\0';
	m_hookids.push_back(SH_ADD_HOOK(INextBot, GetIntentionInterface, bot, SH_MEMBER(this, &CBaseNPC_Entity::CBaseNPC::Hook_GetIntentionInterface), false));
	m_hookids.push_back(SH_ADD_HOOK(INextBot, GetLocomotionInterface, bot, SH_MEMBER(this, &CBaseNPC_Entity::CBaseNPC::Hook_GetLocomotionInterface), false));
	m_hookids.push_back(SH_ADD_HOOK(INextBot, GetBodyInterface, bot, SH_MEMBER(this, &CBaseNPC_Entity::CBaseNPC::Hook_GetBodyInterface), false));
	m_hookids.push_back(SH_ADD_MANUALHOOK(CBaseNPC_Dtor, ent, SH_MEMBER((CBaseNPC_Entity*)ent, &CBaseNPC_Entity::Hook_Destructor), false));
}

void CBaseNPC_Entity::CBaseNPC::SendPawnEvent(const char* eventName, cell_t eventData)
{
	INextBot* bot = GetBot();

	for (INextBotEventResponder* sub = bot->FirstContainedResponder(); sub; sub = bot->NextContainedResponder(sub))
	{
		IBaseNPCComponent* responder = dynamic_cast<IBaseNPCComponent*>(sub);
		if (responder)
		{
			responder->OnPawnEvent(eventName, eventData);
		}
	}
}

QueryResultType CBaseNPC_Entity::CBaseNPC::SendPawnQuery(const char* queryName, cell_t data) const
{
	return m_pIntention->OnPawnQuery(GetBot(), queryName, data);
}

CBaseNPC_Entity::CBaseNPC::~CBaseNPC()
{
	m_pMover->Destroy();

	delete m_pIntention;
	delete m_pMover;
	delete m_pBody;

	for (auto it = m_hookids.begin(); it != m_hookids.end(); it++)
	{
		SH_REMOVE_HOOK_ID((*it));
	}
}

IIntention* CBaseNPC_Entity::CBaseNPC::Hook_GetIntentionInterface(void) const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_pIntention);
}

ILocomotion* CBaseNPC_Entity::CBaseNPC::Hook_GetLocomotionInterface(void) const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_pMover);
}

IBody* CBaseNPC_Entity::CBaseNPC::Hook_GetBodyInterface(void) const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_pBody);
}

void CBaseNPC_Entity::BotUpdateOnRemove()
{
	mOriginalUpdateOnRemove(this);
}

#ifdef __linux__
void CBaseNPC_Entity::Hook_Destructor( void )
#else
// MSVC uses helper function in vtable instead of the destructor
void CBaseNPC_Entity::Hook_Destructor( unsigned int flags )
#endif
{
	BotDestroy();
	RETURN_META( MRES_IGNORED );
}

void CBaseNPC_Entity::BotDestroy(void)
{
	CBaseNPC* npc = this->GetNPC();
	npc->~CBaseNPC();

	g_pPluginEntityFactories->NotifyEntityDestruction(this);
}

void CBaseNPC_Entity::BotSpawn(void)
{
	CBaseNPC* npc = this->GetNPC();
	mOriginalSpawn(this);

	SetThink(&CBaseNPC_Entity::BotThink);
	SetNextThink(gpGlobals->curtime);
}

void CBaseNPC_Entity::BotThink(void)
{
	INextBot* bot = MyNextBotPointer();

	UpdateLastKnownArea();
	bot->Update();

	SetNextThink(gpGlobals->curtime + 0.06);
}

int CBaseNPC_Entity::OnTakeDamage(const CTakeDamageInfo& info)
{
	CTakeDamageInfo newInfo = info;
	if (TFGameRules())
	{
		TFGameRules()->ApplyOnDamageModifyRules(newInfo, this, true);
	}
	return mOriginalOnTakeDamage(this, newInfo);
}

int CBaseNPC_Entity::OnTakeDamage_Alive(const CTakeDamageInfo& info)
{
	CTakeDamageInfo newInfo = info;
	if (TFGameRules())
	{
		CTFGameRules::DamageModifyExtras_t outParams;
		newInfo.SetDamage(TFGameRules()->ApplyOnDamageAliveModifyRules(info, this, outParams));
	}
	return mOriginalOnTakeDamage_Alive(this, newInfo);
}

CBaseNPC_Entity::CBaseNPC* CBaseNPC_Entity::GetNPC(void)
{
	return (CBaseNPC_Entity::CBaseNPC*)(((uint8_t*)this) + g_pBaseNPCFactory->GetEntitySize() - sizeof(CBaseNPC_Entity::CBaseNPC));
}

void CBaseNPC_Entity::DebugConColorMsg( NextBotDebugType debugType, const Color &color, const char *fmt, ... )
{ 
	va_list argptr;
	va_start(argptr, fmt);
	MyNextBotPointer()->DebugConColorMsg( debugType, color, fmt, argptr); 
	va_end(argptr);
}

// ============================================
// IBody Hooks
// ============================================

CBaseNPC_Body::CBaseNPC_Body(INextBot* bot) : IBody(bot)
{
	this->m_vecBodyMins = Vector(-10.0, -10.0, 0.0);
	this->m_vecBodyMaxs = Vector(10.0, 10.0, 90.0);
}

void CBaseNPC_Body::Update()
{
	// VPROF_ENTER_SCOPE("CBaseNPC_Body::Update");
	CBaseCombatCharacterHack* entity = GetBot()->GetEntity();
	entity->DispatchAnimEvents(entity);
	entity->StudioFrameAdvance();
	// VPROF_EXIT_SCOPE();
}

bool CBaseNPC_Body::StartActivity(Activity aAct, unsigned int iFlags)
{
	return true;
}

float CBaseNPC_Body::GetHullWidth() const
{
	float flWidth = m_vecBodyMaxs.x;
	if (flWidth < m_vecBodyMaxs.y) flWidth = m_vecBodyMaxs.y;

	return flWidth * 2.0;
}

float CBaseNPC_Body::GetHullHeight() const
{
	return m_vecBodyMaxs.z;
}

float CBaseNPC_Body::GetStandHullHeight() const
{
	return m_vecBodyMaxs.z;
}

float CBaseNPC_Body::GetCrouchHullHeight() const
{
	return m_vecBodyMaxs.z / 2;
}

const Vector& CBaseNPC_Body::GetHullMins() const
{
	return m_vecBodyMins;
}

const Vector& CBaseNPC_Body::GetHullMaxs() const
{
	return m_vecBodyMaxs;
}

unsigned int CBaseNPC_Body::GetSolidMask() const
{
	return (MASK_NPCSOLID | MASK_PLAYERSOLID);
}
