#ifndef _DATAMAPHANDLER_H_
#define _DATAMAPHANDLER_H_

#include <itoolentity.h>
#include <tier0/platform.h>
#include <IEngineTrace.h>
#include <shareddefs.h>
#include <util.h>
#include <utldict.h>

#include "helpers.h"

class IDataMapContainer
{
protected:
	datamap_t* m_pDataMap;
	bool m_bHasDataMapDesc;
	size_t m_DataMapDescSizeInBytes;
	CUtlVector<typedescription_t> m_vecEntityDataTypeDescriptors;

	CUtlDict<sm_datatable_info_t, unsigned short> m_DataMapCache;

public:
	IDataMapContainer();
	virtual ~IDataMapContainer();

	virtual IDataMapContainer* DataMapContainerPointer() final { return dynamic_cast< IDataMapContainer* >(this); }

	// Creates a datamap (if it doesn't exist) that uses the defined type
	// descriptors.
	virtual datamap_t* CreateDataDescMap(datamap_t* pBaseMap);

	// Destroys the instantiated datamap.
	virtual void DestroyDataDescMap();

	// Returns the instantiated datamap.
	virtual datamap_t* GetDataDescMap() const final { return m_pDataMap; }

	// True if type descriptors were defined for a datamap, false otheriwse.
	virtual bool HasDataDesc() const final { return m_bHasDataMapDesc; }

	// The classname that an instantiated datamap will use.
	virtual const char* GetDataDescMapClass() const=0;

	// The starting offset of the data defined by the type descriptors within
	// the object that will use the datamap.
	virtual int GetDataDescOffset() const=0;

	// The total size of the data defined by the type descriptors.
	virtual size_t GetDataDescSize() const final { return HasDataDesc() ? m_DataMapDescSizeInBytes : 0; }

	// Starts defining the type descriptors of the datamap.
	virtual void BeginDataDesc();

	// Defines a data property type descriptor of the datamap.
	virtual void DefineField(const char* name, fieldtype_t fieldType, unsigned short count, short flags, const char* externalName, float fieldTolerance);

	// Finishes defining type descriptors.
	virtual void EndDataDesc();

	// Destroys the datamap and stored type descriptors. This must be called to
	// clean up the object. Because it is a virtual function, you must call
	// this before destructing the object.
	virtual void DestroyDataDesc();

	// Finds data property info inside the container's datamap.
	virtual bool FindDataMapInfo(const char* name, sm_datatable_info_t *pDataTable, char* error=nullptr, size_t maxlen=0);

	// Finds the data offset using the provided info.
	virtual bool GetDataOffset( const sm_datatable_info_t &info, int &offset, int element=0, char* error=nullptr, size_t maxlen=0 ) const;

	// Gets data stored at the int/short/char/bool within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool GetObjectData( void* obj, const char* prop, int &data, int element=0, char* error=nullptr, size_t maxlen=0 );

	// Sets data stored at the int/short/char/bool within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool SetObjectData( void* obj, const char* prop, int data, int element=0, char* error=nullptr, size_t maxlen=0 );

	// Gets data stored at the float within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool GetObjectDataFloat( void* obj, const char* prop, float &data, int element=0, char* error=nullptr, size_t maxlen=0 );

	// Sets data stored at the int/short/char/bool within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool SetObjectDataFloat( void* obj, const char* prop, float data, int element=0, char* error=nullptr, size_t maxlen=0 );

	// Gets data stored at the Vector within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool GetObjectDataVector( void* obj, const char* prop, float data[3], int element=0, char* error=nullptr, size_t maxlen=0 );

	// Sets data stored at the Vector within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool SetObjectDataVector( void* obj, const char* prop, const float data[3], int element=0, char* error=nullptr, size_t maxlen=0 );

	// Gets data stored at the char[]/string_t within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool GetObjectDataString( void* obj, const char* prop, char* data, size_t datamaxlen, int element=0, char* error=nullptr, size_t maxlen=0 );

	// Sets data stored at the char[]/string_t within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool SetObjectDataString( void* obj, const char* prop, const char* data, int element=0, char* error=nullptr, size_t maxlen=0 );

	// Gets the entity stored at the handle/pointer/output (variant_t)/edict within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool GetObjectDataEntity( void* obj, const char* prop, CBaseEntity** data, int element=0, char* error=nullptr, size_t maxlen=0 );

	// Sets the entity stored at the handle/pointer/output (variant_t)/edict within the provided object.
	// This class's datamap is used to calculate offsets.
	virtual bool SetObjectDataEntity( void* obj, const char* prop, CBaseEntity* data, int element=0, char* error=nullptr, size_t maxlen=0 );

protected:
	// Destroys a type description object allocated by this class.
	virtual void DestroyDataTypeDescriptor(typedescription_t *desc) const;
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

public:
	virtual void DestroyDataDesc() override;

	virtual void DestroyDataDescMap() override;

	using IDataMapContainer::DefineField;

	// Adds a field type descriptor with the specified name and type.
	virtual void DefineField(const char* name, fieldtype_t fieldType, unsigned short count=1);

	// Adds a field type descriptor with the specified name and type,
	// additionally with key name to be used by maps/fgd.
	virtual void DefineKeyField(const char* name, fieldtype_t fieldType, const char* mapname);

	// Assigns an input function to a plugin callback for use with the entity
	// I/O system.
	virtual void DefineInputFunc(const char* name, fieldtype_t fieldType, const char* mapname, IEntityDataMapInputFuncDelegate* inputFunc);

	// Adds an output for use with the entity I/O system.
	virtual void DefineOutput(const char* name, const char* externalName);

	virtual void CreateFields(CBaseEntity* pEntity);

	virtual void DestroyFields(CBaseEntity* pEntity);
};

#endif