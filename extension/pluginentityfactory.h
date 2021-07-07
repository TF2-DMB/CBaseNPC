#ifndef CPLUGINENTITYFACTORY_H
#define CPLUGINENTITYFACTORY_H

#include <itoolentity.h>
#include <tier0/platform.h>
#include <IEngineTrace.h>
#include <shareddefs.h>
#include <util.h>

#include "helpers.h"

class CBaseEntity;
class CPluginEntityFactory;

// Returns the CPluginEntityFactory the entity belongs to.
CPluginEntityFactory* GetPluginEntityFactory(CBaseEntity* pEntity);

class CPluginEntityFactory : public IEntityFactory
{    
public:
	std::string m_iClassname;
	IPluginFunction *m_pPostConstructor;
	IPluginFunction *m_pOnRemove;
	bool m_bInstalled;

	Handle_t m_Handle;

	CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor=nullptr, IPluginFunction *onRemove=nullptr);
	~CPluginEntityFactory();

	void Install();
	void Uninstall();

	// IEntityFactory
	virtual IServerNetworkable* Create(const char*) override final;
	virtual size_t GetEntitySize() override final;
	virtual void Destroy(IServerNetworkable*) override final;

	void OnRemove(CBaseEntity* pEntity);

protected:
	enum PluginEntityFactoryDeriveType_t
	{
		DERIVETYPE_NONE,
		DERIVETYPE_BASECLASS,
		DERIVETYPE_CBASENPC,
		DERIVETYPE_CLASSNAME,
		DERIVETYPE_MAX
	};

	enum PluginEntityFactoryDeriveFromBaseType_t
	{
		DERIVEBASECLASSTYPE_ENTITY,
		DERIVEBASECLASSTYPE_MAX
	};

	struct PluginEntityFactoryDeriveInfo_t
	{
		PluginEntityFactoryDeriveType_t m_DeriveFrom;

		PluginEntityFactoryDeriveFromBaseType_t m_BaseType;
		std::string m_iBaseClassname;
		bool m_bBaseEntityServerOnly;
	} m_Derive;

	static size_t m_DeriveBaseClassSizes[DERIVEBASECLASSTYPE_MAX];

public:

	bool DoesNotDerive() const { return (m_Derive.m_DeriveFrom == DERIVETYPE_NONE); }

	void DeriveFromBaseEntity(bool bServerOnly=false);
	void DeriveFromNPC();
	void DeriveFromClass(const char* classname);

	// The size of the entity created by the factory without adding user datamap fields.
	size_t GetBaseEntitySize() const;

protected:

	// The factory that first called Create in the entity creation chain.
	CPluginEntityFactory* m_pCreatingFactory;

public:

	// Collects all entities that were created by this factory.
	// This does not include entities that are created by factories that derive from this one.
	void GetEntities(CUtlVector< CBaseEntity* > *pVec) const;

	// Whether or not this factory is allowed to use a custom user datamap.
	bool CanUseDataMap() const;

private:
	std::string m_iDataClassname;

	datamap_t* m_pEntityDataMap;

	size_t m_DataMapDescSizeInBytes;
	CUtlVector<typedescription_t> m_vecEntityDataTypeDescriptors;

	// Creates the datamap used by entities created by the factory if it doesn't already exist.
	datamap_t* CreateDataMap(datamap_t* pBaseMap);

	// Destroys a user-defined type descriptor.
	void DestroyDataMapTypeDescriptor(typedescription_t *desc) const;

	// Destroys the entity datamap.
	void DestroyDataMap();

public:
	bool m_bHasDataMapDesc;

	// The datamap used by entities created by this factory.
	datamap_t* GetDataMap() const;

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

	void DefineOutput(const char* name, const char* externalName);

	// The size of the fields in the factory's type descriptor list, in bytes.
	size_t GetUserEntityDataSize() const;

	// The starting offset of the user-defined datamap fields stored on the entity.
	int GetUserEntityDataOffset() const;

protected:
	void CreateUserEntityData(CBaseEntity* pEntity);

	void DestroyUserEntityData(CBaseEntity* pEntity);

public:

	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	static void SDK_OnAllLoaded();

	static void SDK_OnUnload();

	static void OnEntityDestroyed( CBaseEntity* pEntity );

	static CPluginEntityFactory* ToPluginEntityFactory( IEntityFactory* pFactory );

	static int GetInstalledFactoryHandles( Handle_t* pHandleArray, int arraySize );

	// The entity factory that is called while creating this factory's entity.
	IEntityFactory * GetBaseFactory() const;

protected:
	CPluginEntityFactory* m_pBasePluginEntityFactory;
	CUtlVector< CPluginEntityFactory* > m_DerivedFactories;

public:
	// Returns the base factory as CPluginEntityFactory*, or nullptr if not a CPluginEntityFactory*.
	CPluginEntityFactory* GetBasePluginEntityFactory() const { return m_pBasePluginEntityFactory; }

private:

	void SetBasePluginEntityFactory(CPluginEntityFactory* pFactory);

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