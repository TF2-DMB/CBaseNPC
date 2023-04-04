
#include "idatamapcontainer.h"
#include "helpers.h"
#include "sh_pagealloc.h"
#include "baseentityoutput.h"

// For datamap cache deletion hacking purposes
#include <am-hashmap.h>
#include <sm_namehashset.h>
#include <sh_stack.h>

IDataMapContainer::IDataMapContainer()
{
	m_pDataMap = nullptr;
	m_bHasDataMapDesc = false;
	m_DataMapDescSizeInBytes = 0;
}

IDataMapContainer::~IDataMapContainer()
{
}

datamap_t* IDataMapContainer::CreateDataDescMap(datamap_t* pBaseMap)
{
	if (!m_pDataMap)
	{
		const char* classname = GetDataDescMapClass();

		m_pDataMap = new datamap_t;
		m_pDataMap->baseMap = pBaseMap;
		m_pDataMap->dataClassName = classname ? strdup(classname) : nullptr;
		m_pDataMap->packed_offsets_computed = false;
		m_pDataMap->packed_size = 0;

		int numDataDesc = m_vecEntityDataTypeDescriptors.Count();
		typedescription_t * dataDesc = new typedescription_t[numDataDesc];
		m_pDataMap->dataDesc = dataDesc;
		m_pDataMap->dataNumFields = numDataDesc;

		for (int i = 0; i < numDataDesc; i++)
		{
			dataDesc[i] = m_vecEntityDataTypeDescriptors[i];
		}
	}

	return m_pDataMap;
}

void IDataMapContainer::DestroyDataDescMap()
{
	if (!m_pDataMap)
		return;
	
	m_DataMapCache.Purge();

	if (m_pDataMap->dataClassName)
		delete m_pDataMap->dataClassName;
	
	delete[] m_pDataMap->dataDesc;
	delete m_pDataMap;

	m_pDataMap = nullptr;
}

void IDataMapContainer::DestroyDataTypeDescriptor(typedescription_t *desc) const
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

void IDataMapContainer::DestroyDataDesc()
{
	DestroyDataDescMap();

	m_bHasDataMapDesc = false;

	for (int i = 0; i < m_vecEntityDataTypeDescriptors.Count(); i++) 
	{
		DestroyDataTypeDescriptor(&m_vecEntityDataTypeDescriptors[i]);
	}

	m_vecEntityDataTypeDescriptors.Purge();
}

