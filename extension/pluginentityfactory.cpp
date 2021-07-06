
#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"
#include "cbasenpc_internal.h"

extern ISDKHooks* g_pSDKHooks;

SH_DECL_MANUALHOOK0(MHook_GetDataDescMap, 0, 0, 0, datamap_t* );
SH_DECL_MANUALHOOK0(MHook_GetServerClass, 0, 0, 0, ServerClass* );

SH_DECL_HOOK1(IVEngineServer, PvAllocEntPrivateData, SH_NOATTRIB, false, void *, long);

SourceMod::IForward * g_fwdInstalledFactory;
SourceMod::IForward * g_fwdUninstalledFactory;

struct PluginFactoryEntityRecord_t;

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

struct PluginFactoryEntityRecord_t
{
	CBaseEntity* pEntity;
	CPluginEntityFactory* pFactory;
	bool m_bDataMapHooked;
	bool m_bServerClassHooked;
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
			index = g_EntityRecords.Insert(key, { pEntity, nullptr, false });
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

CPluginEntityFactory* GetPluginEntityFactory(CBaseEntity* pEntity)
{
	PluginFactoryEntityRecord_t* pEntityRecord = GetPluginFactoryEntityRecord(pEntity);
	if (!pEntityRecord)
		return nullptr;

	return pEntityRecord->pFactory;
}

datamap_t* Hook_GetDataDescMap()
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META_VALUE(MRES_IGNORED, 0);

	CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEntity);
	if (!pFactory) RETURN_META_VALUE(MRES_IGNORED, 0);

	datamap_t* pDataMap = pFactory->GetDataMap();
	if (pDataMap)
	{
		RETURN_META_VALUE(MRES_SUPERCEDE, pDataMap);
	}
	
	RETURN_META_VALUE(MRES_IGNORED, 0);
}

ServerClass* Hook_GetServerClass()
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META_VALUE(MRES_IGNORED, 0);

	CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEntity);
	if (!pFactory) RETURN_META_VALUE(MRES_IGNORED, 0);

	RETURN_META_VALUE(MRES_SUPERCEDE, pFactory->GetServerClass());
}

bool CPluginEntityFactory::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	g_fwdInstalledFactory = forwards->CreateForward("CEntityFactory_OnInstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);
	g_fwdUninstalledFactory = forwards->CreateForward("CEntityFactory_OnUninstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);

	SetDefLessFunc(g_EntityRecords);

	g_EntityMemAllocHook.Init();

	IEntityFactoryDictionary* factoryDictionary = servertools->GetEntityFactoryDictionary();
	IEntityFactory* factory = nullptr;
	if (!factoryDictionary)
	{
		snprintf(error, maxlength, "Failed to get IEntityFactoryDictionary");
		return false;
	}

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

	return true;
}

void CPluginEntityFactory::SDK_OnAllLoaded()
{
	SH_MANUALHOOK_RECONFIGURE(MHook_GetDataDescMap, CBaseEntityHack::offset_GetDataDescMap, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(MHook_GetServerClass, CBaseEntityHack::offset_GetServerClass, 0, 0);
}

void CPluginEntityFactory::SDK_OnUnload()
{
	g_EntityMemAllocHook.Shutdown();

	forwards->ReleaseForward(g_fwdInstalledFactory);
	forwards->ReleaseForward(g_fwdUninstalledFactory);
}

void CPluginEntityFactory::OnEntityDestroyed(CBaseEntity* pEntity)
{
	CBaseEntityHack* pEnt = reinterpret_cast<CBaseEntityHack*>(pEntity);

	CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEnt);
	if (pFactory)
	{
		pFactory->OnRemove(pEnt);
	}
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

CPluginEntityFactory* CPluginEntityFactory::ToPluginEntityFactory( IEntityFactory* pFactory )
{
	if (!pFactory)
		return nullptr;

	CPluginEntityFactory* pAsPluginFactory = reinterpret_cast<CPluginEntityFactory*>(pFactory);
	return g_PluginEntityFactories.HasElement(pAsPluginFactory) ? pAsPluginFactory : nullptr;
}

CPluginEntityFactory::CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor, IPluginFunction *onRemove) :
	m_iClassname(classname),
	m_pPostConstructor(postConstructor),
	m_pOnRemove(onRemove)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_NONE;
	m_bInstalled = false;

	m_Handle = 0;

	m_pCreatingFactory = nullptr;

	m_pEntityServerClass = nullptr;

	m_pEntityDataMap = nullptr;
	m_bHasDataMapDesc = false;
	m_DataMapDescSizeInBytes = 0;

	m_pBasePluginEntityFactory = nullptr;

	g_PluginEntityFactories.AddToTail(this);
}

