
#include "pluginentityfactory.h"
#include "entityfactorydictionary.h"
#include "cbasenpc_internal.h"

SourceMod::IForward * g_fwdInstalledFactory;
SourceMod::IForward * g_fwdUninstalledFactory;

// TODO: CUtlVector is O(n); use CUtlMap instead mapped to entity references
CUtlVector<CPluginEntityFactory*>* g_PluginEntityFactories;
CUtlVector<PluginFactoryEntityRecord_t>* g_pFactoryEntities;

ISMEntityListener* g_pFactoryEntityListener;

extern ISDKHooks* g_pSDKHooks;

int AddPluginFactoryEntityRecord(CBaseEntityHack* pEntity, CPluginEntityFactory* pFactory)
{
	return g_pFactoryEntities->AddToTail({ pEntity, pFactory, 0 });
}

int FindPluginFactoryEntityRecord(CBaseEntityHack* pEntity)
{
	if (!pEntity)
		return -1;

	for (int i = 0; i < g_pFactoryEntities->Count(); i++)
	{
		PluginFactoryEntityRecord_t *entityRecord = &g_pFactoryEntities->operator[](i);
		if (entityRecord->pEntity == pEntity)
			return i;
	}

	return -1;
}

PluginFactoryEntityRecord_t* GetPluginFactoryEntityRecord(CBaseEntityHack* pEntity)
{
	if (!pEntity)
		return nullptr;

	int index = FindPluginFactoryEntityRecord(pEntity);
	if (index == -1)
		return nullptr;

	return &g_pFactoryEntities->Element(index);
}

void RemovePluginFactoryEntityRecord(CBaseEntityHack* pEntity)
{
	int index = FindPluginFactoryEntityRecord(pEntity);

	if (index != -1)
		g_pFactoryEntities->Remove(index);
}

// Sets pEntity to belong to CPluginEntityFactory pFactory.
void SetPluginEntityFactory(CBaseEntityHack* pEntity, CPluginEntityFactory* pFactory)
{
	int index = FindPluginFactoryEntityRecord(pEntity);
	if (index == -1)
	{
		AddPluginFactoryEntityRecord(pEntity, pFactory);
	}
	else 
	{
		(&g_pFactoryEntities->operator[](index))->pFactory = pFactory;
	}
}

// Returns the CPluginEntityFactory the entity belongs to.
CPluginEntityFactory* GetPluginEntityFactory(CBaseEntityHack* pEntity)
{
	int index = FindPluginFactoryEntityRecord(pEntity);
	if (index == -1)
		return nullptr;
	
	return g_pFactoryEntities->operator[](index).pFactory;
}

abstract_class CPluginEntityFactoryBaseEntity : public CBaseEntityHack
{
public:
	CPluginEntityFactory* GetFactory() { return GetPluginEntityFactory(this); }

	datamap_t* GetFactoryDataDescMap()
	{
		return GetFactory()->m_pEntityDataMap;
	}
};

SH_DECL_MANUALHOOK0(MHook_GetDataDescMap, 0, 0, 0, datamap_t* );

datamap_t* Hook_GetDataDescMap()
{
	CBaseEntityHack *pEntity = META_IFACEPTR(CBaseEntityHack);
	if (!pEntity) RETURN_META_VALUE(MRES_IGNORED, nullptr);

	CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEntity);
	if (!pFactory) RETURN_META_VALUE(MRES_IGNORED, nullptr);

	RETURN_META_VALUE(MRES_SUPERCEDE, pFactory->m_pEntityDataMap);
}

class CPluginEntityFactoryEntityListener : public ISMEntityListener
{
public:
	virtual void OnEntityDestroyed(CBaseEntity* pEntity)
	{
		CBaseEntityHack* pEnt = reinterpret_cast<CBaseEntityHack*>(pEntity);

		CPluginEntityFactory* pFactory = GetPluginEntityFactory(pEnt);
		if (pFactory)
		{
			pFactory->OnRemove(pEnt);
			RemovePluginFactoryEntityRecord(pEnt);
		}
	}
};

