#ifndef NATIVE_CPLUGINENTITYFACTORY_H_
#define NATIVE_CPLUGINENTITYFACTORY_H_
#pragma once

#include "helpers.h"
#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"
#include "cbasenpc_behavior.h"

#define ENTINDEX_TO_CBASEENTITY(ref, buffer) \
	buffer = gamehelpers->ReferenceToEntity(ref); \
	if (!buffer) \
	{ \
		return pContext->ThrowNativeError("Entity %d (%d) is not a CBaseEntity", gamehelpers->ReferenceToIndex(ref), ref); \
	}

#define PLUGINENTITYFACTORYNATIVE(name) \
	cell_t CPluginEntityFactory_##name(IPluginContext *pContext, const cell_t *params) \
	{ \
		HandleSecurity security; \
		security.pOwner = NULL; \
		security.pIdentity = myself->GetIdentity(); \
		Handle_t hndlObject = static_cast<Handle_t>(params[1]); \
		CPluginEntityFactory *pFactory = nullptr; \
		READHANDLE(hndlObject, PluginEntityFactory, pFactory) \

#define PLUGINENTITYFACTORYDATAMAPNATIVE(name) \
PLUGINENTITYFACTORYNATIVE(name) \
	if (pFactory->m_bInstalled) \
		return pContext->ThrowNativeError("Cannot edit entity datamap while factory is installed");

// the naming is getting pretty wild ngl
#define PLUGINENTITYFACTORYDATAMAP_DECLFIELDNATIVE(name) \
PLUGINENTITYFACTORYDATAMAPNATIVE(name) \
	char* fieldName; \
	pContext->LocalToString(params[2], &fieldName); \
	if (!fieldName || (fieldName && !fieldName[0])) return pContext->ThrowNativeError("Field name cannot be NULL or empty"); \
	int numElements = params[3]; \
	if (numElements <= 0) return pContext->ThrowNativeError("Elements must be >= 1");

#define PLUGINENTITYFACTORYDATAMAP_DECLKEYFIELDNATIVE(name) \
PLUGINENTITYFACTORYDATAMAP_DECLFIELDNATIVE(name) \
	char* keyName; \
	pContext->LocalToStringNULL(params[4], &keyName); \
	if (keyName && !keyName[0]) return pContext->ThrowNativeError("Key name cannot be NULL or empty"); \
	bool isKeyField = !!keyName; \
	if (isKeyField && numElements > 1) return pContext->ThrowNativeError("Key field cannot be an array");

cell_t CPluginEntityFactory_CPluginEntityFactory(IPluginContext * pContext, const cell_t * params)
{
	char* classname;
	pContext->LocalToString(params[1], &classname);

	if (!classname || !strlen(classname))
		return pContext->ThrowNativeError("Entity factory must have a classname");

	IPlugin* plugin = plsys->FindPluginByContext( pContext->GetContext() );
	IPluginFunction *postConstructor = pContext->GetFunctionById(params[2]);
	IPluginFunction *onRemove = pContext->GetFunctionById(params[3]);

	CPluginEntityFactory* pFactory = new CPluginEntityFactory(plugin, classname, postConstructor, onRemove);
	return pFactory->m_Handle;
}

cell_t CPluginEntityFactory_GetFactoryOfEntity(IPluginContext * pContext, const cell_t * params)
{
	CBaseEntity* pEnt;
	ENTINDEX_TO_CBASEENTITY(params[1], pEnt);
	
	CPluginEntityFactory* pFactory = g_pPluginEntityFactories->GetFactory(pEnt);
	if (!pFactory)
		return BAD_HANDLE;
	
	return pFactory->m_Handle;
}

cell_t CPluginEntityFactory_GetNumInstalledFactories(IPluginContext * pContext, const cell_t * params)
{
	return g_pPluginEntityFactories->GetInstalledFactoryHandles(nullptr, 0);
}

cell_t CPluginEntityFactory_GetInstalledFactories(IPluginContext * pContext, const cell_t * params)
{
	cell_t * addr;
	pContext->LocalToPhysAddr(params[1], &addr);
	Handle_t * pArray = reinterpret_cast<Handle_t *>(addr);

	int arraySize = params[2];

	return g_pPluginEntityFactories->GetInstalledFactoryHandles(pArray, arraySize);
}

PLUGINENTITYFACTORYNATIVE(DeriveFromBaseEntity)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");

	pFactory->DeriveFromBaseEntity(!!params[2]);

	return 0;
}

