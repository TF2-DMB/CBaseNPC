#ifndef NATIVE_BEHAVIOR_H_
#define NATIVE_BEHAVIOR_H_

#include "cbasenpc_behavior.h"

#define ACTIONFACTORYNATIVE(name) \
cell_t NextBotActionFactory_##name(IPluginContext *pContext, const cell_t *params) \
{ \
	HandleSecurity security; \
	security.pOwner = NULL; \
	security.pIdentity = myself->GetIdentity(); \
	Handle_t hndlObject = static_cast<Handle_t>(params[1]); \
	CBaseNPCPluginActionFactory *pFactory = nullptr; \
	READHANDLE(hndlObject, BaseNPCPluginActionFactory, pFactory) \

#define ACTIONNATIVE(name) \
cell_t NextBotAction_##name(IPluginContext *pContext, const cell_t *params) \
{ \
	CBaseNPCPluginAction *pAction = (CBaseNPCPluginAction*)params[1]; \
	if (!pAction) return pContext->ThrowNativeError("CBaseNPCPluginAction is NULL"); \
	CBaseNPCPluginActionFactory *pFactory = pAction->GetFactory(); \
	if (!pFactory) return pContext->ThrowNativeError("pFactory is NULL");

#define ACTIONDATANATIVE(name) \
	ACTIONNATIVE(name) \
	void* pData = pAction->GetData(); \
	if (!pData) return pContext->ThrowNativeError("Action has no user data"); \
	datamap_t* pDataMap = pAction->GetDataDescMap(); \
	if (!pDataMap) return pContext->ThrowNativeError("Action has no datamap"); \
	char* prop; pContext->LocalToString(params[2], &prop); \
	char error[256];

cell_t NextBotActionFactory_NextBotActionFactory(IPluginContext *pContext, const cell_t *params)
{
	char* name;
	pContext->LocalToString(params[1], &name);

	if (!name || !strlen(name))
		return pContext->ThrowNativeError("Action must have a name");
	
	IPlugin* plugin = plsys->FindPluginByContext( pContext->GetContext() );
	CBaseNPCPluginActionFactory* pFactory = new CBaseNPCPluginActionFactory( plugin, name );
	return pFactory->m_Handle;
}

ACTIONFACTORYNATIVE(SetCallback)
	IPluginFunction* pCallback = pContext->GetFunctionById( params[3] );
	pFactory->SetCallback( (CBaseNPCPluginActionFactory::CallbackType)params[2], pCallback );
	return 0;
}

ACTIONFACTORYNATIVE(SetQueryCallback)
	IPluginFunction* pCallback = pContext->GetFunctionById( params[3] );
	pFactory->SetQueryCallback( (CBaseNPCPluginActionFactory::QueryCallbackType)params[2], pCallback );
	return 0;
}

ACTIONFACTORYNATIVE(SetEventCallback)
	IPluginFunction* pCallback = pContext->GetFunctionById( params[3] );
	pFactory->SetEventCallback( (CBaseNPCPluginActionFactory::EventResponderCallbackType)params[2], pCallback );
	return 0;
}

ACTIONFACTORYNATIVE(Create)
	return (cell_t)pFactory->Create();
}

ACTIONNATIVE(Destroy)
	delete pAction;
	return 0;
}

ACTIONFACTORYNATIVE(BeginDataMapDesc)
	pFactory->BeginDataDesc();
	return params[1];
}

ACTIONFACTORYNATIVE(DefineIntField)
	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);
	pFactory->DefineField(fieldName, FIELD_INTEGER, params[3], 0, nullptr, 0);
	return params[1];
}

ACTIONFACTORYNATIVE(DefineFloatField)
	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);
	pFactory->DefineField(fieldName, FIELD_FLOAT, params[3], 0, nullptr, 0);
	return params[1];
}

ACTIONFACTORYNATIVE(DefineCharField)
	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);
	pFactory->DefineField(fieldName, FIELD_CHARACTER, params[3], 0, nullptr, 0);
	return params[1];
}

ACTIONFACTORYNATIVE(DefineBoolField)
	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);
	pFactory->DefineField(fieldName, FIELD_BOOLEAN, params[3], 0, nullptr, 0);
	return params[1];
}

