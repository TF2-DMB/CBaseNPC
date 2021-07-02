
#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"
#include "cbasenpc_internal.h"

CPluginEntityFactory::CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor)
{
	m_Derive.m_DeriveFrom = NONE;
	m_iClassname = classname;
	m_pConstructor = postConstructor;
	m_bInstalled = false;
}

CPluginEntityFactory::~CPluginEntityFactory()
{
	Uninstall();
}

void CPluginEntityFactory::Install()
{
	if (m_bInstalled)
		return;

	CEntityFactoryDictionaryHack* pFactoryDict = EntityFactoryDictionaryHack();
	if (pFactoryDict) 
	{
		const char* classname = m_iClassname.c_str();
		pFactoryDict->InstallFactory(this, classname);

		m_bInstalled = true;

		g_pSM->LogMessage(myself, "Installed %s entity factory", classname);
	}
}

void CPluginEntityFactory::Uninstall()
{
	if (!m_bInstalled)
		return;

	CEntityFactoryDictionaryHack* pFactoryDict = EntityFactoryDictionaryHack();
	if (pFactoryDict)
	{
		const char* classname = m_iClassname.c_str();

		pFactoryDict->UninstallFactory(this);

		m_bInstalled = false;

		g_pSM->LogMessage(myself, "Uninstalled %s entity factory", classname);
	}
}

IEntityFactory* CPluginEntityFactory::GetBaseFactory()
{
	if (m_Derive.m_DeriveFrom == NONE)
	{
		return nullptr;
	}
	else if (m_Derive.m_DeriveFrom == BASE)
	{
		switch (m_Derive.m_BaseType)
		{
			case ENTITY:
				return nullptr;

			case NPC:
				return g_pBaseNPCFactory;
		}
	}
	else if (m_Derive.m_DeriveFrom == CLASSNAME)
	{
		return EntityFactoryDictionaryHack()->FindFactory(m_Derive.m_iBaseClassname.c_str());
	}

	return nullptr;
}

size_t CPluginEntityFactory::GetEntitySize()
{
	IEntityFactory *factory = GetBaseFactory();
	if (factory)
	{
		return factory->GetEntitySize();
	}
	else if (m_Derive.m_DeriveFrom == BASE && m_Derive.m_BaseType == ENTITY)
	{
		// TODO: CBaseEntity size?
		return 0;
	}

	return 0;
}

IServerNetworkable* CPluginEntityFactory::Create(const char* classname)
{
	CBaseEntityHack* pEnt;

	IEntityFactory *factory = GetBaseFactory();
	if (factory)
	{
		IServerNetworkable * pNet = factory->Create(classname);
		if (!pNet)
			return nullptr;
		
		pEnt = (CBaseEntityHack*)pNet->GetBaseEntity();
	}
	else if (m_Derive.m_DeriveFrom == BASE && m_Derive.m_BaseType == ENTITY)
	{
		// TODO: A barebones CBaseEntity maybe???
		/*
		pEnt = (CBaseEntityHack*)engine->PvAllocEntPrivateData(this->GetEntitySize());
		CBaseEntityHack::CBaseEntity_Ctor.operator()(pEnt);
		pEnt->PostConstructor(classname);
		*/
		
		return nullptr;
	}

	if (m_pConstructor && m_pConstructor->IsRunnable())
	{
		IPluginContext* pContext = m_pConstructor->GetParentRuntime()->GetDefaultContext();
		m_pConstructor->PushCell(pEnt->entindex());
		m_pConstructor->Execute(0);
	}

	return pEnt->NetworkProp();
}

void CPluginEntityFactory::Destroy(IServerNetworkable* pNetworkable)
{
	if (pNetworkable)
	{
		pNetworkable->Release();
	}
}

HandleType_t g_PluginEntityFactoryHandle;
CPluginEntityFactoryHandler g_PluginEntityFactoryHandler;

void CPluginEntityFactoryHandler::OnHandleDestroy(HandleType_t type, void * object)
{
	CPluginEntityFactory* factory = (CPluginEntityFactory*)object;

	delete factory;
}