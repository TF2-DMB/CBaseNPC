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

// Returns the CPluginEntityFactory the entity belongs to.
CPluginEntityFactory* GetPluginEntityFactory(CBaseEntity* pEntity);

class CPluginEntityFactory : public IEntityFactory, public IEntityDataMapContainer
{    
public:
	std::string m_iClassname;
	IPluginFunction *m_pPostConstructor;
	IPluginFunction *m_pOnRemove;
	bool m_bInstalled;

	Handle_t m_Handle;

	CPluginEntityFactory(const char* classname, IPluginFunction *postConstructor=nullptr, IPluginFunction *onRemove=nullptr);
	virtual ~CPluginEntityFactory();

	bool Install();
	void Uninstall();

	// IEntityFactory
	virtual IServerNetworkable* Create(const char*) override final;
	virtual size_t GetEntitySize() override final { return GetBaseEntitySize() + GetDataDescSize(); };
	virtual void Destroy(IServerNetworkable*) override final;

	void OnRemove(CBaseEntity* pEntity);
	void OnRemovePost(CBaseEntity* pEntity);

protected:
	enum PluginEntityFactoryDeriveType_t
	{
		DERIVETYPE_NONE,
		DERIVETYPE_BASECLASS,
		DERIVETYPE_CBASENPC,
		DERIVETYPE_CLASSNAME,
		DERIVETYPE_HANDLE,
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

		union
		{
			PluginEntityFactoryDeriveFromBaseType_t m_BaseType;
			Handle_t m_BaseFactoryHandle;
		};
		
		std::string m_iBaseClassname;
		bool m_bBaseEntityServerOnly;

	} m_Derive;

	static size_t m_DeriveBaseClassSizes[DERIVEBASECLASSTYPE_MAX];

	CBaseNPCPluginActionFactory* m_pBaseNPCInitialActionFactory;

public:
	bool DoesNotDerive() const { return (m_Derive.m_DeriveFrom == DERIVETYPE_NONE); }

	void DeriveFromBaseEntity(bool bServerOnly=false);
	void DeriveFromNPC();
	void DeriveFromClass(const char* classname);
	void DeriveFromHandle(Handle_t handle);

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
	void GetEntities(CUtlVector< CBaseEntity* > *pVec) const;

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

	IEntityDataMapInputFuncDelegate * CreateInputFuncDelegate(IPluginFunction* pCallback);

protected:
	void CreateUserEntityData(CBaseEntity* pEntity);

	void DestroyUserEntityData(CBaseEntity* pEntity);

public:

	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	static void SDK_OnAllLoaded();

	static void SDK_OnUnload();

	static CPluginEntityFactory* ToPluginEntityFactory( IEntityFactory* pFactory );

	static int GetInstalledFactoryHandles( Handle_t* pHandleArray, int arraySize );

private:
	static void OnFactoryInstall( CPluginEntityFactory* pFactory );
	static void OnFactoryUninstall( CPluginEntityFactory* pFactory );
};

class CPluginEntityFactoryHandler : public IHandleTypeDispatch
{
public:
	virtual void OnHandleDestroy(HandleType_t type, void * object) override;
};

extern HandleType_t g_PluginEntityFactoryHandle;
extern CPluginEntityFactoryHandler g_PluginEntityFactoryHandler;

#endif