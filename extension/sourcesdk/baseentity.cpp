#include "sourcesdk/baseentity.h"
#include "sourcesdk/baseanimating.h"
#include "smsdk_ext.h"
#include <CDetour/detours.h>
#include <shareddefs.h>
#include <worldsize.h>
#include <enginecallback.h>

int(CBaseEntityHack::CBaseEntityHack::offset_UpdateOnRemove) = 0;
int CBaseEntityHack::offset_GetDataDescMap = 0;

MCall<void, bool> CBaseEntityHack::CBaseEntity_Ctor;
VCall<void, const char*> CBaseEntityHack::vPostConstructor;
VCall<void> CBaseEntityHack::vUpdateOnRemove;
VCall<void> CBaseEntityHack::vSpawn;
VCall<void, Vector*, Vector*, Vector*> CBaseEntityHack::vGetVectors;
VCall<void, const Vector*, const QAngle*, const Vector*> CBaseEntityHack::vTeleport;
VCall<void, char const*> CBaseEntityHack::vSetModel;
VCall<const QAngle&> CBaseEntityHack::vEyeAngles;
VCall<CBaseCombatCharacterHack*> CBaseEntityHack::vMyCombatCharacterPointer;
VCall<CBaseAnimatingHack*> CBaseEntityHack::vGetBaseAnimating;
VCall<INextBot*> CBaseEntityHack::vMyNextBotPointer;
MCall<void, int> CBaseEntityHack::mInvalidatePhysicsRecursive;
VCall<const Vector&> CBaseEntityHack::vWorldSpaceCenter;
VCall<int, const CTakeDamageInfo&> CBaseEntityHack::vOnTakeDamage;
MCall<void> CBaseEntityHack::mCalcAbsolutePosition;

#ifndef __linux__
class IEntityListener;
MCall<void, CBaseEntity*> SimThink_EntityChanged; // In reality CSimThinkManager::EntityChanged
IEntityListener* g_pSimThinkManager = nullptr;
CDetour* g_pSimThink_EntityChangedDetour = nullptr;
DETOUR_DECL_MEMBER1(SimThink_EntityChanged, void, CBaseEntity*, ent)
{
	if (g_pSimThinkManager == nullptr)
	{
		g_pSimThinkManager = (IEntityListener*)this;
	}
	DETOUR_MEMBER_CALL(SimThink_EntityChanged)(ent);
}
#else
FCall<void, CBaseEntity*> SimThink_EntityChanged;
#endif

float k_flMaxEntityEulerAngle = 360.0 * 1000.0f;
float k_flMaxEntityPosCoord = MAX_COORD_FLOAT;

// Members
DEFINEVAR(CBaseEntityHack, m_pfnThink);
DEFINEVAR(CBaseEntityHack, m_Network);
DEFINEVAR(CBaseEntityHack, m_iClassname);
DEFINEVAR(CBaseEntityHack, m_nModelIndex);
DEFINEVAR(CBaseEntityHack, m_flSimulationTime);
DEFINEVAR(CBaseEntityHack, m_nLastThinkTick);
DEFINEVAR(CBaseEntityHack, m_nNextThinkTick);
DEFINEVAR(CBaseEntityHack, m_aThinkFunctions);
DEFINEVAR(CBaseEntityHack, m_fFlags);
DEFINEVAR(CBaseEntityHack, m_iEFlags);
DEFINEVAR(CBaseEntityHack, m_iParentAttachment);
DEFINEVAR(CBaseEntityHack, m_MoveType);
DEFINEVAR(CBaseEntityHack, m_hMoveParent);
DEFINEVAR(CBaseEntityHack, m_hMoveChild);
DEFINEVAR(CBaseEntityHack, m_hMovePeer);
DEFINEVAR(CBaseEntityHack, m_Collision);
DEFINEVAR(CBaseEntityHack, m_vecAbsOrigin);
DEFINEVAR(CBaseEntityHack, m_angAbsRotation);
DEFINEVAR(CBaseEntityHack, m_vecAbsVelocity);
DEFINEVAR(CBaseEntityHack, m_vecOrigin);
DEFINEVAR(CBaseEntityHack, m_angRotation);
DEFINEVAR(CBaseEntityHack, m_vecVelocity);
DEFINEVAR(CBaseEntityHack, m_rgflCoordinateFrame);
DEFINEVAR(CBaseEntityHack, m_iTeamNum);
DEFINEVAR(CBaseEntityHack, m_hGroundEntity);
DEFINEVAR(CBaseEntityHack, m_ModelName);

