#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"
#include "cbasenpc_internal.h"
#include "cbasenpc_behavior.h"
#include "baseentityoutput.h"
#include "sh_pagealloc.h"

SH_DECL_MANUALHOOK0(FactoryEntity_GetDataDescMap, 0, 0, 0, datamap_t* );
SH_DECL_MANUALHOOK0_void(FactoryEntity_UpdateOnRemove, 0, 0, 0 );

SH_DECL_MANUALHOOK0(EntityRecord_MyNextBotPointer, 0, 0, 0, INextBot* );
SH_DECL_EXTERN0(INextBot, GetIntentionInterface, const, 0, IIntention* );

SH_DECL_HOOK1(IEntityFactoryDictionary, Create, SH_NOATTRIB, 0, IServerNetworkable*, const char*);
SH_DECL_HOOK2_void(IEntityFactoryDictionary, Destroy, SH_NOATTRIB, 0, const char*, IServerNetworkable*);
SH_DECL_HOOK1(IEntityFactoryDictionary, GetCannonicalName, SH_NOATTRIB, 0, const char*, const char*);

#ifdef __linux__
SH_DECL_MANUALHOOK0_void(FactoryEntity_Dtor, 1, 0, 0);
#else
SH_DECL_MANUALHOOK1_void(FactoryEntity_Dtor, 0, 0, 0, unsigned int);
#endif

// must match sdktools.inc
enum SDKFuncConfSource
{
	SDKConf_Virtual,
	SDKConf_Signature,
	SDKConf_Address
};

SH_DECL_HOOK1(IVEngineServer, PvAllocEntPrivateData, SH_NOATTRIB, false, void *, long);

HandleType_t g_PluginEntityFactoryHandle;

CPluginEntityFactories* g_pPluginEntityFactories = new CPluginEntityFactories();

class EntityMemAllocHook_t
{
private:
	long m_iBaseEntitySize;
	long m_iRequestedEntitySize;

	bool m_bHooked;

	bool m_bWatching;
	bool m_bHandledAllocation;

	int m_iHookID;

public:
	void StartWatching(long oldSize, long newSize) 
	{
		m_bWatching = true;
		m_bHandledAllocation = false;
		m_iBaseEntitySize = oldSize;
		m_iRequestedEntitySize = newSize;
	}

	void StopWatching()
	{
		m_bWatching = false;
	}

	bool DidHandleAlloc() { return m_bHandledAllocation; }

	void* PvAllocEntPrivateData(long cb)
	{
		if (m_bWatching && cb == m_iBaseEntitySize)
		{
			m_bWatching = false;
			m_bHandledAllocation = true;
			RETURN_META_VALUE_NEWPARAMS(MRES_IGNORED, nullptr, &IVEngineServer::PvAllocEntPrivateData, (m_iRequestedEntitySize) );
		}

		RETURN_META_VALUE(MRES_IGNORED, nullptr);
	}

	void Hook()
	{
		if (m_bHooked)
			return;

		m_iHookID = SH_ADD_HOOK(IVEngineServer, PvAllocEntPrivateData, engine, SH_MEMBER(this, &EntityMemAllocHook_t::PvAllocEntPrivateData), false);
		m_bHooked = true;
	}

	void Unhook()
	{
		if (!m_bHooked)
			return;

		SH_REMOVE_HOOK_ID(m_iHookID);
		m_bHooked = false;
	}

	void Init()
	{
		Hook();
	}

	void Shutdown()
	{
		Unhook();
	}

} g_EntityMemAllocHook;

CPluginEntityFactories::CPluginEntityFactories()
{
}

PluginFactoryEntityRecord_t* CPluginEntityFactories::FindRecord(CBaseEntity* pEntity, bool create)
{
	if (!pEntity)
	{
		return nullptr;
	}
	
	cell_t key = (cell_t)pEntity;
	if (m_Records.find(key) == m_Records.end())
	{
		if (create)
		{
			m_Records.emplace(key, std::make_unique<PluginFactoryEntityRecord_t>(pEntity));
		}
		else
		{
			return nullptr;
		}
	}

	return m_Records[key].get();
}

void CPluginEntityFactories::RemoveRecord(CBaseEntity* pEntity)
{
	if (!pEntity)
	{
		return;
	}

	cell_t key = (cell_t)pEntity;
	m_Records.erase(key);
}

CPluginEntityFactory* CPluginEntityFactories::GetFactory(CBaseEntity* pEntity)
{
	PluginFactoryEntityRecord_t* pEntityRecord = FindRecord(pEntity);
	if (!pEntityRecord)
	{
		return nullptr;
	}

	return pEntityRecord->pFactory;
}

