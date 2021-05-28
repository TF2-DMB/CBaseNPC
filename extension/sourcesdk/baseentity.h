#ifndef H_BASEENTITY_CBASENPC_
#define H_BASEENTITY_CBASENPC_
#ifdef _WIN32
#pragma once
#endif
#include <vector.h>
#include <IGameConfigs.h>
#include <iserverentity.h>
#include <itoolentity.h>
#include <shareddefs.h>
#include "helpers.h"
#include "servernetworkproperty.h"

class INextBot;

enum InvalidatePhysicsBits_t
{
	POSITION_CHANGED	= 0x1,
	ANGLES_CHANGED		= 0x2,
	VELOCITY_CHANGED	= 0x4,
	ANIMATION_CHANGED	= 0x8,
};

extern float k_flMaxEntityEulerAngle;
extern float k_flMaxEntityPosCoord;
extern CGlobalVars* gpGlobals;
extern IServerTools* servertools;

#define DECLAREVAR(type, var) \
	static int32_t offset_##var; \
	inline type* var() const \
	{ \
		return (type*)((uint8_t*)this + offset_##var); \
	} 

#define DEFINEVAR(classname, var) \
int32_t (classname::classname::offset_##var) = 0

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
	snprintf(error, maxlength, "Failed to retrieve "  #classname "::"  #var  "!"); \
	return false; \
} \
offset_##var = offset_send_info.actual_offset

#define OFFSETVAR_DATA(classname, var) \
if (!gamehelpers->FindDataMapInfo(offsetMap, #var, &offset_data_info)) \
{ \
	snprintf(error, maxlength, "Failed to retrieve "  #classname "::"  #var  "!"); \
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

class CBaseEntityHack;

#define SetThink( a ) ThinkSet( static_cast <void (CBaseEntityHack::*)(void)> (a), 0, nullptr )
#define SetContextThink( a, b, context ) ThinkSet( static_cast <void (CBaseEntityHack::*)(void)> (a), (b), context )

typedef void(CBaseEntityHack::*HBASEPTR)(void);

class CBaseCombatCharacterHack;
class CBaseAnimatingHack;

struct thinkfunc_t
{
	HBASEPTR	m_pfnThink;
	string_t	m_iszContext;
	int			m_nNextThinkTick;
	int			m_nLastThinkTick;

	DECLARE_SIMPLE_DATADESC();
};

// For seamless conversion from CBaseEntityHack to CBaseEntity
class CBaseEntity : public IServerEntity
{};

class CBaseEntityHack : public CBaseEntity
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	static VCall<void, const char*> vPostConstructor;
	void PostConstructor(const char*);

	static VCall<void> vUpdateOnRemove;
	void UpdateOnRemove(void);

	static VCall<void> vSpawn;
	void Spawn(void);

	static VCall<void, Vector*, Vector*, Vector*> vGetVectors;
	void GetVectors(Vector*, Vector*, Vector*);

	static VCall<void, const Vector*, const QAngle*, const Vector*> vTeleport;
	void Teleport(const Vector*, const QAngle*, const Vector*);

	static VCall<void, char const*> vSetModel;
	void SetModel(char const*);

	static VCall<const QAngle &> vEyeAngles;
	virtual const QAngle &EyeAngles(void);

	static VCall<CBaseCombatCharacterHack*> vMyCombatCharacterPointer;
	CBaseCombatCharacterHack* MyCombatCharacterPointer(void);
	
	static VCall<CBaseAnimatingHack*> vGetBaseAnimating;
	CBaseAnimatingHack* GetBaseAnimating(void);

	static VCall<INextBot*> vMyNextBotPointer;
	INextBot* MyNextBotPointer(void);

	static MCall<void, int> mInvalidatePhysicsRecursive;
	void InvalidatePhysicsRecursive(int nChangeFlags);

	static VCall<const Vector&> vWorldSpaceCenter;
	const Vector& WorldSpaceCenter(void);

	static MCall<void> mCalcAbsolutePosition;
	void CalcAbsolutePosition(void);
	void CalcAbsoluteVelocity(void);

	static VCall<int, const CTakeDamageInfo&> vOnTakeDamage;
	int OnTakeDamage(const CTakeDamageInfo& info);

	//static MCall<void, const Vector&> mSetAbsVelocity;
	//void SetAbsVelocity(const Vector&);

	//static MCall<void, CBaseEntity*> mSetGroundEntity;
	//void SetGroundEntity(CBaseEntity* ground);
	
public:
	friend class CServerNetworkProperty;
	CServerNetworkProperty* NetworkProp();
	const CServerNetworkProperty* NetworkProp() const;

	void DispatchUpdateTransmitState(void);

	int	entindex(void) const;
	bool IsWorld(void) const;

	int				GetFlags(void) const;

	int				GetEFlags(void) const;
	bool			IsEFlagSet(int nEFlagMask) const;
	void			SetEFlags(int iEFlags);
	void			AddEFlags(int nEFlagMask);
	void			RemoveEFlags(int nEFlagMask);

	
	int		 RegisterThinkContext(const char* szContext);
	HBASEPTR ThinkSet(HBASEPTR func, float flNextThinkTime = 0, const char* szContext = nullptr);
	void	 SetNextThink(float nextThinkTime, const char* szContext = nullptr);
	float	 GetNextThink(const char* szContext = nullptr);
	float	 GetLastThink(const char* szContext = nullptr);
	int		 GetNextThinkTick(const char* szContext = nullptr);
	int		 GetLastThinkTick(const char* szContext = nullptr);
	bool	 WillThink(void);
	void	 CheckHasThinkFunction(bool isThinking);

	const Vector& GetAbsOrigin(void) const;
	const QAngle& GetAbsAngles(void) const;
	const Vector& GetAbsVelocity(void) const;

	void SetAbsOrigin(const Vector& absOrigin);
	void SetAbsAngles(const QAngle& absAngles);
	void SetAbsVelocity(const Vector& vecAbsVelocity);
	
	matrix3x4_t&		EntityToWorldTransform(void);
	const matrix3x4_t&	EntityToWorldTransform(void) const;
	matrix3x4_t&		GetParentToWorldTransform(matrix3x4_t &tempMatrix);

	CBaseEntity	*GetGroundEntity(void);

	void	NetworkStateChanged(void);
	void	NetworkStateChanged(void* pVar);

	float GetSimulationTime(void) const;
	void  SetSimulationTime(float st);

	void SetLocalAngles(const QAngle& angles);
	const QAngle& GetLocalAngles(void) const;

	void SetLocalOrigin(const Vector& origin);
	const Vector& GetLocalOrigin(void) const;

	int		   GetParentAttachment(void);
	MoveType_t GetMoveType(void) const;
	CBaseEntityHack* GetMoveParent(void);
	CBaseEntityHack* FirstMoveChild(void);
	CBaseEntityHack* NextMovePeer(void);

	int GetTeamNumber(void) const;

	static int offset_UpdateOnRemove;

protected:
	enum thinkmethods_t
	{
		THINK_FIRE_ALL_FUNCTIONS,
		THINK_FIRE_BASE_ONLY,
		THINK_FIRE_ALL_BUT_BASE,
	};
	int	GetIndexForThinkContext(const char*);

private:
	// Members
	using think_ptr = void (CBaseEntityHack::*)(void);
	DECLAREVAR(think_ptr, m_pfnThink);
	DECLAREVAR(CServerNetworkProperty, m_Network);
	DECLAREVAR(string_t, m_iClassname);
	DECLAREVAR(short, m_nModelIndex);
	DECLAREVAR(float, m_flSimulationTime);

	DECLAREVAR(int, m_fFlags);
	DECLAREVAR(int, m_iEFlags);

	DECLAREVAR(unsigned char, m_iParentAttachment);
	DECLAREVAR(CUtlVector<thinkfunc_t>, m_aThinkFunctions);
	DECLAREVAR(int, m_nLastThinkTick);
	DECLAREVAR(int, m_nNextThinkTick);
	DECLAREVAR(unsigned char, m_MoveType);
	DECLAREVAR(EHANDLE, m_hMoveParent);
	DECLAREVAR(EHANDLE, m_hMoveChild);
	DECLAREVAR(EHANDLE, m_hMovePeer);

	DECLAREVAR(Vector, m_vecAbsOrigin);
	DECLAREVAR(QAngle, m_angAbsRotation);
	DECLAREVAR(Vector, m_vecAbsVelocity);

	DECLAREVAR(Vector, m_vecOrigin);
	DECLAREVAR(QAngle, m_angRotation);
	DECLAREVAR(Vector, m_vecVelocity);

	DECLAREVAR(matrix3x4_t, m_rgflCoordinateFrame);

	DECLAREVAR(int, m_iTeamNum);
	
	DECLAREVAR(EHANDLE, m_hGroundEntity);
};

class CBasePropDoorHack : public CBaseEntityHack
{
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

inline int CBaseEntityHack::entindex(void) const
{
	return m_Network()->entindex();
}

inline bool CBaseEntityHack::IsWorld(void) const 
{
	return entindex() == 0;
}

inline int CBaseEntityHack::GetFlags(void) const
{
	return *m_fFlags();
}

inline int CBaseEntityHack::GetEFlags(void) const
{
	return *m_iEFlags();
}

inline void CBaseEntityHack::SetEFlags(int iEFlags)
{
	*m_iEFlags() = iEFlags;

	if (iEFlags & (EFL_FORCE_CHECK_TRANSMIT | EFL_IN_SKYBOX))
	{
		this->DispatchUpdateTransmitState();
	}
}

inline void CBaseEntityHack::AddEFlags(int nEFlagMask)
{
	*m_iEFlags() |= nEFlagMask;

	if (nEFlagMask & (EFL_FORCE_CHECK_TRANSMIT | EFL_IN_SKYBOX))
	{
		this->DispatchUpdateTransmitState();
	}
}

inline void CBaseEntityHack::RemoveEFlags(int nEFlagMask)
{
	*m_iEFlags() &= ~nEFlagMask;
	
	if (nEFlagMask & (EFL_FORCE_CHECK_TRANSMIT | EFL_IN_SKYBOX))
	{
		this->DispatchUpdateTransmitState();
	}
}

inline bool CBaseEntityHack::IsEFlagSet(int nEFlagMask) const
{
	return (*m_iEFlags() & nEFlagMask) != 0;
}

inline CBaseEntity* CBaseEntityHack::GetGroundEntity(void)
{
	return *(m_hGroundEntity());
}

inline const Vector& CBaseEntityHack::GetAbsOrigin(void) const
{
	if (IsEFlagSet(EFL_DIRTY_ABSTRANSFORM))
	{
		const_cast<CBaseEntityHack*>(this)->CalcAbsolutePosition();
	}
	return *m_vecAbsOrigin();
}

inline const QAngle& CBaseEntityHack::GetAbsAngles(void) const
{
	if (IsEFlagSet(EFL_DIRTY_ABSTRANSFORM))
	{
		const_cast<CBaseEntityHack*>(this)->CalcAbsolutePosition();
	}
	return *m_angAbsRotation();
}

inline const Vector& CBaseEntityHack::GetAbsVelocity() const
{
	if (IsEFlagSet(EFL_DIRTY_ABSVELOCITY))
	{
		const_cast<CBaseEntityHack*>(this)->CalcAbsoluteVelocity();
	}
	return *m_vecAbsVelocity();
}

inline matrix3x4_t &CBaseEntityHack::EntityToWorldTransform() 
{
	if (IsEFlagSet(EFL_DIRTY_ABSTRANSFORM))
	{
		CalcAbsolutePosition();
	}
	return *m_rgflCoordinateFrame(); 
}

inline const matrix3x4_t &CBaseEntityHack::EntityToWorldTransform() const
{
	if (IsEFlagSet(EFL_DIRTY_ABSTRANSFORM))
	{
		const_cast<CBaseEntityHack*>(this)->CalcAbsolutePosition();
	}
	return *m_rgflCoordinateFrame(); 
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

inline MoveType_t CBaseEntityHack::GetMoveType() const
{
	return (MoveType_t)(unsigned char)*m_MoveType();
}

inline int CBaseEntityHack::GetTeamNumber(void) const
{
	return *m_iTeamNum();
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