CPluginEntityFactory::~CPluginEntityFactory()
{
	Uninstall();

	DestroyDataMap();
	
	DestroyServerClass();

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

void CPluginEntityFactory::SetBasePluginEntityFactory(CPluginEntityFactory* pFactory)
{
	if (pFactory == m_pBasePluginEntityFactory)
		return;
	
	if (m_pBasePluginEntityFactory)
	{
		m_pBasePluginEntityFactory->m_DerivedFactories.FindAndRemove(this);
		m_pBasePluginEntityFactory = nullptr;
	}

	if (pFactory)
	{
		m_pBasePluginEntityFactory = pFactory;
		pFactory->m_DerivedFactories.AddToTail(this);
	}
}

void CPluginEntityFactory::OnRemove(CBaseEntity* pEntity)
{
	PluginFactoryEntityRecord_t * pEntityRecord = GetPluginFactoryEntityRecord(pEntity);
	bool bBelongsToFactory = pEntityRecord->pFactory == this;

	if (m_pOnRemove && m_pOnRemove->IsRunnable())
	{
		m_pOnRemove->PushCell(gamehelpers->EntityToBCompatRef(pEntity));
		m_pOnRemove->Execute(nullptr);
	}

	if (m_pBasePluginEntityFactory)
	{
		m_pBasePluginEntityFactory->OnRemove(pEntity);
	}

	if (bBelongsToFactory)
	{
		// Unhook here after all OnRemove calls have been made.
		if (pEntityRecord->m_bDataMapHooked)
		{
			SH_REMOVE_MANUALHOOK(MHook_GetDataDescMap, pEntity, SH_STATIC(Hook_GetDataDescMap), false);
			pEntityRecord->m_bDataMapHooked = false;
		}

		if (pEntityRecord->m_bServerClassHooked)
		{
			SH_REMOVE_MANUALHOOK(MHook_GetServerClass, pEntity, SH_STATIC(Hook_GetServerClass), false);
			pEntityRecord->m_bServerClassHooked = false;
		}

		DestroyUserEntityData(pEntity);

		RemovePluginFactoryEntityRecord(pEntity);
	}
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

	SetBasePluginEntityFactory(ToPluginEntityFactory(GetBaseFactory()));
	
	m_bInstalled = true;

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
		CBaseEntity * pEntity = entities[i];

		// Clean up immediately, do not wait for OnEntityDestroyed to call.
		OnRemove(pEntity);

		servertools->RemoveEntity(pEntity);
	}

	SetBasePluginEntityFactory(nullptr);

	m_bInstalled = false;

	OnFactoryUninstall(this);

	g_pSM->LogMessage(myself, "Uninstalled %s entity factory", classname);
}

