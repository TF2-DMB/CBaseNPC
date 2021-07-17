#ifndef _CBASENPC_BEHAVIOR_H
#define _CBASENPC_BEHAVIOR_H

#include "NextBotIntentionInterface.h"
#include "NextBotBehavior.h"
#include "cbasenpc_internal.h"
#include "idatamapcontainer.h"

#include <utlstack.h>

class CBaseNPCPluginActionFactory;

class CBaseNPCPluginAction : public Action <CBaseNPC_Entity>
{
public:
	
private:
	ActionResult< CBaseNPC_Entity > m_pluginActionResult;

	CUtlStack<EventDesiredResult< CBaseNPC_Entity >> m_eventResultStack;
	EventDesiredResult< CBaseNPC_Entity > m_pluginEventResult;

	void * m_pData;

	CBaseNPCPluginActionFactory * m_pFactory;

public:
    CBaseNPCPluginAction(CBaseNPCPluginActionFactory * pFactory);
	virtual ~CBaseNPCPluginAction();
	virtual const char* GetName() const override final;

	void* GetData() const { return m_pData; }
	datamap_t* GetDataDescMap() const;

	CBaseNPCPluginActionFactory * GetFactory() const { return m_pFactory; };

	void ResetPluginActionResult();
	void PluginContinue();
	void PluginChangeTo( Action< CBaseNPC_Entity > *action, const char *reason );
	void PluginSuspendFor( Action< CBaseNPC_Entity > *action, const char *reason );
	void PluginDone( const char *reason );

	virtual ActionResult< CBaseNPC_Entity > OnStart( CBaseNPC_Entity *me, Action< CBaseNPC_Entity > *prevAction ) override final;
	virtual ActionResult< CBaseNPC_Entity > Update( CBaseNPC_Entity *me, float interval ) override final;
	virtual void OnEnd( CBaseNPC_Entity *me, Action< CBaseNPC_Entity > *nextAction ) override final;
	virtual ActionResult< CBaseNPC_Entity > OnSuspend( CBaseNPC_Entity *me, Action< CBaseNPC_Entity > *interruptingAction ) override final;
	virtual ActionResult< CBaseNPC_Entity > OnResume( CBaseNPC_Entity *me, Action< CBaseNPC_Entity > *interruptingAction ) override final;
	virtual Action< CBaseNPC_Entity > * InitialContainedAction( CBaseNPC_Entity *me ) override final;
	virtual bool IsAbleToBlockMovementOf( const INextBot *botInMotion ) const override final;

	virtual QueryResultType			ShouldPickUp( const INextBot *me, CBaseEntity *item ) const override final;
	virtual QueryResultType			ShouldHurry( const INextBot *me ) const override final;
	virtual QueryResultType			ShouldRetreat( const INextBot *me ) const override final;
	virtual QueryResultType			ShouldAttack( const INextBot *me, const CKnownEntity *them ) const override final;
	virtual QueryResultType			IsHindrance( const INextBot *me, CBaseEntity *blocker ) const override final;

	virtual Vector					SelectTargetPoint( const INextBot *me, const CBaseCombatCharacterHack *subject ) const override final;
	virtual QueryResultType IsPositionAllowed( const INextBot *me, const Vector &pos ) const override final;

	virtual const CKnownEntity *	SelectMoreDangerousThreat( const INextBot *me, 
															   const CBaseCombatCharacterHack *subject,
															   const CKnownEntity *threat1, 
															   const CKnownEntity *threat2 ) const override final;

	void ResetPluginEventResult();
	void PluginTryContinue( EventResultPriorityType priority );
	void PluginTryChangeTo( Action< CBaseNPC_Entity > *action, EventResultPriorityType priority, const char *reason );
	void PluginTrySuspendFor( Action< CBaseNPC_Entity > *action, EventResultPriorityType priority, const char *reason );
	void PluginTryDone( EventResultPriorityType priority, const char *reason );
	void PluginTryToSustain( EventResultPriorityType priority, const char *reason );

