#ifndef H_BASEENTITY_CBASENPC_
#define H_BASEENTITY_CBASENPC_
#ifdef _WIN32
#pragma once
#endif
#include <vector.h>
#include <IGameConfigs.h>
#include <iserverentity.h>
#include <itoolentity.h>
#include "servernetworkproperty.h"

class INextBot;

extern float k_flMaxEntityEulerAngle;
extern float k_flMaxEntityPosCoord;
extern CGlobalVars* gpGlobals;
extern IServerTools* servertools;

enum InvalidatePhysicsBits_t
{
	POSITION_CHANGED = 0x1,
	ANGLES_CHANGED = 0x2,
	VELOCITY_CHANGED = 0x4,
	ANIMATION_CHANGED = 0x8,
};

#define DECLAREVAR(type, var) \
private: \
	static int(offset_##var); \
public: \
	inline type* ##var() const \
	{ \
		return (type*)((uint8_t*)this + offset_##var); \
	} \
private:

#define DEFINEVAR(classname, var) \
int (classname::classname::offset_##var) = 0

#define BEGIN_VAR(classentity) \
SourceMod::sm_sendprop_info_t offset_send_info; \
SourceMod::sm_datatable_info_t offset_data_info; \
CBaseEntity* offsetEntity = servertools->CreateEntityByName(classentity); \
datamap_t* offsetMap = gamehelpers->GetDataMap(offsetEntity)

#define END_VAR \
	servertools->RemoveEntityImmediate(offsetEntity)

#define OFFSETVAR_SEND(classname, var) \
if (!gamehelpers->FindSendPropInfo(#classname, #var, &offset_send_info)) \
{ \
	snprintf(error, maxlength, "Failed to retrieve"  #classname "::"  #var  "!"); \
	return false; \
} \
offset_##var = offset_send_info.actual_offset

#define OFFSETVAR_DATA(classname, var) \
if (!gamehelpers->FindDataMapInfo(offsetMap, #var, &offset_data_info)) \
{ \
	snprintf(error, maxlength, "Failed to retrieve"  #classname "::"  #var  "!"); \
	return false; \
} \
offset_##var = offset_data_info.actual_offset

#define VAR_OFFSET(var) \
	offset_##var

#define VAR_OFFSET_SET(var, val) \
	offset_##var = val


#define NETWORKVAR_UPDATE(var, value) \
	*var = value; \
	this->NetworkStateChanged(var);

#define DECLAREFUNCTION_virtual(name, returntype, parameters) \
public: \
	returntype name parameters; \
private: \
	static int(ThisClass::offset_func_##name); \
	typedef returntype (ThisClass::*func_##name) parameters

#define DECLAREFUNCTION(name, returntype, parameters) \
public: \
	returntype name parameters; \
private: \
	static returntype (ThisClass::*func_##name) parameters

#define DEFINEFUNCTION_virtual_void(classname, name, parameters, paramscall) \
int (classname::classname::offset_func_##name) = 0; \
void classname::name parameters \
{ \
	classname::func_##name func = (*reinterpret_cast<classname::func_##name **>(this))[this->offset_func_##name]; \
	(this->*func) paramscall; \
}

#define DEFINEFUNCTION_virtual(classname, name, returntype, parameters, paramscall) \
int (classname::classname::offset_func_##name) = 0; \
returntype classname::name parameters \
{ \
	classname::func_##name func = (*reinterpret_cast<classname::func_##name **>(this))[this->offset_func_##name]; \
	return (this->*func) paramscall; \
}

#define DEFINEFUNCTION_void(classname, name, parameters, paramscall) \
void (classname::*classname::func_##name) parameters = nullptr; \
void classname::name parameters \
{ \
	(this->*func_##name) paramscall; \
}

#define DEFINEFUNCTION(classname, name, returntype, parameters, paramscall) \
returntype (classname::*classname::func_##name) parameters = nullptr; \
returntype classname::name parameters \
{ \
	return (this->*func_##name) paramscall; \
}

#define FINDVTABLE(config, name, namestring) \
if (!config->GetOffset(namestring, &ThisClass::offset_func_##name)) \
{ \
	snprintf(error, maxlength, "Failed to retrieve " namestring " offset!"); \
	return false; \
} \
ThisClass::offset_func_##name *= 4

#define FINDSIG(config, name, namestring) \
if (!config->GetMemSig(namestring, reinterpret_cast<void**>(&ThisClass::func_##name))) \
{ \
	snprintf(error, maxlength, "Couldn't locate function " namestring "!"); \
	return false; \
}

class CBaseEntityHack : public IServerEntity
{
public:
	DECLARE_CLASS_NOBASE(CBaseEntityHack);

	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	DECLAREFUNCTION(GetVectors, void, (Vector* forward, Vector* right, Vector* up));
	DECLAREFUNCTION_virtual(Teleport, void, (Vector* origin, QAngle* ang, Vector* velocity));
	DECLAREFUNCTION_virtual(SetModel, void, (char const* model));
	DECLAREFUNCTION_virtual(MyNextBotPointer, INextBot*, ());
	DECLAREFUNCTION_virtual(WorldSpaceCenter, const Vector&, ());

public:
	CServerNetworkProperty* NetworkProp();
	const CServerNetworkProperty* NetworkProp() const;
	void	NetworkStateChanged();
	void	NetworkStateChanged(void* pVar);

	float GetSimulationTime() const;
	void SetSimulationTime(float st);

	void SetLocalAngles(const QAngle& angles);
	const QAngle& GetLocalAngles(void) const;

	void SetLocalOrigin(const Vector& origin);
	const Vector& GetLocalOrigin(void) const;

	void InvalidatePhysicsRecursive(int nChangeFlags);

	int			GetParentAttachment();
	CBaseEntityHack* GetMoveParent(void);
	CBaseEntityHack* FirstMoveChild(void);
	CBaseEntityHack* NextMovePeer(void);

	static int(CBaseEntityHack::offset_UpdateOnRemove);

private:
	// Members
	DECLAREVAR(CServerNetworkProperty, m_Network);
	DECLAREVAR(string_t, m_iClassname);
	DECLAREVAR(Vector, m_vecOrigin);
	DECLAREVAR(QAngle, m_angRotation);
	DECLAREVAR(float, m_flSimulationTime);

	DECLAREVAR(unsigned char, m_iParentAttachment);
	DECLAREVAR(EHANDLE, m_hMoveParent);
	DECLAREVAR(EHANDLE, m_hMoveChild);
	DECLAREVAR(EHANDLE, m_hMovePeer);
};

inline CServerNetworkProperty* CBaseEntityHack::NetworkProp()
{
	return m_Network();
}

inline const CServerNetworkProperty* CBaseEntityHack::NetworkProp() const
{
	return m_Network();
}

inline void	CBaseEntityHack::NetworkStateChanged()
{
	NetworkProp()->NetworkStateChanged();
}


inline void	CBaseEntityHack::NetworkStateChanged(void* pVar)
{
	// Good, they passed an offset so we can track this variable's change
	// and avoid sending the whole entity.
	NetworkProp()->NetworkStateChanged((char*)pVar - (char*)this);
}

inline float CBaseEntityHack::GetSimulationTime() const
{
	return *m_flSimulationTime();
}

inline void CBaseEntityHack::SetSimulationTime(float st)
{
	*m_flSimulationTime() = st;
}

inline const QAngle& CBaseEntityHack::GetLocalAngles(void) const
{
	return *m_angRotation();
}

inline const Vector& CBaseEntityHack::GetLocalOrigin(void) const
{
	return *m_vecOrigin();
}

inline CBaseEntityHack* CBaseEntityHack::GetMoveParent(void)
{
	return (CBaseEntityHack*)m_hMoveParent()->Get();
}

inline CBaseEntityHack* CBaseEntityHack::FirstMoveChild(void)
{
	return (CBaseEntityHack*)m_hMoveChild()->Get();
}

inline CBaseEntityHack* CBaseEntityHack::NextMovePeer(void)
{
	return (CBaseEntityHack*)m_hMovePeer()->Get();
}

inline int CBaseEntityHack::GetParentAttachment()
{
	return *m_iParentAttachment();
}

inline bool IsEntityQAngleReasonable(const QAngle& q)
{
	float r = k_flMaxEntityEulerAngle;
	return
		q.x > -r && q.x < r &&
		q.y > -r && q.y < r &&
		q.z > -r && q.z < r;
}

inline bool IsEntityPositionReasonable(const Vector& v)
{
	float r = k_flMaxEntityPosCoord;
	return
		v.x > -r && v.x < r &&
		v.y > -r && v.y < r &&
		v.z > -r && v.z < r;
}

#endif // H_BASEENTITY_CBASENPC_