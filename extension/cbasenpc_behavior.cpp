
#include "cbasenpc_behavior.h"

#define CBPUSHCELL(cell) pCallback->PushCell((cell_t)(cell));
#define CBPUSHFLOAT(fl) pCallback->PushCell(sp_ftoc(fl));
#define CBPUSHENTITY(ent) CBPUSHCELL(gamehelpers->EntityToBCompatRef((CBaseEntity*)(ent)))
#define CBPUSHSTRING(str) pCallback->PushString(str);
#define CBPUSHVECTOR(vec) \
	{ \
		Vector vecBuffer; vecBuffer = vec; cell_t vecCells[3]; \
		vecCells[0] = sp_ftoc(vecBuffer[0]); vecCells[1] = sp_ftoc(vecBuffer[1]); vecCells[2] = sp_ftoc(vecBuffer[2]);\
		pCallback->PushArray(vecCells, 3); \
	}

#define BEGINACTIONCALLBACKEX(funcName, typeName, ...) \
ActionResult< CBaseNPC_Entity > CBaseNPCPluginAction:: funcName (CBaseNPC_Entity* me, ##__VA_ARGS__) { \
	ResetPluginActionResult(); \
	IPluginFunction* pCallback = m_pFactory->GetCallback( CBaseNPCPluginActionFactory::CallbackType::typeName ); \
	if (pCallback && pCallback->IsRunnable()) { \
		pCallback->PushCell((cell_t)this); pCallback->PushCell(gamehelpers->EntityToBCompatRef(me));

#define BEGINACTIONCALLBACK(funcName, ...) BEGINACTIONCALLBACKEX(funcName, funcName, ##__VA_ARGS__)

#define ENDACTIONCALLBACK() \
		pCallback->Execute(nullptr); \
	} \
	return m_pluginActionResult; \
}

#define BEGINQUERYCALLBACK(funcName, ...) \
QueryResultType CBaseNPCPluginAction:: funcName ( const INextBot *me, ##__VA_ARGS__) const {	\
	cell_t result = ANSWER_UNDEFINED; \
	IPluginFunction* pCallback = m_pFactory->GetQueryCallback( CBaseNPCPluginActionFactory::QueryCallbackType::funcName ); \
	if (pCallback && pCallback->IsRunnable()) { \
		CBPUSHCELL(this); CBPUSHCELL(me);

#define ENDQUERYCALLBACK() \
		pCallback->Execute(&result); \
	}	\
	return (QueryResultType)result; \
}

#define BEGINEVENTCALLBACKEX(funcName, typeName, ...) \
EventDesiredResult< CBaseNPC_Entity > CBaseNPCPluginAction:: funcName (CBaseNPC_Entity* me, ##__VA_ARGS__) {	\
	m_eventResultStack.push( m_pluginEventResult ); \
	ResetPluginEventResult(); \
	IPluginFunction* pCallback = m_pFactory->GetEventCallback( CBaseNPCPluginActionFactory::EventResponderCallbackType::typeName ); \
	if (pCallback && pCallback->IsRunnable()) { \
		pCallback->PushCell((cell_t)this); \
		pCallback->PushCell(gamehelpers->EntityToBCompatRef(me));

#define BEGINEVENTCALLBACK(funcName, ...) BEGINEVENTCALLBACKEX(funcName, funcName, ##__VA_ARGS__)

#define EVENTPUSHCELL(cell) CBPUSHCELL(cell)
#define EVENTPUSHFLOAT(fl) CBPUSHFLOAT(fl)
#define EVENTPUSHENTITY(ent) CBPUSHENTITY(ent)
#define EVENTPUSHSTRING(str) CBPUSHSTRING(str)
#define EVENTPUSHVECTOR(vec) CBPUSHVECTOR(vec)

#define ENDEVENTCALLBACK() \
		pCallback->Execute(nullptr); \
	}	\
	EventDesiredResult< CBaseNPC_Entity > result = m_pluginEventResult; \
	m_pluginEventResult = m_eventResultStack.front(); \
	m_eventResultStack.pop(); \
	return result; \
}

