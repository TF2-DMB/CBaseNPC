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
	CBaseNPCPluginActionFactory *pFactory = pAction->m_pFactory; \
	if (!pFactory) return pContext->ThrowNativeError("pFactory is NULL");

#define ACTIONDATANATIVE(name) \
	ACTIONNATIVE(name) \
	void* pData = pAction->GetData(); \
	if (!pData) return pContext->ThrowNativeError("Action has no user data");


cell_t NextBotActionFactory_NextBotActionFactory(IPluginContext *pContext, const cell_t *params)
{
	char* name;
	pContext->LocalToString(params[1], &name);

	if (!name || !strlen(name))
		return pContext->ThrowNativeError("Action must have a name");
	
	CBaseNPCPluginActionFactory* pFactory = new CBaseNPCPluginActionFactory(name);

	Handle_t handle = CREATEHANDLE(BaseNPCPluginActionFactory, pFactory);
	pFactory->m_Handle = handle;

	return handle;
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

ACTIONFACTORYNATIVE(EndDataMapDesc)
	pFactory->EndDataDesc();
	return 0;
}

ACTIONNATIVE(GetName)
	cell_t buffer = params[2];
	size_t bufferSize = params[3];

	return pContext->StringToLocal(buffer, bufferSize, pAction->GetName());
}

ACTIONNATIVE(GetFullName)

	cell_t buffer = params[2];
	size_t bufferSize = params[3];

	return pContext->StringToLocal(buffer, bufferSize, pAction->GetFullName());
}

ACTIONNATIVE(GetActor)
	return gamehelpers->EntityToBCompatRef( pAction->GetActor() );
}

ACTIONNATIVE(IsSuspended)
	return pAction->IsSuspended();
}

ACTIONDATANATIVE(GetData)
	
	char* prop;
	pContext->LocalToString(params[2], &prop);

	int element = params[3];

	return GetObjectProp(pContext, pData, pAction->GetDataDescMap(), prop, element);
}

ACTIONDATANATIVE(SetData)
	
	char* prop;
	pContext->LocalToString(params[2], &prop);

	cell_t value = params[3];
	int element = params[4];

	return SetObjectProp(pContext, pData, pAction->GetDataDescMap(), prop, value, element);
}

ACTIONDATANATIVE(GetDataFloat)
	
	char* prop;
	pContext->LocalToString(params[2], &prop);

	int element = params[3];

	return GetObjectPropFloat(pContext, pData, pAction->GetDataDescMap(), prop, element);
}

ACTIONDATANATIVE(SetDataFloat)
	
	char* prop;
	pContext->LocalToString(params[2], &prop);

	cell_t value = params[3];
	int element = params[4];

	return SetObjectPropFloat(pContext, pData, pAction->GetDataDescMap(), prop, value, element);
}

ACTIONDATANATIVE(GetDataVector)
	
	char* prop;
	pContext->LocalToString(params[2], &prop);

	int element = params[4];

	return GetObjectPropVector(pContext, pData, pAction->GetDataDescMap(), prop, params[3], element);
}

ACTIONDATANATIVE(SetDataVector)
	
	char* prop;
	pContext->LocalToString(params[2], &prop);

	int element = params[4];

	return SetObjectPropVector(pContext, pData, pAction->GetDataDescMap(), prop, params[3], element);
}

ACTIONDATANATIVE(GetDataString)

	char* prop;
	pContext->LocalToString(params[2], &prop);

	cell_t buffer = params[3];
	size_t bufferSize = params[4];

	int element = params[5];

	return GetObjectPropString(pContext, pData, pAction->GetDataDescMap(), prop, buffer, bufferSize, element);
}

ACTIONDATANATIVE(SetDataString)

	char* prop;
	pContext->LocalToString(params[2], &prop);

	cell_t buffer = params[3];
	int element = params[4];

	return SetObjectPropString(pContext, pData, pAction->GetDataDescMap(), prop, buffer, element);
}

ACTIONNATIVE(Continue)
	pAction->PluginContinue();
	return 0;
}

ACTIONNATIVE(ChangeTo)
	char* reason = nullptr;
	pContext->LocalToStringNULL( params[3], &reason );
	pAction->PluginChangeTo( (CBaseNPCPluginAction*)params[2], reason );
	return 0;
}

ACTIONNATIVE(SuspendFor)
	char* reason = nullptr;
	pContext->LocalToStringNULL( params[3], &reason );
	pAction->PluginSuspendFor( (CBaseNPCPluginAction*)params[2], reason );
	return 0;
}

ACTIONNATIVE(Done)
	char* reason = nullptr;
	pContext->LocalToStringNULL( params[2], &reason );
	pAction->PluginDone(reason);
	return 0;
}

ACTIONNATIVE(TryContinue)
	pAction->PluginTryContinue( (EventResultPriorityType)params[2] );
	return 0;
}

ACTIONNATIVE(TryChangeTo)
	CBaseNPCPluginAction * pOtherAction = (CBaseNPCPluginAction*)params[2];
	if (!pOtherAction) return pContext->ThrowNativeError("action is NULL");

	char* reason = nullptr;
	pContext->LocalToStringNULL( params[4], &reason );

	pAction->PluginTryChangeTo( pOtherAction, (EventResultPriorityType)params[3], reason );
	return 0;
}

ACTIONNATIVE(TrySuspendFor)
	CBaseNPCPluginAction * pOtherAction = (CBaseNPCPluginAction*)params[2];
	if (!pOtherAction) return pContext->ThrowNativeError("action is NULL");

	char* reason = nullptr;
	pContext->LocalToStringNULL( params[4], &reason );

	pAction->PluginTrySuspendFor( pOtherAction, (EventResultPriorityType)params[3], reason );
	return 0;
}

ACTIONNATIVE(TryDone)
	char* reason;
	pContext->LocalToStringNULL( params[3], &reason );

	pAction->PluginTryDone( (EventResultPriorityType)params[2], reason );
	return 0;
}

ACTIONNATIVE(TryToSustain)
	char* reason;
	pContext->LocalToStringNULL( params[3], &reason );

	pAction->PluginTryDone( (EventResultPriorityType)params[2], reason );
	return 0;
}

#endif