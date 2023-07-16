#include "entityfactory.hpp"
#include "pluginentityfactory.h"

namespace natives::entityfactory {

inline CPluginEntityFactory* Get(IPluginContext* context, const cell_t param) {
	HandleSecurity security;
	security.pOwner = nullptr;
	security.pIdentity = myself->GetIdentity();
	Handle_t hndlObject = static_cast<Handle_t>(param);
	CPluginEntityFactory *factory = nullptr;
	READHANDLE(hndlObject, PluginEntityFactory, factory);
	return factory;
}

inline CPluginEntityFactory* Get(IPluginContext* context, const cell_t param, bool shouldBeInstalled) {
	auto factory = Get(context, param);
	if (!factory) {
		return nullptr;
	}

	if (factory->m_bInstalled != shouldBeInstalled) {
		if (shouldBeInstalled) {
			context->ThrowNativeError("Factory must be installed!");
		} else {
			context->ThrowNativeError("Factory must be uninstalled!");
		}
		return nullptr;
	}
	return factory;
}

cell_t CPluginEntityFactory_Ctor(IPluginContext * context, const cell_t * params) {
	char* classname;
	context->LocalToString(params[1], &classname);

	if (!classname || !strlen(classname)) {
		return context->ThrowNativeError("Entity factory must have a classname");
	}

	IPlugin* plugin = plsys->FindPluginByContext( context->GetContext() );
	IPluginFunction *postConstructor = context->GetFunctionById(params[2]);
	IPluginFunction *onRemove = context->GetFunctionById(params[3]);

	CPluginEntityFactory* factory = new CPluginEntityFactory(plugin, classname, postConstructor, onRemove);
	return factory->m_Handle;
}

cell_t GetFactoryOfEntity(IPluginContext* context, const cell_t * params) {
	CBaseEntity* entity = gamehelpers->ReferenceToEntity(params[1]);
	if (!entity && params[1] != -1) {
		return context->ThrowNativeError("Entity %d (%d) is invalid", entity, params[1]);
	}
	
	CPluginEntityFactory* factory = g_pPluginEntityFactories->GetFactory(entity);
	if (!factory) {
		return BAD_HANDLE;
	}
	
	return factory->m_Handle;
}

cell_t GetNumInstalledFactories(IPluginContext* context, const cell_t* params) {
	return g_pPluginEntityFactories->GetInstalledFactoryHandles(nullptr, 0);
}

cell_t GetInstalledFactories(IPluginContext* context, const cell_t* params) {
	cell_t * addr;
	context->LocalToPhysAddr(params[1], &addr);
	Handle_t * pArray = reinterpret_cast<Handle_t *>(addr);

	int arraySize = params[2];

	return g_pPluginEntityFactories->GetInstalledFactoryHandles(pArray, arraySize);
}

cell_t DeriveFromBaseEntity(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	factory->DeriveFromBaseEntity(params[2] == 1);
	return 0;
}

cell_t DeriveFromNPC(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	factory->DeriveFromNPC();
	return 0;
}

cell_t SetInitialActionFactory(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	auto hndlObject = static_cast<Handle_t>(params[2]);
	CBaseNPCPluginActionFactory* action = nullptr;
	if (hndlObject != BAD_HANDLE) {
		action = g_pBaseNPCPluginActionFactories->GetFactoryFromHandle(hndlObject);
		if (!action) {
			return context->ThrowNativeError("Invalid action factory");
		}
	}

	factory->SetBaseNPCInitialActionFactory(action);
	return 0;
}

cell_t DeriveFromClass(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	char* classname;
	context->LocalToString(params[2], &classname);

	IEntityFactory* derivedFactory = g_pPluginEntityFactories->FindFactory(classname);
	if (!derivedFactory) {
		return context->ThrowNativeError("Cannot derive from uninstalled entity factory %s", classname);
	}
	
	factory->DeriveFromClass(classname);

	return 0;
}

cell_t DeriveFromFactory(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	CPluginEntityFactory* otherFactory = Get(context, params[2]);
	if (!otherFactory) {
		return 0;
	}

	if (otherFactory == factory) {
		return context->ThrowNativeError("Cannot derive from self");
	}

	factory->DeriveFromHandle( params[2] );

	return 0;
}

cell_t DeriveFromConf(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}
	
