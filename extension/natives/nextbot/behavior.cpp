#include "behavior.hpp"
#include "cbasenpc_behavior.h"

namespace natives::nextbot::behavior {

namespace factory {

inline CBaseNPCPluginActionFactory* Get(IPluginContext* context, const cell_t param) {
	HandleSecurity security;
	security.pOwner = nullptr;
	security.pIdentity = myself->GetIdentity();
	Handle_t hndlObject = static_cast<Handle_t>(param);
	CBaseNPCPluginActionFactory* factory = nullptr;
	READHANDLE(hndlObject, BaseNPCPluginActionFactory, factory);
	return factory;
}

inline void DefineField(IPluginContext* context, const cell_t* params, fieldtype_t fieldType) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return;
	}

	char* fieldName;
	context->LocalToString(params[2], &fieldName);
	if (!fieldName || (fieldName && !fieldName[0])) {
		context->ThrowNativeError("Field name cannot be NULL or empty");
		return;
	}

	factory->DefineField(fieldName, fieldType, params[3], 0, nullptr, 0);
}

cell_t NextBotActionFactory_Ctor(IPluginContext* context, const cell_t* params) {
	char* name;
	context->LocalToString(params[1], &name);

	if (!name || !strlen(name)) {
		return context->ThrowNativeError("Action must have a name");
	}
	
	IPlugin* plugin = plsys->FindPluginByContext(context->GetContext());
	CBaseNPCPluginActionFactory* factory = new CBaseNPCPluginActionFactory(plugin, name);
	return factory->m_Handle;
}

cell_t SetCallback(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	IPluginFunction* callback = context->GetFunctionById( params[3] );
	factory->SetCallback((CBaseNPCPluginActionFactory::CallbackType)params[2], callback);
	return 0;
}

cell_t SetQueryCallback(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	IPluginFunction* callback = context->GetFunctionById( params[3] );
	factory->SetQueryCallback((CBaseNPCPluginActionFactory::QueryCallbackType)params[2], callback);
	return 0;
}

cell_t SetEventCallback(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	IPluginFunction* callback = context->GetFunctionById( params[3] );
	factory->SetEventCallback((CBaseNPCPluginActionFactory::EventResponderCallbackType)params[2], callback);
	return 0;
}

cell_t Create(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	return PtrToPawnAddress(factory->Create());
}

cell_t BeginDataMapDesc(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	factory->BeginDataDesc();
	return params[1];
}

cell_t DefineIntField(IPluginContext* context, const cell_t* params) {
	DefineField(context, params, FIELD_INTEGER);
	return params[1];
}

cell_t DefineFloatField(IPluginContext* context, const cell_t* params) {
	DefineField(context, params, FIELD_FLOAT);
	return params[1];
}

cell_t DefineCharField(IPluginContext* context, const cell_t* params) {
	DefineField(context, params, FIELD_CHARACTER);
	return params[1];
}

cell_t DefineBoolField(IPluginContext* context, const cell_t* params) {
	DefineField(context, params, FIELD_BOOLEAN);
	return params[1];
}

cell_t DefineVectorField(IPluginContext* context, const cell_t* params) {
	DefineField(context, params, FIELD_VECTOR);
	return params[1];
}

cell_t DefineStringField(IPluginContext* context, const cell_t* params) {
	DefineField(context, params, FIELD_STRING);
	return params[1];
}

cell_t DefineColorField(IPluginContext* context, const cell_t* params) {
	DefineField(context, params, FIELD_COLOR32);
	return params[1];
}

cell_t DefineEntityField(IPluginContext* context, const cell_t* params) {
	DefineField(context, params, FIELD_EHANDLE);
	return params[1];
}

