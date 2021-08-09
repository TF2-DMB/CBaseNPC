
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

SourceMod::IForward * g_fwdInstalledFactory;
SourceMod::IForward * g_fwdUninstalledFactory;

class PluginFactoryEntityRecord_t;

CUtlVector<CPluginEntityFactory*> g_PluginEntityFactories;
CUtlMap<cell_t, PluginFactoryEntityRecord_t> g_EntityRecords;

size_t CPluginEntityFactory::m_DeriveBaseClassSizes[DERIVEBASECLASSTYPE_MAX];

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

class PluginFactoryEntityRecord_t
{
public:
	CBaseEntity* pEntity = nullptr;
	CPluginEntityFactory* pFactory = nullptr;
	datamap_t* m_pDataMap = nullptr;

	void Hook();
	void Unhook();

	PluginFactoryEntityRecord_t() : pEntity( nullptr ) { }
	PluginFactoryEntityRecord_t( CBaseEntity* pEnt ) : pEntity(pEnt) { }

private:
	bool m_bHooked = false;
	std::vector<int> * m_pHookIds = nullptr;
};

PluginFactoryEntityRecord_t* GetPluginFactoryEntityRecord(CBaseEntity* pEntity, bool create=false)
{
	if (!pEntity)
		return nullptr;
	
	cell_t key = gamehelpers->EntityToReference(pEntity);
	unsigned short index = g_EntityRecords.Find(key);
	if (!g_EntityRecords.IsValidIndex(index))
	{
		if (create)
		{
			index = g_EntityRecords.Insert(key, PluginFactoryEntityRecord_t( pEntity ));
		}
		else
		{
			return nullptr;
		}
	}

	return &g_EntityRecords[index];
}

void RemovePluginFactoryEntityRecord(CBaseEntity* pEntity)
{
	if (!pEntity)
		return;
	
	cell_t key = gamehelpers->EntityToReference(pEntity);
	g_EntityRecords.Remove(key);
}

datamap_t* CPluginEntityFactory::Hook_GetDataDescMap()
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META_VALUE(MRES_IGNORED, nullptr);

	PluginFactoryEntityRecord_t* pEntityRecord = GetPluginFactoryEntityRecord(pEntity);
	if (!pEntityRecord)
		RETURN_META_VALUE(MRES_IGNORED, nullptr);

	datamap_t* pDataMap = pEntityRecord->m_pDataMap;
	if (pDataMap)
	{
		RETURN_META_VALUE(MRES_SUPERCEDE, pDataMap);
	}
	
	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}

void CPluginEntityFactory::Hook_UpdateOnRemove()
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META(MRES_IGNORED);

	CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEntity);
	if (!pFactory) RETURN_META(MRES_IGNORED);

	pFactory->OnRemove(pEntity);

	RETURN_META(MRES_IGNORED);
}

#ifdef WIN32
void CPluginEntityFactory::Hook_EntityDestructor( unsigned int flags )
#else
void CPluginEntityFactory::Hook_EntityDestructor( void )
#endif
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META(MRES_IGNORED);

	CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEntity);
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

	m_pHookIds->push_back( SH_ADD_MANUALHOOK(FactoryEntity_GetDataDescMap, pEntity, SH_STATIC(CPluginEntityFactory::Hook_GetDataDescMap), false) );
	m_pHookIds->push_back( SH_ADD_MANUALHOOK(FactoryEntity_UpdateOnRemove, pEntity, SH_STATIC(CPluginEntityFactory::Hook_UpdateOnRemove), false) );
	m_pHookIds->push_back( SH_ADD_MANUALHOOK(FactoryEntity_Dtor, pEntity, SH_STATIC(CPluginEntityFactory::Hook_EntityDestructor), false) );
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

CPluginEntityFactory* GetPluginEntityFactory(CBaseEntity* pEntity)
{
	PluginFactoryEntityRecord_t* pEntityRecord = GetPluginFactoryEntityRecord(pEntity);
	if (!pEntityRecord)
		return nullptr;

	return pEntityRecord->pFactory;
}

// Sets pEntity to belong to CPluginEntityFactory pFactory.
void SetPluginEntityFactory(CBaseEntity* pEntity, CPluginEntityFactory* pFactory)
{
	if (!pEntity)
		return;
	
	PluginFactoryEntityRecord_t* pEntityRecord = GetPluginFactoryEntityRecord(pEntity);
	if (!pEntityRecord)
		return;

	pEntityRecord->pFactory = pFactory;
}

bool CPluginEntityFactory::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	g_fwdInstalledFactory = forwards->CreateForward("CEntityFactory_OnInstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);
	g_fwdUninstalledFactory = forwards->CreateForward("CEntityFactory_OnUninstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);

	SetDefLessFunc(g_EntityRecords);

	g_EntityMemAllocHook.Init();

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

		m_DeriveBaseClassSizes[DERIVEBASECLASSTYPE_ENTITY] = entitySize;
	}

	return true;
}

