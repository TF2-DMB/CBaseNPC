#include "NextBotInterface.h"
#include "NextBotComponentInterface.h"
#include "NextBotManager.h"
#include "sourcesdk/baseentity.h"
#include "sourcesdk/basecombatcharacter.h"

#include <tier0/vprof.h>

ConVar* NextBotDebugHistory = nullptr;

INextBot::INextBot(void) : m_LocoInterface(nullptr),
	m_BodyInterface(nullptr),
	m_IntentionInterface(nullptr),
	m_VisionInterface(nullptr),
	m_debugHistory(MAX_NEXTBOT_DEBUG_HISTORY, 0)
{
	m_iLastUpdateTick = -999;
	m_iManagerIndex = -1;
	m_componentList = nullptr;
	m_iDebugTextOffset = 0;

	m_itImmobileEpoch.Invalidate();
	m_ctImmobileCheck.Invalidate();
	m_vecLastPosition = vec3_origin;

	m_CurrentPath = nullptr;

	m_iManagerIndex = TheNextBots().Register(this);
}


//----------------------------------------------------------------------------------------------------------------
INextBot::~INextBot()
{
	ResetDebugHistory();

	TheNextBots().UnRegister(this);

	if (m_IntentionInterface)
		delete m_IntentionInterface;

	/*if (m_LocoInterface)
		delete m_LocoInterface;*/

	if (m_BodyInterface)
		delete m_BodyInterface;

	/*if (m_VisionInterface)
		delete m_VisionInterface;*/
}

bool INextBot::BeginUpdate()
{
	if (TheNextBots().ShouldUpdate(this))
	{
		TheNextBots().NotifyBeginUpdate(this);
		return true;
	}
	return false;
}

void INextBot::EndUpdate(void)
{
	TheNextBots().NotifyEndUpdate(this);
}

void INextBot::Reset(void)
{
	m_iLastUpdateTick = -999;
	m_Dword18 = 0;
	m_iDebugTextOffset = 0;

	m_itImmobileEpoch.Invalidate();
	m_ctImmobileCheck.Invalidate();
	m_vecLastPosition = vec3_origin;

	for (INextBotComponent* comp = m_componentList; comp; comp = comp->m_nextComponent)
	{
		comp->Reset();
	}
}

void INextBot::Update(void)
{
	VPROF_BUDGET("INextBot::Update", "NextBot");

	m_iDebugTextOffset = 0;

	UpdateImmobileStatus();

	for(INextBotComponent* comp = m_componentList; comp; comp = comp->m_nextComponent)
	{
		if ( comp->ComputeUpdateInterval() )
		{
			comp->Update();
		}
	}
}


void INextBot::Upkeep( void )
{
	VPROF_BUDGET("INextBot::Upkeep", "NextBot");

	for(INextBotComponent* comp = m_componentList; comp; comp = comp->m_nextComponent)
	{
		comp->Upkeep();
	}
}

bool IgnoreActorsTraceFilterFunction(IHandleEntity *pServerEntity, int contentsMask)
{
	CBaseEntity* entity = EntityFromEntityHandle(pServerEntity);
	return (entity->MyCombatCharacterPointer() == NULL);
}

void INextBot::RegisterComponent(INextBotComponent *comp)
{
	comp->m_nextComponent = m_componentList;
	m_componentList = comp;
}

void INextBot::UnregisterComponent(INextBotComponent *_comp)
{
	// Iterate the component list because we need the pointer to the previous
	// component so as to not break the list.

	for (INextBotComponent* comp = m_componentList, *prevComp = nullptr; comp; prevComp = comp, comp = comp->m_nextComponent)
	{
		if (comp == _comp)
		{
			INextBotComponent* nextComp = comp->m_nextComponent;

			if (prevComp)
			{
				prevComp->m_nextComponent = nextComp;
			}
			else
			{
				m_componentList = nextComp;
			}

			break;
		}
	}
}

const char *INextBot::GetDebugIdentifier( void ) const
{
	const int nameSize = 256;
	static char name[ nameSize ];
	
	CBaseCombatCharacter* pEnt = GetEntity(); 

	Q_snprintf( name, nameSize, "%s(#%d)", pEnt->GetClassname(), pEnt->entindex() );

	return name;
}

void INextBot::ResetDebugHistory( void )
{
}

void INextBot::DebugConColorMsg( NextBotDebugType debugType, const Color &color, const char *fmt, va_list arglist )
{
	if ( IsDebugging( debugType ) )
	{
		char data[ 512 ];

		Q_vsnprintf(data, sizeof( data ), fmt, arglist);

		ConColorMsg( color, "%s", data );
	}
}

bool INextBot::SetPosition(const Vector &pos)
{
	IBody *body = GetBodyInterface();
	if (body)
	{
		return body->SetPosition( pos );
	}
	
	GetEntity()->SetAbsOrigin( pos );
	return true;
}

const Vector &INextBot::GetPosition(void) const
{
	return const_cast<INextBot*>( this )->GetEntity()->GetAbsOrigin();
}