bool CPluginEntityFactory::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	g_PluginEntityFactories = new CUtlVector<CPluginEntityFactory*>;
	g_pFactoryEntities = new CUtlVector<PluginFactoryEntityRecord_t>;

	g_fwdInstalledFactory = forwards->CreateForward("CEntityFactory_OnInstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);
	g_fwdUninstalledFactory = forwards->CreateForward("CEntityFactory_OnUninstalled", ET_Ignore, 2, NULL, Param_String, Param_Cell);

	return true;
}

void CPluginEntityFactory::SDK_OnAllLoaded()
{
	if (g_pSDKHooks)
	{
		g_pFactoryEntityListener = new CPluginEntityFactoryEntityListener();
		g_pSDKHooks->AddEntityListener(g_pFactoryEntityListener);
	}

	SH_MANUALHOOK_RECONFIGURE(MHook_GetDataDescMap, CBaseEntityHack::offset_GetDataDescMap, 0, 0);
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
	return g_PluginEntityFactories->HasElement(pAsPluginFactory) ? pAsPluginFactory : nullptr;
}

CPluginEntityFactory::CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor, IPluginFunction *onRemove) :
	m_iClassname(classname),
	m_pPostConstructor(postConstructor),
	m_pOnRemove(onRemove)
{
	m_Derive.m_DeriveFrom = NONE;
	m_bInstalled = false;

	m_pEntityDataMap = nullptr;
	m_bHasDataMapDesc = false;
	m_DataMapDescSizeInBytes = 0;

	m_pBasePluginEntityFactory = nullptr;

	g_PluginEntityFactories->AddToTail(this);
}

