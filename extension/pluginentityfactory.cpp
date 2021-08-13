
#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"
#include "cbasenpc_internal.h"
#include "cbasenpc_behavior.h"
#include "baseentityoutput.h"
#include "sh_pagealloc.h"

SH_DECL_MANUALHOOK0(FactoryEntity_GetDataDescMap, 0, 0, 0, datamap_t* );
SH_DECL_MANUALHOOK0_void(FactoryEntity_UpdateOnRemove, 0, 0, 0 );

#ifdef __linux__
SH_DECL_MANUALHOOK0_void(FactoryEntity_Dtor, 1, 0, 0);
#else
SH_DECL_MANUALHOOK1_void(FactoryEntity_Dtor, 0, 0, 0, unsigned int);
#endif

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

		SH_ADD_HOOK(IVEngineServer, PvAllocEntPrivateData, engine, SH_MEMBER(this, &EntityMemAllocHook_t::PvAllocEntPrivateData), false);

		m_bHooked = true;
	}

	void Unhook()
	{
		if (!m_bHooked)
			return;

		SH_REMOVE_HOOK(IVEngineServer, PvAllocEntPrivateData, engine, SH_MEMBER(this, &EntityMemAllocHook_t::PvAllocEntPrivateData), false);

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
	SetDefLessFunc( m_Records );
}

PluginFactoryEntityRecord_t* CPluginEntityFactories::FindRecord( CBaseEntity* pEntity, bool create )
{
	if (!pEntity)
		return nullptr;
	
	cell_t key = gamehelpers->EntityToReference(pEntity);
	unsigned short index = m_Records.Find( key );
	if ( !m_Records.IsValidIndex(index) )
	{
		if (create)
		{
			index = m_Records.Insert(key, PluginFactoryEntityRecord_t( pEntity ));
		}
		else
		{
			return nullptr;
		}
	}

	return &m_Records[index];
}

void CPluginEntityFactories::RemoveRecord( CBaseEntity* pEntity )
{
	if (!pEntity)
		return;
	
	cell_t key = gamehelpers->EntityToReference(pEntity);
	m_Records.Remove( key );
}