cell_t EndDataMapDesc(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	factory->EndDataDesc();
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"NextBotActionFactory.NextBotActionFactory", NextBotActionFactory_Ctor},
		{"NextBotActionFactory.SetCallback", SetCallback},
		{"NextBotActionFactory.SetQueryCallback", SetQueryCallback},
		{"NextBotActionFactory.SetEventCallback", SetEventCallback},
		{"NextBotActionFactory.Create", Create},
		{"NextBotActionFactory.BeginDataMapDesc", BeginDataMapDesc},
		{"NextBotActionFactory.DefineIntField", DefineIntField},
		{"NextBotActionFactory.DefineFloatField", DefineFloatField},
		{"NextBotActionFactory.DefineCharField", DefineCharField},
		{"NextBotActionFactory.DefineBoolField", DefineBoolField},
		{"NextBotActionFactory.DefineVectorField", DefineVectorField},
		{"NextBotActionFactory.DefineStringField", DefineStringField},
		{"NextBotActionFactory.DefineColorField", DefineColorField},
		{"NextBotActionFactory.DefineEntityField", DefineEntityField},
		{"NextBotActionFactory.EndDataMapDesc", EndDataMapDesc}
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}

inline CBaseNPCPluginAction* Get(IPluginContext* context, const cell_t param) {
	CBaseNPCPluginAction* action = (CBaseNPCPluginAction*)PawnAddressToPtr(param);
	if (!action) {
		context->ThrowNativeError("CBaseNPCPluginAction ptr is null!");
		return nullptr;
	}
	return action;
}

template<typename T>
inline bool SetData(IPluginContext* context, CBaseNPCPluginAction* action, cell_t propt, T value, int element) {
	CBaseNPCPluginActionFactory* factory = action->GetFactory();
	if (!factory) {
		context->ThrowNativeError("Action has no factory");
		return false;
	}

	void* data = action->GetData();
	if (!data) {
		context->ThrowNativeError("Action has no user data");
		return false;
	}

	datamap_t* datamap = action->GetDataDescMap();
	if (!datamap) {
		context->ThrowNativeError("Action has no datamap");
		return false;
	}

	char* prop;
	context->LocalToString(propt, &prop);
	char error[256];

	if (!factory->SetObjectDataEx(data, prop, value, element, error, sizeof(error))) {
		return context->ThrowNativeError(error);
	}

	return true;
}

template<typename T>
inline bool GetData(IPluginContext* context, CBaseNPCPluginAction* action, cell_t propt, T* value, int element) {
	CBaseNPCPluginActionFactory* factory = action->GetFactory();
	if (!factory) {
		context->ThrowNativeError("Action has no factory");
		return false;
	}

	void* data = action->GetData();
	if (!data) {
		context->ThrowNativeError("Action has no user data");
		return false;
	}

	datamap_t* datamap = action->GetDataDescMap();
	if (!datamap) {
		context->ThrowNativeError("Action has no datamap");
		return false;
	}

	char* prop;
	context->LocalToString(propt, &prop);
	char error[256];

	if (!factory->GetObjectDataEx(data, prop, value, element, error, sizeof(error))) {
		return context->ThrowNativeError(error);
	}

	return true;
}

cell_t GetName(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	cell_t buffer = params[2];
	size_t bufferSize = params[3];

	return context->StringToLocal(buffer, bufferSize, action->GetName());
}

cell_t IsNamed(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	char* name;
	context->LocalToString(params[2], &name);
	return action->IsNamed(name);
}

cell_t Destroy(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	delete action;
	return 0;
}

cell_t GetFullName(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	cell_t buffer = params[2];
	size_t bufferSize = params[3];

	return context->StringToLocal(buffer, bufferSize, action->GetFullName());
}

cell_t GetActor(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	return gamehelpers->EntityToBCompatRef( action->GetActor()->GetEntity() );
}

cell_t GetParent(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	return PtrToPawnAddress(action->GetParentAction());
}

cell_t GetActiveChild(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	return PtrToPawnAddress(action->GetActiveChildAction());
}

cell_t GetActionBuriedUnderMe(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	return PtrToPawnAddress(action->GetActionBuriedUnderMe());
}

cell_t GetActionCoveringMe(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	return PtrToPawnAddress(action->GetActionCoveringMe());
}

cell_t GetIsSuspended(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	return action->IsSuspended();
}

