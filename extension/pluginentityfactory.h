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

	bool Install();
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

public:
	bool DoesNotDerive() const { return (m_Derive.m_DeriveFrom == DERIVETYPE_NONE); }

	void DeriveFromBaseEntity(bool bServerOnly=false);
	void DeriveFromNPC();
	void DeriveFromClass(const char* classname);
	void DeriveFromHandle(Handle_t handle);

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

protected:

	class InputFuncDelegate
	{
	public:
		IPluginFunction* m_pCallback;

		void* m_pInputFuncPtr;
		int m_iInputFuncSize;

		InputFuncDelegate(IPluginFunction* pCallback);
		~InputFuncDelegate();

		static void OnInput(InputFuncDelegate*, CBaseEntity* pEntity, inputdata_t &data);
	};

	CUtlVector<InputFuncDelegate*> m_pEntityInputFuncDelegates;

public:

	// Whether or not this factory is allowed to use a custom user datamap.
	bool CanUseDataMap() const;

private:
	std::string m_iDataClassname;

	datamap_t* m_pEntityDataMap;

	size_t m_DataMapDescSizeInBytes;
	CUtlVector<typedescription_t> m_vecEntityDataTypeDescriptors;
	int m_iDataMapStartOffset;

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
	bool BeginDataMapDesc(const char* dataClassName);

	// Finalizes the entity factory's type descriptor list.
	void EndDataMapDesc();

protected:
	void DefineField(const char* name, fieldtype_t fieldType, unsigned short count, short flags, const char* externalName, float fieldTolerance);

public:
	// Adds a field type descriptor to the type descriptor list with the specified name and type.
	void DefineField(const char* name, fieldtype_t fieldType, int numElements=1);

	// Adds a field type descriptor to the type descriptor list with the specified name and type, additionally with key name to be used by maps/fgd.
	void DefineKeyField(const char* name, fieldtype_t fieldType, const char* mapname);

	// Assigns an input function to a plugin callback for use with the entity I/O system.
	void DefineInputFunc(const char* name, fieldtype_t fieldType, const char* mapname, IPluginFunction* inputFunc);

	// Adds an output for use with the entity I/O system.
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

	static CPluginEntityFactory* ToPluginEntityFactory( IEntityFactory* pFactory );

	static int GetInstalledFactoryHandles( Handle_t* pHandleArray, int arraySize );

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