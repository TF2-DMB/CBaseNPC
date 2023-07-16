#ifndef _CBASENPC_BEHAVIOR_H
#define _CBASENPC_BEHAVIOR_H

#include "NextBotIntentionInterface.h"
#include "NextBotBehavior.h"
#include "cbasenpc_internal.h"
#include "idatamapcontainer.h"

#include <sh_stack.h>

class CBaseNPCPluginActionFactory;
class CBaseNPCPluginActionFactories;

// To be used with READHANDLE macro
extern HandleType_t g_BaseNPCPluginActionFactoryHandle;

extern CBaseNPCPluginActionFactories* g_pBaseNPCPluginActionFactories;

class CBaseNPCPluginAction : public Action <INextBot>
{
public:
	
private:
	ActionResult< INextBot > m_pluginActionResult;

	/**
	 * Stores event result states.
	 *
	 * A stack is used to maintain event result states for each event callback.
	 * This is because events are not atomic; an event can trigger another event
	 * during execution of a callback. Since the plugin natives write results
	 * to the shared m_pluginEventResult member, an inner event can overwrite
	 * the result of the outer event causing unexpected behavior, especially if
	 * the outer event does not actually use a Try*() native. Thus, the stack
	 * is used to restore the event result state when exiting an event
	 * callback.
	 * 
	 * A stack is not used for m_pluginActionResult because OnStart(), Update(),
	 * OnSuspend(), OnResume(), and OnEnd() are all atomic; these callbacks
	 * will never execute within each other.
	 */ 
	SourceHook::CStack<EventDesiredResult< INextBot >> m_eventResultStack;
	EventDesiredResult< INextBot > m_pluginEventResult;

	void * m_pData;

	CBaseNPCPluginActionFactory * m_pFactory;

	bool m_bInActionCallback;

public:
    CBaseNPCPluginAction(CBaseNPCPluginActionFactory * pFactory);
	virtual ~CBaseNPCPluginAction();
	virtual const char* GetName() const override final;

	void* GetData() const { return m_pData; }
	datamap_t* GetDataDescMap() const;

	CBaseNPCPluginActionFactory * GetFactory() const { return m_pFactory; };

	void ResetPluginActionResult();
	void PluginContinue();
	void PluginChangeTo( Action< INextBot > *action, const char *reason );
	void PluginSuspendFor( Action< INextBot > *action, const char *reason );
	void PluginDone( const char *reason );

	bool IsInActionCallback()
	{
		return m_bInActionCallback;
	}

	bool IsInEventCallback()
	{
		return m_eventResultStack.size() > 0;
	}

	virtual ActionResult< INextBot > OnStart( INextBot *me, Action< INextBot > *prevAction ) override final;
	virtual ActionResult< INextBot > Update( INextBot *me, float interval ) override final;
	virtual void OnEnd( INextBot *me, Action< INextBot > *nextAction ) override final;
	virtual ActionResult< INextBot > OnSuspend( INextBot *me, Action< INextBot > *interruptingAction ) override final;
	virtual ActionResult< INextBot > OnResume( INextBot *me, Action< INextBot > *interruptingAction ) override final;
	virtual Action< INextBot > * InitialContainedAction( INextBot *me ) override final;
	virtual bool IsAbleToBlockMovementOf( const INextBot *botInMotion ) const override final;

	virtual QueryResultType			ShouldPickUp( const INextBot *me, CBaseEntity *item ) const override final;
	virtual QueryResultType			ShouldHurry( const INextBot *me ) const override final;
	virtual QueryResultType			ShouldRetreat( const INextBot *me ) const override final;
	virtual QueryResultType			ShouldAttack( const INextBot *me, const CKnownEntity *them ) const override final;
	virtual QueryResultType			IsHindrance( const INextBot *me, CBaseEntity *blocker ) const override final;

	virtual Vector					SelectTargetPoint( const INextBot *me, const CBaseCombatCharacter* subject ) const override final;
	virtual QueryResultType IsPositionAllowed( const INextBot *me, const Vector &pos ) const override final;

	virtual const CKnownEntity *	SelectMoreDangerousThreat( const INextBot *me, 
															   const CBaseCombatCharacter* subject,
															   const CKnownEntity *threat1, 
															   const CKnownEntity *threat2 ) const override final;

	void ResetPluginEventResult();
	void PluginTryContinue( EventResultPriorityType priority );
	void PluginTryChangeTo( Action< INextBot > *action, EventResultPriorityType priority, const char *reason );
	void PluginTrySuspendFor( Action< INextBot > *action, EventResultPriorityType priority, const char *reason );
	void PluginTryDone( EventResultPriorityType priority, const char *reason );
	void PluginTryToSustain( EventResultPriorityType priority, const char *reason );