bool CBaseEntityHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	// Some function signatures & offsets can be fetched from Sourcemod, yay!
	SourceMod::IGameConfig* configCore;
	if (!gameconfs->LoadGameConfigFile("core.games", &configCore, error, maxlength))
	{
		return false;
	}

	SourceMod::IGameConfig* configSDKTools;
	if (!gameconfs->LoadGameConfigFile("sdktools.games", &configSDKTools, error, maxlength))
	{
		return false;
	}
	SourceMod::IGameConfig* configSDKHooks;
	if (!gameconfs->LoadGameConfigFile("sdkhooks.games", &configSDKHooks, error, maxlength))
	{
		return false;
	}

	try
	{
		CBaseEntity_Ctor.Init(config, "CBaseEntity::CBaseEntity");
		mInvalidatePhysicsRecursive.Init(config, "CBaseEntity::InvalidatePhysicsRecursive");
		mCalcAbsolutePosition.Init(config, "CBaseEntity::CalcAbsolutePosition");
		vPostConstructor.Init(config, "CBaseEntity::PostConstructor");
		vUpdateOnRemove.Init(config, "CBaseEntity::UpdateOnRemove");
		vSpawn.Init(configSDKHooks, "Spawn");
		vGetVectors.Init(config, "CBaseEntity::GetVectors");
		vTeleport.Init(configSDKTools, "Teleport");
		vSetModel.Init(configSDKTools, "SetEntityModel");
		vMyCombatCharacterPointer.Init(config, "CBaseEntity::MyCombatCharacterPointer");
		vGetBaseAnimating.Init(config, "CBaseEntity::GetBaseAnimating");
		vMyNextBotPointer.Init(config, "CBaseEntity::MyNextBotPointer");
		vWorldSpaceCenter.Init(config, "CBaseEntity::WorldSpaceCenter");
		vEyeAngles.Init(config, "CBaseEntity::EyeAngles");
		vOnTakeDamage.Init(configSDKHooks, "OnTakeDamage");
		// This function also doesn't warrant its own file, as it only ever used by CBaseEntity
		SimThink_EntityChanged.Init(config, "SimThink_EntityChanged");
#ifndef __linux__
		g_pSimThink_EntityChangedDetour = DETOUR_CREATE_MEMBER(SimThink_EntityChanged, "SimThink_EntityChanged")
			g_pSimThink_EntityChangedDetour->EnableDetour();
#endif
	}
	catch (const std::exception & e)
	{
		// Could use strncpy, but compiler complains
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}

	if (!configCore->GetOffset("GetDataDescMap", &CBaseEntityHack::offset_GetDataDescMap))
	{
		snprintf(error, maxlength, "Couldn't find GetDataDescMap offset!");
		return false;
	}

	if (!config->GetOffset("CBaseEntity::UpdateOnRemove", &CBaseEntityHack::offset_UpdateOnRemove))
	{
		snprintf(error, maxlength, "Failed to retrieve CBaseEntity::UpdateOnRemove offset!");
		return false;
	}

	// Any entity that inherits CBaseEntity is good
	BEGIN_VAR("trigger_stun");
	OFFSETVAR_DATA(CBaseEntity, m_pfnThink);
	OFFSETVAR_DATA(CBaseEntity, m_iClassname);
	OFFSETVAR_DATA(CBaseEntity, m_nModelIndex);
	OFFSETVAR_SEND(CBaseEntity, m_flSimulationTime);
	OFFSETVAR_DATA(CBaseEntity, m_nLastThinkTick);
	OFFSETVAR_DATA(CBaseEntity, m_nNextThinkTick);
	OFFSETVAR_DATA(CBaseEntity, m_aThinkFunctions);
	// m_Network is always located right before m_iClassname
	VAR_OFFSET_SET(m_Network, VAR_OFFSET(m_iClassname) - sizeof(CServerNetworkProperty));
	OFFSETVAR_DATA(CBaseEntity, m_fFlags);
	OFFSETVAR_DATA(CBaseEntity, m_iEFlags);
	OFFSETVAR_SEND(CBaseEntity, m_iParentAttachment);
	OFFSETVAR_DATA(CBaseEntity, m_MoveType);
	OFFSETVAR_DATA(CBaseEntity, m_hMoveParent);
	OFFSETVAR_DATA(CBaseEntity, m_hMoveChild);
	OFFSETVAR_DATA(CBaseEntity, m_hMovePeer);
	VAR_OFFSET_SET(m_Collision, VAR_OFFSET(m_hMovePeer) + sizeof(EHANDLE));
	OFFSETVAR_DATA(CBaseEntity, m_vecAbsOrigin);
	OFFSETVAR_DATA(CBaseEntity, m_angAbsRotation);
	OFFSETVAR_DATA(CBaseEntity, m_vecAbsVelocity);
	OFFSETVAR_SEND(CBaseEntity, m_vecOrigin);
	OFFSETVAR_SEND(CBaseEntity, m_angRotation);
	OFFSETVAR_DATA(CBaseEntity, m_vecVelocity);
	OFFSETVAR_DATA(CBaseEntity, m_rgflCoordinateFrame);
	OFFSETVAR_SEND(CBaseEntity, m_iTeamNum);
	OFFSETVAR_DATA(CBaseEntity, m_hGroundEntity);
	OFFSETVAR_DATA(CBaseEntity, m_ModelName);
	END_VAR;

	gameconfs->CloseGameConfigFile(configSDKTools);
	gameconfs->CloseGameConfigFile(configSDKHooks);
	gameconfs->CloseGameConfigFile(configCore);