cell_t HasData(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->GetDataDescMap()) {
		return 0;
	}

	CBaseNPCPluginActionFactory* factory = action->GetFactory();
	if (!factory) {
		context->ThrowNativeError("Action has no factory");
		return 0;
	}

	char* prop;
	context->LocalToString(params[2], &prop);
	char error[256];

	sm_datatable_info_t dt;
	return factory->FindDataMapInfo(prop, &dt, error, sizeof(error));
}

cell_t GetData(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	int value;
	GetData(context, action, params[2], &value, params[3]);
	return value;
}

cell_t SetData(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	SetData(context, action, params[2], params[3], params[4]);
	return 0;
}

cell_t GetDataFloat(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	float value;
	GetData(context, action, params[2], &value, params[3]);
	return sp_ftoc(value);
}

cell_t SetDataFloat(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	SetData(context, action, params[2], sp_ctof(params[3]), params[4]);
	return 0;
}

cell_t GetDataVector(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	cell_t* vec;
	context->LocalToPhysAddr(params[3], &vec);

	float value[3];
	float* ex = value;
	if (GetData(context, action, params[2], &ex, params[4])) {
		vec[0] = sp_ftoc(value[0]); vec[1] = sp_ftoc(value[1]); vec[2] = sp_ftoc(value[2]); 
	}
	return 0;
}

cell_t SetDataVector(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	cell_t* vec;
	context->LocalToPhysAddr(params[3], &vec);
	int element = params[4];

	float value[3];
	float* ex = value;
	value[0] = sp_ctof(vec[0]); value[1] = sp_ctof(vec[1]); value[2] = sp_ctof(vec[2]); 

	SetData(context, action, params[2], ex, params[4]);
	return 0;
}

cell_t GetDataString(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	cell_t dest = params[3];
	size_t bufferSize = params[4];
	char* value = nullptr;
	if (GetData(context, action, params[2], &value, params[5])) {
		size_t len;
		context->StringToLocalUTF8(dest, bufferSize, value, &len);
	}
	return 0;
}

cell_t SetDataString(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	cell_t src = params[3];
	int element = params[4];

	char* value;
	context->LocalToStringNULL(src, &value);
	const char* data = (value) ? value : "";

	SetData(context, action, params[2], data, params[4]);
	return 0;
}

cell_t GetDataEnt(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	CBaseEntity* value = nullptr;
	if (GetData(context, action, params[2], &value, params[3])) {
		return gamehelpers->EntityToBCompatRef(value);
	}
	return 0;
}

cell_t SetDataEnt(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	cell_t entIndex = params[3];
	CBaseEntity* value = gamehelpers->ReferenceToEntity(entIndex);

	if (!value && entIndex != -1) {
		return context->ThrowNativeError("Entity %d (%d) is invalid", gamehelpers->ReferenceToIndex(entIndex), entIndex);
	}

	SetData(context, action, params[2], value, params[4]);
	return 0;
}


cell_t Continue(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInActionCallback() || action->IsInEventCallback()) {
		return context->ThrowNativeError( "Cannot use Continue() outside of Action callback" );
	}

	action->PluginContinue();
	return 0;
}

cell_t ChangeTo(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInActionCallback() || action->IsInEventCallback()) {
		return context->ThrowNativeError("Cannot use ChangeTo() outside of Action callback");
	}

	char* reason = nullptr;
	context->LocalToStringNULL( params[3], &reason );
	action->PluginChangeTo( (CBaseNPCPluginAction*)PawnAddressToPtr(params[2]), reason );
	return 0;
}

cell_t SuspendFor(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInActionCallback() || action->IsInEventCallback()) {
		return context->ThrowNativeError( "Cannot use SuspendFor() outside of Action callback" );
	}

	char* reason = nullptr;
	context->LocalToStringNULL( params[3], &reason );
	action->PluginSuspendFor( (CBaseNPCPluginAction*)PawnAddressToPtr(params[2]), reason );
	return 0;
}

cell_t Done(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInActionCallback() || action->IsInEventCallback()) {
		return context->ThrowNativeError( "Cannot use Done() outside of Action callback" );
	}

	char* reason = nullptr;
	context->LocalToStringNULL( params[2], &reason );
	action->PluginDone(reason);
	return 0;
}

