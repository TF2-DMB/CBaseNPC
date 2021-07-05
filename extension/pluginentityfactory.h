#ifndef CPLUGINENTITYFACTORY_H
#define CPLUGINENTITYFACTORY_H

#include <itoolentity.h>
#include <tier0/platform.h>
#include <IEngineTrace.h>
#include <shareddefs.h>
#include <util.h>
#include <ISDKHooks.h>

#include "helpers.h"

enum PluginEntityFactoryDeriveType_t
{
	NONE,
	BASECLASS,
	CLASSNAME
};

enum PluginEntityFactoryDeriveFromBaseType_t
{
	ENTITY,
	NPC
};

struct PluginEntityFactoryDeriveInfo_t
{
	PluginEntityFactoryDeriveType_t m_DeriveFrom;

	PluginEntityFactoryDeriveFromBaseType_t m_BaseType;
	std::string m_iBaseClassname;
	bool m_bBaseEntityServerOnly;
};

class CBaseEntity;
class CPluginEntityFactory;

struct PluginFactoryEntityRecord_t
{
	CBaseEntity* pEntity;
	CPluginEntityFactory* pFactory;
	bool m_bDataMapHooked;
	bool m_bServerClassHooked;
};

// Returns the CPluginEntityFactory the entity belongs to.
CPluginEntityFactory* GetPluginEntityFactory(CBaseEntity* pEntity);

class CPluginEntityFactory : public IEntityFactory
{    
public:
	std::string m_iClassname;
	IPluginFunction *m_pPostConstructor;
	IPluginFunction *m_pOnRemove;
	PluginEntityFactoryDeriveInfo_t m_Derive;
	bool m_bInstalled;

	Handle_t m_Handle;

	CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor=nullptr, IPluginFunction *onRemove=nullptr);
	~CPluginEntityFactory();
	void Install();
	void Uninstall();
	virtual IServerNetworkable* Create(const char*) override final;
	virtual size_t GetEntitySize() override final;
	virtual void Destroy(IServerNetworkable*) override final;

	void OnRemove(CBaseEntity* pEntity);

	// The size of the entity created by the factory without overriding the datamap.
	size_t GetBaseEntitySize();

protected:

	CPluginEntityFactory* m_pCreatingFactory;



public:

	// Collects all entities that were created by this factory.
	// This does not include entities that are created by factories derived from this factory.
	void GetEntities(CUtlVector< CBaseEntity* > *pVec);

	// Whether or not this factory is allowed to use a custom datamap.
	bool CanUseDataMap();

private:
	std::string m_iDataClassname;

	ServerClass* m_pEntityServerClass;

public:

	ServerClass* GetServerClass() { return m_pEntityServerClass; }

private:
	ServerClass* CreateServerClass(ServerClass* pBaseServerClass);

	void DestroyServerClass();

private:
	datamap_t* m_pEntityDataMap;

	size_t m_DataMapDescSizeInBytes;
	CUtlVector<typedescription_t> m_vecEntityDataTypeDescriptors;

	// Creates the datamap used by entities created by the factory if it doesn't already exist.
	datamap_t* CreateDataMap(datamap_t* pBaseMap);

	// Destroys the user-defined type descriptor.
	void DestroyDataMapTypeDescriptor(typedescription_t *desc);

	// Destroys the entity datamap.
	void DestroyDataMap();

public:
	bool m_bHasDataMapDesc;

	// The datamap used by entities created by this factory.
	datamap_t* GetDataMap();

	// Initializes the list of type descriptors to be used by the factory's datamap.
	void BeginDataMapDesc(const char* dataClassName);

	// Finalizes the entity factory's type descriptor list.
	void EndDataMapDesc();

protected:
	void DefineField(const char* name, fieldtype_t fieldType, unsigned short count, short flags, const char* externalName, float fieldTolerance);

public:
	// Adds a field type descriptor to the type descriptor list with the specified name and type.
	void DefineField(const char* name, fieldtype_t fieldType, int numElements=1);

	// Adds a field type descriptor to the type descriptor list with the specified name and type, additionally with key name to be used by maps/fgd.
	void DefineKeyField(const char* name, fieldtype_t fieldType, const char* mapname);

	// The size of the fields in the factory's type descriptor list, in bytes.
	size_t GetUserEntityDataSize();

	// The starting offset of the user-defined datamap fields stored on the entity.
	int GetUserEntityDataOffset();

protected:

	void DestroyUserEntityData(CBaseEntity* pEntity);

public:

	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	static void SDK_OnAllLoaded();

	static CPluginEntityFactory* ToPluginEntityFactory( IEntityFactory* pFactory );

protected:
	// The entity factory that is called while creating this factory's entity.
	IEntityFactory * GetBaseFactory();

	CPluginEntityFactory* m_pBasePluginEntityFactory;
	CUtlVector< CPluginEntityFactory* > m_ChildFactories;

	void SetBasePluginEntityFactory(CPluginEntityFactory* pFactory);

	// Removes all records of entities that were created by this factory.
	void RemoveEntityRecords();

private:
	static void OnFactoryInstall( CPluginEntityFactory* pFactory );
	static void OnFactoryUninstall( CPluginEntityFactory* pFactory );
};

class CPluginEntityFactoryHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void * object);
};

extern HandleType_t g_PluginEntityFactoryHandle;
extern CPluginEntityFactoryHandler g_PluginEntityFactoryHandler;

#endif