#define ENDEVENTCALLBACK_NOEXECUTE() \
	}	\
	EventDesiredResult< CBaseNPC_Entity > result = m_pluginEventResult; \
	m_pluginEventResult = m_eventResultStack.front(); \
	m_eventResultStack.pop(); \
	return result; \
}

// https://github.com/alliedmodders/sourcemod/blob/6928d21bcf746920b0f2f54e2c28b34097a66be2/core/smn_keyvalues.h#L42
struct KeyValueStack
{
	KeyValues *pBase;
	SourceHook::CStack<KeyValues *> pCurRoot;
	bool m_bDeleteOnDestroy = true;
};

extern IdentityToken_t * g_pCoreIdent;
extern HandleType_t g_KeyValueType;

CBaseNPCPluginAction::CBaseNPCPluginAction(CBaseNPCPluginActionFactory* pFactory) : 
	Action< CBaseNPC_Entity >(),
	m_pFactory(pFactory)
{
	size_t dataSize = pFactory->GetActionDataSize();
	m_pData = (dataSize > 0) ? calloc(1, dataSize) : nullptr;

	ResetPluginActionResult();
	ResetPluginEventResult();

	pFactory->OnActionCreated(this);
}

CBaseNPCPluginAction::~CBaseNPCPluginAction()
{
	if (m_pData)
		free(m_pData);
	
	m_pFactory->OnActionRemoved(this);
}

datamap_t* CBaseNPCPluginAction::GetDataDescMap() const
{
	return m_pFactory->GetDataDescMap();
}

const char* CBaseNPCPluginAction::GetName() const 
{
	return m_pFactory->GetName();
}

void CBaseNPCPluginAction::ResetPluginActionResult()
{
	m_pluginActionResult.m_action = nullptr;
	m_pluginActionResult.m_reason = nullptr;
	m_pluginActionResult.m_type = CONTINUE;
}

void CBaseNPCPluginAction::PluginContinue()
{
	m_pluginActionResult = Continue();
}

void CBaseNPCPluginAction::PluginChangeTo( Action< CBaseNPC_Entity > *action, const char *reason )
{
	m_pluginActionResult = ChangeTo(action, reason);
}

void CBaseNPCPluginAction::PluginSuspendFor( Action< CBaseNPC_Entity > *action, const char *reason )
{
	m_pluginActionResult = SuspendFor(action, reason);
}

void CBaseNPCPluginAction::PluginDone( const char *reason )
{
	m_pluginActionResult = Done(reason);
}

void CBaseNPCPluginAction::ResetPluginEventResult()
{
	m_pluginEventResult.m_priority = RESULT_NONE;
	m_pluginEventResult.m_action = nullptr;
	m_pluginEventResult.m_reason = nullptr;
	m_pluginEventResult.m_type = CONTINUE;
}

void CBaseNPCPluginAction::PluginTryContinue( EventResultPriorityType priority ) 
{ 
	m_pluginEventResult = TryContinue(priority); 
}

void CBaseNPCPluginAction::PluginTryChangeTo( Action< CBaseNPC_Entity > *action, EventResultPriorityType priority, const char *reason ) 
{ 
	m_pluginEventResult = TryChangeTo(action, priority, reason); 
}

void CBaseNPCPluginAction::PluginTrySuspendFor( Action< CBaseNPC_Entity > *action, EventResultPriorityType priority, const char *reason ) 
{ 
	m_pluginEventResult = TrySuspendFor(action, priority, reason); 
}

void CBaseNPCPluginAction::PluginTryDone( EventResultPriorityType priority, const char *reason ) 
{ 
	m_pluginEventResult = TryDone(priority, reason); 
}

void CBaseNPCPluginAction::PluginTryToSustain( EventResultPriorityType priority, const char *reason ) 
{ 
	m_pluginEventResult = TryToSustain(priority, reason); 
}

// Actions

BEGINACTIONCALLBACK(OnStart, Action< CBaseNPC_Entity > *prevAction)
	CBPUSHCELL(prevAction)
ENDACTIONCALLBACK()

BEGINACTIONCALLBACK(Update, float interval)
	CBPUSHFLOAT(interval)
ENDACTIONCALLBACK()