PLUGINENTITYFACTORYNATIVE(DeriveFromNPC)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");

	pFactory->DeriveFromNPC();

	return 0;
}

PLUGINENTITYFACTORYNATIVE(SetInitialActionFactory)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change the initial action factory while factory is installed");

	hndlObject = static_cast<Handle_t>(params[2]);
	CBaseNPCPluginActionFactory *pActionFactory = nullptr;
	if (hndlObject != BAD_HANDLE)
	{
		pActionFactory = g_pBaseNPCPluginActionFactories->GetFactoryFromHandle( hndlObject );
	}

	pFactory->SetBaseNPCInitialActionFactory( pActionFactory );
	return 0;
}

PLUGINENTITYFACTORYNATIVE(DeriveFromClass)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");

	char* classname;
	pContext->LocalToString(params[2], &classname);

	IEntityFactory* pDeriveFromFactory = EntityFactoryDictionaryHack()->FindFactory(classname);
	if (!pDeriveFromFactory)
		return pContext->ThrowNativeError("Cannot derive from uninstalled entity factory %s", classname);
	
	pFactory->DeriveFromClass(classname);

	return 0;
}

PLUGINENTITYFACTORYNATIVE(DeriveFromFactory)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");
	
	Handle_t otherHndl = params[2];

	CPluginEntityFactory* pOtherFactory = g_pPluginEntityFactories->GetFactoryFromHandle( otherHndl, &chnderr );
	if ( !pOtherFactory )
		return pContext->ThrowNativeError("Invalid handle");

	if (pOtherFactory == pFactory)
		return pContext->ThrowNativeError("Cannot derive from self");

	pFactory->DeriveFromHandle( otherHndl );

	return 0;
}

PLUGINENTITYFACTORYNATIVE(DeriveFromConf)
	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");
	
	size_t entitySize = params[2];

	IGameConfig* config = gameconfs->ReadHandle( params[3], pContext->GetIdentity(), nullptr );
	if ( !config )
		return pContext->ThrowNativeError("Invalid gameconfig handle");

	int type = params[4];

	char* entry;
	pContext->LocalToString( params[5], &entry );

	if ( !pFactory->DeriveFromConf( entitySize, config, type, entry ) )
	{
		return pContext->ThrowNativeError("Failed to obtain constructor function from gamedata entry %s", entry);
	}

	return 0;
}

PLUGINENTITYFACTORYNATIVE(Install)

	if (pFactory->m_bInstalled) 
		return 0;
	
	if (!pFactory->IsAbstract())
	{
		const char* classname = pFactory->m_iClassname.c_str();

		if (EntityFactoryDictionaryHack()->FindFactory(classname))
			return pContext->ThrowNativeError("Entity factory already exists with the same classname");
	}

	if (pFactory->DoesNotDerive())
		return pContext->ThrowNativeError("Entity factory must derive from an existing class or classname");

	if (!pFactory->Install())
		return pContext->ThrowNativeError("Failed to install; make sure base factory is installed first");
	
	return 0;
}

PLUGINENTITYFACTORYNATIVE(Uninstall)

	pFactory->Uninstall();
	return 0;
}

PLUGINENTITYFACTORYNATIVE(IsInstalledGet)

	return pFactory->m_bInstalled;
}

PLUGINENTITYFACTORYNATIVE(IsAbstractGet)

	return pFactory->IsAbstract();
}

PLUGINENTITYFACTORYNATIVE(IsAbstractSet)

	if (pFactory->m_bInstalled)
		return pContext->ThrowNativeError("Cannot change IsAbstract while factory is installed");

	pFactory->SetAbstract(!!params[2]);
	return 0;
}

PLUGINENTITYFACTORYNATIVE(GetClassname)

	size_t bufferSize = params[3];

	pContext->StringToLocal(params[2], bufferSize, pFactory->m_iClassname.c_str());
	return 0;
}

PLUGINENTITYFACTORYDATAMAPNATIVE(BeginDataMapDesc)

	char* dataClassName;
	pContext->LocalToStringNULL(params[2], &dataClassName);
	bool startedDesc;
	if (!dataClassName)
	{
		startedDesc = pFactory->BeginDataDesc(pFactory->m_iClassname.c_str());
	}
	else 
	{
		startedDesc = pFactory->BeginDataDesc(dataClassName);
	}

	if (!startedDesc)
		return pContext->ThrowNativeError("Base factory was not installed before datamap definition");

	return params[1];
}