CPluginEntityFactory* CPluginEntityFactories::GetFactoryFromHandle( Handle_t handle, HandleError *err )
{
	CPluginEntityFactory* pFactory;
	HandleError _err;
	HandleSecurity security( nullptr, myself->GetIdentity() );
	if ( ( _err = handlesys->ReadHandle(handle, m_FactoryType, &security, (void **)&pFactory) ) != HandleError_None )
	{
		pFactory = nullptr;
	}

	if (err)
		*err = _err;

	return pFactory;
}

bool CPluginEntityFactories::Init( IGameConfig* config, char* error, size_t maxlength )
{
	CEntityFactoryDictionaryHack* factoryDictionary = EntityFactoryDictionaryHack();
	{
		IEntityFactory* factory = nullptr;

		// CBaseEntity
		int entitySize = 0;
		factory = factoryDictionary->FindFactory("info_target");
		
		if (factory) entitySize = factory->GetEntitySize();

		if (!entitySize)
		{
			snprintf(error, maxlength, "Failed to get size of CBaseEntity");
			return false;
		}

		m_BaseClassSizes[ FACTORYBASECLASS_BASEENTITY ] = entitySize;

		for (unsigned int max = factoryDictionary->m_Factories.Count(), i = 0; i < max; i++)
		{
			m_gameFactories.emplace(factoryDictionary->m_Factories.GetElementName(i), factoryDictionary->m_Factories.Element(i));
		}
		m_hookIds.push_back(SH_ADD_HOOK(IEntityFactoryDictionary, Create, factoryDictionary, SH_MEMBER(this, &CPluginEntityFactories::Hook_Create), false));
		m_hookIds.push_back(SH_ADD_HOOK(IEntityFactoryDictionary, Destroy, factoryDictionary, SH_MEMBER(this, &CPluginEntityFactories::Hook_Destroy), false));
		m_hookIds.push_back(SH_ADD_HOOK(IEntityFactoryDictionary, GetCannonicalName, factoryDictionary, SH_MEMBER(this, &CPluginEntityFactories::Hook_GetCannonicalName), false));
	}

	m_FactoryType = g_PluginEntityFactoryHandle = handlesys->CreateType( "PluginEntityFactory", this, 0, nullptr, nullptr, myself->GetIdentity(), nullptr );
	if ( !m_FactoryType )
	{
		snprintf( error, maxlength, "Failed to register PluginEntityFactory handle type" );
		return false;
	}

	m_fwdInstalledFactory = forwards->CreateForward("CEntityFactory_OnInstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);
	m_fwdUninstalledFactory = forwards->CreateForward("CEntityFactory_OnUninstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);

	plsys->AddPluginsListener( this );

	g_EntityMemAllocHook.Init();

	return true;
}

IServerNetworkable* CPluginEntityFactories::Hook_Create(const char* classname)
{
	auto factory = this->FindFactory(classname);
	if (factory)
	{
		RETURN_META_VALUE(MRES_SUPERCEDE, factory->Create(classname));
	}
	RETURN_META_VALUE(MRES_SUPERCEDE, nullptr);
}

void CPluginEntityFactories::Hook_Destroy(const char* classname, IServerNetworkable* pNetworkable)
{
	auto factory = this->FindFactory(classname);
	if (factory)
	{
		RETURN_META_VALUE(MRES_SUPERCEDE, factory->Destroy(pNetworkable));
	}
	RETURN_META(MRES_SUPERCEDE);
}

const char* CPluginEntityFactories::Hook_GetCannonicalName(const char* classname)
{
	RETURN_META_VALUE(MRES_SUPERCEDE, classname);
}

CPluginEntityFactory* CPluginEntityFactories::FindPluginFactory(const char* classname)
{
	auto itPluginFactory = m_pluginFactories.find(classname);
	if (itPluginFactory != m_pluginFactories.end())
	{
		return itPluginFactory->second;
	}
	return nullptr;
}

IEntityFactory* CPluginEntityFactories::FindGameFactory(const char* classname)
{
	auto itGameFactory = m_gameFactories.find(classname);
	if (itGameFactory != m_gameFactories.end())
	{
		return itGameFactory->second;
	}
	return nullptr;
}

void CPluginEntityFactories::InstallPluginFactory(const char* classname, CPluginEntityFactory* factory)
{
	if (m_pluginFactories.find(classname) == m_pluginFactories.end())
	{
		m_pluginFactories.emplace(classname, factory);
	}
}

void CPluginEntityFactories::RemovePluginFactory(CPluginEntityFactory* factory)
{
	auto it = m_pluginFactories.begin();
	while (it != m_pluginFactories.end())
	{
		if (it->second == factory)
		{
			it = m_pluginFactories.erase(it);
		}
		else
		{
			it++;
		}
	}
}

IEntityFactory* CPluginEntityFactories::FindFactory(const char* classname)
{
	auto itPluginFactory = m_pluginFactories.find(classname);
	if (itPluginFactory != m_pluginFactories.end())
	{
		return itPluginFactory->second;
	}

	auto itGameFactory = m_gameFactories.find(classname);
	if (itGameFactory != m_gameFactories.end())
	{
		return itGameFactory->second;
	}
	return nullptr;
}

void CPluginEntityFactories::InstallGameFactory(const char* classname, IEntityFactory* factory)
{
	if (m_gameFactories.find(classname) == m_gameFactories.end())
	{
		m_gameFactories.emplace(classname, factory);
	}
}

void CPluginEntityFactories::RemoveGameFactory(IEntityFactory* factory)
{
	auto it = m_gameFactories.begin();
	while (it != m_gameFactories.end())
	{
		if (it->second == factory)
		{
			it = m_gameFactories.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void CPluginEntityFactories::SDK_OnAllLoaded()
{
	SH_MANUALHOOK_RECONFIGURE(FactoryEntity_GetDataDescMap, CBaseEntity::offset_GetDataDescMap, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(FactoryEntity_UpdateOnRemove, CBaseEntity::offset_UpdateOnRemove, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(EntityRecord_MyNextBotPointer, CBaseEntity::offset_MyNextBotPointer, 0, 0);
}

void CPluginEntityFactories::OnCoreMapEnd()
{
}

void CPluginEntityFactories::SDK_OnUnload()
{
	g_EntityMemAllocHook.Shutdown();

	for (int i = 0; i < m_Factories.Count(); i++)
	{
		m_Factories[i]->Uninstall();
	}

	handlesys->RemoveType( m_FactoryType, myself->GetIdentity() );

	forwards->ReleaseForward( m_fwdInstalledFactory );
	forwards->ReleaseForward( m_fwdUninstalledFactory );

	plsys->RemovePluginsListener( this );
}

void CPluginEntityFactories::OnFactoryCreated( CPluginEntityFactory* pFactory )
{
	m_Factories.AddToTail( pFactory );
}

void CPluginEntityFactories::OnFactoryDestroyed( CPluginEntityFactory* pFactory )
{
	m_Factories.FindAndRemove( pFactory );
}

void CPluginEntityFactories::OnFactoryInstall(CPluginEntityFactory * pFactory)
{
	m_fwdInstalledFactory->PushString(pFactory->m_iClassname.c_str());
	m_fwdInstalledFactory->PushCell(pFactory->m_Handle);
	m_fwdInstalledFactory->Execute();
}

void CPluginEntityFactories::OnFactoryUninstall(CPluginEntityFactory * pFactory)
{
	m_fwdUninstalledFactory->PushString(pFactory->m_iClassname.c_str());
	m_fwdUninstalledFactory->PushCell(pFactory->m_Handle);
	m_fwdUninstalledFactory->Execute();
}

int CPluginEntityFactories::GetInstalledFactoryHandles(Handle_t* pHandleArray, size_t arraySize)
{
	unsigned int j = 0;
	for (int i = 0; i < m_Factories.Count() && (!pHandleArray || j < arraySize); i++)
	{
		CPluginEntityFactory* pFactory = m_Factories[i];
		if (!pFactory->m_bInstalled)
			continue;

		if (pHandleArray)
			pHandleArray[j] = m_Factories[i]->m_Handle;
		j++;
	}

	return j;
}

void CPluginEntityFactories::OnPluginUnloaded( IPlugin* plugin )
{
	// Uninstall the factories before Handles start to get freed during
	// plugin unload. This is to resolve errors that may occur when entities
	// are removed during plugin unload, and plugin tries to free handles
	// in a cleanup callback.

	CUtlVector< CPluginEntityFactory* > factoriesToUninstall;

	for (int i = 0; i < m_Factories.Count(); i++)
	{
		CPluginEntityFactory* pFactory = m_Factories[i];
		if (!pFactory->m_bInstalled)
			continue;

		if ( pFactory->m_pPlugin != plugin )
			continue;

		// Get base factory since uninstalling base factory will uninstall
		// the derived factories for us.
		while ( true )
		{
			CPluginEntityFactory* pBase = CPluginEntityFactory::ToPluginEntityFactory( pFactory->GetBaseFactory() );
			if ( !pBase )
				break;

			if ( pBase->m_pPlugin != plugin )
				break;
			
			// Assuming base factory is installed otherwise we wouldn't
			// be here in the first place.
			pFactory = pBase;
		}

		if ( !factoriesToUninstall.IsValidIndex( factoriesToUninstall.Find( pFactory ) ) )
		{
			factoriesToUninstall.AddToTail( pFactory );
		}
	}

	for (int i = 0; i < factoriesToUninstall.Count(); i++)
	{
		factoriesToUninstall[i]->Uninstall();
	}
}

void CPluginEntityFactories::OnHandleDestroy( HandleType_t type, void * object )
{
	CPluginEntityFactory* factory = (CPluginEntityFactory*)object;
	factory->Uninstall();
	factory->DestroyDataDesc();
	delete factory;
}

CPluginEntityFactory* CPluginEntityFactories::ToPluginEntityFactory( IEntityFactory* pFactory )
{
	if (!pFactory)
		return nullptr;

	CPluginEntityFactory* pAsPluginFactory = reinterpret_cast< CPluginEntityFactory* >(pFactory);
	return m_Factories.HasElement(pAsPluginFactory) ? pAsPluginFactory : nullptr;
}

void CPluginEntityFactories::NotifyEntityDestruction( CBaseEntity* pEntity )
{
	CPluginEntityFactory* pFactory = GetFactory(pEntity);
	if (pFactory)
	{
		pFactory->OnDestroy(pEntity);
	}
}

void CPluginEntityFactories::Hook_UpdateOnRemove()
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META(MRES_IGNORED);

	CPluginEntityFactory* pFactory = GetFactory(pEntity);
	if (!pFactory) RETURN_META(MRES_IGNORED);

	pFactory->OnRemove(pEntity);

	RETURN_META(MRES_IGNORED);
}

#ifdef __linux__
void CPluginEntityFactories::Hook_EntityDestructor( void )
#else
void CPluginEntityFactories::Hook_EntityDestructor( unsigned int flags )
#endif
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META(MRES_IGNORED);

	CPluginEntityFactory* pFactory = GetFactory(pEntity);
	if (!pFactory) RETURN_META(MRES_IGNORED);

	pFactory->OnDestroy(pEntity);

	RETURN_META(MRES_IGNORED);
}

void PluginFactoryEntityRecord_t::Hook(bool bHookDestructor)
{
	if (m_bHooked)
	{
		return;
	}
	m_bHooked = true;

	m_pHookIds.push_back( SH_ADD_MANUALHOOK(FactoryEntity_GetDataDescMap, pEntity, SH_MEMBER(this, &PluginFactoryEntityRecord_t::Hook_GetDataDescMap), false) );
	m_pHookIds.push_back( SH_ADD_MANUALHOOK(FactoryEntity_UpdateOnRemove, pEntity, SH_MEMBER(g_pPluginEntityFactories, &CPluginEntityFactories::Hook_UpdateOnRemove), false) );

	if (bHookDestructor)
	{
		m_pHookIds.push_back(SH_ADD_MANUALHOOK(FactoryEntity_Dtor, pEntity, SH_MEMBER(g_pPluginEntityFactories, &CPluginEntityFactories::Hook_EntityDestructor), false));
	}

	INextBot* bot = nullptr;
	if (m_pNextBot)
	{
		bot = m_pNextBot;
		m_pHookIds.push_back(SH_ADD_MANUALHOOK(EntityRecord_MyNextBotPointer, pEntity, SH_MEMBER(this, &PluginFactoryEntityRecord_t::Hook_MyNextBotPointer), false));
	}
	else
	{
		bot = pEntity->MyNextBotPointer();
	}

	if (m_pInitialActionFactory && bot)
	{
		IIntention* pOldIntention = bot->GetIntentionInterface();
		if (pOldIntention)
		{
			// Remove the old intention interface from the component list so it no longer
			// gets any updates. Do not destroy it as the INextBot most likely already has 
			// its own methods of destroying it anyways.

			bot->UnregisterComponent(pOldIntention);
		}

		m_pIntentionInterface = new CBaseNPCIntention(bot, m_pInitialActionFactory);
		m_pHookIds.push_back(SH_ADD_HOOK(INextBot, GetIntentionInterface, bot, SH_MEMBER(this, &PluginFactoryEntityRecord_t::Hook_GetIntentionInterface), false));
	}
}

PluginFactoryEntityRecord_t::~PluginFactoryEntityRecord_t()
{
	if (m_pIntentionInterface)
	{
		// Ensure that the interface performs cleanup plugin callbacks first.
		delete m_pIntentionInterface;
	}

	if (m_pNextBot)
	{
		delete m_pNextBot;
	}

	if (m_bHooked)
	{
		m_bHooked = false;

		for (auto it = m_pHookIds.begin(); it != m_pHookIds.end(); it++)
		{
			SH_REMOVE_HOOK_ID((*it));
		}
	}
}

datamap_t* PluginFactoryEntityRecord_t::Hook_GetDataDescMap()
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity)
	{
		RETURN_META_VALUE(MRES_IGNORED, nullptr);
	}

	if (m_pDataMap)
	{
		RETURN_META_VALUE(MRES_SUPERCEDE, m_pDataMap);
	}
	
	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

INextBot* PluginFactoryEntityRecord_t::Hook_MyNextBotPointer()
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_pNextBot);
}

IIntention* PluginFactoryEntityRecord_t::Hook_GetIntentionInterface()
{
	RETURN_META_VALUE(MRES_SUPERCEDE, m_pIntentionInterface);
}

CPluginEntityFactory* CPluginEntityFactory::ToPluginEntityFactory( IEntityFactory* pFactory )
{
	return g_pPluginEntityFactories->ToPluginEntityFactory( pFactory );
}

CPluginEntityFactory::CPluginEntityFactory( IPlugin* plugin, const char* classname, IPluginFunction *postConstructor, IPluginFunction *onRemove ) :
	IEntityDataMapContainer(),
	m_iClassname(classname),
	m_pPlugin(plugin),
	m_pPostConstructor(postConstructor),
	m_pOnRemove(onRemove),
	m_bInstalled(false),
	m_attachNextbot(nullptr),
	m_pBaseNPCInitialActionFactory(nullptr)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_NONE;

	m_Handle = handlesys->CreateHandle( g_pPluginEntityFactories->GetFactoryType(), this, plugin->GetIdentity(), myself->GetIdentity(), nullptr );

	m_bIsAbstract = false;
	m_pBaseFactory = nullptr;

	g_pPluginEntityFactories->OnFactoryCreated( this );
}

CPluginEntityFactory::~CPluginEntityFactory()
{
	g_pPluginEntityFactories->OnFactoryDestroyed( this );
}

void CPluginEntityFactory::DeriveFromBaseEntity(bool bServerOnly)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_BASECLASS;
	m_Derive.m_BaseType = FACTORYBASECLASS_BASEENTITY;
	m_Derive.m_bBaseEntityServerOnly = bServerOnly;
}

