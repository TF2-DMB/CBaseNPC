
#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"
#include "cbasenpc_internal.h"

SourceMod::IForward * g_fwdInstalledFactory;
SourceMod::IForward * g_fwdUninstalledFactory;

CUtlVector<CPluginEntityFactory*>* g_PluginEntityFactories;

bool CPluginEntityFactory::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	g_PluginEntityFactories = new CUtlVector<CPluginEntityFactory*>;

	g_fwdInstalledFactory = forwards->CreateForward("CEntityFactory_OnInstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);
	g_fwdUninstalledFactory = forwards->CreateForward("CEntityFactory_OnUninstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);

	return true;
}

void CPluginEntityFactory::OnFactoryInstall(CPluginEntityFactory * pFactory)
{
	g_fwdInstalledFactory->PushString(pFactory->m_iClassname.c_str());
	g_fwdInstalledFactory->PushCell((cell_t)pFactory);
	g_fwdInstalledFactory->Execute();
}

void CPluginEntityFactory::OnFactoryUninstall(CPluginEntityFactory * pFactory)
{
	g_fwdUninstalledFactory->PushString(pFactory->m_iClassname.c_str());
	g_fwdUninstalledFactory->PushCell((cell_t)pFactory);
	g_fwdUninstalledFactory->Execute();
}

CPluginEntityFactory::CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor)
{
	m_Derive.m_DeriveFrom = NONE;
	m_iClassname = classname;
	m_pConstructor = postConstructor;
	m_bInstalled = false;

	m_pBasePluginEntityFactory = nullptr;

	g_PluginEntityFactories->AddToTail(this);
}

CPluginEntityFactory::~CPluginEntityFactory()
{
	Uninstall();

	g_PluginEntityFactories->FindAndRemove(this);
}

void CPluginEntityFactory::SetBasePluginEntityFactory(CPluginEntityFactory* pFactory)
{
	if (pFactory == m_pBasePluginEntityFactory)
		return;
	
	if (m_pBasePluginEntityFactory)
	{
		m_pBasePluginEntityFactory->m_ChildFactories.FindAndRemove(this);
		m_pBasePluginEntityFactory = nullptr;
	}

	if (pFactory)
	{
		m_pBasePluginEntityFactory = pFactory;
		pFactory->m_ChildFactories.AddToTail(this);
	}
}

void CPluginEntityFactory::Install()
{
	if (m_bInstalled)
		return;

	CEntityFactoryDictionaryHack* pFactoryDict = EntityFactoryDictionaryHack();
	if (!pFactoryDict) 
		return;

	const char* classname = m_iClassname.c_str();

	if (pFactoryDict->FindFactory(classname))
		return;
	
	// DO NOT USE IEntityFactoryDictionary::InstallFactory()!
	// It's a virtual function, which means memory allocation is handled by server.dll, not by this extension.
	// If you use it anyway and you try to uninstall, it will result in heap corruption error. (at least on Windows)
	pFactoryDict->m_Factories.Insert( classname, this );

	m_bInstalled = true;

	CPluginEntityFactory* pBaseFactory = reinterpret_cast<CPluginEntityFactory*>(GetBaseFactory());
	if (g_PluginEntityFactories->HasElement(pBaseFactory))
	{
		SetBasePluginEntityFactory( pBaseFactory );
	}
	else 
	{
		pBaseFactory = nullptr;
	}

	OnFactoryInstall(this);

	g_pSM->LogMessage(myself, "Installed %s entity factory", classname);
}

void CPluginEntityFactory::Uninstall()
{
	if (!m_bInstalled)
		return;

	CEntityFactoryDictionaryHack* pFactoryDict = EntityFactoryDictionaryHack();
	if (!pFactoryDict)
		return;

	const char* classname = m_iClassname.c_str();

	for (size_t i = 0; i < pFactoryDict->m_Factories.Count(); i++)
	{
		if (pFactoryDict->m_Factories[i] == this)
		{
			pFactoryDict->m_Factories.RemoveAt(i);
			break;
		}
	}

	m_bInstalled = false;

	// Uninstall children first.
	for (int i = m_ChildFactories.Count() - 1; i >= 0; i--)
	{
		CPluginEntityFactory* pChild = m_ChildFactories[i];
		if (!pChild)
			continue;
		
		pChild->Uninstall();
	}

	SetBasePluginEntityFactory(NULL);

	OnFactoryUninstall(this);

	g_pSM->LogMessage(myself, "Uninstalled %s entity factory", classname);
}

IEntityFactory* CPluginEntityFactory::GetBaseFactory()
{
	if (m_Derive.m_DeriveFrom == NONE)
	{
		return nullptr;
	}
	else if (m_Derive.m_DeriveFrom == BASECLASS)
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
	else if (m_Derive.m_DeriveFrom == BASECLASS && m_Derive.m_BaseType == ENTITY)
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
	else if (m_Derive.m_DeriveFrom == BASECLASS && m_Derive.m_BaseType == ENTITY)
	{
		// TODO: A barebones CBaseEntity maybe???
		/*
		pEnt = (CBaseEntityHack*)engine->PvAllocEntPrivateData(this->GetEntitySize());
		CBaseEntityHack::CBaseEntity_Ctor.operator()(pEnt, m_Derive.m_bBaseEntityServerOnly);
		pEnt->PostConstructor(classname);
		*/
		
		return nullptr;
	}
	else
	{
		return nullptr;
	}

	if (m_pConstructor && m_pConstructor->IsRunnable())
	{
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