BEGINACTIONCALLBACK(OnSuspend, Action< CBaseNPC_Entity > *interruptingAction)
	CBPUSHCELL(interruptingAction)
ENDACTIONCALLBACK()

BEGINACTIONCALLBACK(OnResume, Action< CBaseNPC_Entity > *interruptingAction)
	CBPUSHCELL(interruptingAction)
ENDACTIONCALLBACK()

void CBaseNPCPluginAction::OnEnd( CBaseNPC_Entity * me, Action< CBaseNPC_Entity > *nextAction )
{
	IPluginFunction* pCallback = m_pFactory->GetCallback( CBaseNPCPluginActionFactory::CallbackType::OnEnd );
	if (pCallback && pCallback->IsRunnable()) 
	{
		CBPUSHCELL(this)
		CBPUSHENTITY(me)
		CBPUSHCELL(nextAction)

		pCallback->Execute(nullptr);
	}
}

Action< CBaseNPC_Entity >* CBaseNPCPluginAction::InitialContainedAction( CBaseNPC_Entity * me )
{
	cell_t result = 0;

	IPluginFunction* pCallback = m_pFactory->GetCallback( CBaseNPCPluginActionFactory::CallbackType::InitialContainedAction );
	if (pCallback && pCallback->IsRunnable()) 
	{
		CBPUSHCELL(this)
		CBPUSHENTITY(me)

		pCallback->Execute(&result);
	}

	return (Action< CBaseNPC_Entity >*)result;
}

bool CBaseNPCPluginAction::IsAbleToBlockMovementOf( const INextBot *botInMotion ) const
{
	cell_t result = Action< CBaseNPC_Entity >::IsAbleToBlockMovementOf( botInMotion );

	IPluginFunction* pCallback = m_pFactory->GetCallback( CBaseNPCPluginActionFactory::CallbackType::IsAbleToBlockMovementOf );
	if (pCallback && pCallback->IsRunnable()) 
	{
		CBPUSHCELL(this)
		CBPUSHCELL(botInMotion)

		pCallback->Execute(&result);
	}

	return !!result;
}

// Queries

BEGINQUERYCALLBACK(ShouldPickUp, CBaseEntity *item ) 
	CBPUSHENTITY(item);
ENDQUERYCALLBACK()

BEGINQUERYCALLBACK(ShouldHurry)
ENDQUERYCALLBACK()

BEGINQUERYCALLBACK(ShouldRetreat)
ENDQUERYCALLBACK()

BEGINQUERYCALLBACK(ShouldAttack, const CKnownEntity *them)
	CBPUSHCELL(them)
ENDQUERYCALLBACK()

BEGINQUERYCALLBACK(IsHindrance, CBaseEntity* blocker)
	CBPUSHENTITY(blocker)
ENDQUERYCALLBACK()

Vector CBaseNPCPluginAction::SelectTargetPoint( const INextBot *me, const CBaseCombatCharacterHack *subject ) const
{
	Vector result = vec3_origin;

	IPluginFunction* pCallback = m_pFactory->GetQueryCallback( CBaseNPCPluginActionFactory::QueryCallbackType::SelectTargetPoint );
	if (pCallback && pCallback->IsRunnable()) 
	{ 
		cell_t buffer[3]; 
		buffer[0] = sp_ftoc(result[0]);
		buffer[1] = sp_ftoc(result[1]);
		buffer[2] = sp_ftoc(result[2]);

		CBPUSHCELL(this)
		CBPUSHCELL(me)
		CBPUSHENTITY(subject)
		pCallback->PushArray(buffer, 3, SM_PARAM_COPYBACK);
		pCallback->Execute(nullptr);

		result[0] = sp_ctof(buffer[0]);
		result[1] = sp_ctof(buffer[1]);
		result[2] = sp_ctof(buffer[2]);
	}

	return result;
}

BEGINQUERYCALLBACK(IsPositionAllowed, const Vector &pos)
	CBPUSHVECTOR(pos)
ENDQUERYCALLBACK()