void CPluginEntityFactory::DeriveFromNPC()
{
	m_Derive.m_DeriveFrom = DERIVETYPE_CBASENPC;
}

void CPluginEntityFactory::DeriveFromClass(const char* classname)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_CLASSNAME;
	m_Derive.m_iBaseClassname = classname;
}

void CPluginEntityFactory::DeriveFromHandle(Handle_t handle)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_HANDLE;
	m_Derive.m_BaseFactoryHandle = handle;
}

bool CPluginEntityFactory::DeriveFromConf(size_t entitySize, IGameConfig* config, int type, const char* name)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_CONFIG;
	m_Derive.m_iRawEntitySize = entitySize;

	switch ( (SDKFuncConfSource)type )
	{
		case SDKConf_Address:
			return config->GetAddress( name, (void**)(&m_Derive.m_pConstructorFunc) ) && m_Derive.m_pConstructorFunc;
		case SDKConf_Signature:
			return config->GetMemSig( name, (void**)(&m_Derive.m_pConstructorFunc) ) && m_Derive.m_pConstructorFunc;
		case SDKConf_Virtual:
			return config->GetOffset( name, (int*)(&m_Derive.m_pConstructorFunc) ) && m_Derive.m_pConstructorFunc;
	}

	return false;
}

void CPluginEntityFactory::OnRemove(CBaseEntity* pEntity)
{
	if (m_pOnRemove && m_pOnRemove->IsRunnable())
	{
		m_pOnRemove->PushCell(gamehelpers->EntityToBCompatRef(pEntity));
		m_pOnRemove->Execute(nullptr);
	}

	CPluginEntityFactory* pBasePluginFactory = ToPluginEntityFactory( GetBaseFactory() );
	if (pBasePluginFactory)
	{
		pBasePluginFactory->OnRemove(pEntity);
	}
}