bool INextBot::IsEnemy(const CBaseEntity* them) const
{
	if (them == nullptr) 
		return false;
	return const_cast<INextBot *>(this)->GetEntity()->GetTeamNumber() != them->GetTeamNumber();
}

bool INextBot::IsFriend(const CBaseEntity* them) const
{
	if (them == nullptr)
		return false;
		
	return const_cast<INextBot*>(this)->GetEntity()->GetTeamNumber() == them->GetTeamNumber();
}

bool INextBot::IsSelf(const CBaseEntity* them) const
{
	if (them == nullptr)
		return false;
	return const_cast<INextBot *>( this )->GetEntity()->entindex() == them->entindex();
}

bool INextBot::IsAbleToClimbOnto(const CBaseEntity* object) const
{
	// TO-DO: HL2 FIX ?
	if (object == nullptr /*|| !const_cast<CBaseEntity*>(object)->IsAIWalkable()*/)
	{
		return false;
	}

	if (FClassnameIs(const_cast<CBaseEntity*>(object), "prop_door*") || FClassnameIs(const_cast<CBaseEntity*>(object), "func_door*"))
	{
		return false;
	}

	return true;
}

bool INextBot::IsAbleToBreak(const CBaseEntity* object) const
{
	if (object && object->GetTakeDamage() == DAMAGE_YES)
	{
		if (FClassnameIs(const_cast<CBaseEntity*>( object ), "func_breakable" ) && 
			 object->GetHealth())
		{
			return true;
		}

		if (FClassnameIs(const_cast<CBaseEntity*>( object ), "func_breakable_surf"))
		{
			return true;
		}

		// TO-DO: Fix
		/*if (dynamic_cast<const CBreakableProp*>(object) != nullptr)
		{
			return true;
		}*/
	}

	return false;
}

bool INextBot::IsRangeLessThan(CBaseEntity* subject, float range) const
{
	Vector botPos;
	CBaseEntity* bot = const_cast<INextBot *>(this)->GetEntity();
	if (!bot || !subject)
		return 0.0f;
	bot->CollisionProp()->CalcNearestPoint(subject->WorldSpaceCenter(), &botPos);
	float computedRange = subject->CollisionProp()->CalcDistanceFromPoint(botPos);
	return computedRange < range;
}

bool INextBot::IsRangeLessThan(const Vector &pos, float range) const
{
	Vector to = pos - GetPosition();
	return to.IsLengthLessThan(range);
}

bool INextBot::IsRangeGreaterThan(CBaseEntity* subject, float range) const
{
	Vector botPos;
	CBaseEntity* bot = const_cast<INextBot *>(this)->GetEntity();
	if (!bot || !subject)
		return true;

	bot->CollisionProp()->CalcNearestPoint( subject->WorldSpaceCenter(), &botPos);
	float computedRange = subject->CollisionProp()->CalcDistanceFromPoint(botPos);
	return computedRange > range;
}

bool INextBot::IsRangeGreaterThan(const Vector &pos, float range) const
{
	Vector to = pos - GetPosition();
	return to.IsLengthGreaterThan(range);
}

float INextBot::GetRangeTo(CBaseEntity* subject) const
{
	Vector botPos;
	CBaseEntity* bot = const_cast<INextBot *>(this)->GetEntity();
	if (!bot || !subject)
		return 0.0f;

	bot->CollisionProp()->CalcNearestPoint(subject->WorldSpaceCenter(), &botPos);
	float computedRange = subject->CollisionProp()->CalcDistanceFromPoint(botPos);
	return computedRange;
}

float INextBot::GetRangeTo(const Vector &pos) const
{
	Vector to = pos - GetPosition();
	return to.Length();
}

float INextBot::GetRangeSquaredTo(CBaseEntity* subject) const
{
	Vector botPos;
	CBaseEntity* bot = const_cast<INextBot*>(this)->GetEntity();
	if (!bot || !subject)
		return 0.0f;

	bot->CollisionProp()->CalcNearestPoint(subject->WorldSpaceCenter(), &botPos);
	float computedRange = subject->CollisionProp()->CalcDistanceFromPoint(botPos);
	return computedRange * computedRange;
}

float INextBot::GetRangeSquaredTo(const Vector &pos) const
{
	Vector to = pos - GetPosition();
	return to.LengthSqr();	
}

bool INextBot::IsDebugging(unsigned int type) const
{
	// TO-DO: Fix
	/*if (TheNextBots().IsDebugging(type))
	{
		return TheNextBots().IsDebugFilterMatch(this);
	}*/
	return false;
}

bool INextBot::IsDebugFilterMatch(const char *name) const
{
	if (!Q_strnicmp(name, GetDebugIdentifier(), Q_strlen(name)))
	{
		return true;
	}

	// TO-DO: fix
	/*CTeam *team = GetEntity()->GetTeam();
	if (team && !Q_strnicmp(name, team->GetName(), Q_strlen(name)))
	{
		return true;
	}
	*/
	return false;
}

void INextBot::DisplayDebugText(const char *text) const
{
	//const_cast< INextBot * >( this )->GetEntity()->EntityText( m_debugDisplayLine++, text, 0.1 );
}