const CKnownEntity * CBaseNPCPluginAction::SelectMoreDangerousThreat( const INextBot *me, 
	const CBaseCombatCharacterHack *subject,
	const CKnownEntity *threat1, 
	const CKnownEntity *threat2 ) const
{
	cell_t result = 0;

	IPluginFunction* pCallback = m_pFactory->GetQueryCallback( CBaseNPCPluginActionFactory::QueryCallbackType::SelectMoreDangerousThreat );
	if (pCallback && pCallback->IsRunnable()) 
	{
		CBPUSHCELL(this)
		CBPUSHCELL(me)
		CBPUSHENTITY(subject)
		CBPUSHCELL(threat1)
		CBPUSHCELL(threat2)
		pCallback->Execute(&result);
	}

	return (const CKnownEntity*)result;
}

// Events

BEGINEVENTCALLBACK(OnLeaveGround, CBaseEntity* ground)
	EVENTPUSHENTITY(ground)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnLandOnGround, CBaseEntity* ground)
	EVENTPUSHENTITY(ground)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnContact, CBaseEntity* other, CGameTrace* traceResult)
	EVENTPUSHENTITY(other)
	EVENTPUSHCELL(traceResult)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnMoveToSuccess, const Path *path)
	EVENTPUSHCELL(path)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnMoveToFailure, const Path *path, MoveToFailureType reason)
	EVENTPUSHCELL(path)
	EVENTPUSHCELL(reason)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnStuck)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnUnStuck)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnPostureChanged)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnAnimationActivityComplete, int activity)
	EVENTPUSHCELL(activity)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnAnimationActivityInterrupted, int activity)
	EVENTPUSHCELL(activity)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnAnimationEvent, animevent_t *event)
	EVENTPUSHCELL(event->event)
	EVENTPUSHSTRING(event->options)
	EVENTPUSHFLOAT(event->cycle)
	EVENTPUSHFLOAT(event->eventtime)
	EVENTPUSHCELL(event->type)
	EVENTPUSHENTITY(event->pSource)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnIgnite)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnInjured, const CTakeDamageInfo &info)
	EVENTPUSHENTITY(info.GetAttacker())
	EVENTPUSHENTITY(info.GetInflictor())
	EVENTPUSHFLOAT(info.GetDamage())
	EVENTPUSHCELL(info.GetDamageType())
	EVENTPUSHENTITY(info.GetWeapon())
	EVENTPUSHVECTOR(info.GetDamageForce())
	EVENTPUSHVECTOR(info.GetDamagePosition())
	EVENTPUSHCELL(info.GetDamageCustom())
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnKilled, const CTakeDamageInfo &info)
	EVENTPUSHENTITY(info.GetAttacker())
	EVENTPUSHENTITY(info.GetInflictor())
	EVENTPUSHFLOAT(info.GetDamage())
	EVENTPUSHCELL(info.GetDamageType())
	EVENTPUSHENTITY(info.GetWeapon())
	EVENTPUSHVECTOR(info.GetDamageForce())
	EVENTPUSHVECTOR(info.GetDamagePosition())
	EVENTPUSHCELL(info.GetDamageCustom())
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnOtherKilled, CBaseCombatCharacterHack *victim, const CTakeDamageInfo &info)
	EVENTPUSHENTITY(info.GetAttacker())
	EVENTPUSHENTITY(info.GetInflictor())
	EVENTPUSHFLOAT(info.GetDamage())
	EVENTPUSHCELL(info.GetDamageType())
	EVENTPUSHENTITY(info.GetWeapon())
	EVENTPUSHVECTOR(info.GetDamageForce())
	EVENTPUSHVECTOR(info.GetDamagePosition())
	EVENTPUSHCELL(info.GetDamageCustom())
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnSight, CBaseEntity *subject)
	EVENTPUSHENTITY(subject)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnLostSight, CBaseEntity *subject)
	EVENTPUSHENTITY(subject)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnSound, CBaseEntity *source, const Vector &pos, KeyValues *keys)
	EVENTPUSHENTITY(source)
	EVENTPUSHVECTOR(pos)

	KeyValueStack *pStk = new KeyValueStack;
	pStk->pBase = keys;
	pStk->pCurRoot.push(pStk->pBase);
	pStk->m_bDeleteOnDestroy = false;

	Handle_t hndl = handlesys->CreateHandle(g_KeyValueType, pStk, g_pCoreIdent, g_pCoreIdent, nullptr);
	EVENTPUSHCELL(hndl)

	pCallback->Execute(nullptr);

	HandleSecurity sec(g_pCoreIdent, g_pCoreIdent);

	// Deletes pStk
	handlesys->FreeHandle(hndl, &sec);

