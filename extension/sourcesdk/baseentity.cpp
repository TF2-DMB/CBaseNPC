#include "sourcesdk/baseentity.h"
#include "smsdk_ext.h"
#include <shareddefs.h>
#include <worldsize.h>

int (CBaseEntityHack::CBaseEntityHack::offset_UpdateOnRemove) = 0;

DEFINEFUNCTION_virtual_void(CBaseEntityHack, Teleport, (const Vector* origin, const QAngle* ang, const Vector* velocity), (origin, ang, velocity));
DEFINEFUNCTION_void(CBaseEntityHack, InvalidatePhysicsRecursive, (int nChangeFlags), (nChangeFlags));
DEFINEFUNCTION_virtual_void(CBaseEntityHack, GetVectors, (Vector* forward, Vector* right, Vector* up), (forward, right, up));
DEFINEFUNCTION_virtual_void(CBaseEntityHack, SetModel, (const char* name), (name));
DEFINEFUNCTION_virtual(CBaseEntityHack, MyCombatCharacterPointer, CBaseCombatCharacterHack*, (), ());
DEFINEFUNCTION_virtual(CBaseEntityHack, MyNextBotPointer, INextBot*, (), ());
DEFINEFUNCTION_virtual(CBaseEntityHack, WorldSpaceCenter, const Vector&, (), ());

// Members
DEFINEVAR(CBaseEntityHack, m_Network);
DEFINEVAR(CBaseEntityHack, m_iClassname);
DEFINEVAR(CBaseEntityHack, m_flSimulationTime);
DEFINEVAR(CBaseEntityHack, m_iParentAttachment);
DEFINEVAR(CBaseEntityHack, m_MoveType);
DEFINEVAR(CBaseEntityHack, m_hMoveParent);
DEFINEVAR(CBaseEntityHack, m_hMoveChild);
DEFINEVAR(CBaseEntityHack, m_hMovePeer);
DEFINEVAR(CBaseEntityHack, m_vecAbsOrigin);
DEFINEVAR(CBaseEntityHack, m_angAbsRotation);
DEFINEVAR(CBaseEntityHack, m_vecAbsVelocity);
DEFINEVAR(CBaseEntityHack, m_vecOrigin);
DEFINEVAR(CBaseEntityHack, m_angRotation);
DEFINEVAR(CBaseEntityHack, m_iTeamNum);

float k_flMaxEntityEulerAngle = 360.0 * 1000.0f;
float k_flMaxEntityPosCoord = MAX_COORD_FLOAT;

bool CBaseEntityHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	SourceMod::IGameConfig* configSDKTools;
	if (!gameconfs->LoadGameConfigFile("sdktools.games", &configSDKTools, error, maxlength))
	{
		return false;
	}

	FINDVTABLE(config, GetVectors, "CBaseEntity::GetVectors");
	FINDSIG(config, InvalidatePhysicsRecursive, "CBaseEntity::InvalidatePhysicsRecursive");
	FINDVTABLE(configSDKTools, Teleport, "Teleport");
	FINDVTABLE(configSDKTools, SetModel, "SetEntityModel");
	FINDVTABLE(config, MyCombatCharacterPointer, "CBaseEntity::MyCombatCharacterPointer");
	FINDVTABLE(config, MyNextBotPointer, "CBaseEntity::MyNextBotPointer");
	FINDVTABLE(config, WorldSpaceCenter, "CBaseEntity::WorldSpaceCenter");

	if (!config->GetOffset("CBaseEntity::UpdateOnRemove", &CBaseEntityHack::offset_UpdateOnRemove))
	{
		snprintf(error, maxlength, "Failed to retrieve CBaseEntity::UpdateOnRemove offset!");
		return false;
	}

	// Any entity that inherits CBaseEntity is good
	BEGIN_VAR("trigger_stun");
	OFFSETVAR_DATA(CBaseEntity, m_iClassname);
	OFFSETVAR_SEND(CBaseEntity, m_flSimulationTime);
	// m_Network is always located right before m_iClassname
	VAR_OFFSET_SET(m_Network, VAR_OFFSET(m_iClassname)-sizeof(CServerNetworkProperty));
	OFFSETVAR_SEND(CBaseEntity, m_iParentAttachment);
	OFFSETVAR_DATA(CBaseEntity, m_MoveType);
	OFFSETVAR_DATA(CBaseEntity, m_hMoveParent);
	OFFSETVAR_DATA(CBaseEntity, m_hMoveChild);
	OFFSETVAR_DATA(CBaseEntity, m_hMovePeer);
	OFFSETVAR_DATA(CBaseEntity, m_vecAbsOrigin);
	OFFSETVAR_DATA(CBaseEntity, m_angAbsRotation);
	OFFSETVAR_DATA(CBaseEntity, m_vecAbsVelocity);
	OFFSETVAR_SEND(CBaseEntity, m_vecOrigin);
	OFFSETVAR_SEND(CBaseEntity, m_angRotation);
	OFFSETVAR_SEND(CBaseEntity, m_iTeamNum);
	END_VAR;

	gameconfs->CloseGameConfigFile(configSDKTools);
	return true;
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