CPluginEntityFactory::~CPluginEntityFactory()
{
	Uninstall();

	DestroyDataMap();
	
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

void CPluginEntityFactory::OnRemove(CBaseEntityHack* pEntity)
{
	PluginFactoryEntityRecord_t * pEntityRecord = GetPluginFactoryEntityRecord(pEntity);
	bool bBelongsToFactory = pEntityRecord->pFactory == this;

	if (m_pOnRemove && m_pOnRemove->IsRunnable())
	{
		m_pOnRemove->PushCell(pEntity->entindex());
		m_pOnRemove->Execute(nullptr);
	}

	if (m_pBasePluginEntityFactory)
	{
		m_pBasePluginEntityFactory->OnRemove(pEntity);
	}

	if (pEntityRecord->m_bGetDataDescMapHooked)
	{
		pEntityRecord->m_bGetDataDescMapHooked = false;
		SH_REMOVE_MANUALHOOK(MHook_GetDataDescMap, pEntity, SH_STATIC(Hook_GetDataDescMap), false);
	}

	if (bBelongsToFactory)
	{
		DestroyUserEntityData(pEntity);
	}
}

void CPluginEntityFactory::RemoveEntityRecords()
{
	if (g_pFactoryEntities->Count() == 0)
		return;

	for (int i = g_pFactoryEntities->Count() - 1; i >= 0; i--)
	{
		PluginFactoryEntityRecord_t *entityRecord = &g_pFactoryEntities->operator[](i);
		if (entityRecord->pFactory == this)
			g_pFactoryEntities->Remove(i);
	}
}

void CPluginEntityFactory::GetEntities(CUtlVector< CBaseEntityHack* > *pVec)
{
	for (int i = 0; i < g_pFactoryEntities->Count(); i++)
	{
		PluginFactoryEntityRecord_t *entityRecord = &g_pFactoryEntities->operator[](i);
		if (entityRecord->pFactory == this)
			pVec->AddToTail(entityRecord->pEntity);
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
	if (m_ChildFactories.Count() > 0)
	{
		for (int i = m_ChildFactories.Count() - 1; i >= 0; i--)
		{
			CPluginEntityFactory* pChild = m_ChildFactories[i];
			if (!pChild)
				continue;
			
			pChild->Uninstall();
		}
	}

	// Remove entities. Clean up quickly, we cannot wait for the listener to fire.
	CUtlVector< CBaseEntityHack* > entities;
	GetEntities(&entities);

	RemoveEntityRecords();

	for (int i = 0; i < entities.Count(); i++)
	{
		CBaseEntityHack * pEntity = entities[i];

		OnRemove(pEntity);

		servertools->RemoveEntity(pEntity);
	}

	SetBasePluginEntityFactory(nullptr);

	m_bInstalled = false;

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

size_t CPluginEntityFactory::GetBaseEntitySize()
{
	IEntityFactory *factory = GetBaseFactory();
	if (factory)
	{
		return factory->GetEntitySize();
	}
	else if (m_Derive.m_DeriveFrom == BASECLASS && m_Derive.m_BaseType == ENTITY)
	{
		return CBaseEntityHack::size_of;
	}

	return CBaseEntityHack::size_of;
}

size_t CPluginEntityFactory::GetEntitySize()
{
	return GetBaseEntitySize() + GetUserEntityDataSize();
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
		
		pEnt = reinterpret_cast<CBaseEntityHack*>(pNet->GetBaseEntity());
	}
	else if (m_Derive.m_DeriveFrom == BASECLASS && m_Derive.m_BaseType == ENTITY)
	{
		pEnt = (CBaseEntityHack*)engine->PvAllocEntPrivateData(CBaseEntityHack::size_of);
		CBaseEntityHack::CBaseEntity_Ctor.operator()(pEnt, m_Derive.m_bBaseEntityServerOnly);
		pEnt->PostConstructor(classname);
	}
	else
	{
		return nullptr;
	}

	SetPluginEntityFactory(pEnt, this);

	if (m_bHasDataMapDesc)
	{
		if (!m_pEntityDataMap)
			CreateDataMap( pEnt->GetDataDescMap() );
	}

	if (m_pEntityDataMap)
	{
		// SH manual hook to GetDataDescMap; can also check if hooked
		// already; we only need to hook it once.
		PluginFactoryEntityRecord_t * pEntityRecord = GetPluginFactoryEntityRecord(pEnt);
		if (!pEntityRecord->m_bGetDataDescMapHooked)
		{
			pEntityRecord->m_bGetDataDescMapHooked = true;
			SH_ADD_MANUALHOOK(MHook_GetDataDescMap, pEnt, SH_STATIC(Hook_GetDataDescMap), false);
		}
		
		// Not sure if zeroing out the user field data is wise, but it's better than leaving it uninitialized.
		memset( ((uint8_t*)pEnt) + GetUserEntityDataOffset(), 0, GetUserEntityDataSize() );
	}

	if (m_pPostConstructor && m_pPostConstructor->IsRunnable())
	{
		m_pPostConstructor->PushCell(pEnt->entindex());
		m_pPostConstructor->Execute(nullptr);
	}

	return pEnt->NetworkProp();
}

bool CPluginEntityFactory::CanUseDataMap()
{
	return true;
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

void CPluginEntityFactory::DestroyDataMapTypeDescriptor(typedescription_t *desc)
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
	for (int i = 0; i < m_vecEntityDataTypeDescriptors.Count(); i++)
	{
		DestroyDataMapTypeDescriptor( &m_vecEntityDataTypeDescriptors[i] );
	}

	m_vecEntityDataTypeDescriptors.Purge();

	m_iDataClassname = dataClassName;
	m_DataMapDescSizeInBytes = 0;
	m_bHasDataMapDesc = true;
}

void CPluginEntityFactory::EndDataMapDesc()
{
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
	int fieldOffset = GetUserEntityDataOffset() + (int)GetUserEntityDataSize();
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

	m_DataMapDescSizeInBytes += fieldSizeInBytes;
}

void CPluginEntityFactory::DefineField(const char* name, fieldtype_t fieldType)
{
	DefineField(name, fieldType, 1, FTYPEDESC_SAVE, NULL, 0);
}

void CPluginEntityFactory::DefineKeyField(const char* name, fieldtype_t fieldType, const char* mapname)
{
	DefineField(name, fieldType, 1, FTYPEDESC_KEY | FTYPEDESC_SAVE, strdup(mapname), 0);
}

int CPluginEntityFactory::GetUserEntityDataOffset()
{
	return GetBaseEntitySize();
}

size_t CPluginEntityFactory::GetUserEntityDataSize()
{
	return m_DataMapDescSizeInBytes;
}

void CPluginEntityFactory::DestroyUserEntityData(CBaseEntityHack* pEntity)
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