#ifndef __linux__
	if (g_pSimThinkManager == nullptr)
	{
		snprintf(error, maxlength, "Failed to retrieve CSimThinkManager - g_SimThinkManager!");
		return false;
	}
#endif

	return true;
}

void CBaseEntityHack::DispatchUpdateTransmitState(void)
{
	// Admittedly a far fetched hack BUT IServerEntity::SetModelIndex is only implemented
	// by CBaseEntity - https://github.com/alliedmodders/hl2sdk/blob/0ef5d3d482157bc0bb3aafd37c08961373f87bfd/game/server/baseentity.cpp#L621
	// If the given model index is equal to m_nModelIndex, then only CBaseEntity::DispatchUpdateTransmitState()
	// will be called. Which is what we want here!
	// Hopefully we will never need the return value! Although we could probably read the assembly register

	// To-do? : There's potentially an assertion to avoid - if this ever happens we need to set
	// m_bDynamicModelAllowed on true when m_nModelIndex is < -1 and m_bDynamicModelAllowed false
	// then reset it. m_bDynamicModelAllowed can be located right after the netprop/datamap m_vecViewOffset
	this->SetModelIndex(*m_nModelIndex());
}

void CBaseEntityHack::InvalidatePhysicsRecursive(int nChangeFlags)
{
	mInvalidatePhysicsRecursive(this, nChangeFlags);
}

/*void CBaseEntityHack::SetGroundEntity(CBaseEntity* ground)
{
	mSetGroundEntity(this, ground);
}*/

void CBaseEntityHack::PostConstructor(const char* name)
{
	vPostConstructor(this, name);
}

void CBaseEntityHack::UpdateOnRemove(void)
{
	vUpdateOnRemove(this);
}

void CBaseEntityHack::Spawn(void)
{
	vSpawn(this);
}