ACTIONFACTORYNATIVE(DefineVectorField)
	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);
	pFactory->DefineField(fieldName, FIELD_VECTOR, params[3], 0, nullptr, 0);
	return params[1];
}

ACTIONFACTORYNATIVE(DefineStringField)
	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);
	pFactory->DefineField(fieldName, FIELD_STRING, params[3], 0, nullptr, 0);
	return params[1];
}

ACTIONFACTORYNATIVE(DefineColorField)
	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);
	pFactory->DefineField(fieldName, FIELD_COLOR32, params[3], 0, nullptr, 0);
	return params[1];
}

ACTIONFACTORYNATIVE(EndDataMapDesc)
	pFactory->EndDataDesc();
	return 0;
}

ACTIONNATIVE(GetName)
	cell_t buffer = params[2];
	size_t bufferSize = params[3];

	return pContext->StringToLocal(buffer, bufferSize, pAction->GetName());
}

ACTIONNATIVE(IsNamed)
	char* name;
	pContext->LocalToString( params[2], &name );
	return pAction->IsNamed( name );
}

ACTIONNATIVE(GetFullName)

	cell_t buffer = params[2];
	size_t bufferSize = params[3];

	return pContext->StringToLocal(buffer, bufferSize, pAction->GetFullName());
}

ACTIONNATIVE(ActorGet)
	return gamehelpers->EntityToBCompatRef( pAction->GetActor() );
}

ACTIONNATIVE(ParentGet)
	return (cell_t)pAction->GetParentAction();
}

ACTIONNATIVE(ActiveChildGet)
	return (cell_t)pAction->GetActiveChildAction();
}

ACTIONNATIVE(ActionBuriedUnderMeGet)
	return (cell_t)pAction->GetActionBuriedUnderMe();
}

ACTIONNATIVE(ActionCoveringMeGet)
	return (cell_t)pAction->GetActionCoveringMe();
}

ACTIONNATIVE(IsSuspendedGet)
	return pAction->IsSuspended();
}

ACTIONNATIVE(HasData)
	if ( !pAction->GetDataDescMap() ) return 0;

	char* prop;
	pContext->LocalToString(params[2], &prop); char error[256];

	sm_datatable_info_t dt;
	return pFactory->FindDataMapInfo( prop, &dt, error, sizeof(error) );
}

ACTIONDATANATIVE(GetData)

	cell_t value;
	int element = params[3];
	if (!pFactory->GetObjectData( pData, prop, value, element, error, sizeof(error)))
		return pContext->ThrowNativeError( error );

	return value;
}

ACTIONDATANATIVE(SetData)
	
	cell_t value = params[3];
	int element = params[4];

	if (!pFactory->SetObjectData( pData, prop, value, element, error, sizeof(error)))
		return pContext->ThrowNativeError( error );

	return 0;
}

ACTIONDATANATIVE(GetDataFloat)
	
	float value;
	int element = params[3];
	if (!pFactory->GetObjectDataFloat( pData, prop, value, element, error, sizeof(error)))
		return pContext->ThrowNativeError( error );

	return sp_ftoc(value);
}

ACTIONDATANATIVE(SetDataFloat)
	
	float value = sp_ctof( params[3] );
	int element = params[4];
	if (!pFactory->SetObjectDataFloat( pData, prop, value, element, error, sizeof(error)))
		return pContext->ThrowNativeError( error );

	return 0;
}

ACTIONDATANATIVE(GetDataVector)
	
	cell_t* vec;
	pContext->LocalToPhysAddr(params[3], &vec);
	int element = params[4];

	float value[3];
	if (!pFactory->GetObjectDataVector( pData, prop, value, element, error, sizeof(error)))
		return pContext->ThrowNativeError( error );

	vec[0] = sp_ftoc(value[0]); vec[1] = sp_ftoc(value[1]); vec[2] = sp_ftoc(value[2]); 
	return 0;
}

ACTIONDATANATIVE(SetDataVector)
	
	cell_t* vec;
	pContext->LocalToPhysAddr(params[3], &vec);
	int element = params[4];

	float value[3];
	value[0] = sp_ctof(vec[0]); value[1] = sp_ctof(vec[1]); value[2] = sp_ctof(vec[2]); 

	if (!pFactory->SetObjectDataVector( pData, prop, value, element, error, sizeof(error)))
		return pContext->ThrowNativeError( error );

	return 0;
}