IEntityFactory* CPluginEntityFactory::GetBaseFactory() const
{
	switch (m_Derive.m_DeriveFrom)
	{
		case DERIVETYPE_BASECLASS:
			return nullptr;
		case DERIVETYPE_CBASENPC:
			return g_pBaseNPCFactory;
		case DERIVETYPE_CLASSNAME:
			return EntityFactoryDictionaryHack()->FindFactory(m_Derive.m_iBaseClassname.c_str());
	}

	return nullptr;
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

size_t CPluginEntityFactory::GetEntitySize()
{
	return GetBaseEntitySize() + GetUserEntityDataSize();
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

			g_EntityMemAllocHook.StartWatching(pBaseFactory->GetEntitySize(), entitySize);
			pNet = pBaseFactory->Create(classname);
			g_EntityMemAllocHook.StopWatching();

			if (!g_EntityMemAllocHook.DidHandleAlloc())
			{
				g_pSM->LogError(myself, "WARNING! Entity %s was instantiated with possibly incorrect size. (instantiating plugin factory: %s)", classname, m_iClassname.c_str());
			}
		}
	}
	else if (m_Derive.m_DeriveFrom == DERIVETYPE_BASECLASS && m_Derive.m_BaseType == DERIVEBASECLASSTYPE_ENTITY)
	{
		CBaseEntityHack* pEnt = (CBaseEntityHack*)engine->PvAllocEntPrivateData(entitySize);
		CBaseEntityHack::CBaseEntity_Ctor.operator()(pEnt, m_Derive.m_bBaseEntityServerOnly);
		pEnt->PostConstructor(classname);
		pNet = pEnt->NetworkProp();
	}

	if (pNet)
	{
		CBaseEntityHack* pEnt = reinterpret_cast<CBaseEntityHack*>(pNet->GetBaseEntity());

		if (m_bHasDataMapDesc)
		{
			if (!m_pEntityDataMap)
				CreateDataMap( gamehelpers->GetDataMap(pEnt) );
		}

		if (false)
		{
			if (!m_pEntityServerClass)
			{
				CreateServerClass( pEnt->NetworkProp()->GetServerClass() );
			}
		}

		PluginFactoryEntityRecord_t * pEntityRecord = GetPluginFactoryEntityRecord(pEnt, true);
		pEntityRecord->pFactory = this;

		// SH manual hook to GetDataDescMap and GetServerClass; can also check if hooked
		// already; we only need to hook it once.

		if (m_pEntityDataMap)
		{
			if (!pEntityRecord->m_bDataMapHooked)
			{
				SH_ADD_MANUALHOOK(MHook_GetDataDescMap, pEnt, SH_STATIC(Hook_GetDataDescMap), false);
				pEntityRecord->m_bDataMapHooked = true;
			}
		}

		if (m_pEntityServerClass)
		{
			if (!pEntityRecord->m_bServerClassHooked)
			{
				SH_ADD_MANUALHOOK(MHook_GetServerClass, pEnt, SH_STATIC(Hook_GetServerClass), false);
				pEntityRecord->m_bServerClassHooked = true;
			}
		}

		if (m_pPostConstructor && m_pPostConstructor->IsRunnable())
		{
			m_pPostConstructor->PushCell(pEnt->entindex());
			m_pPostConstructor->Execute(nullptr);
		}
	}

	if (m_pCreatingFactory == this)
	{
		m_pCreatingFactory = nullptr;
	}

	return pNet;
}

ServerClass* CPluginEntityFactory::CreateServerClass(ServerClass* pBaseServerClass)
{
	if (!m_pEntityServerClass)
	{
		
	}

	return m_pEntityServerClass;
}

void CPluginEntityFactory::DestroyServerClass()
{
	if (!m_pEntityServerClass)
		return;
}

bool CPluginEntityFactory::CanUseDataMap() const
{
	return true;
}

datamap_t* CPluginEntityFactory::GetDataMap() const
{
	if (m_pEntityDataMap)
		return m_pEntityDataMap;
	
	if (m_pBasePluginEntityFactory)
		return m_pBasePluginEntityFactory->GetDataMap();

	return nullptr;
}

datamap_t* CPluginEntityFactory::CreateDataMap(datamap_t* pBaseMap)
{
	if (!m_pEntityDataMap)
	{
		m_pEntityDataMap = new datamap_t;
		m_pEntityDataMap->baseMap = pBaseMap;
		m_pEntityDataMap->dataClassName = strdup(m_iDataClassname.c_str());

		int numDataDesc = m_vecEntityDataTypeDescriptors.Count();
		typedescription_t * dataDesc = new typedescription_t[numDataDesc];
		m_pEntityDataMap->dataDesc = dataDesc;
		m_pEntityDataMap->dataNumFields = numDataDesc;

		for (int i = 0; i < numDataDesc; i++)
		{
			dataDesc[i] = m_vecEntityDataTypeDescriptors[i];
		}
	}

	return m_pEntityDataMap;
}

void CPluginEntityFactory::DestroyDataMap()
{
	if (!m_pEntityDataMap)
		return;
	
	for (int i = 0; i < m_pEntityDataMap->dataNumFields; i++)
	{
		auto dataDesc = &m_pEntityDataMap->dataDesc[i];
		DestroyDataMapTypeDescriptor(dataDesc);
	}

	delete m_pEntityDataMap->dataDesc;
	free((void*)m_pEntityDataMap->dataClassName);
	delete m_pEntityDataMap;

	m_pEntityDataMap = nullptr;
}

void CPluginEntityFactory::DestroyDataMapTypeDescriptor(typedescription_t *desc) const
{
	if (desc->fieldName)
	{
		free((void*)desc->fieldName);
		desc->fieldName = nullptr;
	}

	if (desc->externalName)
	{
		free((void*)desc->externalName);
		desc->externalName = nullptr;
	}
}

