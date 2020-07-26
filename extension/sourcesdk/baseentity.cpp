#include "sourcesdk/baseentity.h"
#include "smsdk_ext.h"
#include <shareddefs.h>
#include <worldsize.h>

int (CBaseEntityHack::CBaseEntityHack::offset_UpdateOnRemove) = 0;

DEFINEFUNCTION_virtual_void(CBaseEntityHack, Teleport, (Vector* origin, QAngle* ang, Vector* velocity), (origin, ang, velocity));
DEFINEFUNCTION_void(CBaseEntityHack, GetVectors, (Vector* forward, Vector* right, Vector* up), (forward, right, up));
DEFINEFUNCTION_virtual_void(CBaseEntityHack, SetModel, (const char* name), (name));
DEFINEFUNCTION_virtual(CBaseEntityHack, MyNextBotPointer, INextBot*, (), ());
DEFINEFUNCTION_virtual(CBaseEntityHack, WorldSpaceCenter, const Vector&, (), ());

// Members
DEFINEVAR(CBaseEntityHack, m_Network);
DEFINEVAR(CBaseEntityHack, m_iClassname);
DEFINEVAR(CBaseEntityHack, m_angRotation);
DEFINEVAR(CBaseEntityHack, m_flSimulationTime);

float k_flMaxEntityEulerAngle = 360.0 * 1000.0f;
float k_flMaxEntityPosCoord = MAX_COORD_FLOAT;

bool CBaseEntityHack::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	SourceMod::IGameConfig* configSDKTools;
	if (!gameconfs->LoadGameConfigFile("sdktools.games", &configSDKTools, error, maxlength))
	{
		return false;
	}

	FINDSIG(config, GetVectors, "CBaseEntity::GetVectors");
	FINDVTABLE(configSDKTools, Teleport, "Teleport");
	FINDVTABLE(configSDKTools, SetModel, "SetEntityModel");
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
	OFFSETVAR_SEND(CBaseEntity, m_vecOrigin);
	OFFSETVAR_SEND(CBaseEntity, m_angRotation);
	OFFSETVAR_SEND(CBaseEntity, m_flSimulationTime);
	// m_Network is always located right before m_iClassname
	VAR_OFFSET_SET(m_Network, VAR_OFFSET(m_iClassname)-sizeof(CServerNetworkProperty));
	OFFSETVAR_SEND(CBaseEntity, m_iParentAttachment);
	OFFSETVAR_SEND(CBaseEntity, m_hMoveParent);
	VAR_OFFSET_SET(m_hMoveChild, VAR_OFFSET(m_hMoveParent) + sizeof(EHANDLE));
	VAR_OFFSET_SET(m_hMovePeer, VAR_OFFSET(m_hMoveChild) + sizeof(EHANDLE));
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

void CBaseEntityHack::InvalidatePhysicsRecursive(int nChangeFlags)
{
	int nDirtyFlags = 0;

	if (nChangeFlags & VELOCITY_CHANGED)
	{
		nDirtyFlags |= EFL_DIRTY_ABSVELOCITY;
	}

	if (nChangeFlags & POSITION_CHANGED)
	{
		nDirtyFlags |= EFL_DIRTY_ABSTRANSFORM;
		NetworkProp()->MarkPVSInformationDirty();
		// NOTE: This will also mark shadow projection + client leaf dirty
		CollisionProp()->MarkPartitionHandleDirty();
	}

	// NOTE: This has to be done after velocity + position are changed
	// because we change the nChangeFlags for the child entities
	if (nChangeFlags & ANGLES_CHANGED)
	{
		nDirtyFlags |= EFL_DIRTY_ABSTRANSFORM;
		if (CollisionProp()->DoesRotationInvalidateSurroundingBox())
		{
			// NOTE: This will handle the KD-tree, surrounding bounds, PVS
			// render-to-texture shadow, shadow projection, and client leaf dirty
			CollisionProp()->MarkSurroundingBoundsDirty();
		}

		// This is going to be used for all children: children
		// have position + velocity changed
		nChangeFlags |= POSITION_CHANGED | VELOCITY_CHANGED;
	}

	AddEFlags(nDirtyFlags);

	// Set flags for children
	bool bOnlyDueToAttachment = false;
	if (nChangeFlags & ANIMATION_CHANGED)
	{
		// Only set this flag if the only thing that changed us was the animation.
		// If position or something else changed us, then we must tell all children.
		if (!(nChangeFlags & (POSITION_CHANGED | VELOCITY_CHANGED | ANGLES_CHANGED)))
		{
			bOnlyDueToAttachment = true;
		}

		nChangeFlags = POSITION_CHANGED | ANGLES_CHANGED | VELOCITY_CHANGED;
	}

	for (CBaseEntityHack* pChild = FirstMoveChild(); pChild; pChild = pChild->NextMovePeer())
	{
		// If this is due to the parent animating, only invalidate children that are parented to an attachment
		// Entities that are following also access attachments points on parents and must be invalidated.
		if (bOnlyDueToAttachment)
		{
			if (pChild->GetParentAttachment() == 0)
				continue;
		}
		pChild->InvalidatePhysicsRecursive(nChangeFlags);
	}
}