CPluginEntityFactory* CPluginEntityFactories::GetFactory( CBaseEntity* pEntity )
{
	PluginFactoryEntityRecord_t* pEntityRecord = FindRecord( pEntity );
	if (!pEntityRecord)
		return nullptr;

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
	IEntityFactoryDictionary* factoryDictionary = servertools->GetEntityFactoryDictionary();
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

void CPluginEntityFactories::SDK_OnAllLoaded()
{
	SH_MANUALHOOK_RECONFIGURE(FactoryEntity_GetDataDescMap, CBaseEntityHack::offset_GetDataDescMap, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(FactoryEntity_UpdateOnRemove, CBaseEntityHack::offset_UpdateOnRemove, 0, 0);
}

void CPluginEntityFactories::OnCoreMapEnd()
{
	// Remove entities early. This is to make sure that some ending callbacks
	// are called before some Handles are freed (like timer mapchange handles).

	for (int i = 0; i < m_Factories.Count(); i++)
	{
		CPluginEntityFactory* pFactory = m_Factories[i];
		if ( !pFactory->m_bInstalled ) continue;
		
		m_Factories[i]->RemoveAllEntities();
	}
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

datamap_t* CPluginEntityFactories::Hook_GetDataDescMap()
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META_VALUE(MRES_IGNORED, nullptr);

	PluginFactoryEntityRecord_t* pEntityRecord = FindRecord(pEntity);
	if (!pEntityRecord)
		RETURN_META_VALUE(MRES_IGNORED, nullptr);

	datamap_t* pDataMap = pEntityRecord->m_pDataMap;
	if (pDataMap)
	{
		RETURN_META_VALUE(MRES_SUPERCEDE, pDataMap);
	}
	
	RETURN_META_VALUE(MRES_IGNORED, nullptr);
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

#ifdef WIN32
void CPluginEntityFactories::Hook_EntityDestructor( unsigned int flags )
#else
void CPluginEntityFactories::Hook_EntityDestructor( void )
#endif
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META(MRES_IGNORED);

	CPluginEntityFactory* pFactory = GetFactory(pEntity);
	if (!pFactory) RETURN_META(MRES_IGNORED);

	pFactory->OnDestroy(pEntity);

	RETURN_META(MRES_IGNORED);
}

void PluginFactoryEntityRecord_t::Hook()
{
	if ( m_bHooked ) return;
	m_bHooked = true;

	if ( !m_pHookIds )
		m_pHookIds = new std::vector<int>;

	m_pHookIds->push_back( SH_ADD_MANUALHOOK(FactoryEntity_GetDataDescMap, pEntity, SH_MEMBER(g_pPluginEntityFactories, &CPluginEntityFactories::Hook_GetDataDescMap), false) );
	m_pHookIds->push_back( SH_ADD_MANUALHOOK(FactoryEntity_UpdateOnRemove, pEntity, SH_MEMBER(g_pPluginEntityFactories, &CPluginEntityFactories::Hook_UpdateOnRemove), false) );
	m_pHookIds->push_back( SH_ADD_MANUALHOOK(FactoryEntity_Dtor, pEntity, SH_MEMBER(g_pPluginEntityFactories, &CPluginEntityFactories::Hook_EntityDestructor), false) );
}

void PluginFactoryEntityRecord_t::Unhook()
{
	if ( !m_bHooked ) return;
	m_bHooked = false;
	
	if ( m_pHookIds )
	{
		for (auto it = m_pHookIds->begin(); it != m_pHookIds->end(); it++)
		{
			SH_REMOVE_HOOK_ID((*it));
		}

		delete m_pHookIds;
		m_pHookIds = nullptr;
	}
}

CPluginEntityFactory* CPluginEntityFactory::ToPluginEntityFactory( IEntityFactory* pFactory )
{
	return g_pPluginEntityFactories->ToPluginEntityFactory( pFactory );
}

CPluginEntityFactory::CPluginEntityFactory( IPlugin* plugin, const char* classname, IPluginFunction *postConstructor, IPluginFunction *onRemove ) :
	IEntityDataMapContainer(),
	m_pPlugin(plugin),
	m_iClassname(classname),
	m_pPostConstructor(postConstructor),
	m_pOnRemove(onRemove)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_NONE;
	m_pBaseNPCInitialActionFactory = nullptr;
	m_bInstalled = false;

	m_Handle = handlesys->CreateHandle( g_pPluginEntityFactories->GetFactoryType(), this, plugin->GetIdentity(), myself->GetIdentity(), nullptr );

	m_pCreatingFactory = nullptr;

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
	DestroyUserEntityData(pEntity);

	CPluginEntityFactory* pBasePluginFactory = ToPluginEntityFactory( GetBaseFactory() );
	if (pBasePluginFactory)
	{
		pBasePluginFactory->OnDestroy( pEntity );
	}

	PluginFactoryEntityRecord_t * pEntityRecord = g_pPluginEntityFactories->FindRecord( pEntity );
	if ( pEntityRecord->pFactory == this )
	{
		pEntityRecord->Unhook();
	}

	g_pPluginEntityFactories->RemoveRecord( pEntity );
}

void CPluginEntityFactories::GetEntitiesOfFactory( CPluginEntityFactory* pFactory, CUtlVector< CBaseEntity* > &vector )
{
	if (!m_Records.Count())
		return;

	for ( unsigned short i = 0; i < m_Records.Count(); i++ )
	{
		PluginFactoryEntityRecord_t *entityRecord = &m_Records[i];
		if ( entityRecord->pFactory == pFactory )
			vector.AddToTail( entityRecord->pEntity );
	}
}

void CPluginEntityFactory::GetEntities( CUtlVector< CBaseEntity* > &vector )
{
	g_pPluginEntityFactories->GetEntitiesOfFactory( this, vector );
}

bool CPluginEntityFactory::Install()
{
	if (m_bInstalled)
		return true;

	IEntityFactory *pBaseFactory = FindBaseFactory();
	if (!pBaseFactory && IsBaseFactoryRequired())
		return false;

	const char* classname = m_iClassname.c_str();

	if (!IsAbstract())
	{
		CEntityFactoryDictionaryHack* pFactoryDict = EntityFactoryDictionaryHack();
		if (pFactoryDict->FindFactory(classname))
			return false;
		
		// DO NOT USE IEntityFactoryDictionary::InstallFactory()!
		// It's a virtual function, which means memory allocation is handled by server.dll, not by this extension.
		// If you use it anyway and you try to uninstall, it will result in heap corruption error. (at least on Windows)
		pFactoryDict->m_Factories.Insert( classname, this );
	}

	SetBaseFactory(pBaseFactory);

	m_bInstalled = true;

	g_pPluginEntityFactories->OnFactoryInstall( this );

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
		bool removed = false;

		CEntityFactoryDictionaryHack* pFactoryDict = EntityFactoryDictionaryHack();

		for (size_t i = 0; i < pFactoryDict->m_Factories.Count(); i++)
		{
			if (pFactoryDict->m_Factories[i] == this)
			{
				removed = true;
				pFactoryDict->m_Factories.RemoveAt(i);
				break;
			}
		}

		if ( !removed )
		{
			// BUGFIX: For some reason, sometimes the first loop doesn't detect it.
			pFactoryDict->m_Factories.Remove( classname );
			removed = pFactoryDict->FindFactory( classname ) == nullptr;
		}

		if ( !removed )
		{
			g_pSM->LogError( myself, "WARNING! Uninstalling non-abstract factory %s, but was not found in the dictionary!", classname );
		}
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
		servertools->RemoveEntityImmediate(entities[i]);
	}
}