void CBaseEntityHack::GetVectors(Vector* v1, Vector* v2, Vector* v3)
{
	vGetVectors(this, v1, v2, v3);
}

void CBaseEntityHack::Teleport(const Vector* v1, const QAngle* v2, const Vector* v3)
{
	vTeleport(this, v1, v2, v3);
}

void CBaseEntityHack::SetModel(char const* mdl)
{
	vSetModel(this, mdl);
}

CBaseCombatCharacterHack* CBaseEntityHack::MyCombatCharacterPointer(void)
{
	return vMyCombatCharacterPointer(this);
}

CBaseAnimatingHack* CBaseEntityHack::GetBaseAnimating(void)
{
	return vGetBaseAnimating(this);
}

INextBot* CBaseEntityHack::MyNextBotPointer(void)
{
	return vMyNextBotPointer(this);
}

const Vector& CBaseEntityHack::WorldSpaceCenter(void)
{
	return vWorldSpaceCenter(this);
}

int CBaseEntityHack::OnTakeDamage(const CTakeDamageInfo& info)
{
	return vOnTakeDamage(this, info);
}

const QAngle& CBaseEntityHack::EyeAngles(void)
{
	return vEyeAngles(this);
}

void CBaseEntityHack::CalcAbsolutePosition(void)
{
	mCalcAbsolutePosition(this);
}

void CBaseEntityHack::CalcAbsoluteVelocity(void)
{
	if (!IsEFlagSet(EFL_DIRTY_ABSVELOCITY))
	{
		return;
	}

	RemoveEFlags(EFL_DIRTY_ABSVELOCITY);

	CBaseEntityHack* pMoveParent = GetMoveParent();
	if (!pMoveParent)
	{
		*m_vecAbsVelocity() = *m_vecVelocity();
		return;
	}

	// This transforms the local velocity into world space
	Vector out;
	VectorRotate(*m_vecVelocity(), pMoveParent->EntityToWorldTransform(), out);
	// Now add in the parent abs velocity
	out += pMoveParent->GetAbsVelocity();

	*m_vecAbsVelocity() = out;
}

void CBaseEntityHack::SetAbsOrigin(const Vector& absOrigin)
{
	AssertMsg(absOrigin.IsValid(), "Invalid origin set");

	// This is necessary to get the other fields of m_rgflCoordinateFrame ok
	CalcAbsolutePosition();

	if (*m_vecAbsOrigin() == absOrigin)
	{
		return;
	}

	// All children are invalid, but we are not
	InvalidatePhysicsRecursive(POSITION_CHANGED);
	RemoveEFlags(EFL_DIRTY_ABSTRANSFORM);

	*m_vecAbsOrigin() = absOrigin;

	MatrixSetColumn(absOrigin, 3, *m_rgflCoordinateFrame());

	Vector vecNewOrigin;
	CBaseEntityHack* pMoveParent = GetMoveParent();
	if (!pMoveParent)
	{
		vecNewOrigin = absOrigin;
	}
	else
	{
		matrix3x4_t tempMat;
		matrix3x4_t& parentTransform = GetParentToWorldTransform(tempMat);

		// Moveparent case: transform the abs position into local space
		VectorITransform(absOrigin, parentTransform, vecNewOrigin);
	}

	if (*m_vecOrigin() != vecNewOrigin)
	{
		NETWORKVAR_UPDATE(m_vecOrigin(), vecNewOrigin);
		SetSimulationTime(gpGlobals->curtime);
	}
}