void IDataMapContainer::BeginDataDesc()
{
	DestroyDataDesc();
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

size_t GetAlignedOffset( size_t offset, size_t align, size_t* padding = nullptr )
{
	if ( align == 0 ) return offset;

	size_t pad = ~( offset - 1 ) & ( align - 1 );
	if ( padding ) *padding = pad;

	return offset + pad;
}

size_t GetAlignedOffset( size_t offset, fieldtype_t fieldType, size_t* padding = nullptr )
{
	size_t size;

	switch ( fieldType )
	{
		case FIELD_VECTOR:
		case FIELD_QUATERNION:
		case FIELD_POSITION_VECTOR:
		case FIELD_VMATRIX:
		case FIELD_VMATRIX_WORLDSPACE:
		case FIELD_MATRIX3X4_WORLDSPACE:
		case FIELD_INTERVAL:
		case FIELD_VECTOR2D:
			size = 4;
			break;
		default:
			size = g_DataMapDescFieldSizes[fieldType];
			break;
	}

	return GetAlignedOffset( offset, size, padding );
}

void IDataMapContainer::EndDataDesc()
{
	m_bHasDataMapDesc = true;

	if (m_vecEntityDataTypeDescriptors.Count() == 0)
	{
		// For "empty" tables
		m_vecEntityDataTypeDescriptors.AddToTail( { FIELD_VOID, 0, {0,0}, 0, 0, 0, 0, 0, 0 } );
	}

	m_DataMapDescSizeInBytes = GetAlignedOffset( m_DataMapDescSizeInBytes, sizeof(void*) );
}

void IDataMapContainer::DefineField(const char* name, fieldtype_t fieldType, unsigned short count, short flags, const char* externalName, float fieldTolerance)
{
	size_t padding = 0;
	int fieldOffset = GetAlignedOffset( GetDataDescOffset() + m_DataMapDescSizeInBytes, fieldType, &padding );
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

	m_DataMapDescSizeInBytes += padding + fieldSizeInBytes;
}

// from sourcemod/public/compat_wrappers.h
// NOTE: Adjust if expanding to other games
inline int GetTypeDescOffs(typedescription_t *td)
{
	return td->fieldOffset[TD_OFFSET_NORMAL];
}

// from sourcemod/core/HalfLife2.cpp
bool UTIL_FindDataMapInfo(datamap_t *pMap, const char *name, sm_datatable_info_t *pDataTable)
{
	while (pMap)
	{
		for (int i = 0; i < pMap->dataNumFields; ++i)
		{
			if (pMap->dataDesc[i].fieldName == NULL)
			{
				continue;
			}
			if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
			{
				pDataTable->prop = &(pMap->dataDesc[i]);
				pDataTable->actual_offset = GetTypeDescOffs(pDataTable->prop);
				return true;
			}
			if (pMap->dataDesc[i].td == NULL || !UTIL_FindDataMapInfo(pMap->dataDesc[i].td, name, pDataTable))
			{
				continue;
			}
			
			pDataTable->actual_offset += GetTypeDescOffs(&(pMap->dataDesc[i]));
			return true;
		}
		
		pMap = pMap->baseMap;
	}

	return false; 
}

bool IDataMapContainer::FindDataMapInfo(const char* name, sm_datatable_info_t *pDataTable, char* error, size_t maxlen)
{
	datamap_t* pDataMap = GetDataDescMap();
	if (!pDataMap)
	{
		if (error) snprintf( error, maxlen, "Could not retrieve datamap" );

		return false;
	}

	// Search in cache first.
	unsigned short index = m_DataMapCache.Find( name );
	if (m_DataMapCache.IsValidIndex(index))
	{
		*pDataTable = m_DataMapCache[index];
		return true;
	}

	if (UTIL_FindDataMapInfo( pDataMap, name, pDataTable ))
	{
		m_DataMapCache.Insert(name, *pDataTable);
		return true;
	}

	if (error) snprintf( error, maxlen, "Property \"%s\" not found", name );

	return false;
}

bool IDataMapContainer::GetDataOffset( const sm_datatable_info_t &info, int &offset, int element, char* error, size_t maxlen) const
{
	typedescription_t* td = info.prop;

	if (element < 0 || element >= td->fieldSize)
	{
		if (error) snprintf( error, maxlen, "Element %d is out of bounds (Prop %s has %d elements).",
			element, td->fieldName, td->fieldSize );

		return false;
	}
	
	offset = info.actual_offset + (element * (td->fieldSizeInBytes / td->fieldSize));
	return true;
}

bool IDataMapContainer::GetObjectData( void* obj, const char* prop, int &data, int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
		return false;
	
	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
		return false;
	
	switch ( info.prop->fieldType )
	{
		case FIELD_TICK:
		case FIELD_MODELINDEX:
		case FIELD_MATERIALINDEX:
		case FIELD_INTEGER:
		case FIELD_COLOR32:
			data = *(int32_t*)((int8_t*)obj + offset);
			return true;
		case FIELD_CHARACTER:
			data = *((int8_t*)obj + offset);
		case FIELD_BOOLEAN:
			data = *(bool*)((int8_t*)obj + offset);
			return true;
		case FIELD_SHORT:
			data = *(int16_t*)((int8_t*)obj + offset);
			return true;
	}

	if (error) snprintf( error, maxlen, "Data field %s is not an integer (%d)", prop, info.prop->fieldType );
	return false;
}

bool IDataMapContainer::SetObjectData( void* obj, const char* prop, int data, int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
		return false;
	
	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
		return false;
	
	switch ( info.prop->fieldType )
	{
		case FIELD_TICK:
		case FIELD_MODELINDEX:
		case FIELD_MATERIALINDEX:
		case FIELD_INTEGER:
		case FIELD_COLOR32:
			*(int32_t*)((int8_t*)obj + offset) = data;
			return true;
		case FIELD_CHARACTER:
			*((int8_t*)obj + offset) = (int8_t)data;
			return true;
		case FIELD_BOOLEAN:
			*(bool*)((int8_t*)obj + offset) = !!data;
			return true;
		case FIELD_SHORT:
			*(int16_t*)((int8_t*)obj + offset) = (int16_t)data;
			return true;
	}

	if (error) snprintf( error, maxlen, "Data field %s is not an integer (%d)", prop, info.prop->fieldType );
	return false;
}

bool IDataMapContainer::GetObjectDataFloat( void* obj, const char* prop, float &data, int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
		return false;
	
	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
		return false;

	switch ( info.prop->fieldType )
	{
		case FIELD_FLOAT:
		case FIELD_TIME:
			data = *(float*)((int8_t*)obj + offset);
			return true;
	}

	if (error) snprintf( error, maxlen, "Data field %s is not a float (%d != [%d,%d])", prop, info.prop->fieldType, FIELD_FLOAT, FIELD_TIME );
	return false;
}

bool IDataMapContainer::SetObjectDataFloat( void* obj, const char* prop, float data, int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
		return false;
	
	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
		return false;
	
	switch ( info.prop->fieldType )
	{
		case FIELD_FLOAT:
		case FIELD_TIME:
			*(float*)((int8_t*)obj + offset) = data;
			return true;
	}

	if (error) snprintf( error, maxlen, "Data field %s is not a float (%d != [%d,%d])", prop, info.prop->fieldType, FIELD_FLOAT, FIELD_TIME );
	return false;
}

bool IDataMapContainer::GetObjectDataVector( void* obj, const char* prop, float data[3], int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
		return false;
	
	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
		return false;

	switch ( info.prop->fieldType )
	{
		case FIELD_VECTOR:
		case FIELD_POSITION_VECTOR:
			float* src = (float*)((int8_t*)obj + offset);
			data[0] = src[0]; data[1] = src[1]; data[2] = src[2]; 
			return true;
	}

	if (error) snprintf( error, maxlen, "Data field %s is not a vector (%d != [%d,%d])", prop, info.prop->fieldType, FIELD_VECTOR, FIELD_POSITION_VECTOR );
	return false;
}

bool IDataMapContainer::SetObjectDataVector( void* obj, const char* prop, const float data[3], int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
		return false;
	
	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
		return false;

	switch ( info.prop->fieldType )
	{
		case FIELD_VECTOR:
		case FIELD_POSITION_VECTOR:
			float* dest = (float*)((int8_t*)obj + offset);
			dest[0] = data[0]; dest[1] = data[1]; dest[2] = data[2]; 
			return true;
	}

	if (error) snprintf( error, maxlen, "Data field %s is not a vector (%d != [%d,%d])", prop, info.prop->fieldType, FIELD_VECTOR, FIELD_POSITION_VECTOR );
	return false;
}

bool IDataMapContainer::GetObjectDataString( void* obj, const char* prop, char* data, size_t datamaxlen, int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
		return false;
	
	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
		return false;
	
	bool bIsStringIndex = (info.prop->fieldType != FIELD_CHARACTER);

	switch ( info.prop->fieldType )
	{
		case FIELD_CHARACTER:
		{
			if (element != 0)
			{
				if (error) snprintf(error, maxlen, "Prop %s is not an array. Element %d is invalid.", prop, element);
				return false;
			}

			char* src = (char *)((uint8_t *)obj + offset);
			ke::SafeStrcpyN(data, datamaxlen, src, info.prop->fieldSize );

			return true;
		}
		case FIELD_STRING:
		case FIELD_MODELNAME:
		case FIELD_SOUNDNAME:
		{
			string_t idx = *(string_t *)((uint8_t *)obj + offset);
			const char* src = (idx == NULL_STRING) ? "" : STRING(idx);
			ke::SafeStrcpy(data, datamaxlen, src);

			return true;
		}
	}

	if (error) snprintf(error, maxlen, "Data field %s is not a string (%d != %d)", prop, info.prop->fieldType, FIELD_CHARACTER);
	return false;
}

bool IDataMapContainer::SetObjectDataString( void* obj, const char* prop, const char* data, int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
		return false;
	
	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
		return false;
	
	bool bIsStringIndex = (info.prop->fieldType != FIELD_CHARACTER);

	switch ( info.prop->fieldType )
	{
		case FIELD_CHARACTER:
		{
			if (element != 0)
			{
				if (error) snprintf(error, maxlen, "Prop %s is not an array. Element %d is invalid.", prop, element);
				return false;
			}

			char* dest = (char *)((uint8_t *)obj + offset);
			ke::SafeStrcpy(dest, info.prop->fieldSize, data);

			return true;
		}
		case FIELD_STRING:
		case FIELD_MODELNAME:
		case FIELD_SOUNDNAME:
		{
			*(string_t *)((uint8_t *)obj + offset) = AllocPooledString( data );
			return true;
		}
	}

	if (error) snprintf(error, maxlen, "Data field %s is not a string (%d != %d)", prop, info.prop->fieldType, FIELD_CHARACTER);
	return false;
}

bool IDataMapContainer::GetObjectDataEntity( void* obj, const char* prop, CBaseEntity** data, int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
	{
		return false;
	}

	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
	{
		return false;
	}

	switch (info.prop->fieldType)
	{
		case FIELD_EHANDLE:
		case FIELD_CUSTOM:
		{
			CBaseHandle* hndl;

			if (info.prop->fieldType == FIELD_EHANDLE)
			{
				hndl = (CBaseHandle *)((uint8_t *)obj + offset);
			}
			else if (info.prop->fieldType == FIELD_CUSTOM && (info.prop->flags & FTYPEDESC_OUTPUT) == FTYPEDESC_OUTPUT)
			{
				auto *pVariant = (variant_t *)((intptr_t)obj + offset);
				hndl = &pVariant->eVal;
			}
			else
			{
				break;
			}
			 
			CBaseEntity* pHandleEntity = gamehelpers->ReferenceToEntity(hndl->GetEntryIndex());

			if (pHandleEntity && *hndl == reinterpret_cast<IHandleEntity *>(pHandleEntity)->GetRefEHandle())
			{
				*data = pHandleEntity;
			}
			else
			{
				*data = nullptr;
			}

			return true;
		}
		case FIELD_CLASSPTR:
		{
			*data = *(CBaseEntity **)((uint8_t *)obj + offset);
			return true;
		}
		case FIELD_EDICT:
		{
			edict_t *pEdict = *(edict_t **)((uint8_t *)obj + offset);
			if (pEdict && !pEdict->IsFree())
			{
				*data = gamehelpers->ReferenceToEntity(gamehelpers->IndexOfEdict(pEdict));
			}
			else
			{
				*data = nullptr;
			}

			return true;
		}
	}

	if (error)
	{
		snprintf(error, maxlen, "Data field %s is not an entity nor edict (%d)", prop, info.prop->fieldType);
	}

	return false;
}

bool IDataMapContainer::SetObjectDataEntity( void* obj, const char* prop, CBaseEntity* data, int element, char* error, size_t maxlen )
{
	sm_datatable_info_t info;
	if (!FindDataMapInfo(prop, &info, error, maxlen))
	{
		return false;
	}

	int offset;
	if (!GetDataOffset(info, offset, element, error, maxlen))
	{
		return false;
	}

	switch (info.prop->fieldType)
	{
		case FIELD_EHANDLE:
		case FIELD_CUSTOM:
		{
			CBaseHandle* hndl;

			if (info.prop->fieldType == FIELD_EHANDLE)
			{
				hndl = (CBaseHandle *)((uint8_t *)obj + offset);
			}
			else if (info.prop->fieldType == FIELD_CUSTOM && (info.prop->flags & FTYPEDESC_OUTPUT) == FTYPEDESC_OUTPUT)
			{
				auto *pVariant = (variant_t *)((intptr_t)obj + offset);
				hndl = &pVariant->eVal;
			}
			else
			{
				break;
			}

			hndl->Set((IHandleEntity *)data);
			return true;
		}
		case FIELD_CLASSPTR:
		{
			*(CBaseEntity **)((uint8_t *)obj + offset) = data;
			return true;
		}
		case FIELD_EDICT:
		{
			edict_t *pEdict = nullptr;

			if (data)
			{
				IServerNetworkable *pNetworkable = ((IServerUnknown *)data)->GetNetworkable();
				if (!pNetworkable)
				{
					if (error)
					{
						snprintf(error, maxlen, "Entity does not have a valid edict");
					}
					
					return false;
				}

				pEdict = pNetworkable->GetEdict();
				if (!pEdict || pEdict->IsFree())
				{
					if (error)
					{
						snprintf(error, maxlen, "Entity does not have a valid edict");
					}

					return false;
				}
			}

			*(edict_t **)((uint8_t *)obj + offset) = pEdict;
			return true;
		}
	}

	if (error)
	{
		snprintf(error, maxlen, "Data field %s is not an entity nor edict (%d)", prop, info.prop->fieldType);
	}

	return false;
}

SourceHook::CPageAlloc g_InputFuncAlloc;

IEntityDataMapInputFuncDelegate::IEntityDataMapInputFuncDelegate()
{
	m_pInputFuncPtr = nullptr;

	Alloc();
}

void IEntityDataMapInputFuncDelegate::Alloc()
{
	if (m_pInputFuncPtr)
		return;

	uint32_t thisAddr = (uint32_t)this;
	uint32_t callFuncAddr = (uint32_t)(&IEntityDataMapInputFuncDelegate::HandleInput);

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

void IEntityDataMapInputFuncDelegate::Free()
{
	if (!m_pInputFuncPtr)
		return;
	
	g_InputFuncAlloc.Free(m_pInputFuncPtr);
	m_pInputFuncPtr = nullptr;
}

IEntityDataMapInputFuncDelegate::~IEntityDataMapInputFuncDelegate()
{
	Free();
}

void IEntityDataMapContainer::DestroyDataDesc()
{
	IDataMapContainer::DestroyDataDesc();

	m_pEntityInputFuncDelegates.PurgeAndDeleteElements();
}

// https://github.com/alliedmodders/sourcemod/blob/38eecd5ece26b2469560db1822511a7e2685286e/core/HalfLife2.h#L263
struct CHalfLife2Hack
{
#if SM_BUILD_MINOR < 12
	struct DataMapCachePolicy
	{
		static inline bool matches(const char *name, const sm_datatable_info_t &info)
		{
			return strcmp(name, info.prop->fieldName) == 0;
		}

		static inline uint32_t hash(const detail::CharsAndLength &key)
		{
			return key.hash();
		}
	};

	typedef NameHashSet<sm_datatable_info_t, DataMapCachePolicy> DataMapCache;
#else
	struct DataMapCacheInfo
	{
		static inline bool matches(const char *name, const DataMapCacheInfo &info)
		{
			return strcmp(name, info.name.c_str()) == 0;
		}

		static inline uint32_t hash(const detail::CharsAndLength &key)
		{
			return key.hash();
		}

		DataMapCacheInfo()
			: name(), info{nullptr, 0}
		{
		}

		std::string name;
		sm_datatable_info_t info;
	};

	typedef NameHashSet<DataMapCacheInfo> DataMapCache;

#endif
	typedef ke::HashMap<datamap_t *, DataMapCache *, ke::PointerPolicy<datamap_t> > DataTableMap;

	void** vptr;
	NameHashSet<void *> m_Classes;
	DataTableMap m_Maps;
};

void IEntityDataMapContainer::DestroyDataDescMap()
{
	if (!m_pDataMap)
	{
		return;
	}

	// HACK: Force gamehelpers (CHalfLife2 *) to delete the cache of our datamap if it exists.
#if SMINTERFACE_GAMEHELPERS_VERSION < 12
	CHalfLife2Hack* pHL2 = reinterpret_cast<CHalfLife2Hack*>(gamehelpers);
	auto result = pHL2->m_Maps.find( m_pDataMap );
	if (result.found())
	{
		CHalfLife2Hack::DataMapCache* cache = result->value;
		if (cache) delete cache;

		pHL2->m_Maps.remove( result );
	}
#else
	gamehelpers->RemoveDataTableCache(m_pDataMap);
#endif

	IDataMapContainer::DestroyDataDescMap();
}

void IEntityDataMapContainer::DefineField(const char* name, fieldtype_t fieldType, unsigned short count)
{
	DefineField(name, fieldType, count, FTYPEDESC_SAVE, NULL, 0);
}

void IEntityDataMapContainer::DefineKeyField(const char* name, fieldtype_t fieldType, const char* mapname)
{
	DefineField(name, fieldType, 1, FTYPEDESC_KEY | FTYPEDESC_SAVE, strdup(mapname), 0);
}

void IEntityDataMapContainer::DefineInputFunc(const char* name, fieldtype_t fieldType, const char* mapname, IEntityDataMapInputFuncDelegate *pDelegate)
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

	m_pEntityInputFuncDelegates.AddToTail(pDelegate);
	
	// this shuts up the compiler
	*(uint32_t*)(&(typeDesc.inputFunc)) = (uint32_t)pDelegate->m_pInputFuncPtr;

	m_vecEntityDataTypeDescriptors.AddToTail(typeDesc);
}

void IEntityDataMapContainer::DefineOutput(const char* name, const char* mapname)
{
	size_t padding = 0;
	int fieldOffset = GetAlignedOffset( GetDataDescOffset() + m_DataMapDescSizeInBytes, sizeof(int*), &padding );
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

	m_DataMapDescSizeInBytes += padding + fieldSizeInBytes;
}

void IEntityDataMapContainer::CreateFields(CBaseEntity* pEntity)
{
	if (!m_pDataMap)
		return;
	
	for (int i = 0; i < m_pDataMap->dataNumFields; i++)
	{
		typedescription_t * pTypeDesc = &m_pDataMap->dataDesc[i];
		if ( ( pTypeDesc->fieldType == FIELD_CUSTOM ) && ( pTypeDesc->flags & FTYPEDESC_OUTPUT ) )
		{
			CBaseEntityOutputHack *pOutput = (CBaseEntityOutputHack *)((uint8_t*)pEntity + pTypeDesc->fieldOffset[0]);
			pOutput->Init();
		}
	}
}

void IEntityDataMapContainer::DestroyFields(CBaseEntity* pEntity)
{
	if (!m_pDataMap)
		return;

	for (int i = 0; i < m_pDataMap->dataNumFields; i++)
	{
		typedescription_t * pTypeDesc = &m_pDataMap->dataDesc[i];
		if ( ( pTypeDesc->fieldType == FIELD_CUSTOM ) && ( pTypeDesc->flags & FTYPEDESC_OUTPUT ) )
		{
			CBaseEntityOutputHack *pOutput = (CBaseEntityOutputHack *)((uint8_t*)pEntity + pTypeDesc->fieldOffset[0]);
			pOutput->Destroy();
		}
	}
}
