#ifndef _HELPERS_H_INCLUDED_
	#define _HELPERS_H_INCLUDED_

#pragma once

#include "smsdk_ext.h"
#include <iserverunknown.h>
#include <iservernetworkable.h>
#include <server_class.h>
#include <exception>
#include <stdexcept>
#include <string>
	
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

void VectorToPawnVector(cell_t *vecAddr, const Vector vector);
void VectorToPawnVector(cell_t *vecAddr, const Vector *vector);
void VectorToPawnVector(cell_t *angAddr, const QAngle angle);
void VectorToPawnVector(cell_t *angAddr, const QAngle *angle);

void PawnVectorToVector(cell_t *vecAddr, Vector *vector);
void PawnVectorToVector(cell_t *vecAddr, Vector &vector);
void PawnVectorToVector(cell_t *angAddr, QAngle *angle);
void PawnVectorToVector(cell_t *angAddr, QAngle &angle);

void MatrixToPawnMatrix( cell_t *matAddr, const matrix3x4_t &mat );
void PawnMatrixToMatrix( cell_t *matAddr, matrix3x4_t &mat );

const char *HandleErrorToString(HandleError err);

IPluginFunction *GetFunctionByNameEx(IPluginContext *pContext, const char *name);

string_t AllocPooledString(const char *pszValue);


template<typename ReturnType, typename... Args>
class FCall
{
public:
	FCall() : func(nullptr) {};

	void Init(SourceMod::IGameConfig* config, const char* name)
	{
		if (!config->GetMemSig(name, reinterpret_cast<void**>(&func)) || !((void*)func))
		{
			std::string error("Failed to find signature: ");
			throw std::runtime_error(error + name);
		}
#ifdef DEBUG
		g_pSM->LogMessage(myself, "FCall<%s> Got addr 0x%08X!", name, (void*)func);
#endif
	}

	ReturnType operator()(Args... args)
	{
		return (*func)(args...);
	}

private:
	ReturnType (*func)(Args...);
};

template<typename ReturnType, typename... Args>
class CCall
{
protected:
	CCall() {};
	class funcEmptyClass {};

	// https://wiki.alliedmods.net/Virtual_Offsets_(Source_Mods)
	ReturnType Call(void* thisPtr, void* func, Args... args)
	{
		void **this_ptr = *(void ***)&thisPtr;
		union
		{
			ReturnType (funcEmptyClass::*mfpnew)(Args...);
#ifndef __linux__
			void *addr;
		} u;
		u.addr = func;
#else /* GCC's member function pointers all contain a this pointer adjustor. You'd probably set it to 0 */
			struct
			{
				void *addr;
				intptr_t adjustor;
			} s;
		} u;
		u.s.addr = func;
		u.s.adjustor = 0;
#endif
		return (ReturnType)(reinterpret_cast<funcEmptyClass*>(this_ptr)->*u.mfpnew)(args...);
	}
};

template<typename ReturnType, typename... Args>
class VCall : public CCall<ReturnType, Args...>
{
public:
	VCall() : offset(0) {};

	void Init(SourceMod::IGameConfig* config, const char* name)
	{
		if (!config->GetOffset(name, &offset))
		{
			std::string error("Failed to find offset: ");
			throw std::runtime_error(error + name);
		}
#ifdef DEBUG
		g_pSM->LogMessage(myself, "VCall<%s> Got offset %d", name, offset);
#endif
	}

	template<typename Function>
	int Init(Function f)
	{
		SourceHook::MemFuncInfo mfi = {true, -1, 0, 0};
		SourceHook::GetFuncInfo(f, mfi);
		if (mfi.thisptroffs < 0 || !mfi.isVirtual)
		{
			throw new std::runtime_error("Given function instance is not virtual!");
		}
		offset = mfi.vtblindex;
	}

	template<typename hackClass>
	void* Replace(void** vtable, ReturnType (hackClass::*infecfunc)(Args...))
	{
		void* func = nullptr;
		union
		{
			ReturnType (hackClass::*mfpnew)(Args...);
#ifndef __linux__
			void *addr;
		} u;
		u.mfpnew = infecfunc;
		func = u.addr;
#else
			struct
			{
				void *addr;
				intptr_t adjustor;
			} s;
		} u;
		u.mfpnew = infecfunc;
		func = u.s.addr;
#endif
		void* oldFunc = vtable[offset];
		vtable[offset] = func;
		return oldFunc;
	}

	ReturnType operator()(void* thisPtr, Args... args)
	{
		void **vtable = *(void ***)thisPtr;
		void *func = vtable[offset];

		return this->Call(thisPtr, func, args...);
	}

private:
	int offset;
};

template<typename ReturnType, typename... Args>
class MCall : public CCall<ReturnType, Args...>
{
public:
	MCall() : func(nullptr) {};

	void Init(SourceMod::IGameConfig* config, const char* name)
	{
		if (!config->GetMemSig(name, &func) || !func)
		{
			std::string error("Failed to find signature: ");
			throw std::runtime_error(error + name);
		}
#ifdef DEBUG
		g_pSM->LogMessage(myself, "MCall<%s> Got addr 0x%08X!", name, func);
#endif
	}

	void Init(void* addr)
	{
		func = addr;
	}

	ReturnType operator()(void* thisPtr, Args... args)
	{
		return this->Call(thisPtr, func, args...);
	}

private:
	void* func;
};

void** vtable_dup(void* thisPtr, size_t numFuncs);
void vtable_replace(void* thisPtr, void** newVtable);

/*template<typename ReturnType, typename... Args>
class vtableChange
{
	template<typename X>
	vtableChange(void* thisPtr, void* detour, X instance)
	{
		SourceHook::MemFuncInfo mfi = {true, -1, 0, 0};
		SourceHook::GetFuncInfo(instance, mfi);
		if (mfi.thisptroffs < 0 || !mfi.isVirtual)
		{
			throw new std::invalid_argument("Given function instance is not virtual!");
		}

		void **vtable = *(void ***)thisPtr;
		void *func = vtable[mfi.vtblindex];
		this->mOriginal.Init(func);
		this->mDetour.Init(detour);
		this->realThis = thisPtr;
	}
private:
	void* newThis;
	void* realThis;
	MCall<ReturnType, Args...> mDetour;
	MCall<ReturnType, Args...> mOriginal;
};*/

#endif