PLUGINENTITYFACTORYDATAMAPNATIVE(EndDataMapDesc)

	pFactory->EndDataDesc();
	return 0;
}

PLUGINENTITYFACTORYDATAMAP_DECLKEYFIELDNATIVE(DefineIntField)

	if (isKeyField)
		pFactory->DefineKeyField(fieldName, FIELD_INTEGER, keyName);
	else
		pFactory->DefineField(fieldName, FIELD_INTEGER, numElements);
	
	return params[1];
}

PLUGINENTITYFACTORYDATAMAP_DECLKEYFIELDNATIVE(DefineFloatField)

	if (isKeyField)
		pFactory->DefineKeyField(fieldName, FIELD_FLOAT, keyName);
	else
		pFactory->DefineField(fieldName, FIELD_FLOAT, numElements);
	
	return params[1];
}

PLUGINENTITYFACTORYDATAMAP_DECLKEYFIELDNATIVE(DefineCharField)

	if (isKeyField)
		pFactory->DefineKeyField(fieldName, FIELD_CHARACTER, keyName);
	else
		pFactory->DefineField(fieldName, FIELD_CHARACTER, numElements);
	
	return params[1];
}

PLUGINENTITYFACTORYDATAMAP_DECLKEYFIELDNATIVE(DefineBoolField)

	if (isKeyField)
		pFactory->DefineKeyField(fieldName, FIELD_BOOLEAN, keyName);
	else
		pFactory->DefineField(fieldName, FIELD_BOOLEAN, numElements);
	
	return params[1];
}

PLUGINENTITYFACTORYDATAMAP_DECLKEYFIELDNATIVE(DefineVectorField)

	if (isKeyField)
		pFactory->DefineKeyField(fieldName, FIELD_VECTOR, keyName);
	else
		pFactory->DefineField(fieldName, FIELD_VECTOR, numElements);
	
	return params[1];
}

PLUGINENTITYFACTORYDATAMAP_DECLKEYFIELDNATIVE(DefineStringField)

	if (isKeyField)
		pFactory->DefineKeyField(fieldName, FIELD_STRING, keyName);
	else
		pFactory->DefineField(fieldName, FIELD_STRING, numElements);
	
	return params[1];
}

PLUGINENTITYFACTORYDATAMAP_DECLKEYFIELDNATIVE(DefineColorField)

	if (isKeyField)
		pFactory->DefineKeyField(fieldName, FIELD_COLOR32, keyName);
	else
		pFactory->DefineField(fieldName, FIELD_COLOR32, numElements);
	
	return params[1];
}

PLUGINENTITYFACTORYDATAMAP_DECLFIELDNATIVE(DefineEntityField)

	pFactory->DefineField(fieldName, FIELD_EHANDLE, numElements);

	return params[1];
}

PLUGINENTITYFACTORYDATAMAPNATIVE(DefineInputFunc)

	char* keyName;
	pContext->LocalToString(params[2], &keyName);
	if (!keyName || (keyName && !keyName[0])) return pContext->ThrowNativeError("Input name cannot be NULL or empty");

	cell_t handlerType = params[3];
	IPluginFunction *handlerFunc = pContext->GetFunctionById(params[4]);

	fieldtype_t fieldType;
	switch (handlerType)
	{
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

	pFactory->DefineInputFunc(fieldName, fieldType, keyName, pFactory->CreateInputFuncDelegate(handlerFunc) );

	delete[] fieldName;
	return params[1];
}

PLUGINENTITYFACTORYDATAMAPNATIVE(DefineOutput)

	char* keyName;
	pContext->LocalToString(params[2], &keyName);
	if (!keyName || (keyName && !keyName[0])) return pContext->ThrowNativeError("Output name cannot be NULL or empty");

	int fieldNameSize = strlen(keyName) + 7;
	char* fieldName = new char[fieldNameSize];
	snprintf(fieldName, fieldNameSize, "m_%s", keyName);

	pFactory->DefineOutput(fieldName, keyName);

	delete[] fieldName;
	return params[1];
}

#endif