	virtual EventDesiredResult< CBaseNPC_Entity > OnLeaveGround( CBaseNPC_Entity *me, CBaseEntity *ground )							override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnLandOnGround( CBaseNPC_Entity *me, CBaseEntity *ground )						override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnContact( CBaseNPC_Entity *me, CBaseEntity *other, CGameTrace *result = NULL )	override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnMoveToSuccess( CBaseNPC_Entity *me, const Path *path )							override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnMoveToFailure( CBaseNPC_Entity *me, const Path *path, MoveToFailureType reason ) override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnStuck( CBaseNPC_Entity *me )													override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnUnStuck( CBaseNPC_Entity *me )													override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnPostureChanged( CBaseNPC_Entity *me )											override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnAnimationActivityComplete( CBaseNPC_Entity *me, int activity )					override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnAnimationActivityInterrupted( CBaseNPC_Entity *me, int activity )				override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnAnimationEvent( CBaseNPC_Entity *me, animevent_t *event )						override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnIgnite( CBaseNPC_Entity *me )													override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnInjured( CBaseNPC_Entity *me, const CTakeDamageInfo &info )						override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnKilled( CBaseNPC_Entity *me, const CTakeDamageInfo &info )						override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnOtherKilled( CBaseNPC_Entity *me, CBaseCombatCharacterHack *victim, const CTakeDamageInfo &info )	override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnSight( CBaseNPC_Entity *me, CBaseEntity *subject )								override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnLostSight( CBaseNPC_Entity *me, CBaseEntity *subject )							override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnSound( CBaseNPC_Entity *me, CBaseEntity *source, const Vector &pos, KeyValues *keys )	override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnSpokeConcept( CBaseNPC_Entity *me, CBaseCombatCharacterHack *who, AIConcept_t concept, AI_Response *response )	override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnWeaponFired( CBaseNPC_Entity *me, CBaseCombatCharacterHack *whoFired, CBaseEntity *weapon )	override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnNavAreaChanged( CBaseNPC_Entity *me, CNavArea *newArea, CNavArea *oldArea )		override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnModelChanged( CBaseNPC_Entity *me )												override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnPickUp( CBaseNPC_Entity *me, CBaseEntity *item, CBaseCombatCharacterHack *giver )	override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnDrop( CBaseNPC_Entity *me, CBaseEntity *item )									override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnActorEmoted( CBaseNPC_Entity *me, CBaseCombatCharacterHack *emoter, int emote )			override final;

	virtual EventDesiredResult< CBaseNPC_Entity > OnCommandAttack( CBaseNPC_Entity *me, CBaseEntity *victim )						override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnCommandApproach( CBaseNPC_Entity *me, const Vector &pos, float range )			override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnCommandApproach( CBaseNPC_Entity *me, CBaseEntity *goal )						override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnCommandRetreat( CBaseNPC_Entity *me, CBaseEntity *threat, float range )			override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnCommandPause( CBaseNPC_Entity *me, float duration )								override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnCommandResume( CBaseNPC_Entity *me )											override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnCommandString( CBaseNPC_Entity *me, const char *command )						override final;

	virtual EventDesiredResult< CBaseNPC_Entity > OnShoved( CBaseNPC_Entity *me, CBaseEntity *pusher )								override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnBlinded( CBaseNPC_Entity *me, CBaseEntity *blinder )							override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnTerritoryContested( CBaseNPC_Entity *me, int territoryID )						override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnTerritoryCaptured( CBaseNPC_Entity *me, int territoryID )						override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnTerritoryLost( CBaseNPC_Entity *me, int territoryID )							override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnWin( CBaseNPC_Entity *me )														override final;
	virtual EventDesiredResult< CBaseNPC_Entity > OnLose( CBaseNPC_Entity *me )														override final;
};

class CBaseNPCIntention : public IIntention
{
public:
	CBaseNPCPluginActionFactory* m_pInitialActionFactory;

