#ifndef NATIVE_CPLUGINENTITYFACTORY_H_
#define NATIVE_CPLUGINENTITYFACTORY_H_
#pragma once

#include "helpers.h"
#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"

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
		return pContext->ThrowNativeError("Cannot edit entity datamap while factory is installed"); \
	if (!pFactory->CanUseDataMap()) \
		return pContext->ThrowNativeError("This factory cannot use a custom datamap");

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

	IPluginFunction *postConstructor = pContext->GetFunctionById(params[2]);
	IPluginFunction *onRemove = pContext->GetFunctionById(params[3]);

	CPluginEntityFactory* pFactory = new CPluginEntityFactory(classname, postConstructor, onRemove);

	Handle_t handle = CREATEHANDLE(PluginEntityFactory, pFactory);
	pFactory->m_Handle = handle;

	return handle;
}

cell_t CPluginEntityFactory_GetFactoryOfEntity(IPluginContext * pContext, const cell_t * params)
{
	CBaseEntity* pEnt;
	ENTINDEX_TO_CBASEENTITY(params[1], pEnt);
	if (!pEnt)
		return 0;
	
	CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEnt);
	if (!pFactory)
		return 0;
	
	return pFactory->m_Handle;
}

PLUGINENTITYFACTORYNATIVE(DeriveFromBaseEntity)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");

	pFactory->m_Derive.m_DeriveFrom = BASECLASS;
	pFactory->m_Derive.m_BaseType = ENTITY;
	pFactory->m_Derive.m_bBaseEntityServerOnly = !!params[2];

	return 0;
}

PLUGINENTITYFACTORYNATIVE(DeriveFromNPC)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");

	pFactory->m_Derive.m_DeriveFrom = BASECLASS;
	pFactory->m_Derive.m_BaseType = NPC;

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
	
	if (pDeriveFromFactory == pFactory)
		return pContext->ThrowNativeError("Cannot derive from self");
	
	pFactory->m_Derive.m_DeriveFrom = CLASSNAME;
	pFactory->m_Derive.m_iBaseClassname = classname;

	return 0;
}

PLUGINENTITYFACTORYNATIVE(Install)

	if (pFactory->m_bInstalled) 
		return 0;
	
	if (EntityFactoryDictionaryHack()->FindFactory(pFactory->m_iClassname.c_str()))
		return pContext->ThrowNativeError("Entity factory already exists with the same classname");

	if (pFactory->m_Derive.m_DeriveFrom == NONE)
		return pContext->ThrowNativeError("Entity factory must derive from an existing class");

	pFactory->Install();
	return 0;
}

PLUGINENTITYFACTORYNATIVE(Uninstall)

	pFactory->Uninstall();
	return 0;
}

PLUGINENTITYFACTORYNATIVE(Installed)

	return pFactory->m_bInstalled;
}

PLUGINENTITYFACTORYDATAMAPNATIVE(GetClassname)

	size_t bufferSize = params[3];

	pContext->StringToLocal(params[2], bufferSize, pFactory->m_iClassname.c_str());
	return 0;
}

PLUGINENTITYFACTORYDATAMAPNATIVE(BeginDataMapDesc)

	char* dataClassName;
	pContext->LocalToStringNULL(params[2], &dataClassName);
	if (!dataClassName)
	{
		pFactory->BeginDataMapDesc(pFactory->m_iClassname.c_str());
	}
	else 
	{
		pFactory->BeginDataMapDesc(dataClassName);
	}

	return params[1];
}

PLUGINENTITYFACTORYDATAMAPNATIVE(EndDataMapDesc)

	pFactory->EndDataMapDesc();
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

PLUGINENTITYFACTORYDATAMAP_DECLFIELDNATIVE(DefineEntityField)

	pFactory->DefineField(fieldName, FIELD_EHANDLE, numElements);

	return params[1];
}

#endif