void CBaseEntityHack::SetAbsAngles(const QAngle& absAngles)
{
	// This is necessary to get the other fields of m_rgflCoordinateFrame ok
	CalcAbsolutePosition();

	// FIXME: The normalize caused problems in server code like momentary_rot_button that isn't
	//        handling things like +/-180 degrees properly. This should be revisited.
	//QAngle angleNormalize( AngleNormalize( absAngles.x ), AngleNormalize( absAngles.y ), AngleNormalize( absAngles.z ) );

	if (*m_angAbsRotation() == absAngles)
	{
		return;
	}

	// All children are invalid, but we are not
	InvalidatePhysicsRecursive(ANGLES_CHANGED);
	RemoveEFlags(EFL_DIRTY_ABSTRANSFORM);

	*m_angAbsRotation() = absAngles;
	AngleMatrix(absAngles, *m_rgflCoordinateFrame());
	auto m = *m_vecAbsOrigin();
	MatrixSetColumn(m, 3, *m_rgflCoordinateFrame());
	*m_vecAbsOrigin() = m;

	QAngle angNewRotation;
	CBaseEntityHack* pMoveParent = GetMoveParent();
	if (!pMoveParent)
	{
		angNewRotation = absAngles;
	}
	else
	{
		if (absAngles == pMoveParent->GetAbsAngles())
		{
			angNewRotation.Init();
		}
		else
		{
			// Moveparent case: transform the abs transform into local space
			matrix3x4_t worldToParent, localMatrix;
			MatrixInvert(pMoveParent->EntityToWorldTransform(), worldToParent);
			ConcatTransforms(worldToParent, *m_rgflCoordinateFrame(), localMatrix);
			MatrixAngles(localMatrix, angNewRotation);
		}
	}

	if (*m_angRotation() != angNewRotation)
	{
		NETWORKVAR_UPDATE(m_angRotation(), angNewRotation);
		SetSimulationTime(gpGlobals->curtime);
	}
}

void CBaseEntityHack::SetAbsVelocity(const Vector& vecAbsVelocity)
{
	if (*m_vecAbsVelocity() == vecAbsVelocity)
	{
		return;
	}

	// The abs velocity won't be dirty since we're setting it here
	// All children are invalid, but we are not
	InvalidatePhysicsRecursive(VELOCITY_CHANGED);
	RemoveEFlags(EFL_DIRTY_ABSVELOCITY);

	*m_vecAbsVelocity() = vecAbsVelocity;

	// NOTE: Do *not* do a network state change in this case.
	// m_vecVelocity is only networked for the player, which is not manual mode
	CBaseEntityHack* pMoveParent = GetMoveParent();
	if (!pMoveParent)
	{
		NETWORKVAR_UPDATE(m_vecVelocity(), vecAbsVelocity)
			return;
	}

	// First subtract out the parent's abs velocity to get a relative
	// velocity measured in world space
	Vector relVelocity;
	VectorSubtract(vecAbsVelocity, pMoveParent->GetAbsVelocity(), relVelocity);

	// Transform relative velocity into parent space
	Vector vNew;
	VectorIRotate(relVelocity, pMoveParent->EntityToWorldTransform(), vNew);
	NETWORKVAR_UPDATE(m_vecVelocity(), vNew);
}

matrix3x4_t& CBaseEntityHack::GetParentToWorldTransform(matrix3x4_t& tempMatrix)
{
	CBaseEntityHack* pMoveParent = GetMoveParent();
	if (!pMoveParent)
	{
		SetIdentityMatrix(tempMatrix);
		return tempMatrix;
	}

	auto attach = *m_iParentAttachment();
	if (attach != 0)
	{
		MDLCACHE_CRITICAL_SECTION();

		CBaseAnimatingHack* pAnimating = pMoveParent->GetBaseAnimating();
		if (pAnimating && pAnimating->GetAttachment(attach, tempMatrix))
		{
			return tempMatrix;
		}
	}
	// If we fall through to here, then just use the move parent's abs origin and angles.
	return pMoveParent->EntityToWorldTransform();
}

int	CBaseEntityHack::GetIndexForThinkContext(const char* pszContext)
{
	CUtlVector<thinkfunc_t>* funcs = m_aThinkFunctions();
	for (int i = 0; i < funcs->Size(); i++)
	{
		if (!Q_strncmp(STRING(funcs->Element(i).m_iszContext), pszContext, MAX_CONTEXT_LENGTH))
		{
			return i;
		}
	}
	return NO_THINK_CONTEXT;
}

