#ifndef CPLUGINENTITYFACTORY_H
#define CPLUGINENTITYFACTORY_H

#include <itoolentity.h>
#include <tier0/platform.h>
#include <IEngineTrace.h>
#include <shareddefs.h>
#include <util.h>

#include "idatamapcontainer.h"
#include "helpers.h"

class CBaseEntity;
class CPluginEntityFactory;
class CBaseNPCPluginActionFactory;

// To be used with READHANDLE macro
extern HandleType_t g_PluginEntityFactoryHandle;

enum PluginEntityFactoryBaseClass_t
{
	FACTORYBASECLASS_BASEENTITY,
	FACTORYBASECLASS_MAX
};

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

class CPluginEntityFactories : public IPluginsListener,
	public IHandleTypeDispatch
{
public:
	CPluginEntityFactories();

	// IPluginsListener
	virtual void OnPluginUnloaded( IPlugin* plugin ) override final;

	// IHandleTypeDispatch
	virtual void OnHandleDestroy(HandleType_t type, void * object) override final;

	bool Init( IGameConfig* config, char* error, size_t maxlength );
	void OnCoreMapEnd();
	void SDK_OnAllLoaded();
	void SDK_OnUnload();

	HandleType_t GetFactoryType() const { return m_FactoryType; }
	CPluginEntityFactory* GetFactory( CBaseEntity* pEntity );
	CPluginEntityFactory* GetFactoryFromHandle( Handle_t handle, HandleError *err = nullptr );
	int GetInstalledFactoryHandles( Handle_t* pHandleArray, size_t arraySize );
	void OnFactoryCreated( CPluginEntityFactory* pFactory );
	void OnFactoryDestroyed( CPluginEntityFactory* pFactory );
	void OnFactoryInstall( CPluginEntityFactory* pFactory );
	void OnFactoryUninstall( CPluginEntityFactory* pFactory );

	PluginFactoryEntityRecord_t* FindRecord( CBaseEntity* pEntity, bool create=false );
	void RemoveRecord( CBaseEntity* pEntity );
	void GetEntitiesOfFactory( CPluginEntityFactory* pFactory, CUtlVector< CBaseEntity* > &vector );
	CPluginEntityFactory* ToPluginEntityFactory( IEntityFactory* pFactory );

	size_t GetBaseClassSize( PluginEntityFactoryBaseClass_t classType ) const { 
		return m_BaseClassSizes[ classType ];
	};

	datamap_t* Hook_GetDataDescMap();
	void Hook_UpdateOnRemove();
#ifdef __linux__
	void Hook_EntityDestructor( void );
#else
	void Hook_EntityDestructor( unsigned int flags );
#endif

private:
	HandleType_t m_FactoryType;
	IForward * m_fwdInstalledFactory;
	IForward * m_fwdUninstalledFactory;

	size_t m_BaseClassSizes[ FACTORYBASECLASS_MAX ];
	CUtlVector< CPluginEntityFactory* > m_Factories;
	CUtlMap< cell_t, PluginFactoryEntityRecord_t > m_Records;
};

extern CPluginEntityFactories* g_pPluginEntityFactories;

class CPluginEntityFactory : public IEntityFactory, public IEntityDataMapContainer
{    
public:
	std::string m_iClassname;
	IPlugin* m_pPlugin;
	IPluginFunction *m_pPostConstructor;
	IPluginFunction *m_pOnRemove;
	bool m_bInstalled;

	Handle_t m_Handle;

	CPluginEntityFactory( IPlugin* plugin, const char* classname, IPluginFunction *postConstructor=nullptr, IPluginFunction *onRemove=nullptr );
	virtual ~CPluginEntityFactory();

	bool Install();
	void Uninstall();

	// IEntityFactory
	virtual IServerNetworkable* Create(const char*) override final;
	virtual size_t GetEntitySize() override final { return GetBaseEntitySize() + GetDataDescSize(); };
	virtual void Destroy(IServerNetworkable*) override final;

	void OnRemove(CBaseEntity* pEntity);