void CPluginEntityFactory::BeginDataMapDesc(const char* dataClassName)
{
	DestroyDataMap();

	m_bHasDataMapDesc = false;

	for (int i = 0; i < m_vecEntityDataTypeDescriptors.Count(); i++)
	{
		DestroyDataMapTypeDescriptor( &m_vecEntityDataTypeDescriptors[i] );
	}

	m_vecEntityDataTypeDescriptors.Purge();

	m_iDataClassname = dataClassName;
	m_DataMapDescSizeInBytes = 0;
}

void CPluginEntityFactory::EndDataMapDesc()
{
	m_bHasDataMapDesc = true;

	if (m_vecEntityDataTypeDescriptors.Count() == 0)
	{
		// For "empty" tables
		m_vecEntityDataTypeDescriptors.AddToTail( { FIELD_VOID, 0, {0,0}, 0, 0, 0, 0, 0, 0 } );
	}
}

int g_DataMapDescFieldSizes[] = {
	0,						// FIELD_VOID
	sizeof(float), 			// FIELD_FLOAT
	sizeof(int),			// FIELD_STRING
	3 * sizeof(float),		// FIELD_VECTOR
	4 * sizeof(float),		// FIELD_QUATERNION
	sizeof(int),			// FIELD_INTEGER
	sizeof(char),			// FIELD_BOOLEAN
	sizeof(short),			// FIELD_SHORT
	sizeof(char),			// FIELD_CHARACTER
	sizeof(int),			// FIELD_COLOR32
	0,						// FIELD_EMBEDDED
	0,						// FIELD_CUSTOM
	sizeof(int),			// FIELD_CLASSPTR
	sizeof(int),			// FIELD_EHANDLE
	sizeof(int),			// FIELD_EDICT
	3 * sizeof(float),		// FIELD_POSITION_VECTOR
	sizeof(float),			// FIELD_TIME
	sizeof(int),			// FIELD_TICK
	sizeof(int),			// FIELD_MODELNAME
	sizeof(int),			// FIELD_SOUNDNAME
	sizeof(int),			// FIELD_INPUT
	
	// FIELD_FUNCTION
#ifdef POSIX
	sizeof(uint64),
#else
	sizeof(int *),
#endif

	16 * sizeof(float),		// FIELD_VMATRIX
	16 * sizeof(float),		// FIELD_VMATRIX_WORLDSPACE
	12 * sizeof(float),		// FIELD_MATRIX3X4_WORLDSPACE
	2 * sizeof(float),		// FIELD_INTERVAL
	sizeof(int),			// FIELD_MODELINDEX
	sizeof(int),			// FIELD_MATERIALINDEX
	2 * sizeof(float),		// FIELD_VECTOR2D

};

void CPluginEntityFactory::DefineField(const char* name, fieldtype_t fieldType, unsigned short count, short flags, const char* externalName, float fieldTolerance)
{
	int fieldOffset = GetUserEntityDataOffset() + m_DataMapDescSizeInBytes;
	int fieldSizeInBytes = g_DataMapDescFieldSizes[fieldType];

	name = name ? strdup(name) : NULL;
	externalName = externalName ? strdup(externalName) : NULL;

	m_vecEntityDataTypeDescriptors.AddToTail({ 
		fieldType, 
		name, 
		{ fieldOffset, 0 }, 
		count,
		flags,
		externalName,
		NULL, NULL, NULL,
		fieldSizeInBytes,
		NULL,
		0,
		fieldTolerance
	});

	m_DataMapDescSizeInBytes += (count * fieldSizeInBytes);
}

void CPluginEntityFactory::DefineField(const char* name, fieldtype_t fieldType, int numElements)
{
	DefineField(name, fieldType, numElements, FTYPEDESC_SAVE, NULL, 0);
}

void CPluginEntityFactory::DefineKeyField(const char* name, fieldtype_t fieldType, const char* mapname)
{
	DefineField(name, fieldType, 1, FTYPEDESC_KEY | FTYPEDESC_SAVE, strdup(mapname), 0);
}

int CPluginEntityFactory::GetUserEntityDataOffset() const
{
	return GetBaseEntitySize();
}

size_t CPluginEntityFactory::GetUserEntityDataSize() const
{
	if (!m_bHasDataMapDesc)
		return 0;
	
	return m_DataMapDescSizeInBytes;
}

void CPluginEntityFactory::DestroyUserEntityData(CBaseEntity* pEntity)
{
	if (m_pEntityDataMap)
	{
	}

	if (m_pBasePluginEntityFactory)
	{
		m_pBasePluginEntityFactory->DestroyUserEntityData(pEntity);
	}
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