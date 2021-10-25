#include "extension.h"
#include "cbasenpc_internal.h"
#include "cbasenpc_behavior.h"
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

// INextBotEventResponder
SH_DECL_HOOK0_void(INextBotComponent, Update, SH_NOATTRIB, 0);

// INextBot
SH_DECL_HOOK0(INextBot, GetIntentionInterface, const, 0, IIntention*);
SH_DECL_HOOK0(INextBot, GetLocomotionInterface, const, 0, ILocomotion*);
SH_DECL_HOOK0(INextBot, GetBodyInterface, const, 0, IBody*);

// ILocomotion
SH_DECL_HOOK1_void(ILocomotion, FaceTowards, SH_NOATTRIB, 0, Vector const&);
SH_DECL_HOOK0(ILocomotion, IsAbleToJumpAcrossGaps, const, 0, bool);
SH_DECL_HOOK0(ILocomotion, IsAbleToClimb, const, 0, bool);
SH_DECL_HOOK3(ILocomotion, ClimbUpToLedge, SH_NOATTRIB, 0, bool, const Vector&, const Vector&, const CBaseEntity*);
SH_DECL_HOOK0(ILocomotion, GetStepHeight, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetMaxJumpHeight, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetDeathDropHeight, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetWalkSpeed, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetRunSpeed, const, 0, float);
SH_DECL_HOOK0(ILocomotion, GetMaxAcceleration, const, 0, float);
SH_DECL_HOOK1(ILocomotion, ShouldCollideWith, const, 0, bool, const CBaseEntity*);
SH_DECL_HOOK2(ILocomotion, IsEntityTraversable, const, 0, bool, CBaseEntity*, ILocomotion::TraverseWhenType);
SH_DECL_HOOK0(ILocomotion, IsStuck, const, 0, bool);
SH_DECL_HOOK0(ILocomotion, GetStuckDuration, const, 0, float);
SH_DECL_HOOK1_void(ILocomotion, ClearStuckStatus, SH_NOATTRIB, 0, const char*);

SH_DECL_HOOK0(NextBotGroundLocomotion, GetGravity, const, 0, float);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetAcceleration, const, 0, const Vector&);
SH_DECL_HOOK1_void(NextBotGroundLocomotion, SetAcceleration, SH_NOATTRIB, 0, const Vector&);
SH_DECL_HOOK1_void(NextBotGroundLocomotion, SetVelocity, SH_NOATTRIB, 0, const Vector&);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetFrictionForward, const, 0, float);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetFrictionSideways, const, 0, float);
SH_DECL_HOOK0(NextBotGroundLocomotion, GetMaxYawRate, const, 0, float);

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
}

void CBaseNPCFactory::Create_PostConstructor(CBaseEntityHack* ent)
{
	((CBaseNPC_Entity*)ent)->GetNPC()->SetEntity(ent);
}

size_t CBaseNPCFactory::GetEntitySize()
{
	return sizeof(CBaseNPC_Entity::CBaseNPC) + NextBotCombatCharacter::size_of;
}