	// Not the entity listener; when the entity's destructor is called.
	void OnDestroy(CBaseEntity* pEntity);

protected:
	enum PluginEntityFactoryDeriveType_t
	{
		DERIVETYPE_NONE,
		DERIVETYPE_BASECLASS,
		DERIVETYPE_CBASENPC,
		DERIVETYPE_CLASSNAME,
		DERIVETYPE_HANDLE,
		DERIVETYPE_CONFIG,
		DERIVETYPE_MAX
	};

	typedef void* (CBaseEntity::*RawEntityConstructor)(void);

	struct PluginEntityFactoryDeriveInfo_t
	{
		PluginEntityFactoryDeriveType_t m_DeriveFrom;

		union
		{
			PluginEntityFactoryBaseClass_t m_BaseType;
			Handle_t m_BaseFactoryHandle;
			RawEntityConstructor m_pConstructorFunc;
		};
		
		std::string m_iBaseClassname;

		union
		{
			bool m_bBaseEntityServerOnly;
			size_t m_iRawEntitySize;
		};
	} m_Derive;

	CBaseNPCPluginActionFactory* m_pBaseNPCInitialActionFactory;

public:
	bool DoesNotDerive() const { return (m_Derive.m_DeriveFrom == DERIVETYPE_NONE); }

	void DeriveFromBaseEntity(bool bServerOnly=false);
	void DeriveFromNPC();
	void DeriveFromClass(const char* classname);
	void DeriveFromHandle(Handle_t handle);
	bool DeriveFromConf(size_t entitySize, IGameConfig* config, int type, const char* name);

	CBaseNPCPluginActionFactory* GetBaseNPCInitialActionFactory() const { return m_pBaseNPCInitialActionFactory; }
	void SetBaseNPCInitialActionFactory( CBaseNPCPluginActionFactory* pFactory ) { m_pBaseNPCInitialActionFactory = pFactory; }

protected:
	bool m_bIsAbstract;

public:
	bool IsAbstract() const { return m_bIsAbstract; }
	void SetAbstract(bool abstract) { m_bIsAbstract = abstract; }

protected:
	IEntityFactory* m_pBaseFactory;
	CUtlVector< CPluginEntityFactory* > m_DerivedFactories;

	IEntityFactory* FindBaseFactory() const;

public:
	// The entity factory that is called while creating this factory's entity.
	// The base factory is only set when the factory is installed to
	// establish a clear and stable hierarchy.
	IEntityFactory * GetBaseFactory() const { return m_pBaseFactory; };
	
	void SetBaseFactory(IEntityFactory* pBaseFactory);

	bool IsBaseFactoryRequired() const;

	// If uses a base factory, then returns the size of the entity created by
	// the base factory. If not, returns the size of the entity created by
	// this factory.
	size_t GetBaseEntitySize() const;

protected:

	// The factory that first called Create in the entity creation chain.
	CPluginEntityFactory* m_pCreatingFactory;

public:

	// Collects all entities that were created by this factory.
	// This does not include entities that are created by factories that derive from this one.
	void GetEntities( CUtlVector< CBaseEntity* > &vector );

	void RemoveAllEntities();

private:
	// Datamap

	std::string m_iDataClassname;
	int m_iDataMapStartOffset;

public:
	virtual const char* GetDataDescMapClass() const override final { return m_iDataClassname.c_str(); }
	virtual int GetDataDescOffset() const override final { return m_iDataMapStartOffset; }

	using IEntityDataMapContainer::BeginDataDesc;

	// Initializes the list of type descriptors to be used by the factory's datamap.
	bool BeginDataDesc(const char* dataClassName);

	IEntityDataMapInputFuncDelegate * CreateInputFuncDelegate(IPluginFunction* pCallback, fieldtype_t fieldType);

protected:
	void CreateUserEntityData(CBaseEntity* pEntity);

	void DestroyUserEntityData(CBaseEntity* pEntity);

public:
	static CPluginEntityFactory* ToPluginEntityFactory( IEntityFactory* pFactory );

	friend class CPluginEntityFactories;
};

#endif