	CBaseNPCIntention( INextBot * me, CBaseNPCPluginActionFactory* initialActionFactory=nullptr );
	virtual ~CBaseNPCIntention();
	virtual void Reset() override;
	virtual void Update() override;
	virtual INextBotEventResponder *FirstContainedResponder() const override { return m_pBehavior; };

	void InitBehavior();

private:
	Behavior< CBaseNPC_Entity > * m_pBehavior;
};

class CBaseNPCPluginActionFactory : public IDataMapContainer
{
public:
	enum CallbackType
	{
		OnStart = 0,
		Update,
		OnSuspend,
		OnResume,
		OnEnd,
		InitialContainedAction,
		CreateInitialAction,
		IsAbleToBlockMovementOf
	};

	enum QueryCallbackType
	{
		ShouldPickUp = 0,
		ShouldHurry,
		ShouldRetreat,
		ShouldAttack,
		IsHindrance,
		SelectTargetPoint,
		IsPositionAllowed,
		SelectMoreDangerousThreat
	};

	enum EventResponderCallbackType
	{
		OnLeaveGround = 0,
		OnLandOnGround,
		OnContact,
		OnMoveToSuccess,
		OnMoveToFailure,
		OnStuck,
		OnUnStuck,
		OnPostureChanged,
		OnAnimationActivityComplete,
		OnAnimationActivityInterrupted,
		OnAnimationEvent,
		OnIgnite,
		OnInjured,
		OnKilled,
		OnOtherKilled,
		OnSight,
		OnLostSight,
		OnSound,
		OnSpokeConcept,
		OnWeaponFired,
		OnNavAreaChanged,
		OnModelChanged,
		OnPickUp,
		OnDrop,
		OnActorEmoted,

		OnCommandAttack,
		OnCommandApproach,
		OnCommandApproachEntity,
		OnCommandRetreat,
		OnCommandPause,
		OnCommandResume,
		OnCommandString,

		OnShoved,
		OnBlinded,
		OnTerritoryContested,
		OnTerritoryCaptured,
		OnTerritoryLost,
		OnWin,
		OnLose
	};

private:
	CUtlMap<CallbackType, IPluginFunction*> m_Callbacks;
	CUtlMap<QueryCallbackType, IPluginFunction*> m_QueryCallbacks;
	CUtlMap<EventResponderCallbackType, IPluginFunction*> m_EventCallbacks;

	std::string m_iActionName;

	CUtlVector< Action< CBaseNPC_Entity >* > m_Actions;

public:
	Handle_t m_Handle;

	CBaseNPCPluginActionFactory(const char* actionName);
	virtual ~CBaseNPCPluginActionFactory();

	virtual int GetDataDescOffset() const override final { return 0; }
	virtual const char* GetDataDescMapClass() const override final { return m_iActionName.c_str(); }

	size_t GetActionDataSize() const { return GetDataDescSize(); }

	const char* GetName() const { return m_iActionName.c_str(); }
	void SetName( const char* name ) { m_iActionName = name; }

	IPluginFunction* GetCallback(CallbackType cbType);
	void SetCallback(CallbackType cbType, IPluginFunction* pCallback);
	IPluginFunction* GetQueryCallback(QueryCallbackType cbType);
	void SetQueryCallback(QueryCallbackType cbType, IPluginFunction* pCallback);
	IPluginFunction* GetEventCallback(EventResponderCallbackType cbType);
    void SetEventCallback(EventResponderCallbackType cbType, IPluginFunction* pCallback);

	Action <CBaseNPC_Entity>* Create();
	void OnActionCreated( Action <CBaseNPC_Entity>* pAction );
	void OnActionRemoved( Action <CBaseNPC_Entity>* pAction );
	void OnCreateInitialAction( Action <CBaseNPC_Entity>* pAction );
};

class CBaseNPCActionFactoryHandler : public IHandleTypeDispatch
{
public:
	virtual void OnHandleDestroy(HandleType_t type, void * object) override;
};

extern HandleType_t g_BaseNPCPluginActionFactoryHandle;
extern CBaseNPCActionFactoryHandler g_BaseNPCPluginActionFactoryHandler;

#endif