	size_t entitySize = params[2];
	IGameConfig* config = gameconfs->ReadHandle(params[3], context->GetIdentity(), nullptr);
	if (!config) {
		return context->ThrowNativeError("Invalid gameconfig handle");
	}
	
	int type = params[4];
	char* entry;
	context->LocalToString(params[5], &entry);

	if (!factory->DeriveFromConf(entitySize, config, type, entry))
	{
		context->ThrowNativeError("Failed to obtain constructor function from gamedata entry %s", entry);
	}
	return 0;
}

cell_t Install(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}
	
	if (!factory->IsAbstract()) {
		const char* classname = factory->m_iClassname.c_str();
		if (g_pPluginEntityFactories->FindPluginFactory(classname) != nullptr) {
			return context->ThrowNativeError("Entity factory already exists with the same classname");
		}
	}

	if (factory->DoesNotDerive()) {
		return context->ThrowNativeError("Entity factory must derive from an existing class or classname");
	}

	if (!factory->Install()) {
		return context->ThrowNativeError("Failed to install; make sure base factory is installed first");
	}
	
	return 0;
}

cell_t Uninstall(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], true);
	if (!factory) {
		return 0;
	}

	factory->Uninstall();
	return 0;
}

cell_t GetIsInstalled(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	return factory->m_bInstalled;
}

cell_t GetIsAbstract(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	return factory->IsAbstract();
}

cell_t SetIsAbstract(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	factory->SetAbstract(params[2] == 1);
	return 0;
}

cell_t GetClassname(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	if (!factory) {
		return 0;
	}

	size_t bufferSize = params[3];
	context->StringToLocal(params[2], bufferSize, factory->m_iClassname.c_str());
	return 0;
}

cell_t AttachNextBot(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	// TO-DO: 2.0.0
	bool old = ((params[0] < 2) || context->GetFunctionById(params[2]) == nullptr);
	factory->AttachNextBot((old) ? (IPluginFunction*)0x1 : context->GetFunctionById(params[2]));
	return 0;
}

cell_t BeginDataMapDesc(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	char* dataClassName;
	context->LocalToStringNULL(params[2], &dataClassName);
	bool startedDesc;
	if (!dataClassName) {
		startedDesc = factory->BeginDataDesc(factory->m_iClassname.c_str());
	} else {
		startedDesc = factory->BeginDataDesc(dataClassName);
	}

	if (!startedDesc) {
		return context->ThrowNativeError("Base factory was not installed before datamap definition");
	}

	return params[1];
}

cell_t EndDataMapDesc(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	factory->EndDataDesc();
	return 0;
}

inline void DefineField(IPluginContext* context, const cell_t* params, fieldtype_t fieldType) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return;
	}

	char* fieldName;
	context->LocalToString(params[2], &fieldName);
	if (!fieldName || (fieldName && !fieldName[0])) {
		context->ThrowNativeError("Field name cannot be NULL or empty");
		return;
	}

	int numElements = params[3];
	if (numElements <= 0) {
		context->ThrowNativeError("Elements must be >= 1");
		return;
	}

	char* keyName;
	context->LocalToStringNULL(params[4], &keyName);
	if (keyName && !keyName[0]) {
		context->ThrowNativeError("Key name cannot be NULL or empty");
		return;
	}

	if (keyName && numElements > 1) {
		context->ThrowNativeError("Key field cannot be an array");
		return;
	}

	if (!!keyName) {
		factory->DefineKeyField(fieldName, fieldType, keyName);
	} else {
		factory->DefineField(fieldName, fieldType, numElements);
	}
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
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	char* fieldName;
	context->LocalToString(params[2], &fieldName);
	if (!fieldName || (fieldName && !fieldName[0])) {
		return context->ThrowNativeError("Field name cannot be NULL or empty");
	}

	int numElements = params[3];
	if (numElements <= 0) {
		return context->ThrowNativeError("Elements must be >= 1");
	}

	factory->DefineField(fieldName, FIELD_EHANDLE, numElements);
	return params[1];
}