int CBaseEntityHack::RegisterThinkContext(const char* szContext)
{
	int iIndex = GetIndexForThinkContext(szContext);
	if (iIndex != NO_THINK_CONTEXT)
	{
		return iIndex;
	}

	// Make a new think func
	thinkfunc_t sNewFunc;
	Q_memset(&sNewFunc, 0, sizeof(sNewFunc));
	sNewFunc.m_pfnThink = nullptr;
	sNewFunc.m_nNextThinkTick = 0;
	sNewFunc.m_iszContext = AllocPooledString(szContext);

	// Insert it into our list
	return m_aThinkFunctions()->AddToTail(sNewFunc);
}

HBASEPTR CBaseEntityHack::ThinkSet(HBASEPTR func, float thinkTime, const char* szContext)
{
	// Old system?
	if (!szContext)
	{
		union
		{
			HBASEPTR mfpnew;
#ifndef __linux__
			struct
			{
				void* addr;
			} s;
		} u;
		u.mfpnew = func;
#else
			struct
			{
				void* addr;
				intptr_t adjustor;
			} s;
		} u;
		u.mfpnew = func;
#endif
		memcpy((((uint8_t*)this) + CBaseEntityHack::offset_m_pfnThink), &u, sizeof(u.s));
		return func;
	}

	// Find the think function in our list, and if we couldn't find it, register it
	int iIndex = GetIndexForThinkContext(szContext);
	if (iIndex == NO_THINK_CONTEXT)
	{
		iIndex = RegisterThinkContext(szContext);
	}

	CUtlVector<thinkfunc_t>* funcs = m_aThinkFunctions();
	funcs->Element(iIndex).m_pfnThink = func;
	if (thinkTime != 0)
	{
		int thinkTick = (thinkTime == TICK_NEVER_THINK) ? TICK_NEVER_THINK : TIME_TO_TICKS(thinkTime);
		funcs->Element(iIndex).m_nNextThinkTick = thinkTick;
		CheckHasThinkFunction(thinkTick == TICK_NEVER_THINK ? false : true);
	}
	return func;
}

void CBaseEntityHack::SetNextThink(float thinkTime, const char* szContext)
{
	int thinkTick = (thinkTime == TICK_NEVER_THINK) ? TICK_NEVER_THINK : TIME_TO_TICKS(thinkTime);

	// Are we currently in a think function with a context?
	int iIndex = 0;
	if (!szContext)
	{
		// Old system
		NETWORKVAR_UPDATE(m_nNextThinkTick(), thinkTick);
		CheckHasThinkFunction(thinkTick == TICK_NEVER_THINK ? false : true);
		return;
	}
	else
	{
		// Find the think function in our list, and if we couldn't find it, register it
		iIndex = GetIndexForThinkContext(szContext);
		if (iIndex == NO_THINK_CONTEXT)
		{
			iIndex = RegisterThinkContext(szContext);
		}
	}

	// Old system
	m_aThinkFunctions()->Element(iIndex).m_nNextThinkTick = thinkTick;
	CheckHasThinkFunction(thinkTick == TICK_NEVER_THINK ? false : true);
}

float CBaseEntityHack::GetNextThink(const char* szContext)
{
	// Are we currently in a think function with a context?
	int iIndex = 0;
	if (!szContext)
	{
		int nextThink = *(m_nNextThinkTick());
		if (nextThink == TICK_NEVER_THINK)
		{
			return TICK_NEVER_THINK;
		}

		// Old system
		return TICK_INTERVAL * (nextThink);
	}
	else
	{
		// Find the think function in our list
		iIndex = GetIndexForThinkContext(szContext);
	}

	CUtlVector<thinkfunc_t>* funcs = m_aThinkFunctions();
	if (iIndex == funcs->InvalidIndex())
	{
		return TICK_NEVER_THINK;
	}

	if (funcs->Element(iIndex).m_nNextThinkTick == TICK_NEVER_THINK)
	{
		return TICK_NEVER_THINK;
	}
	return TICK_INTERVAL * (funcs->Element(iIndex).m_nNextThinkTick);
}

