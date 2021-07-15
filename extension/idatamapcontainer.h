#ifndef _DATAMAPHANDLER_H_
#define _DATAMAPHANDLER_H_

#include <itoolentity.h>
#include <tier0/platform.h>
#include <IEngineTrace.h>
#include <shareddefs.h>
#include <util.h>

#include "helpers.h"

class IDataMapContainer
{
protected:
	datamap_t* m_pDataMap;
	bool m_bHasDataMapDesc;
	size_t m_DataMapDescSizeInBytes;
	CUtlVector<typedescription_t> m_vecEntityDataTypeDescriptors;

public:
	virtual ~IDataMapContainer();

	// Creates a datamap (if it doesn't exist) that uses the defined type
	// descriptors.
	datamap_t* CreateDataDescMap(datamap_t* pBaseMap);

	// Destroys the instantiated datamap.
	void DestroyDataDescMap();

	// Returns the instantiated datamap.
	datamap_t* GetDataDescMap() const { return m_pDataMap; }

	// True if type descriptors were defined for a datamap, false otheriwse.
	bool HasDataDesc() const { return m_bHasDataMapDesc; }

	// The classname that an instantiated datamap will use.
	virtual const char* GetDataDescMapClass() const=0;

	// The starting offset of the data defined by the type descriptors within
	// the object that will use the datamap.
	virtual int GetDataDescOffset() const=0;

	// The total size of the data defined by the type descriptors.
	size_t GetDataDescSize() const { return HasDataDesc() ? m_DataMapDescSizeInBytes : 0; }

	// Starts defining the type descriptors of the datamap.
	virtual void BeginDataDesc();

	// Defines a data property type descriptor of the datamap.
	void DefineField(const char* name, fieldtype_t fieldType, unsigned short count, short flags, const char* externalName, float fieldTolerance);

	// Finishes defining type descriptors.
	virtual void EndDataDesc();

	int FindDataOffset(const char* propName, unsigned short element=0, fieldtype_t * fieldType=nullptr, unsigned short * elements=nullptr);

protected:
	void DestroyDataTypeDescriptor(typedescription_t *desc) const;

	virtual void DestroyDataDesc();
};

class IEntityDataMapInputFuncDelegate
{
protected:
	void* m_pInputFuncPtr;
	int m_iInputFuncSize;

public:
	IEntityDataMapInputFuncDelegate();
	virtual ~IEntityDataMapInputFuncDelegate();
	virtual void Alloc();
	virtual void Free();
	virtual void OnInput(CBaseEntity* pEntity, inputdata_t &data)=0;

private:
	static void HandleInput(IEntityDataMapInputFuncDelegate* pDelegate, CBaseEntity* pEntity, inputdata_t &data)
	{
		pDelegate->OnInput(pEntity, data);
	}

	friend class IEntityDataMapContainer;
};

class IEntityDataMapContainer : public IDataMapContainer
{
protected:
	CUtlVector<IEntityDataMapInputFuncDelegate*> m_pEntityInputFuncDelegates;

	virtual void DestroyDataDesc() override;

public:
	using IDataMapContainer::DefineField;

	// Adds a field type descriptor with the specified name and type.
	void DefineField(const char* name, fieldtype_t fieldType, unsigned short count=1);

	// Adds a field type descriptor with the specified name and type,
	// additionally with key name to be used by maps/fgd.
	void DefineKeyField(const char* name, fieldtype_t fieldType, const char* mapname);

	// Assigns an input function to a plugin callback for use with the entity
	// I/O system.
	void DefineInputFunc(const char* name, fieldtype_t fieldType, const char* mapname, IEntityDataMapInputFuncDelegate* inputFunc);

	// Adds an output for use with the entity I/O system.
	void DefineOutput(const char* name, const char* externalName);

	virtual void CreateFields(CBaseEntity* pEntity);

	virtual void DestroyFields(CBaseEntity* pEntity);
};

cell_t GetObjectProp( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, int element=0 );
cell_t SetObjectProp( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t value, int element=0 );
cell_t GetObjectPropFloat( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, int element=0 );
cell_t SetObjectPropFloat( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t value, int element=0 );
cell_t GetObjectPropVector( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t buffer, int element=0 );
cell_t SetObjectPropVector( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t src, int element=0 );
cell_t GetObjectPropString( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t buffer, size_t bufferSize, int element=0 );
cell_t SetObjectPropString( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t src, int element=0 );

#endif