cell_t TryContinue(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInEventCallback()) {
		return context->ThrowNativeError( "Cannot use TryContinue() outside of event callback" );
	}

	action->PluginTryContinue( (EventResultPriorityType)params[2] );
	return 0;
}

cell_t TryChangeTo(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInEventCallback()) {
		return context->ThrowNativeError( "Cannot use TryChangeTo() outside of event callback" );
	}

	CBaseNPCPluginAction* otherAction = (CBaseNPCPluginAction*)PawnAddressToPtr(params[2]);
	if (!otherAction) {
		return context->ThrowNativeError("action is NULL");
	}

	char* reason = nullptr;
	context->LocalToStringNULL( params[4], &reason );

	action->PluginTryChangeTo(otherAction, (EventResultPriorityType)params[3], reason);
	return 0;
}

cell_t TrySuspendFor(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInEventCallback()) {
		return context->ThrowNativeError( "Cannot use TrySuspendFor() outside of event callback" );
	}
		
	CBaseNPCPluginAction* otherAction = (CBaseNPCPluginAction*)PawnAddressToPtr(params[2]);
	if (!otherAction) {
		return context->ThrowNativeError("action is NULL");
	}

	char* reason = nullptr;
	context->LocalToStringNULL( params[4], &reason );

	action->PluginTrySuspendFor( otherAction, (EventResultPriorityType)params[3], reason );
	return 0;
}

cell_t TryDone(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInEventCallback()) {
		return context->ThrowNativeError( "Cannot use TryDone() outside of event callback" );
	}

	char* reason;
	context->LocalToStringNULL( params[3], &reason );

	action->PluginTryDone( (EventResultPriorityType)params[2], reason );
	return 0;
}

cell_t TryToSustain(IPluginContext* context, const cell_t* params) {
	auto action = Get(context, params[1]);
	if (!action) {
		return 0;
	}

	if (!action->IsInEventCallback()) {
		return context->ThrowNativeError("Cannot use TryToSustain() outside of event callback");
	}

	char* reason;
	context->LocalToStringNULL(params[3], &reason);

	action->PluginTryToSustain((EventResultPriorityType)params[2], reason);
	return 0;
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	factory::setup(natives);

	sp_nativeinfo_t list[] = {
		{"NextBotAction.Actor.get", GetActor},
		{"NextBotAction.Parent.get", GetParent},
		{"NextBotAction.ActiveChild.get", GetActiveChild},
		{"NextBotAction.ActionBuriedUnderMe.get", GetActionBuriedUnderMe},
		{"NextBotAction.ActionCoveringMe.get", GetActionCoveringMe},
		{"NextBotAction.GetName", GetName},
		{"NextBotAction.GetFullName", GetFullName},
		{"NextBotAction.IsNamed", IsNamed},
		{"NextBotAction.Destroy", Destroy},
		{"NextBotAction.HasData", HasData},
		{"NextBotAction.GetData", GetData},
		{"NextBotAction.SetData", SetData},
		{"NextBotAction.GetDataFloat", GetDataFloat},
		{"NextBotAction.SetDataFloat", SetDataFloat},
		{"NextBotAction.GetDataVector", GetDataVector},
		{"NextBotAction.SetDataVector", SetDataVector},
		{"NextBotAction.GetDataString", GetDataString},
		{"NextBotAction.SetDataString", SetDataString},
		{"NextBotAction.SetDataEnt", SetDataEnt},
		{"NextBotAction.GetDataEnt", GetDataEnt},
		{"NextBotAction.IsSuspended.get", GetIsSuspended},
		{"NextBotAction.Continue", Continue},
		{"NextBotAction.ChangeTo", ChangeTo},
		{"NextBotAction.SuspendFor", SuspendFor},
		{"NextBotAction.Done", Done},
		{"NextBotAction.TryContinue", TryContinue},
		{"NextBotAction.TryChangeTo", TryChangeTo},
		{"NextBotAction.TrySuspendFor", TrySuspendFor},
		{"NextBotAction.TryDone", TryDone},
		{"NextBotAction.TryToSustain", TryToSustain}
	};
	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}