IEntityFactory* CPluginEntityFactory::FindBaseFactory() const
{
	switch (m_Derive.m_DeriveFrom)
	{
		case DERIVETYPE_BASECLASS:
			return nullptr;
		case DERIVETYPE_CBASENPC:
			return g_pBaseNPCFactory;
		case DERIVETYPE_CLASSNAME:
			return servertools->GetEntityFactoryDictionary()->FindFactory(m_Derive.m_iBaseClassname.c_str());
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
	else if (m_Derive.m_DeriveFrom == DERIVETYPE_BASECLASS)
	{
		return g_pPluginEntityFactories->GetBaseClassSize( m_Derive.m_BaseType );
	}

	return 0; // should never reach here
}

IServerNetworkable* CPluginEntityFactory::Create(const char* classname)
{
	IServerNetworkable * pNet = nullptr;

	if (!m_pCreatingFactory)
	{
		// m_pCreatingFactory wasn't set before the call was made, so this is the creating factory.
		m_pCreatingFactory = this;
	}

	size_t entitySize = m_pCreatingFactory->GetEntitySize();

	IEntityFactory *pBaseFactory = GetBaseFactory();
	if (pBaseFactory)
	{
		CPluginEntityFactory* pAsPluginFactory = ToPluginEntityFactory(pBaseFactory);
		if (pAsPluginFactory)
		{
			// Defer instantiation to the base CPluginEntityFactory.
			pAsPluginFactory->m_pCreatingFactory = m_pCreatingFactory;
			pNet = pBaseFactory->Create(classname);
			pAsPluginFactory->m_pCreatingFactory = nullptr;
		}
		else
		{
			// At the end of a creation chain. This factory is responsible for allocating the entity so that
			// it fits the datamap size.

			bool bIsBaseNPCFactory = pBaseFactory == g_pBaseNPCFactory;
			if (bIsBaseNPCFactory)
			{
				CBaseNPCPluginActionFactory* pInitialActionFactory = nullptr;

				CPluginEntityFactory* pFactory = m_pCreatingFactory;
				while (pFactory)
				{
					if ( pFactory->GetBaseNPCInitialActionFactory() )
					{
						pInitialActionFactory = pFactory->GetBaseNPCInitialActionFactory();
						break;
					}
					pFactory = ToPluginEntityFactory( pFactory->GetBaseFactory() );
				}

				g_pBaseNPCFactory->SetInitialActionFactory( pInitialActionFactory );
			}

			g_EntityMemAllocHook.StartWatching(pBaseFactory->GetEntitySize(), entitySize);
			pNet = pBaseFactory->Create(classname);
			g_EntityMemAllocHook.StopWatching();

			if (bIsBaseNPCFactory)
			{
				g_pBaseNPCFactory->SetInitialActionFactory( nullptr );
			}

			if (!g_EntityMemAllocHook.DidHandleAlloc())
			{
				g_pSM->LogError(myself, "WARNING! Entity %s was instantiated with possibly incorrect size. (instantiating plugin factory: %s)", classname, m_iClassname.c_str());
			}
		}
	}
	else if (m_Derive.m_DeriveFrom == DERIVETYPE_BASECLASS && m_Derive.m_BaseType == FACTORYBASECLASS_BASEENTITY)
	{
		CBaseEntityHack* pEnt = (CBaseEntityHack*)engine->PvAllocEntPrivateData(entitySize);
		CBaseEntityHack::CBaseEntity_Ctor(pEnt, m_Derive.m_bBaseEntityServerOnly);
		pEnt->PostConstructor(classname);
		pNet = pEnt->NetworkProp();
	}

	if (pNet)
	{
		CBaseEntityHack* pEnt = reinterpret_cast<CBaseEntityHack*>(pNet->GetBaseEntity());

		if (HasDataDesc())
		{
			CreateDataDescMap( gamehelpers->GetDataMap(pEnt) );
		}

		PluginFactoryEntityRecord_t * pEntityRecord = g_pPluginEntityFactories->FindRecord(pEnt, true);
		if (!pEntityRecord->pFactory)
			pEntityRecord->pFactory = m_pCreatingFactory;

		datamap_t * pDataMap = GetDataDescMap();

		if (pDataMap)
		{
			pEntityRecord->m_pDataMap = pDataMap;
			CreateUserEntityData(pEnt);
		}

		pEntityRecord->Hook();

		if (m_pPostConstructor && m_pPostConstructor->IsRunnable())
		{
			m_pPostConstructor->PushCell(gamehelpers->EntityToBCompatRef(pEnt));
			m_pPostConstructor->Execute(nullptr);
		}
	}

	if (m_pCreatingFactory == this)
	{
		m_pCreatingFactory = nullptr;
	}

	return pNet;
}

bool CPluginEntityFactory::BeginDataDesc(const char* dataClassName)
{
	IEntityFactory *pBaseFactory = FindBaseFactory();
	if (!pBaseFactory && IsBaseFactoryRequired())
		return false;

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

public:
	PluginInputFuncDelegate(IPluginFunction* pCallback) : 
		IEntityDataMapInputFuncDelegate(),
		m_pCallback(pCallback)
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

		switch (value.fieldType)
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

IEntityDataMapInputFuncDelegate* CPluginEntityFactory::CreateInputFuncDelegate(IPluginFunction* pCallback)
{
	return new PluginInputFuncDelegate(pCallback);
}