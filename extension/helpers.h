#ifndef _HELPERS_H_INCLUDED_
	#define _HELPERS_H_INCLUDED_

#pragma once

#include "smsdk_ext.h"
#include <iserverunknown.h>
#include <iservernetworkable.h>
#include <server_class.h>

#define GETGAMEDATASIGNATURE(name, var) \
	if(!g_pGameConf->GetMemSig(name, &var) || var == nullptr) { snprintf(error, maxlength, "FAILED TO GET GAMEDATA SIGNATURE FOR %s", name); return false; } \
	else { smutils->LogMessage(myself, "FOUND GAMEDATA SIGNATURE FOR %s SUCCESSFULLY =| @ 0x%x", name, var); } \
	
#define GETGAMEDATAOFFSET(name, var) \
	if(!g_pGameConf->GetOffset(name, &var) || var == -1) { snprintf(error, maxlength, "FAILED TO GET GAMEDATA OFFSET FOR %s", name); return false; } \
	else { smutils->LogMessage(myself, "FOUND GAMEDATA OFFSET FOR %s SUCCESSFULLY =| %i", name, var); } \

#define DEFINEHANDLE(name) \
	class C##name##Handler: public IHandleTypeDispatch \
	{ \
		public: \
			void OnHandleDestroy(HandleType_t type, void *object) \
			{ \
				name *p##name = (name *)object; \
				delete p##name; \
			} \
	}; \
	C##name##Handler g_##name##Handler; \
	HandleType_t g_##name##Handle; \

#define DEFINEHANDLEOBJ(name, obj) \
	class C##name##Handler: public IHandleTypeDispatch \
	{ \
		public: \
			void OnHandleDestroy(HandleType_t type, void *object) \
			{ \
				obj *p##name = (obj *)object; \
				delete p##name; \
			} \
	}; \
	C##name##Handler g_##name##Handler; \
	HandleType_t g_##name##Handle; \

#define DEFINEHANDLE2(name, objtype) \
	class C##name##Handler: public IHandleTypeDispatch \
	{ \
		public: \
			void OnHandleDestroy(HandleType_t type, void *object) \
			{ \
				objtype *p##name = (objtype *)object; \
				delete p##name; \
			} \
	}; \
	C##name##Handler g_##name##Handler; \
	HandleType_t g_##name##Handle; \

#define CREATEHANDLETYPE(name) \
	g_##name##Handle = handlesys->CreateType(#name, &g_##name##Handler, 0, nullptr, nullptr, myself->GetIdentity(), nullptr); \

#define CREATEHANDLE(name, obj) \
	handlesys->CreateHandle(g_##name##Handle, obj, pContext->GetIdentity(), myself->GetIdentity(), nullptr) \

#define REMOVEHANDLETYPE(name) \
	handlesys->RemoveType(g_##name##Handle, myself->GetIdentity()); \

#define REMOVEHANDLE(hnd) \
	HandleError dhnderr = handlesys->FreeHandle(hnd, &security); \
	if(dhnderr != HandleError_None) \
	{ \
		return pContext->ThrowNativeError("Invalid Handle %x (error %i: %s)", hnd, dhnderr, HandleErrorToString(dhnderr)); \
	} \

#define HANDLENAME(name) \
	g_##name##Handle \

#define READHANDLE(hnd, name, obj) \
	HandleError chnderr = handlesys->ReadHandle(hnd, g_##name##Handle, &security, (void **)&obj); \
	if(chnderr != HandleError_None) \
	{ \
		return pContext->ThrowNativeError("Invalid Handle %x (error %i: %s)", hnd, chnderr, HandleErrorToString(chnderr)); \
	} \

extern int nothing;
	
SendProp *GetEntSendProp(CBaseEntity *pEntity, const char *prop);
int GetEntPropOffset(CBaseEntity *pEntity, const char *prop, bool local=false, int &fieldsize=nothing, int &fieldsizeinbytes=nothing);

void VectorToPawnVector(cell_t *vecAddr, const Vector vector);
void VectorToPawnVector(cell_t *vecAddr, const Vector *vector);
void VectorToPawnVector(cell_t *angAddr, const QAngle angle);
void VectorToPawnVector(cell_t *angAddr, const QAngle *angle);

void PawnVectorToVector(cell_t *vecAddr, Vector *vector);
void PawnVectorToVector(cell_t *vecAddr, Vector &vector);
void PawnVectorToVector(cell_t *angAddr, QAngle *angle);
void PawnVectorToVector(cell_t *angAddr, QAngle &angle);

const char *HandleErrorToString(HandleError err);

IPluginFunction *GetFunctionByNameEx(IPluginContext *pContext, const char *name);

#endif