ENDEVENTCALLBACK_NOEXECUTE()

BEGINEVENTCALLBACK(OnSpokeConcept, CBaseCombatCharacterHack* who, AIConcept_t concept, AI_Response *response)
	EVENTPUSHENTITY(who)
	EVENTPUSHCELL(concept)
	EVENTPUSHCELL(response)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnWeaponFired, CBaseCombatCharacterHack* whoFired, CBaseEntity* weapon )
	EVENTPUSHENTITY(whoFired)
	EVENTPUSHENTITY(weapon)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnNavAreaChanged, CNavArea *newArea, CNavArea *oldArea)
	EVENTPUSHCELL(newArea)
	EVENTPUSHCELL(oldArea)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnModelChanged)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnPickUp, CBaseEntity* item, CBaseCombatCharacterHack* giver)
	EVENTPUSHENTITY(item)
	EVENTPUSHENTITY(giver)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnDrop, CBaseEntity* item)
	EVENTPUSHENTITY(item)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnActorEmoted, CBaseCombatCharacterHack* emoter, int emote)
	EVENTPUSHENTITY(emoter)
	EVENTPUSHCELL(emote)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnCommandAttack, CBaseEntity *victim)
	EVENTPUSHENTITY(victim)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnCommandApproach, const Vector &pos, float range )
	EVENTPUSHVECTOR(pos)
	EVENTPUSHFLOAT(range)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACKEX(OnCommandApproach, OnCommandApproachEntity, CBaseEntity* goal)
	EVENTPUSHENTITY(goal)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnCommandRetreat, CBaseEntity *threat, float range)
	EVENTPUSHENTITY(threat)
	EVENTPUSHFLOAT(range)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnCommandPause, float duration)
	EVENTPUSHFLOAT(duration)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnCommandResume)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnCommandString, const char *command)
	EVENTPUSHSTRING(command)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnShoved, CBaseEntity *pusher)
	EVENTPUSHENTITY(pusher)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnBlinded, CBaseEntity *blinder)
	EVENTPUSHENTITY(blinder)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnTerritoryContested, int territoryID)
	EVENTPUSHCELL(territoryID)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnTerritoryCaptured, int territoryID)
	EVENTPUSHCELL(territoryID)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnTerritoryLost, int territoryID)
	EVENTPUSHCELL(territoryID)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnWin)
ENDEVENTCALLBACK()

BEGINEVENTCALLBACK(OnLose)
ENDEVENTCALLBACK()

CBaseNPCIntention::CBaseNPCIntention( INextBot * bot, CBaseNPCPluginActionFactory* initialActionFactory ) 
	: IIntention( bot ), m_pInitialActionFactory(initialActionFactory)
{
	CBaseNPC_Entity* me = (CBaseNPC_Entity*)bot->GetEntity();

	m_pBehavior = nullptr;

	InitBehavior();
}

CBaseNPCIntention::~CBaseNPCIntention()
{
	DestroyBehavior();
}

void CBaseNPCIntention::Reset()
{ 
	DestroyBehavior();
	InitBehavior();
}

void CBaseNPCIntention::InitBehavior()
{
	if (m_pInitialActionFactory)
	{
		Action< CBaseNPC_Entity > * pAction = m_pInitialActionFactory->Create();
		m_pInitialActionFactory->OnCreateInitialAction( pAction );
		m_pBehavior = new Behavior< CBaseNPC_Entity >( pAction );
	}
	else 
	{
		m_pBehavior = nullptr;
	}
}

void CBaseNPCIntention::DestroyBehavior()
{
	if ( !m_pBehavior )
		return;

	delete m_pBehavior;
	m_pBehavior = nullptr;
}

void CBaseNPCIntention::Update()
{
	CBaseNPC_Entity* me = reinterpret_cast<CBaseNPC_Entity*>(GetBot()->GetEntity());

	if (m_pBehavior)
	{
		m_pBehavior->Update( me, GetUpdateInterval() );
	}
}

