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

cell_t CPluginEntityFactory_CPluginEntityFactory(IPluginContext * pContext, const cell_t * params)
{
    char* classname;
	pContext->LocalToString(params[1], &classname);

    if (!classname || !strlen(classname))
        return pContext->ThrowNativeError("Entity factory must have a classname");

    IPluginFunction *postConstructor = pContext->GetFunctionById(params[2]);

    CPluginEntityFactory* pFactory = new CPluginEntityFactory(classname, postConstructor);
    return CREATEHANDLE(PluginEntityFactory, pFactory);
}

PLUGINENTITYFACTORYNATIVE(DeriveFromNPC)

    pFactory->m_Derive.m_DeriveFrom = BASECLASS;
    pFactory->m_Derive.m_BaseType = NPC;

    return 1;
}

PLUGINENTITYFACTORYNATIVE(DeriveFromClass)

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

#endif