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

cell_t CPluginEntityFactory_CPluginEntityFactory(IPluginContext * pContext, const cell_t * params)
{
	char* classname;
	pContext->LocalToString(params[1], &classname);

	if (!classname || !strlen(classname))
		return pContext->ThrowNativeError("Entity factory must have a classname");

	IPluginFunction *postConstructor = pContext->GetFunctionById(params[2]);
	IPluginFunction *onRemove = pContext->GetFunctionById(params[3]);

	CPluginEntityFactory* pFactory = new CPluginEntityFactory(classname, postConstructor, onRemove);
	return CREATEHANDLE(PluginEntityFactory, pFactory);
}

PLUGINENTITYFACTORYNATIVE(DeriveFromBaseEntity)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");

	pFactory->m_Derive.m_DeriveFrom = BASECLASS;
	pFactory->m_Derive.m_BaseType = ENTITY;
	pFactory->m_Derive.m_bBaseEntityServerOnly = !!params[2];

	return 1;
}

PLUGINENTITYFACTORYNATIVE(DeriveFromNPC)

	if (pFactory->m_bInstalled) 
		return pContext->ThrowNativeError("Cannot change base factory while factory is installed");

	pFactory->m_Derive.m_DeriveFrom = BASECLASS;
	pFactory->m_Derive.m_BaseType = NPC;

	return 1;
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

	return 1;
}

PLUGINENTITYFACTORYNATIVE(Install)

	if (pFactory->m_bInstalled) 
		return 1;
	
	if (EntityFactoryDictionaryHack()->FindFactory(pFactory->m_iClassname.c_str()))
		return pContext->ThrowNativeError("Entity factory already exists with the same classname");

	if (pFactory->m_Derive.m_DeriveFrom == NONE)
		return pContext->ThrowNativeError("Entity factory must derive from an existing class");

	pFactory->Install();
	return 1;
}

PLUGINENTITYFACTORYNATIVE(Uninstall)

	pFactory->Uninstall();
	return 1;
}

PLUGINENTITYFACTORYNATIVE(Installed)

	return pFactory->m_bInstalled;
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
	return 1;
}

PLUGINENTITYFACTORYDATAMAPNATIVE(DefineField)

	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);

	fieldtype_t fieldType = (fieldtype_t)(params[3]);

	pFactory->DefineField(fieldName, fieldType);
	return params[1];
}

PLUGINENTITYFACTORYDATAMAPNATIVE(DefineKeyField)

	char* fieldName;
	pContext->LocalToString(params[2], &fieldName);

	fieldtype_t fieldType = (fieldtype_t)(params[3]);

	char* mapName;
	pContext->LocalToString(params[4], &mapName);

	pFactory->DefineKeyField(fieldName, fieldType, mapName);
	return params[1];
}

#endif