void CPluginEntityFactory::OnDestroy( CBaseEntity* pEntity )
{
	CPluginEntityFactory* pBasePluginFactory = ToPluginEntityFactory( GetBaseFactory() );
	if (pBasePluginFactory)
	{
		pBasePluginFactory->OnDestroy( pEntity );
	}

	if (g_pPluginEntityFactories->GetFactory(pEntity) == this)
	{
		g_pPluginEntityFactories->RemoveRecord( pEntity );

		for (CPluginEntityFactory* pFactory = this; pFactory; pFactory = ToPluginEntityFactory(pFactory->GetBaseFactory()))
		{
			pFactory->DestroyUserEntityData(pEntity);
		}
	}
}

void CPluginEntityFactories::GetEntitiesOfFactory(CPluginEntityFactory* pFactory, CUtlVector< CBaseEntity* > &vector)
{
	for (auto it = m_Records.begin(); it != m_Records.end(); it++)
	{
		if (it->second->pFactory == pFactory)
		{
			vector.AddToTail(it->second->pEntity);
		}
	}
}

void CPluginEntityFactory::GetEntities( CUtlVector< CBaseEntity* > &vector )
{
	g_pPluginEntityFactories->GetEntitiesOfFactory( this, vector );
}

bool CPluginEntityFactory::Install()
{
	if (m_bInstalled)
	{
		return true;
	}

	IEntityFactory *pBaseFactory = FindBaseFactory();
	if (!pBaseFactory && IsBaseFactoryRequired())
	{
		return false;
	}

	const char* classname = m_iClassname.c_str();

	if (!IsAbstract())
	{
		if (g_pPluginEntityFactories->FindPluginFactory(classname) != nullptr)
		{
			return false;
		}
		
		g_pPluginEntityFactories->InstallPluginFactory(classname, this);
	}

	SetBaseFactory(pBaseFactory);

	m_bInstalled = true;

	g_pPluginEntityFactories->OnFactoryInstall(this);

	g_pSM->LogMessage(myself, "Installed %s entity factory", classname);

	return true;
}