CBaseNPC_Entity::CBaseNPC::CBaseNPC(NextBotCombatCharacter* ent, CBaseNPCPluginActionFactory* initialActionFactory) : CExtNPC()
{
	INextBot* bot = ent->MyNextBotPointer();
	m_pIntention = new CBaseNPCIntention(bot, initialActionFactory);
	m_pMover = CBaseNPC_Locomotion::New(bot);
	m_pBody = new CBaseNPC_Body(bot);
	m_type[0] = '\0';
	m_hookids.push_back(SH_ADD_HOOK(INextBot, GetIntentionInterface, bot, SH_MEMBER(this, &CBaseNPC_Entity::CBaseNPC::Hook_GetIntentionInterface), false));
	m_hookids.push_back(SH_ADD_HOOK(INextBot, GetLocomotionInterface, bot, SH_MEMBER(this, &CBaseNPC_Entity::CBaseNPC::Hook_GetLocomotionInterface), false));
	m_hookids.push_back(SH_ADD_HOOK(INextBot, GetBodyInterface, bot, SH_MEMBER(this, &CBaseNPC_Entity::CBaseNPC::Hook_GetBodyInterface), false));
	m_hookids.push_back(SH_ADD_MANUALHOOK(CBaseNPC_Dtor, ent, SH_MEMBER((CBaseNPC_Entity*)ent, &CBaseNPC_Entity::Hook_Destructor), false));
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
	// Destroy Behavior system early so Action OnEnd() callbacks can
	// still access entity properties and virtual functions to perform cleanup
	// on the entity.
	GetNPC()->m_pIntention->DestroyBehavior();

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
// ILocomotion Hooks
// ============================================

void CBaseNPC_Locomotion::Init()
{
	m_pHookIds = new std::vector<int>();
	m_flJumpHeight = 0.0;
	m_flStepSize = 18.0;
	m_flGravity = 800.0;
	m_flAcceleration = 4000.0,
	m_flDeathDropHeight = 1000.0;
	m_flWalkSpeed = 400.0;
	m_flRunSpeed = 400.0;
	m_flFrictionForward = 0.0;
	m_flFrictionSideways = 3.0;
	m_flMaxYawRate = 1250.0;

	m_pHookIds->push_back(SH_ADD_HOOK(INextBotComponent, Update, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_Update), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, IsAbleToJumpAcrossGaps, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_IsAbleToJumpAcrossGaps), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, IsAbleToClimb, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_IsAbleToClimb), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, ClimbUpToLedge, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_ClimbUpToLedge), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, GetStepHeight, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetStepHeight), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, GetMaxJumpHeight, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetMaxJumpHeight), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, GetDeathDropHeight, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetDeathDropHeight), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, GetWalkSpeed, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetWalkSpeed), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, GetRunSpeed, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetRunSpeed), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, GetMaxAcceleration, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetMaxAcceleration), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, ShouldCollideWith, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_ShouldCollideWith), false));
	m_pHookIds->push_back(SH_ADD_HOOK(ILocomotion, IsEntityTraversable, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_IsEntityTraversable), false));
	m_pHookIds->push_back(SH_ADD_HOOK(NextBotGroundLocomotion, GetGravity, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetGravity), false));
	m_pHookIds->push_back(SH_ADD_HOOK(NextBotGroundLocomotion, GetFrictionForward, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetFrictionForward), false));
	m_pHookIds->push_back(SH_ADD_HOOK(NextBotGroundLocomotion, GetFrictionSideways, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetFrictionSideways), false));
	m_pHookIds->push_back(SH_ADD_HOOK(NextBotGroundLocomotion, GetMaxYawRate, this, SH_MEMBER(this, &CBaseNPC_Locomotion::Hook_GetMaxYawRate), false));
}

void CBaseNPC_Locomotion::Destroy()
{
	for (auto it = m_pHookIds->begin(); it != m_pHookIds->end(); it++)
	{
		SH_REMOVE_HOOK_ID((*it));
	}

	delete m_pHookIds;
}

CBaseNPC_Locomotion* CBaseNPC_Locomotion::New(INextBot* bot)
{
	CBaseNPC_Locomotion* mover = (CBaseNPC_Locomotion*)::operator new(sizeof(CBaseNPC_Locomotion));
	NextBotGroundLocomotion::NextBotGroundLocomotion_Ctor(mover, bot);
	mover->Init();

	return mover;
}

void CBaseNPC_Locomotion::Hook_Update()
{
	// VPROF_ENTER_SCOPE("CBaseNPC_Locomotion::Update");

	CBaseCombatCharacterHack* entity = GetBot()->GetEntity();
	entity->UpdateLastKnownArea();

	if (IsStuck())
	{
		INextBot* bot = GetBot();
		PathFollower* path = bot->GetCurrentPath();
		if (path && GetStuckDuration() > 1.0f)
		{
			const Path::Segment* seg = path->GetCurrentGoal();
			const Path::Segment* finalGoal = path->LastSegment();
			const Path::Segment* prior = nullptr;
			if (seg)
				prior = path->PriorSegment(seg);

			if (prior && prior != finalGoal)
			{
				bot->SetPosition(prior->pos);
				ClearStuckStatus("Un-Stuck moved to previous segment");
			}
			else if (seg && seg != finalGoal)
			{
				bot->SetPosition(seg->pos);
				ClearStuckStatus("Un-Stuck moved to previous segment");
			}
			else if (prior)
			{
				bot->SetPosition(path->GetPosition(40.0, prior));
				ClearStuckStatus("Un-Stuck");
			}
			else if (seg)
			{
				bot->SetPosition(path->GetPosition(40.0, seg));
				ClearStuckStatus("Un-Stuck");
			}
		}
	}
	// VPROF_EXIT_SCOPE();
	// this->NonVirtualUpdate();
	RETURN_META(MRES_IGNORED);
}