CBaseNPCPluginActionFactory::CBaseNPCPluginActionFactory(const char* actionName) : 
	IDataMapContainer(),
	m_iActionName(actionName)
{
	SetDefLessFunc(m_Callbacks);
	SetDefLessFunc(m_QueryCallbacks);
	SetDefLessFunc(m_EventCallbacks);
}

CBaseNPCPluginActionFactory::~CBaseNPCPluginActionFactory()
{
	if (m_Actions.Count())
	{
		// Remove entities that use Actions created from this factory to prevent
		// undefined behavior.

		CUtlVector< CBaseEntity* > entities;
		for (int i = 0; i < m_Actions.Count(); i++)
		{
			Action< CBaseNPC_Entity > *pAction = m_Actions[i];
			if (!pAction) continue;

			CBaseNPC_Entity* pActor = pAction->GetActor();
			if (pActor && !entities.IsValidIndex(entities.Find(pActor)))
			{
				entities.AddToTail(pActor);
			}
		}

		for (int i = 0; i < entities.Count(); i++)
		{
			servertools->RemoveEntityImmediate(entities[i]);
		}
	}
}

IPluginFunction* CBaseNPCPluginActionFactory::GetCallback(CallbackType cbType)
{
	unsigned short index = m_Callbacks.Find(cbType);
	if (!m_Callbacks.IsValidIndex(index))
		return nullptr;
	
	return m_Callbacks[index];
}

void CBaseNPCPluginActionFactory::SetCallback(CallbackType cbType, IPluginFunction* pCallback)
{
	m_Callbacks.Insert(cbType, pCallback);
}

IPluginFunction* CBaseNPCPluginActionFactory::GetQueryCallback(QueryCallbackType cbType)
{
	unsigned short index = m_QueryCallbacks.Find(cbType);
	if (!m_QueryCallbacks.IsValidIndex(index))
		return nullptr;
	
	return m_QueryCallbacks[index];
}

void CBaseNPCPluginActionFactory::SetQueryCallback(QueryCallbackType cbType, IPluginFunction* pCallback)
{
	m_QueryCallbacks.Insert(cbType, pCallback);
}

IPluginFunction* CBaseNPCPluginActionFactory::GetEventCallback(EventResponderCallbackType eventType)
{
	unsigned short index = m_EventCallbacks.Find(eventType);
	if (!m_EventCallbacks.IsValidIndex(index))
		return nullptr;
	
	return m_EventCallbacks[index];
}

void CBaseNPCPluginActionFactory::SetEventCallback(EventResponderCallbackType eventType, IPluginFunction* pCallback)
{
	m_EventCallbacks.Insert(eventType, pCallback);
}

Action <CBaseNPC_Entity>* CBaseNPCPluginActionFactory::Create()
{
	if (HasDataDesc() && !GetDataDescMap())
		CreateDataDescMap(nullptr);

	CBaseNPCPluginAction * action = new CBaseNPCPluginAction( this );
	return action;
}

void CBaseNPCPluginActionFactory::OnActionCreated(Action <CBaseNPC_Entity>* pAction)
{
	m_Actions.AddToTail(pAction);
}

void CBaseNPCPluginActionFactory::OnActionRemoved(Action <CBaseNPC_Entity>* pAction)
{
	m_Actions.FindAndRemove(pAction);
}

void CBaseNPCPluginActionFactory::OnCreateInitialAction(Action <CBaseNPC_Entity>* pAction)
{
	IPluginFunction * pCallback = GetCallback( CreateInitialAction );
	if (pCallback && pCallback->IsRunnable())
	{
		pCallback->PushCell((cell_t)pAction);
		pCallback->Execute(nullptr);
	}
}

void CBaseNPCActionFactoryHandler::OnHandleDestroy(HandleType_t type, void * object)
{
	CBaseNPCPluginActionFactory* factory = (CBaseNPCPluginActionFactory*)object;
	factory->DestroyDataDesc();
	delete factory;
}

HandleType_t g_BaseNPCPluginActionFactoryHandle;
CBaseNPCActionFactoryHandler g_BaseNPCPluginActionFactoryHandler;