void CPluginEntityFactory::Uninstall()
{
	if (!m_bInstalled)
		return;

	const char* classname = m_iClassname.c_str();

	if (!IsAbstract())
	{
		g_pPluginEntityFactories->RemovePluginFactory(this);
	}

	// Uninstall children, or factories that derive from this factory.
	if (m_DerivedFactories.Count() > 0)
	{
		for (int i = m_DerivedFactories.Count() - 1; i >= 0; i--)
		{
			CPluginEntityFactory* pChild = m_DerivedFactories[i];
			if (!pChild)
				continue;
			
			pChild->Uninstall();
		}
	}

	RemoveAllEntities();

	SetBaseFactory(nullptr);

	m_bInstalled = false;

	g_pPluginEntityFactories->OnFactoryUninstall( this );

	g_pSM->LogMessage(myself, "Uninstalled %s entity factory", classname);
}

void CPluginEntityFactory::RemoveAllEntities()
{
	CUtlVector< CBaseEntity* > entities;
	GetEntities( entities );

	for (int i = 0; i < entities.Count(); i++)
	{
		CBaseEntity* pEntity = reinterpret_cast< CBaseEntity* >( entities[i] );
		servertools->RemoveEntityImmediate( pEntity );
	}
}

IEntityFactory* CPluginEntityFactory::FindBaseFactory() const
{
	switch (m_Derive.m_DeriveFrom)
	{
		case DERIVETYPE_CBASENPC:
			return g_pBaseNPCFactory;
		case DERIVETYPE_CLASSNAME:
		{
			IEntityFactory* factory = g_pPluginEntityFactories->FindFactory(m_Derive.m_iBaseClassname.c_str());
			if (factory == this)
			{
				// Shouldn't happen, but just in case
				factory = g_pPluginEntityFactories->FindGameFactory(m_Derive.m_iBaseClassname.c_str());
			}
			return factory;
		}
		case DERIVETYPE_HANDLE:
		{
			CPluginEntityFactory* pFactory = g_pPluginEntityFactories->GetFactoryFromHandle( m_Derive.m_BaseFactoryHandle );
			if (pFactory && !pFactory->m_bInstalled) 
				return nullptr;
			return pFactory;
		}
	}

	return nullptr;
}

