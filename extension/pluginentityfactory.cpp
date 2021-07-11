
#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"
#include "cbasenpc_internal.h"
#include "baseentityoutput.h"
#include "sh_pagealloc.h"

SH_DECL_MANUALHOOK0(MHook_GetDataDescMap, 0, 0, 0, datamap_t* );
SH_DECL_MANUALHOOK0_void(MHook_UpdateOnRemove, 0, 0, 0 );

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
	datamap_t* m_pDataMap;
	bool m_bUpdateOnRemoveHooked;
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
			index = g_EntityRecords.Insert(key, { pEntity, nullptr, false, nullptr, false });
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

void Hook_UpdateOnRemove()
{
	CBaseEntity *pEntity = META_IFACEPTR(CBaseEntity);
	if (!pEntity) RETURN_META(MRES_IGNORED);

	CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEntity);
	if (!pFactory) RETURN_META(MRES_IGNORED);

	pFactory->OnRemove(pEntity);

	RETURN_META(MRES_IGNORED);
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
	SH_MANUALHOOK_RECONFIGURE(MHook_GetDataDescMap, CBaseEntityHack::offset_GetDataDescMap, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(MHook_UpdateOnRemove, CBaseEntityHack::offset_UpdateOnRemove, 0, 0);
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
	m_iClassname(classname),
	m_pPostConstructor(postConstructor),
	m_pOnRemove(onRemove)
{
	m_Derive.m_DeriveFrom = DERIVETYPE_NONE;
	m_bInstalled = false;

	m_Handle = BAD_HANDLE;

	m_pCreatingFactory = nullptr;

	m_pEntityDataMap = nullptr;
	m_bHasDataMapDesc = false;
	m_DataMapDescSizeInBytes = 0;
	m_iDataMapStartOffset = 0;

	m_bIsAbstract = false;
	m_pBaseFactory = nullptr;

	g_PluginEntityFactories.AddToTail(this);
}

CPluginEntityFactory::~CPluginEntityFactory()
{
	Uninstall();

	DestroyDataMap();

	m_pEntityInputFuncDelegates.PurgeAndDeleteElements();
	
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
	PluginFactoryEntityRecord_t * pEntityRecord = GetPluginFactoryEntityRecord(pEntity);
	bool bBelongsToFactory = pEntityRecord->pFactory == this;

	if (m_pOnRemove && m_pOnRemove->IsRunnable())
	{
		m_pOnRemove->PushCell(gamehelpers->EntityToBCompatRef(pEntity));
		m_pOnRemove->Execute(nullptr);
	}

	DestroyUserEntityData(pEntity);

	CPluginEntityFactory* pBasePluginFactory = ToPluginEntityFactory(GetBaseFactory());
	if (pBasePluginFactory)
	{
		pBasePluginFactory->OnRemove(pEntity);
	}

	if (bBelongsToFactory)
	{
		// Unhook here after all OnRemove calls have been made.
		if (pEntityRecord->m_bDataMapHooked)
		{
			SH_REMOVE_MANUALHOOK(MHook_GetDataDescMap, pEntity, SH_STATIC(Hook_GetDataDescMap), false);
			pEntityRecord->m_bDataMapHooked = false;
		}

		if (pEntityRecord->m_bUpdateOnRemoveHooked)
		{
			SH_REMOVE_MANUALHOOK(MHook_UpdateOnRemove, pEntity, SH_STATIC(Hook_UpdateOnRemove), false);
			pEntityRecord->m_bUpdateOnRemoveHooked = false;
		}

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
		servertools->RemoveEntity(entities[i]);
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
		CBaseEntityHack::CBaseEntity_Ctor(pEnt, m_Derive.m_bBaseEntityServerOnly);
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

		PluginFactoryEntityRecord_t * pEntityRecord = GetPluginFactoryEntityRecord(pEnt, true);
		if (!pEntityRecord->pFactory)
			pEntityRecord->pFactory = m_pCreatingFactory;

		if (m_pEntityDataMap)
		{
			pEntityRecord->m_pDataMap = m_pEntityDataMap;

			if (!pEntityRecord->m_bDataMapHooked)
			{
				SH_ADD_MANUALHOOK(MHook_GetDataDescMap, pEnt, SH_STATIC(Hook_GetDataDescMap), false);
				pEntityRecord->m_bDataMapHooked = true;
			}

			CreateUserEntityData(pEnt);
		}

		if (!pEntityRecord->m_bUpdateOnRemoveHooked)
		{
			SH_ADD_MANUALHOOK(MHook_UpdateOnRemove, pEnt, SH_STATIC(Hook_UpdateOnRemove), false);
			pEntityRecord->m_bUpdateOnRemoveHooked = true;
		}

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

bool CPluginEntityFactory::CanUseDataMap() const
{
	return true;
}

datamap_t* CPluginEntityFactory::GetDataMap() const
{
	if (m_pEntityDataMap)
		return m_pEntityDataMap;
	
	CPluginEntityFactory* pBasePluginFactory = ToPluginEntityFactory(GetBaseFactory());
	if (pBasePluginFactory)
		return pBasePluginFactory->GetDataMap();

	return nullptr;
}

datamap_t* CPluginEntityFactory::CreateDataMap(datamap_t* pBaseMap)
{
	if (!m_pEntityDataMap)
	{
		m_pEntityDataMap = new datamap_t;
		m_pEntityDataMap->baseMap = pBaseMap;
		m_pEntityDataMap->dataClassName = m_iDataClassname.c_str();
		m_pEntityDataMap->packed_offsets_computed = false;
		m_pEntityDataMap->packed_size = 0;

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

	delete[] m_pEntityDataMap->dataDesc;
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

bool CPluginEntityFactory::BeginDataMapDesc(const char* dataClassName)
{
	IEntityFactory *pBaseFactory = FindBaseFactory();
	if (!pBaseFactory && IsBaseFactoryRequired())
		return false;

	// Don't use GetBaseEntitySize() if base factory is required because 
	// the base factory hasn't been set yet. The base factory is only set when
	// the factory is installed.
	m_iDataMapStartOffset = pBaseFactory ? pBaseFactory->GetEntitySize() : GetBaseEntitySize();

	DestroyDataMap();

	m_bHasDataMapDesc = false;

	m_pEntityInputFuncDelegates.PurgeAndDeleteElements();

	for (int i = 0; i < m_vecEntityDataTypeDescriptors.Count(); i++)
	{
		DestroyDataMapTypeDescriptor( &m_vecEntityDataTypeDescriptors[i] );
	}

	m_vecEntityDataTypeDescriptors.Purge();

	m_iDataClassname = dataClassName;
	m_DataMapDescSizeInBytes = 0;

	return true;
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
	int fieldSizeInBytes = g_DataMapDescFieldSizes[fieldType] * count;

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

	m_DataMapDescSizeInBytes += fieldSizeInBytes;
}

void CPluginEntityFactory::DefineField(const char* name, fieldtype_t fieldType, int numElements)
{
	DefineField(name, fieldType, numElements, FTYPEDESC_SAVE, NULL, 0);
}

void CPluginEntityFactory::DefineKeyField(const char* name, fieldtype_t fieldType, const char* mapname)
{
	DefineField(name, fieldType, 1, FTYPEDESC_KEY | FTYPEDESC_SAVE, strdup(mapname), 0);
}

void CPluginEntityFactory::DefineInputFunc(const char* name, fieldtype_t fieldType, const char* mapname, IPluginFunction *inputFunc)
{
	name = name ? strdup(name) : NULL;
	mapname = mapname ? strdup(mapname) : NULL;

	typedescription_t typeDesc = { 
		fieldType, 
		name, 
		{ 0, 0 }, 
		1,
		FTYPEDESC_INPUT,
		mapname,
		NULL, 
		NULL, NULL,
		0,
		NULL,
		0,
		0
	};

	InputFuncDelegate* pDelegate = new InputFuncDelegate(inputFunc);
	m_pEntityInputFuncDelegates.AddToTail(pDelegate);
	
	// this shuts up the compiler
	*(uint32_t*)(&(typeDesc.inputFunc)) = (uint32_t)pDelegate->m_pInputFuncPtr;

	m_vecEntityDataTypeDescriptors.AddToTail(typeDesc);
}

void CPluginEntityFactory::DefineOutput(const char* name, const char* mapname)
{
	int fieldOffset = GetUserEntityDataOffset() + m_DataMapDescSizeInBytes;
	int fieldSizeInBytes = sizeof(CBaseEntityOutputHack);

	name = name ? strdup(name) : NULL;
	mapname = mapname ? strdup(mapname) : NULL;

	m_vecEntityDataTypeDescriptors.AddToTail({ 
		FIELD_CUSTOM, 
		name, 
		{ fieldOffset, 0 }, 
		1,
		FTYPEDESC_OUTPUT | FTYPEDESC_SAVE | FTYPEDESC_KEY,
		mapname,
		eventFuncs, 
		NULL, NULL,
		0,
		NULL,
		0,
		0
	});

	m_DataMapDescSizeInBytes += fieldSizeInBytes;
}

int CPluginEntityFactory::GetUserEntityDataOffset() const
{
	return m_iDataMapStartOffset;
}

size_t CPluginEntityFactory::GetUserEntityDataSize() const
{
	if (!m_bHasDataMapDesc)
		return 0;
	
	return m_DataMapDescSizeInBytes;
}

void CPluginEntityFactory::CreateUserEntityData(CBaseEntity* pEntity)
{
	if (!m_pEntityDataMap)
		return;
	
	for (int i = 0; i < m_pEntityDataMap->dataNumFields; i++)
	{
		typedescription_t * pTypeDesc = &m_pEntityDataMap->dataDesc[i];
		if ( ( pTypeDesc->fieldType == FIELD_CUSTOM ) && ( pTypeDesc->flags & FTYPEDESC_OUTPUT ) )
		{
			CBaseEntityOutputHack *pOutput = (CBaseEntityOutputHack *)((uint8_t*)pEntity + pTypeDesc->fieldOffset[0]);
			pOutput->Init();
		}
	}
}

void CPluginEntityFactory::DestroyUserEntityData(CBaseEntity* pEntity)
{
	if (!m_pEntityDataMap)
		return;

	for (int i = 0; i < m_pEntityDataMap->dataNumFields; i++)
	{
		typedescription_t * pTypeDesc = &m_pEntityDataMap->dataDesc[i];
		if ( ( pTypeDesc->fieldType == FIELD_CUSTOM ) && ( pTypeDesc->flags & FTYPEDESC_OUTPUT ) )
		{
			CBaseEntityOutputHack *pOutput = (CBaseEntityOutputHack *)((uint8_t*)pEntity + pTypeDesc->fieldOffset[0]);
			pOutput->Destroy();
		}
	}
}

void CPluginEntityFactory::Destroy(IServerNetworkable* pNetworkable)
{
	if (pNetworkable)
	{
		pNetworkable->Release();
	}
}

SourceHook::CPageAlloc g_InputFuncAlloc;

void CPluginEntityFactory::InputFuncDelegate::OnInput(InputFuncDelegate* pDelegate, CBaseEntity* pEntity, inputdata_t &data)
{
	IPluginFunction* m_pCallback = pDelegate->m_pCallback;

	if (m_pCallback && m_pCallback->IsRunnable())
	{
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
}

CPluginEntityFactory::InputFuncDelegate::InputFuncDelegate(IPluginFunction* pCallback)
	: m_pCallback(pCallback)
{
	uint32_t thisAddr = (uint32_t)this;
	uint32_t callFuncAddr = (uint32_t)(&InputFuncDelegate::OnInput);

	uint8_t funcBytes[] = { 
#ifdef WIN32
		// MSVC __thiscall
		0x55,									// push ebp
		0x89, 0xe5,								// mov ebp, esp
		0x57,									// push edi
		0x51,									// push ecx
		0x8B, 0x7D, 0x08,						// mov edi, [ebp+8] (inputdata_t*)
		0x57,									// push edi
		0x51,									// push ecx (CBaseEntity*)
		0x68, 0, 0, 0, 0,						// push thisAddr
		0xBA, 0, 0, 0, 0, 						// mov edx, callFuncAddr
		0xFF, 0xD2,								// call edx
		0x83, 0xC4, 0x0C,						// add esp,12
		0x59,									// pop ecx
		0x5f,									// pop edi
		0x89, 0xec,								// mov esp, ebp
		0x5d,									// pop ebp
		0xc2, 0x04, 0x00						// ret 4
#else
		// GCC __thiscall
		0x55,									// push ebp
		0x89, 0xE5,								// mov ebp, esp
		0x52,									// push edx
		0x57,									// push edi
		0x8B, 0x7D, 0x0C,						// mov edi, [ebp+12] (inputdata_t*)
		0x57,									// push edi
		0x8B, 0x7D, 0x08,						// mov edi, [ebp+8] (CBaseEntity*)
		0x57,									// push edi
		0x68, 0, 0, 0, 0,						// push thisAddr
		0xBA, 0, 0, 0, 0,						// mov edx, callFuncAddr
		0xFF, 0xD2,								// call edx
		0x83, 0xC4, 0x0C,						// add esp,12
		0x5A,									// pop edi
		0x5F,									// pop edx
		0x89, 0xEC,								// mov esp, ebp
		0x5D,									// pop ebp
		0xC3									// ret
#endif
	};

	m_iInputFuncSize = sizeof(funcBytes);
	m_pInputFuncPtr = g_InputFuncAlloc.Alloc(m_iInputFuncSize);

	if (m_pInputFuncPtr)
	{
		g_InputFuncAlloc.SetRW(m_pInputFuncPtr);

#ifdef WIN32
		*((uint32_t*)(&funcBytes[11])) = thisAddr;
		*((uint32_t*)(&funcBytes[16])) = callFuncAddr;
#else
		*((uint32_t*)(&funcBytes[14])) = thisAddr;
		*((uint32_t*)(&funcBytes[19])) = callFuncAddr;
#endif

		memcpy(m_pInputFuncPtr, funcBytes, m_iInputFuncSize);

		g_InputFuncAlloc.SetRE(m_pInputFuncPtr);
	}
}

CPluginEntityFactory::InputFuncDelegate::~InputFuncDelegate()
{
	if (m_pInputFuncPtr)
	{
		g_InputFuncAlloc.Free(m_pInputFuncPtr);
		m_pInputFuncPtr = nullptr;
	}
}

HandleType_t g_PluginEntityFactoryHandle;
CPluginEntityFactoryHandler g_PluginEntityFactoryHandler;

void CPluginEntityFactoryHandler::OnHandleDestroy(HandleType_t type, void * object)
{
	CPluginEntityFactory* factory = (CPluginEntityFactory*)object;

	delete factory;
}