ACTIONDATANATIVE(GetDataString)

	cell_t dest = params[3];
	size_t bufferSize = params[4];
	int element = params[5];

	char * value = new char[ bufferSize ];
	bool result = pFactory->GetObjectDataString( pData, prop, value, bufferSize, element, error, sizeof(error) );
	if (result)
	{
		size_t len;
		pContext->StringToLocalUTF8(dest, bufferSize, value, &len);
	}
	else
	{
		pContext->StringToLocal(dest, bufferSize, "");
	}

	delete[] value;

	return result ? 0 : pContext->ThrowNativeError( error );
}

ACTIONDATANATIVE(SetDataString)

	cell_t src = params[3];
	int element = params[4];

	char* value;
	pContext->LocalToStringNULL(src, &value);
	const char* data = (value) ? value : "";
	if (!pFactory->SetObjectDataString( pData, prop, data, element, error, sizeof(error)))
	{
		return pContext->ThrowNativeError( error );
	}

	return 0;
}

ACTIONNATIVE(Continue)
	if ( !pAction->IsInActionCallback() || pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use Continue() outside of Action callback" );

	pAction->PluginContinue();
	return 0;
}

ACTIONNATIVE(ChangeTo)
	if ( !pAction->IsInActionCallback() || pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use ChangeTo() outside of Action callback" );

	char* reason = nullptr;
	pContext->LocalToStringNULL( params[3], &reason );
	pAction->PluginChangeTo( (CBaseNPCPluginAction*)params[2], reason );
	return 0;
}

ACTIONNATIVE(SuspendFor)
	if ( !pAction->IsInActionCallback() || pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use SuspendFor() outside of Action callback" );

	char* reason = nullptr;
	pContext->LocalToStringNULL( params[3], &reason );
	pAction->PluginSuspendFor( (CBaseNPCPluginAction*)params[2], reason );
	return 0;
}

ACTIONNATIVE(Done)
	if ( !pAction->IsInActionCallback() || pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use Done() outside of Action callback" );

	char* reason = nullptr;
	pContext->LocalToStringNULL( params[2], &reason );
	pAction->PluginDone(reason);
	return 0;
}

ACTIONNATIVE(TryContinue)
	if ( !pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use TryContinue() outside of event callback" );

	pAction->PluginTryContinue( (EventResultPriorityType)params[2] );
	return 0;
}

ACTIONNATIVE(TryChangeTo)
	if ( !pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use TryChangeTo() outside of event callback" );

	CBaseNPCPluginAction * pOtherAction = (CBaseNPCPluginAction*)params[2];
	if (!pOtherAction) return pContext->ThrowNativeError("action is NULL");

	char* reason = nullptr;
	pContext->LocalToStringNULL( params[4], &reason );

	pAction->PluginTryChangeTo( pOtherAction, (EventResultPriorityType)params[3], reason );
	return 0;
}

ACTIONNATIVE(TrySuspendFor)
	if ( !pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use TrySuspendFor() outside of event callback" );

	CBaseNPCPluginAction * pOtherAction = (CBaseNPCPluginAction*)params[2];
	if (!pOtherAction) return pContext->ThrowNativeError("action is NULL");

	char* reason = nullptr;
	pContext->LocalToStringNULL( params[4], &reason );

	pAction->PluginTrySuspendFor( pOtherAction, (EventResultPriorityType)params[3], reason );
	return 0;
}

ACTIONNATIVE(TryDone)
	if ( !pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use TryDone() outside of event callback" );

	char* reason;
	pContext->LocalToStringNULL( params[3], &reason );

	pAction->PluginTryDone( (EventResultPriorityType)params[2], reason );
	return 0;
}

ACTIONNATIVE(TryToSustain)
	if ( !pAction->IsInEventCallback() )
		return pContext->ThrowNativeError( "Cannot use TryToSustain() outside of event callback" );

	char* reason;
	pContext->LocalToStringNULL( params[3], &reason );

	pAction->PluginTryToSustain( (EventResultPriorityType)params[2], reason );
	return 0;
}

#endif