bool CPluginEntityFactory::IsBaseFactoryRequired() const
{
	auto deriveFrom = m_Derive.m_DeriveFrom;
	if (deriveFrom == DERIVETYPE_CBASENPC || deriveFrom == DERIVETYPE_CLASSNAME || deriveFrom == DERIVETYPE_HANDLE)
		return true;

	return false;
}

void CPluginEntityFactory::SetBaseFactory(IEntityFactory* pBaseFactory)
{
	IEntityFactory* pOldBaseFactory = m_pBaseFactory;
	if (pOldBaseFactory == pBaseFactory)
		return;
	
	m_pBaseFactory = pBaseFactory;

	if (pOldBaseFactory)
	{
		CPluginEntityFactory* pPluginFactory = ToPluginEntityFactory(pOldBaseFactory);
		if (pPluginFactory)
		{
			pPluginFactory->m_DerivedFactories.FindAndRemove(this);
		}
	}

	if (m_pBaseFactory)
	{
		CPluginEntityFactory* pPluginFactory = ToPluginEntityFactory(m_pBaseFactory);
		if (pPluginFactory)
		{
			pPluginFactory->m_DerivedFactories.AddToTail(this);
		}
	}
}

size_t CPluginEntityFactory::GetBaseEntitySize() const
{
	IEntityFactory *factory = GetBaseFactory();
	if (factory)
	{
		return factory->GetEntitySize();
	}

	switch ( m_Derive.m_DeriveFrom )
	{
		case DERIVETYPE_BASECLASS:
			return g_pPluginEntityFactories->GetBaseClassSize( m_Derive.m_BaseType );
		case DERIVETYPE_CONFIG:
			return m_Derive.m_iRawEntitySize;
	}

	return 0; // should never reach here
}

