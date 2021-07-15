
#include "idatamapcontainer.h"
#include "helpers.h"
#include "sh_pagealloc.h"
#include "baseentityoutput.h"

IDataMapContainer::~IDataMapContainer()
{
	DestroyDataDesc();
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

void IDataMapContainer::EndDataDesc()
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

void IDataMapContainer::DefineField(const char* name, fieldtype_t fieldType, unsigned short count, short flags, const char* externalName, float fieldTolerance)
{
	int fieldOffset = GetDataDescOffset() + m_DataMapDescSizeInBytes;
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

int IDataMapContainer::FindDataOffset(const char* propName, unsigned short element, fieldtype_t *fieldType, unsigned short* elements)
{
	datamap_t* pDataMap = GetDataDescMap();

	while (pDataMap && pDataMap->dataNumFields)
	{
		for (int i = 0; i < pDataMap->dataNumFields; i++)
		{
			typedescription_t &typeDesc = pDataMap->dataDesc[i];
			if (typeDesc.fieldName && !V_strcmp( propName, typeDesc.fieldName ))
			{
				if (fieldType)
					*fieldType = typeDesc.fieldType;

				if (elements)
					*elements = typeDesc.fieldSize;

				if (element >= typeDesc.fieldSize)
					return -1;
				
				int elementSize = g_DataMapDescFieldSizes[typeDesc.fieldType];
				return typeDesc.fieldOffset[0] + (element * elementSize);
			}
		}

		pDataMap = pDataMap->baseMap;
	}

	return -1;
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
	int fieldOffset = GetDataDescOffset() + m_DataMapDescSizeInBytes;
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

#define GET_DATA_OFFSET() \
	if (!datamap) return pContext->ThrowNativeError("Object has no datamap"); \
	typedescription_t* pTypeDesc = gamehelpers->FindInDataMap(datamap, prop); \
	if (!pTypeDesc) \
	{ \
		return pContext->ThrowNativeError("Property %s not found", prop); \
	} \
	if (element < 0 || element >= pTypeDesc->fieldSize) \
	{ \
		return pContext->ThrowNativeError("Element %d is out of bounds (Prop %s has %d elements).", \
			element, \
			prop, \
			pTypeDesc->fieldSize); \
	} \
	int offset = pTypeDesc->fieldOffset[0] + (element * (pTypeDesc->fieldSizeInBytes / pTypeDesc->fieldSize)); \
	fieldtype_t fieldType = pTypeDesc->fieldType; \
	uint8_t* addr = (uint8_t*)obj + offset; \
	do { } while(0)

cell_t GetObjectProp( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, int element )
{
	GET_DATA_OFFSET();

	switch (fieldType)
	{
		case FIELD_INTEGER:
		case FIELD_COLOR32:
			return *((int32_t*)addr);
		case FIELD_SHORT:
			return *((int16_t*)addr);
		case FIELD_CHARACTER:
		case FIELD_BOOLEAN:
			return *addr;
	}

	return pContext->ThrowNativeError("Property %s is not an integer", prop);
}

cell_t SetObjectProp( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t value, int element )
{
	GET_DATA_OFFSET();

	switch (fieldType)
	{
		case FIELD_INTEGER:
		case FIELD_COLOR32:
			*((int32_t*)addr) = (int32_t)value;
			break;
		case FIELD_SHORT:
			*((int16_t*)addr) = (int16_t)value;
			break;
		case FIELD_CHARACTER:
		case FIELD_BOOLEAN:
			*((int8_t*)addr) = (int8_t)value;
			break;
		default:
			return pContext->ThrowNativeError("Property %s is not an integer", prop);
	}

	return 0;
}

cell_t GetObjectPropFloat( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, int element )
{
	GET_DATA_OFFSET();

	switch (fieldType)
	{
		case FIELD_FLOAT:
			return sp_ftoc(*((float*)addr));
		default:
			return pContext->ThrowNativeError("Property %s is not a float", prop);
	}

	return 0;
}

cell_t SetObjectPropFloat( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t value, int element )
{
	GET_DATA_OFFSET();

	switch (fieldType)
	{
		case FIELD_FLOAT:
			*((float*)addr) = sp_ctof(value);
			break;
		default:
			return pContext->ThrowNativeError("Property %s is not a float", prop);
	}

	return 0;
}

cell_t GetObjectPropVector( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t buffer, int element )
{
	GET_DATA_OFFSET();

	float* vecAddr = (float*)addr;
	cell_t* vec;
	pContext->LocalToPhysAddr(buffer, &vec);

	switch (fieldType)
	{
		case FIELD_VECTOR:
		case FIELD_POSITION_VECTOR:
			vec[0] = sp_ftoc(vecAddr[0]); 
			vec[1] = sp_ftoc(vecAddr[1]); 
			vec[2] = sp_ftoc(vecAddr[2]);
			return 1;
		default:
			return pContext->ThrowNativeError("Property %s is not a Vector", prop);
	}

	return 0;
}

cell_t SetObjectPropVector( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t src, int element )
{
	GET_DATA_OFFSET();

	float* vecAddr = (float*)addr;
	cell_t* vec;
	pContext->LocalToPhysAddr(src, &vec);

	switch (fieldType)
	{
		case FIELD_VECTOR:
		case FIELD_POSITION_VECTOR:
			vecAddr[0] = sp_ctof(vec[0]);
			vecAddr[1] = sp_ctof(vec[1]);
			vecAddr[2] = sp_ctof(vec[2]);
			return 0;
		default:
			return pContext->ThrowNativeError("Property %s is not a Vector", prop);
	}

	return 0;
}

cell_t GetObjectPropString( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t buffer, size_t bufferSize, int element )
{
	GET_DATA_OFFSET();

	const char* src;

	switch (fieldType)
	{
		case FIELD_CHARACTER:
			src = (char*)addr;
			break;
		case FIELD_STRING:
			string_t idx = *((string_t*)addr);
			src = (idx == NULL_STRING) ? "" : STRING(idx);
			break;
		default:
			return pContext->ThrowNativeError("Property %s is not a string", prop);
	}

	if (src)
	{
		size_t len;
		pContext->StringToLocalUTF8(buffer, bufferSize, src, &len);
		return len;
	}

	pContext->StringToLocal(buffer, bufferSize, "");
	return 0;
}

cell_t SetObjectPropString( IPluginContext *pContext, void* obj, datamap_t* datamap, const char* prop, cell_t src, int element )
{
	GET_DATA_OFFSET();

	char* srcstr;
	pContext->LocalToStringNULL(src, &srcstr);

	switch (fieldType)
	{
		case FIELD_CHARACTER:
			ke::SafeStrcpy((char *)addr, pTypeDesc->fieldSize, srcstr);
			break;
		case FIELD_STRING:
			*((string_t *)addr) = AllocPooledString(srcstr);
			break;
		default:
			return pContext->ThrowNativeError("Property %s is not a string", prop);
	}

	return 0;
}