int	CBaseEntityHack::GetNextThinkTick(const char* szContext)
{
	// Are we currently in a think function with a context?
	int iIndex = 0;
	if (!szContext)
	{
		int nextThink = *(m_nNextThinkTick());
		if (nextThink == TICK_NEVER_THINK)
		{
			return TICK_NEVER_THINK;
		}
		// Old system
		return nextThink;
	}
	else
	{
		// Find the think function in our list
		iIndex = GetIndexForThinkContext(szContext);

		// Looking up an invalid think context!
		Assert(iIndex != -1);
	}

	CUtlVector<thinkfunc_t>* funcs = m_aThinkFunctions();
	if ((iIndex == -1) || (funcs->Element(iIndex).m_nNextThinkTick == TICK_NEVER_THINK))
	{
		return TICK_NEVER_THINK;
	}

	return funcs->Element(iIndex).m_nNextThinkTick;
}

float CBaseEntityHack::GetLastThink(const char* szContext)
{
	// Are we currently in a think function with a context?
	int iIndex = 0;
	if (!szContext)
	{
		// Old system
		return *(m_nLastThinkTick()) * TICK_INTERVAL;
	}
	else
	{
		// Find the think function in our list
		iIndex = GetIndexForThinkContext(szContext);
	}

	return m_aThinkFunctions()->Element(iIndex).m_nLastThinkTick * TICK_INTERVAL;
}

int CBaseEntityHack::GetLastThinkTick(const char* szContext)
{
	// Are we currently in a think function with a context?
	int iIndex = 0;
	if (!szContext)
	{
		// Old system
		return *m_nLastThinkTick();
	}
	else
	{
		// Find the think function in our list
		iIndex = GetIndexForThinkContext(szContext);
	}

	return m_aThinkFunctions()->Element(iIndex).m_nLastThinkTick;
}

bool CBaseEntityHack::WillThink(void)
{
	if (*m_nNextThinkTick() > 0)
	{
		return true;
	}

	CUtlVector<thinkfunc_t>* funcs = m_aThinkFunctions();
	for (int i = 0; i < funcs->Count(); i++)
	{
		if (funcs->Element(i).m_nNextThinkTick > 0)
		{
			return true;
		}
	}

	return false;
}

void CBaseEntityHack::CheckHasThinkFunction(bool isThinking)
{
	if (IsEFlagSet(EFL_NO_THINK_FUNCTION) && isThinking)
	{
		RemoveEFlags(EFL_NO_THINK_FUNCTION);
	}
	else if (!isThinking && !IsEFlagSet(EFL_NO_THINK_FUNCTION) && !WillThink())
	{
		AddEFlags(EFL_NO_THINK_FUNCTION);
	}
#ifndef __linux__
	SimThink_EntityChanged(g_pSimThinkManager, this);
#else
	SimThink_EntityChanged(this);
#endif
}

void CBaseEntityHack::SetLocalOrigin(const Vector& origin)
{
	if (!IsEntityPositionReasonable(origin))
	{
		return;
	}

	Vector* entOrigin = m_vecOrigin();
	if (*entOrigin != origin)
	{
		NETWORKVAR_UPDATE(entOrigin, origin);
		InvalidatePhysicsRecursive(POSITION_CHANGED);
		SetSimulationTime(gpGlobals->curtime);
	}
}


void CBaseEntityHack::SetLocalAngles(const QAngle& angles)
{
	if (!IsEntityQAngleReasonable(angles))
	{
		return;
	}

	QAngle* ang = m_angRotation();
	if (*ang != angles)
	{
		NETWORKVAR_UPDATE(ang, angles);
		InvalidatePhysicsRecursive(ANGLES_CHANGED);
		SetSimulationTime(gpGlobals->curtime);
	}
}