IServerNetworkable* CPluginEntityFactory::Create(const char* classname)
{
	return RecursiveCreate(classname, this);
}

IServerNetworkable* CPluginEntityFactory::RecursiveCreate(const char* classname, CPluginEntityFactory *pCreatingFactory)
{
	IServerNetworkable * pNet = nullptr;

	bool bIsInstantiating = false;
	bool bHookDestructor = true;

	size_t entitySize = pCreatingFactory->GetEntitySize();

	static MCall<void> ctorCall;

	IEntityFactory *pBaseFactory = GetBaseFactory();
	if (pBaseFactory)
	{
		CPluginEntityFactory* pAsPluginFactory = ToPluginEntityFactory(pBaseFactory);
		if (pAsPluginFactory)
		{
			// Defer instantiation to the base CPluginEntityFactory.
			pNet = pAsPluginFactory->RecursiveCreate(classname, pCreatingFactory);
		}
		else
		{
			bIsInstantiating = true;

			if (pBaseFactory == g_pBaseNPCFactory)
			{
				// Do not hook destructor manually. Instead, we'll let CBaseNPC tell us when it's destroyed
				// before performing cleanup so that any plugin callbacks/actions can run their own cleanup
				// code first before removing our hooks.
				bHookDestructor = false;
			}

			g_EntityMemAllocHook.StartWatching(pBaseFactory->GetEntitySize(), entitySize);
			pNet = pBaseFactory->Create(classname);
			g_EntityMemAllocHook.StopWatching();

			if (!g_EntityMemAllocHook.DidHandleAlloc())
			{
				g_pSM->LogError(myself, "WARNING! Entity %s was instantiated with possibly incorrect size. (instantiating plugin factory: %s)", classname, m_iClassname.c_str());
			}
		}
	}
	else if (m_Derive.m_DeriveFrom == DERIVETYPE_BASECLASS && m_Derive.m_BaseType == FACTORYBASECLASS_BASEENTITY)
	{
		bIsInstantiating = true;

		CBaseEntity* pEnt = (CBaseEntity*)engine->PvAllocEntPrivateData(entitySize + ((4 - (entitySize % 4)) % 4));
		CBaseEntity::CBaseEntity_Ctor(pEnt, m_Derive.m_bBaseEntityServerOnly);
		pEnt->PostConstructor(classname);
		pNet = pEnt->NetworkProp();
	}
	else if (m_Derive.m_DeriveFrom == DERIVETYPE_CONFIG)
	{
		bIsInstantiating = true;

		CBaseEntity* pEnt = (CBaseEntity*)engine->PvAllocEntPrivateData(entitySize + ((4 - (entitySize % 4)) % 4));
		ctorCall.Init((void*)m_Derive.m_pConstructorFunc);
		ctorCall(pEnt);
		pEnt->PostConstructor(classname);
		pNet = pEnt->NetworkProp();
	}

	if (pNet)
	{
		CBaseEntity* pEnt = pNet->GetBaseEntity();

		if (HasDataDesc())
		{
			CreateDataDescMap( gamehelpers->GetDataMap(pEnt) );
		}

		PluginFactoryEntityRecord_t * pEntityRecord = g_pPluginEntityFactories->FindRecord(pEnt, bIsInstantiating);
		if (bIsInstantiating)
		{
			pEntityRecord->pFactory = pCreatingFactory;

			IPluginFunction* nextBotFactory = nullptr;
			CBaseNPCPluginActionFactory* pInitialActionFactory = nullptr;
			CPluginEntityFactory* pFactory = pCreatingFactory;
			while (pFactory)
			{
				if (!pInitialActionFactory && pFactory->GetBaseNPCInitialActionFactory())
				{
					pInitialActionFactory = pFactory->GetBaseNPCInitialActionFactory();
				}
				if (!nextBotFactory)
				{
					nextBotFactory = pFactory->GetNextBotFactory();
				}
				pFactory = ToPluginEntityFactory( pFactory->GetBaseFactory() );
			}

			// If requested, attach a INextBot interface. The entity must be nextbotless and deriving from CBaseCombatCharacter
			if (nextBotFactory && pEnt->MyCombatCharacterPointer() && !pEnt->MyNextBotPointer())
			{
				INextBot* createdBot = nullptr;
				if (nextBotFactory != (IPluginFunction*)0x1)
				{
					if (nextBotFactory->IsRunnable())
					{
						nextBotFactory->PushCell(PtrToPawnAddress(pEnt));
						cell_t address = 0;
						nextBotFactory->Execute(&address);
						createdBot = (INextBot*)PawnAddressToPtr(address);
					}
				}
				else
				{
					createdBot = new ToolsNextBot(pEnt->MyCombatCharacterPointer());
				}
				pEntityRecord->m_pNextBot = createdBot;
			}

			if (pEntityRecord->m_pNextBot || pEnt->MyNextBotPointer())
			{
				pEntityRecord->m_pInitialActionFactory = pInitialActionFactory;
			}
		}

		datamap_t * pDataMap = GetDataDescMap();

		if (pDataMap)
		{
			pEntityRecord->m_pDataMap = pDataMap;
			CreateUserEntityData(pEnt);
		}

		if (bIsInstantiating)
		{
			pEntityRecord->Hook(bHookDestructor);
		}

		if (m_pPostConstructor && m_pPostConstructor->IsRunnable())
		{
			m_pPostConstructor->PushCell(gamehelpers->EntityToBCompatRef(pEnt));
			m_pPostConstructor->Execute(nullptr);
		}
	}

	return pNet;
}