cell_t DefineInputFunc(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1], false);
	if (!factory) {
		return 0;
	}

	char* keyName;
	context->LocalToString(params[2], &keyName);
	if (!keyName || (keyName && !keyName[0])) {
		return context->ThrowNativeError("Input name cannot be NULL or empty");
	}

	cell_t handlerType = params[3];
	IPluginFunction *handlerFunc = context->GetFunctionById(params[4]);

	fieldtype_t fieldType;
	switch (handlerType) {
		case 0: fieldType = FIELD_VOID; break;
		case 1: fieldType = FIELD_STRING; break;
		case 2: fieldType = FIELD_BOOLEAN; break;
		case 3: fieldType = FIELD_COLOR32; break;
		case 4: fieldType = FIELD_FLOAT; break;
		case 5:	fieldType = FIELD_INTEGER; break;
		case 6: fieldType = FIELD_VECTOR; break;
		default: fieldType = FIELD_CUSTOM; break;
	}

	int fieldNameSize = strlen(keyName) + 7;
	char* fieldName = new char[fieldNameSize];
	snprintf(fieldName, fieldNameSize, "Input%s", keyName);

	factory->DefineInputFunc(fieldName, fieldType, keyName, factory->CreateInputFuncDelegate(handlerFunc, fieldType) );

	delete[] fieldName;
	return params[1];
}

cell_t DefineOutput(IPluginContext* context, const cell_t* params) {
	auto factory = Get(context, params[1]);
	
	char* keyName;
	context->LocalToString(params[2], &keyName);
	if (!keyName || (keyName && !keyName[0])) {
		return context->ThrowNativeError("Output name cannot be NULL or empty.");
	}

	int fieldNameSize = strlen(keyName) + 7;
	char* fieldName = new char[fieldNameSize];
	snprintf(fieldName, fieldNameSize, "m_%s", keyName);

	factory->DefineOutput(fieldName, keyName);

	delete[] fieldName;
	return params[1];
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CEntityFactory.CEntityFactory", CPluginEntityFactory_Ctor},
		{"CEntityFactory.DeriveFromBaseEntity", DeriveFromBaseEntity},
		{"CEntityFactory.DeriveFromClass", DeriveFromClass},
		{"CEntityFactory.DeriveFromNPC", DeriveFromNPC},
		{"CEntityFactory.DeriveFromFactory", DeriveFromFactory},
		{"CEntityFactory.DeriveFromConf", DeriveFromConf},
		{"CEntityFactory.SetInitialActionFactory", SetInitialActionFactory},
		{"CEntityFactory.Install", Install},
		{"CEntityFactory.Uninstall", Uninstall},
		{"CEntityFactory.IsInstalled.get", GetIsInstalled},
		{"CEntityFactory.IsAbstract.get", GetIsAbstract},
		{"CEntityFactory.IsAbstract.set", SetIsAbstract},
		{"CEntityFactory.GetClassname", GetClassname},
		{"CEntityFactory.GetFactoryOfEntity", GetFactoryOfEntity},
		{"CEntityFactory.GetNumInstalledFactories", GetNumInstalledFactories},
		{"CEntityFactory.GetInstalledFactories", GetInstalledFactories},
		{"CEntityFactory.AttachNextBot", AttachNextBot},
		{"CEntityFactory.BeginDataMapDesc", BeginDataMapDesc},
		{"CEntityFactory.DefineIntField", DefineIntField},
		{"CEntityFactory.DefineFloatField", DefineFloatField},
		{"CEntityFactory.DefineCharField", DefineCharField},
		{"CEntityFactory.DefineBoolField", DefineBoolField},
		{"CEntityFactory.DefineVectorField", DefineVectorField},
		{"CEntityFactory.DefineStringField", DefineStringField},
		{"CEntityFactory.DefineColorField", DefineColorField},
		{"CEntityFactory.DefineEntityField", DefineEntityField},
		{"CEntityFactory.DefineInputFunc", DefineInputFunc},
		{"CEntityFactory.DefineOutput", DefineOutput},
		{"CEntityFactory.EndDataMapDesc", EndDataMapDesc},
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}