	virtual EventDesiredResult< INextBot > OnLeaveGround( INextBot *me, CBaseEntity *ground )							override final;
	virtual EventDesiredResult< INextBot > OnLandOnGround( INextBot *me, CBaseEntity *ground )						override final;
	virtual EventDesiredResult< INextBot > OnContact( INextBot *me, CBaseEntity *other, CGameTrace *result = NULL )	override final;
	virtual EventDesiredResult< INextBot > OnMoveToSuccess( INextBot *me, const Path *path )							override final;
	virtual EventDesiredResult< INextBot > OnMoveToFailure( INextBot *me, const Path *path, MoveToFailureType reason ) override final;
	virtual EventDesiredResult< INextBot > OnStuck( INextBot *me )													override final;
	virtual EventDesiredResult< INextBot > OnUnStuck( INextBot *me )													override final;
	virtual EventDesiredResult< INextBot > OnPostureChanged( INextBot *me )											override final;
	virtual EventDesiredResult< INextBot > OnAnimationActivityComplete( INextBot *me, int activity )					override final;
	virtual EventDesiredResult< INextBot > OnAnimationActivityInterrupted( INextBot *me, int activity )				override final;
	virtual EventDesiredResult< INextBot > OnAnimationEvent( INextBot *me, animevent_t *event )						override final;
	virtual EventDesiredResult< INextBot > OnIgnite( INextBot *me )													override final;
	virtual EventDesiredResult< INextBot > OnInjured( INextBot *me, const CTakeDamageInfo &info )						override final;
	virtual EventDesiredResult< INextBot > OnKilled( INextBot *me, const CTakeDamageInfo &info )						override final;
	virtual EventDesiredResult< INextBot > OnOtherKilled( INextBot *me, CBaseCombatCharacter* victim, const CTakeDamageInfo &info )	override final;
	virtual EventDesiredResult< INextBot > OnSight( INextBot *me, CBaseEntity *subject )								override final;
	virtual EventDesiredResult< INextBot > OnLostSight( INextBot *me, CBaseEntity *subject )							override final;
	virtual EventDesiredResult< INextBot > OnSound( INextBot *me, CBaseEntity *source, const Vector &pos, KeyValues *keys )	override final;
	virtual EventDesiredResult< INextBot > OnSpokeConcept( INextBot *me, CBaseCombatCharacter* who, AIConcept_t concept, AI_Response *response )	override final;
	virtual EventDesiredResult< INextBot > OnWeaponFired( INextBot *me, CBaseCombatCharacter* whoFired, CBaseEntity *weapon )	override final;
	virtual EventDesiredResult< INextBot > OnNavAreaChanged( INextBot *me, CNavArea *newArea, CNavArea *oldArea )		override final;
	virtual EventDesiredResult< INextBot > OnModelChanged( INextBot *me )												override final;
	virtual EventDesiredResult< INextBot > OnPickUp( INextBot *me, CBaseEntity *item, CBaseCombatCharacter* giver )	override final;
	virtual EventDesiredResult< INextBot > OnDrop( INextBot *me, CBaseEntity *item )									override final;
	virtual EventDesiredResult< INextBot > OnActorEmoted( INextBot *me, CBaseCombatCharacter* emoter, int emote )			override final;

	virtual EventDesiredResult< INextBot > OnCommandAttack( INextBot *me, CBaseEntity *victim )						override final;
	virtual EventDesiredResult< INextBot > OnCommandApproach( INextBot *me, const Vector &pos, float range )			override final;
	virtual EventDesiredResult< INextBot > OnCommandApproach( INextBot *me, CBaseEntity *goal )						override final;
	virtual EventDesiredResult< INextBot > OnCommandRetreat( INextBot *me, CBaseEntity *threat, float range )			override final;
	virtual EventDesiredResult< INextBot > OnCommandPause( INextBot *me, float duration )								override final;
	virtual EventDesiredResult< INextBot > OnCommandResume( INextBot *me )											override final;
	virtual EventDesiredResult< INextBot > OnCommandString( INextBot *me, const char *command )						override final;

	virtual EventDesiredResult< INextBot > OnShoved( INextBot *me, CBaseEntity *pusher )								override final;
	virtual EventDesiredResult< INextBot > OnBlinded( INextBot *me, CBaseEntity *blinder )							override final;
	virtual EventDesiredResult< INextBot > OnTerritoryContested( INextBot *me, int territoryID )						override final;
	virtual EventDesiredResult< INextBot > OnTerritoryCaptured( INextBot *me, int territoryID )						override final;
	virtual EventDesiredResult< INextBot > OnTerritoryLost( INextBot *me, int territoryID )							override final;
	virtual EventDesiredResult< INextBot > OnWin( INextBot *me )														override final;
	virtual EventDesiredResult< INextBot > OnLose( INextBot *me )														override final;
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
	void DestroyBehavior();


private:
	Behavior< INextBot > * m_pBehavior;
};

class CBaseNPCPluginActionFactories : public IHandleTypeDispatch
{
public:
	CBaseNPCPluginActionFactories();

	// IHandleTypeDispatch
	virtual void OnHandleDestroy( HandleType_t type, void * object ) override final;

	bool Init( IGameConfig* config, char* error, size_t maxlength );
	void OnCoreMapEnd();
	void SDK_OnUnload();

	HandleType_t GetFactoryType() const { return m_FactoryType; }
	CBaseNPCPluginActionFactory* GetFactoryFromHandle( Handle_t handle, HandleError *err = nullptr );
	void OnFactoryCreated( CBaseNPCPluginActionFactory* pFactory );
	void OnFactoryDestroyed( CBaseNPCPluginActionFactory* pFactory );

private:
	HandleType_t m_FactoryType;

	CUtlVector< CBaseNPCPluginActionFactory* > m_Factories;
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
	bool m_bDestroying;

	CUtlMap<CallbackType, IPluginFunction*> m_Callbacks;
	CUtlMap<QueryCallbackType, IPluginFunction*> m_QueryCallbacks;
	CUtlMap<EventResponderCallbackType, IPluginFunction*> m_EventCallbacks;

	std::string m_iActionName;

	CUtlVector< Action< INextBot >* > m_Actions;

public:
	Handle_t m_Handle;

	CBaseNPCPluginActionFactory( IPlugin* plugin, const char* actionName );
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

	Action <INextBot>* Create();
	void OnActionCreated( Action <INextBot>* pAction );
	void OnActionRemoved( Action <INextBot>* pAction );
	void OnCreateInitialAction( Action <INextBot>* pAction );
};

#endif