bool CPluginEntityFactory::BeginDataDesc(const char* dataClassName)
{
	IEntityFactory *pBaseFactory = FindBaseFactory();
	if (!pBaseFactory && IsBaseFactoryRequired())
	{
		return false;
	}

	// Don't use GetBaseEntitySize() if base factory is required because 
	// the base factory hasn't been set yet. The base factory is only set when
	// the factory is installed.
	m_iDataMapStartOffset = pBaseFactory ? pBaseFactory->GetEntitySize() : GetBaseEntitySize();
	m_iDataClassname = dataClassName;

	BeginDataDesc();

	return true;
}

void CPluginEntityFactory::CreateUserEntityData(CBaseEntity* pEntity)
{
	CreateFields(pEntity);
}

void CPluginEntityFactory::DestroyUserEntityData(CBaseEntity* pEntity)
{
	DestroyFields(pEntity);
}

void CPluginEntityFactory::Destroy(IServerNetworkable* pNetworkable)
{
	if (pNetworkable)
	{
		pNetworkable->Release();
	}
}

class PluginInputFuncDelegate : public IEntityDataMapInputFuncDelegate
{
private:
	IPluginFunction* m_pCallback;
	fieldtype_t m_fieldType;

public:
	PluginInputFuncDelegate(IPluginFunction* pCallback, fieldtype_t fieldType) : 
		IEntityDataMapInputFuncDelegate(),
		m_pCallback(pCallback),
		m_fieldType(fieldType)
	{
	}

	virtual void OnInput(CBaseEntity* pEntity, inputdata_t &data) override final
	{
		if (!m_pCallback || !m_pCallback->IsRunnable())
			return;
		
		m_pCallback->PushCell(gamehelpers->EntityToBCompatRef(pEntity));
		m_pCallback->PushCell(gamehelpers->EntityToBCompatRef(data.pActivator));
		m_pCallback->PushCell(gamehelpers->EntityToBCompatRef(data.pCaller));

		variant_t &value = data.value;

		switch (m_fieldType)
		{
			case FIELD_STRING:
				m_pCallback->PushString(value.iszVal.ToCStr());
				break;
			case FIELD_BOOLEAN:
				m_pCallback->PushCell(value.bVal);
				break;
			case FIELD_COLOR32:
			{
				cell_t color[4] = { 
					value.rgbaVal.r,
					value.rgbaVal.g,
					value.rgbaVal.b,
					value.rgbaVal.a
				};

				m_pCallback->PushArray(color, 4);
				break;
			}
			case FIELD_FLOAT:
				m_pCallback->PushCell(sp_ftoc(value.flVal));
				break;
			case FIELD_INTEGER:
				m_pCallback->PushCell(value.iVal);
				break;
			case FIELD_VECTOR:
			{
				cell_t vec[3] = {
					sp_ftoc(value.vecVal[0]),
					sp_ftoc(value.vecVal[1]),
					sp_ftoc(value.vecVal[2])
				};
				m_pCallback->PushArray(vec, 3);
				break;
			}	
			case FIELD_VOID:
				break;
			default:
				m_pCallback->PushCell(value.iVal);
				break;
		}

		m_pCallback->Execute(nullptr);
	}
};

IEntityDataMapInputFuncDelegate* CPluginEntityFactory::CreateInputFuncDelegate(IPluginFunction* pCallback, fieldtype_t fieldType)
{
	return new PluginInputFuncDelegate(pCallback, fieldType);
}