void CPluginEntityFactory::SDK_OnAllLoaded()
{
	SH_MANUALHOOK_RECONFIGURE(FactoryEntity_GetDataDescMap, CBaseEntityHack::offset_GetDataDescMap, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(FactoryEntity_UpdateOnRemove, CBaseEntityHack::offset_UpdateOnRemove, 0, 0);
}

void CPluginEntityFactory::SDK_OnUnload()
{
	g_EntityMemAllocHook.Shutdown();

	forwards->ReleaseForward(g_fwdInstalledFactory);
	forwards->ReleaseForward(g_fwdUninstalledFactory);
}

void CPluginEntityFactory::OnFactoryInstall(CPluginEntityFactory * pFactory)
{
	g_fwdInstalledFactory->PushString(pFactory->m_iClassname.c_str());
	g_fwdInstalledFactory->PushCell(pFactory->m_Handle);
	g_fwdInstalledFactory->Execute();
}

void CPluginEntityFactory::OnFactoryUninstall(CPluginEntityFactory * pFactory)
{
	g_fwdUninstalledFactory->PushString(pFactory->m_iClassname.c_str());
	g_fwdUninstalledFactory->PushCell(pFactory->m_Handle);
	g_fwdUninstalledFactory->Execute();
}

CPluginEntityFactory* CPluginEntityFactory::ToPluginEntityFactory( IEntityFactory* pFactory )
{
	if (!pFactory)
		return nullptr;

	CPluginEntityFactory* pAsPluginFactory = reinterpret_cast<CPluginEntityFactory*>(pFactory);
	return g_PluginEntityFactories.HasElement(pAsPluginFactory) ? pAsPluginFactory : nullptr;
}

CPluginEntityFactory::CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor, IPluginFunction *onRemove) :
	IEntityDataMapContainer(),
	m_iClassname(classname),
	m_pPostConstructor(postConstructor),
	m_pOnRemove(onRemove)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_NONE;
	m_pBaseNPCInitialActionFactory = nullptr;
	m_bInstalled = false;

	m_Handle = BAD_HANDLE;

	m_pCreatingFactory = nullptr;

	m_bIsAbstract = false;
	m_pBaseFactory = nullptr;

	g_PluginEntityFactories.AddToTail(this);
}

CPluginEntityFactory::~CPluginEntityFactory()
{
	g_PluginEntityFactories.FindAndRemove(this);
}

void CPluginEntityFactory::DeriveFromBaseEntity(bool bServerOnly)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_BASECLASS;
	m_Derive.m_BaseType = DERIVEBASECLASSTYPE_ENTITY;
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

	PluginFactoryEntityRecord_t * pEntityRecord = GetPluginFactoryEntityRecord( pEntity );
	if ( pEntityRecord->pFactory == this )
	{
		pEntityRecord->Unhook();
	}

	RemovePluginFactoryEntityRecord( pEntity );
}

void CPluginEntityFactory::GetEntities(CUtlVector< CBaseEntity* > *pVec) const
{
	if (!g_EntityRecords.Count())
		return;

	for (unsigned short i = 0; i < g_EntityRecords.Count(); i++)
	{
		PluginFactoryEntityRecord_t *entityRecord = &g_EntityRecords[i];
		if (entityRecord->pFactory == this)
			pVec->AddToTail(entityRecord->pEntity);
	}
}

int CPluginEntityFactory::GetInstalledFactoryHandles(Handle_t* pHandleArray, int arraySize)
{
	int j = 0;
	for (int i = 0; i < g_PluginEntityFactories.Count() && (!pHandleArray || j < arraySize); i++)
	{
		CPluginEntityFactory* pFactory = g_PluginEntityFactories[i];
		if (!pFactory->m_bInstalled)
			continue;

		if (pHandleArray)
			pHandleArray[j] = g_PluginEntityFactories[i]->m_Handle;
		j++;
	}

	return j;
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

	OnFactoryInstall(this);

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
		CEntityFactoryDictionaryHack* pFactoryDict = EntityFactoryDictionaryHack();

		for (size_t i = 0; i < pFactoryDict->m_Factories.Count(); i++)
		{
			if (pFactoryDict->m_Factories[i] == this)
			{
				pFactoryDict->m_Factories.RemoveAt(i);
				break;
			}
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

	// Remove entities.
	CUtlVector< CBaseEntity* > entities;
	GetEntities(&entities);

	for (int i = 0; i < entities.Count(); i++)
	{
		servertools->RemoveEntityImmediate(entities[i]);
	}

	SetBaseFactory(nullptr);

	m_bInstalled = false;

	OnFactoryUninstall(this);

	g_pSM->LogMessage(myself, "Uninstalled %s entity factory", classname);
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
			Handle_t handle = m_Derive.m_BaseFactoryHandle;
			HandleSecurity security(nullptr, myself->GetIdentity());
			CPluginEntityFactory* pFactory;
			HandleError err = handlesys->ReadHandle(handle, g_PluginEntityFactoryHandle, &security, (void **)&pFactory);
			if (err != HandleError_None)
				return nullptr;
			if (!pFactory->m_bInstalled)
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
		return m_DeriveBaseClassSizes[m_Derive.m_BaseType];
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
	else if (m_Derive.m_DeriveFrom == DERIVETYPE_BASECLASS && m_Derive.m_BaseType == DERIVEBASECLASSTYPE_ENTITY)
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

		PluginFactoryEntityRecord_t * pEntityRecord = GetPluginFactoryEntityRecord(pEnt, true);
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

HandleType_t g_PluginEntityFactoryHandle;
CPluginEntityFactoryHandler g_PluginEntityFactoryHandler;

void CPluginEntityFactoryHandler::OnHandleDestroy(HandleType_t type, void * object)
{
	CPluginEntityFactory* factory = (CPluginEntityFactory*)object;
	factory->DestroyDataDesc();
	factory->Uninstall();
	delete factory;
}