bool CBaseNPC_Locomotion::Hook_IsAbleToJumpAcrossGaps() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, (m_flJumpHeight > 0.0));
}

bool CBaseNPC_Locomotion::Hook_IsAbleToClimb() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, (m_flJumpHeight > 0.0));
}

bool CBaseNPC_Locomotion::Hook_ClimbUpToLedge(const Vector& vecGoal, const Vector& vecForward, const CBaseEntity* pEntity)
{
	// VPROF_BUDGET("CBaseNPC_Locomotion::ClimbUpToLedge", "CBaseNPC" );
	Vector vecMyPos = GetBot()->GetPosition();
	vecMyPos.z += m_flStepSize;

	float flActualHeight = vecGoal.z - vecMyPos.z;
	float height = flActualHeight;
	if (height < 16.0)
	{
		height = 16.0;
	}

	float additionalHeight = 20.0;
	if (height < 32)
	{
		additionalHeight += 8.0;
	}

	height += additionalHeight;

	float speed = sqrt(2.0 * m_flGravity * height);
	float time = speed / m_flGravity;

	time += sqrt((2.0 * additionalHeight) / m_flGravity);

	Vector vecJumpVel = vecGoal - vecMyPos;
	vecJumpVel /= time;
	vecJumpVel.z = speed;

	float flJumpSpeed = vecJumpVel.Length();
	float flMaxSpeed = 650.0;
	if (flJumpSpeed > flMaxSpeed)
	{
		vecJumpVel[0] *= flMaxSpeed / flJumpSpeed;
		vecJumpVel[1] *= flMaxSpeed / flJumpSpeed;
		vecJumpVel[2] *= flMaxSpeed / flJumpSpeed;
	}

	GetBot()->SetPosition(vecMyPos);
	SetVelocity(vecJumpVel);
	RETURN_META_VALUE(MRES_SUPERCEDE, true);
}

float CBaseNPC_Locomotion::Hook_GetStepHeight() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flStepSize);
}

float CBaseNPC_Locomotion::Hook_GetMaxJumpHeight() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flJumpHeight);
}

float CBaseNPC_Locomotion::Hook_GetDeathDropHeight() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flDeathDropHeight);
}

float CBaseNPC_Locomotion::Hook_GetWalkSpeed() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flWalkSpeed);
}

float CBaseNPC_Locomotion::Hook_GetRunSpeed() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flRunSpeed);
}

float CBaseNPC_Locomotion::Hook_GetMaxAcceleration() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flAcceleration);
}

float CBaseNPC_Locomotion::Hook_GetGravity() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flGravity);
}

bool CBaseNPC_Locomotion::Hook_ShouldCollideWith(const CBaseEntity* pEntity) const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, false);
}

bool CBaseNPC_Locomotion::Hook_IsEntityTraversable(CBaseEntity* pEntity, ILocomotion::TraverseWhenType when) const
{
	VPROF_BUDGET("CBaseNPC_Locomotion::IsEntityTraversable", "CBaseNPC" );
	if (((CBaseEntityHack *)pEntity)->MyCombatCharacterPointer())
	{
		RETURN_META_VALUE(MRES_SUPERCEDE, true);
	}
	RETURN_META_VALUE(MRES_SUPERCEDE, false);
}

float CBaseNPC_Locomotion::Hook_GetFrictionForward() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flFrictionForward);
}

float CBaseNPC_Locomotion::Hook_GetFrictionSideways() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flFrictionSideways);
}

float CBaseNPC_Locomotion::Hook_GetMaxYawRate() const
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_flMaxYawRate);
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
