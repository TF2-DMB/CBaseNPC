#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <dhooks>
#define CBASENPC_CORE
#include <cbasenpc>

//#define DEBUG
#define EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS	(1<<14)

#define EF_BONEMERGE			0x001 	// Performs bone merge on client side
#define	EF_BRIGHTLIGHT 			0x002	// DLIGHT centered at entity origin
#define	EF_DIMLIGHT 			0x004	// player flashlight
#define	EF_NOINTERP				0x008	// don't interpolate the next frame
#define	EF_NOSHADOW				0x010	// Don't cast no shadow
#define	EF_NODRAW				0x020	// don't draw entity
#define	EF_NORECEIVESHADOW		0x040	// Don't receive no shadow
#define	EF_BONEMERGE_FASTCULL	0x080	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
										// parent and uses the parent's bbox + the max extents of the aiment.
										// Otherwise, it sets up the parent's bones every frame to figure out where to place
										// the aiment, which is inefficient because it'll setup the parent's bones even if
										// the parent is not in the PVS.
#define	EF_ITEM_BLINK			0x100	// blink an item so that the user notices it.
#define	EF_PARENT_ANIMATES		0x200	// always assume that the parent entity is animating
#define	EF_MAX_BITS = 10

#if defined DEBUG
int g_iLaserIndex;
#endif

int g_CBaseNPCEntityRef[MAX_NPCS] = {-1, ...};

ArrayList				g_CBaseNPCHooks[MAX_NPCS];
INextBot				g_CBaseNPCNextBotInterface[MAX_NPCS];
NextBotGroundLocomotion g_CBaseNPCLocomotionInterface[MAX_NPCS];
IBody					g_CBaseNPCBodyInterface[MAX_NPCS];

float g_CBaseNPCflStepSize[MAX_NPCS];
float g_CBaseNPCflGravity[MAX_NPCS];
float g_CBaseNPCflAcceleration[MAX_NPCS];
float g_CBaseNPCflJumpHeight[MAX_NPCS];
float g_CBaseNPCflWalkSpeed[MAX_NPCS];
float g_CBaseNPCflRunSpeed[MAX_NPCS];
float g_CBaseNPCflFrictionForward[MAX_NPCS];
float g_CBaseNPCflFrictionSideways[MAX_NPCS];
float g_CBaseNPCflLastStuckTime[MAX_NPCS];

char g_CBaseNPCType[MAX_NPCS][64];


/////////////
// SDKCall //
/////////////

//CBaseEntity
Handle g_hSDKGetVectors;

//CBaseCombatCharacter
Handle g_hSDKUpdateLastKnownArea;
Handle g_hSDKGetLastKnownArea;

//CBaseAnimating
Handle g_hSDKLookupSequence;
Handle g_hSDKSelectWeightedSequence;
Handle g_hSDKSequenceDuration;
Handle g_hSDKResetSequence;
Handle g_hSDKLookupPoseParameter;
Handle g_hSDKSetPoseParameter;
Handle g_hSDKStudioFrameAdvance;
Handle g_hSDKDispatchAnimEvents;

//ILocomotion
Handle g_hSDKStuckMonitor;
Handle g_hSDKIsStuck;
Handle g_hSDKClearStuckStatus;

int g_ipStudioHdrOffset;

//CBaseAnimatingOverlay
Handle g_hSDKAddGestureSequence;

/////////////
// DETOURS //
////////////

//INextBot

//Locomotion
Handle g_hIsAbleToClimb;
Handle g_hIsAbleToJump;
Handle g_hClimbUpToLedge;
Handle g_hGetAcceleration;
Handle g_hGetStepHeight;
Handle g_hGetMaxJumpHeight;
Handle g_hGetWalkSpeed;
Handle g_hGetRunSpeed;
Handle g_hGetGravity;
Handle g_hShouldCollide;
Handle g_hGetFrictionForward;
Handle g_hGetFrictionSideways;

//Body
Handle g_hStartActivity;
Handle g_hGetHullWidth;
Handle g_hGetHullHeight;
Handle g_hGetStandHullHeight;
Handle g_hGetCrouchHullHeight;
Handle g_hGetSolidMask;

public Plugin myinfo = 
{
	name = "[TF2] CBaseNPC", 
	author = "Benoist3012 & Pelipoika(figured most of CBaseAnimating)", 
	description = "", 
	version = "0.1", 
	url = ""
};

public APLRes AskPluginLoad2(Handle myself, bool late, char[] error, int err_max)
{
	RegPluginLibrary("cbasenpc");
	
	CreateNative("CBaseNPC.CBaseNPC", Native_CBaseNPCConstructor);
	CreateNative("CBaseNPC.GetEntity", Native_CBaseNPCGetEntity);
	CreateNative("CBaseNPC.Teleport", Native_CBaseNPCTeleport);
	CreateNative("CBaseNPC.Spawn", Native_CBaseNPCSpawn);
	CreateNative("CBaseNPC.SetModel", Native_CBaseNPCSetModel);
	CreateNative("CBaseNPC.SetCollisionBounds", Native_CBaseNPCSetCollisionBounds);
	CreateNative("CBaseNPC.GetVectors", Native_CBaseNPCGetVectors);
	CreateNative("CBaseNPC.GetLastKnownArea", Native_CBaseNPCGetLastKnownArea);
	
	CreateNative("CBaseNPC.GetBot", Native_CBaseNPCGetBot);
	CreateNative("CBaseNPC.GetLocomotion", Native_CBaseNPCGetLocomotion);
	CreateNative("CBaseNPC.GetBody", Native_CBaseNPCGetBody);
	
	CreateNative("CBaseNPC.SetType", Native_CBaseNPCSetType);
	CreateNative("CBaseNPC.GetType", Native_CBaseNPCGetType);
	
	CreateNative("CBaseNPC.EquipItem", Native_CBaseNPCEquipItem);
	
	CreateNative("CBaseNPC.Approach", Native_CBaseNPCApproach);
	CreateNative("CBaseNPC.FaceTowards", Native_CBaseNPCFaceTowards);
	CreateNative("CBaseNPC.Walk", Native_CBaseNPCWalk);
	CreateNative("CBaseNPC.Run", Native_CBaseNPCRun);
	CreateNative("CBaseNPC.Stop", Native_CBaseNPCStop);
	CreateNative("CBaseNPC.Jump", Native_CBaseNPCJump);
	CreateNative("CBaseNPC.IsOnGround", Native_CBaseNPCIsOnGround);
	CreateNative("CBaseNPC.IsClimbingOrJumping", Native_CBaseNPCIsClimbingOrJumping);
	CreateNative("CBaseNPC.SetVelocity", Native_CBaseNPCSetVelocity);
	CreateNative("CBaseNPC.GetVelocity", Native_CBaseNPCGetVelocity);
	
	CreateNative("CBaseNPC.flStepSize.set", Native_CBaseNPCflStepSizeSet);
	CreateNative("CBaseNPC.flStepSize.get", Native_CBaseNPCflStepSizeGet);
	CreateNative("CBaseNPC.flGravity.set", Native_CBaseNPCflGravitySet);
	CreateNative("CBaseNPC.flGravity.get", Native_CBaseNPCflGravityGet);
	CreateNative("CBaseNPC.flAcceleration.set", Native_CBaseNPCflAccelerationSet);
	CreateNative("CBaseNPC.flAcceleration.get", Native_CBaseNPCflAccelerationGet);
	CreateNative("CBaseNPC.flJumpHeight.set", Native_CBaseNPCflJumpHeightSet);
	CreateNative("CBaseNPC.flJumpHeight.get", Native_CBaseNPCflJumpHeightGet);
	CreateNative("CBaseNPC.flWalkSpeed.set", Native_CBaseNPCflWalkSpeedSet);
	CreateNative("CBaseNPC.flWalkSpeed.get", Native_CBaseNPCflWalkSpeedGet);
	CreateNative("CBaseNPC.flRunSpeed.set", Native_CBaseNPCflRunSpeedSet);
	CreateNative("CBaseNPC.flRunSpeed.get", Native_CBaseNPCflRunSpeedGet);
	CreateNative("CBaseNPC.flFrictionForward.set", Native_CBaseNPCflFrictionForwardSpeedSet);
	CreateNative("CBaseNPC.flFrictionForward.get", Native_CBaseNPCflFrictionForwardSpeedGet);
	CreateNative("CBaseNPC.flFrictionSideways.set", Native_CBaseNPCflFrictionSidewaysSet);
	CreateNative("CBaseNPC.flFrictionSideways.get", Native_CBaseNPCflFrictionSidewaysGet);
	
	CreateNative("CNPCs.FindNPCByEntIndex", Native_CNPCsFindNPCByEntIndex);
	CreateNative("CNPCs.IsValidNPC", Native_CNPCsIsValidNPC);
	
	CreateNative("CBaseAnimating.LookupSequence", Native_CBaseAnimatingLookupSequence);
	CreateNative("CBaseAnimating.SelectWeightedSequence", Native_CBaseAnimatingSelectWeightedSequence);
	CreateNative("CBaseAnimating.SequenceDuration", Native_CBaseAnimatingSequenceDuration);
	CreateNative("CBaseAnimating.ResetSequence", Native_CBaseAnimatingResetSequence);
	CreateNative("CBaseAnimating.GetModelPtr", Native_CBaseAnimatingGetModelPtr);
	CreateNative("CBaseAnimating.LookupPoseParameter", Native_CBaseAnimatingLookupPoseParameter);
	CreateNative("CBaseAnimating.SetPoseParameter", Native_CBaseAnimatingSetPoseParameter);
	
	CreateNative("CBaseAnimatingOverlay.AddGestureSequence", Native_CBaseAnimatingOverlayAddGestureSequence);
	return APLRes_Success;
}

public void OnPluginStart()
{
	ConVar nb_update_frequency = FindConVar("nb_update_frequency");
	nb_update_frequency.FloatValue = 0.01;
	HookConVarChange(nb_update_frequency, Hook_BlockCvarValue);
	SDK_Init();
	
	for (int iClient = 1; iClient <= MaxClients; iClient++)
		if (IsClientInGame(iClient))
			OnClientPutInServer(iClient);
}

public void OnPluginEnd()
{
	for (int i = 0; i < MAX_NPCS; i++)
	{
		int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[i]);
		if (iEntity > MaxClients)
			AcceptEntityInput(iEntity, "Kill");
	}
}

public void Hook_BlockCvarValue(ConVar convar, const char[] oldValue, const char[] newValue)
{
	float flnewvalue = StringToFloat(newValue);
	if (flnewvalue > 0.01)
		convar.FloatValue = 0.01;
	else
		convar.FloatValue = flnewvalue;
}

public void OnMapStart()
{
	PrecacheModel("models/empty.mdl");
#if defined DEBUG
	g_iLaserIndex = PrecacheModel("materials/sprites/laser.vmt");
#endif
}

public void OnClientPutInServer(int iClient)
{
	SDKHook(iClient, SDKHook_PreThink, Client_OnThink);
}

public void Client_OnThink(int iClient)
{
	if (IsPlayerAlive(iClient))
		SDK_UpdateLastKnownArea(iClient);//Update last known nav area so SDK_GetLastKnownArea returns something
}

public Action CBaseEntity_SetLocalAngles(int iEntity, float vecNewAngles[3])
{
	if (NPCFindByEntityIndex(iEntity) != INVALID_NPC)
	{
		vecNewAngles[0] = 0.0;
		vecNewAngles[2] = 0.0;
		return Plugin_Changed;
	}
	return Plugin_Continue;
}

public int Native_CBaseNPCConstructor(Handle plugin, int numParams)
{
	int iNpc = CreateEntityByName("base_boss");
	CBaseNPC Npc = CBaseNPC_GiveIDToEntity(iNpc);
	if (Npc != INVALID_NPC)
	{
		SDKHook(iNpc, SDKHook_Think, CBaseNPC_Think);
		
		int iIndex = Npc.Index;
		
		if (g_CBaseNPCHooks[iIndex] == null)
			g_CBaseNPCHooks[iIndex] = new ArrayList();
		
		//Remove old hooks if there's any
		if (g_CBaseNPCHooks[iIndex].Length > 0)
		{
			for (int i = 0; i < g_CBaseNPCHooks[iIndex].Length; i++)
			{
				int hookID = g_CBaseNPCHooks[Npc.Index].Get(i);
				DHookRemoveHookID(hookID);
			}
		}
		
		g_CBaseNPCHooks[iIndex].Clear();
		g_CBaseNPCflFrictionSideways[iIndex] = 3.0;
		g_CBaseNPCflFrictionForward[iIndex] = 0.0;
		g_CBaseNPCflLastStuckTime[iIndex] = 0.0;
		
		DispatchKeyValue(iNpc,"health","2147483647");
		Npc.iHealth = 2147483647;
		Npc.iMaxHealth = 2147483647;
		
		g_CBaseNPCNextBotInterface[iIndex] = CBaseNPC_GetNextBotOfEntity(iNpc);
		g_CBaseNPCLocomotionInterface[iIndex] = view_as<NextBotGroundLocomotion>(g_CBaseNPCNextBotInterface[iIndex].GetLocomotionInterface());
		g_CBaseNPCBodyInterface[iIndex] = g_CBaseNPCNextBotInterface[iIndex].GetBodyInterface();
		
		// Locomotion detours
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hIsAbleToJump, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hIsAbleToClimb, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hClimbUpToLedge, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetAcceleration, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetStepHeight, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetMaxJumpHeight, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetWalkSpeed, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetRunSpeed, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetGravity, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hShouldCollide, true, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetFrictionForward, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetFrictionSideways, false, view_as<Address>(g_CBaseNPCLocomotionInterface[iIndex])));
		// IBody detours
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hStartActivity, false, view_as<Address>(g_CBaseNPCBodyInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetHullWidth, false, view_as<Address>(g_CBaseNPCBodyInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetHullHeight, false, view_as<Address>(g_CBaseNPCBodyInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetStandHullHeight, false, view_as<Address>(g_CBaseNPCBodyInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetCrouchHullHeight, false, view_as<Address>(g_CBaseNPCBodyInterface[iIndex])));
		g_CBaseNPCHooks[iIndex].Push(DHookRaw(g_hGetSolidMask, true, view_as<Address>(g_CBaseNPCBodyInterface[iIndex])));
	}
	else
		AcceptEntityInput(iNpc, "Kill");
	return view_as<int>(Npc);
}

public Action CBaseNPC_Think(int iEnt)
{
	SDK_UpdateLastKnownArea(iEnt);
	SDK_StudioFrameAdvance(iEnt);
	SDK_DispatchAnimEvents(iEnt);
	
	CBaseNPC npc = NPCFindByEntityIndex(iEnt);
	if (npc != INVALID_NPC)
	{
		SDKCall(g_hSDKStuckMonitor, g_CBaseNPCLocomotionInterface[npc.Index]);
		
		bool bStuck = SDKCall(g_hSDKIsStuck, g_CBaseNPCLocomotionInterface[npc.Index]);
		if (bStuck)
		{
			if (g_CBaseNPCflLastStuckTime[npc.Index] != 0.0 && GetGameTime()-g_CBaseNPCflLastStuckTime[npc.Index] >= 0.5)
			{
				PathFollower path = g_CBaseNPCNextBotInterface[npc.Index].GetCurrentPath();
				if (view_as<Address>(path) != Address_Null)
				{
					Segment seg = path.GetCurrentGoal();
					Segment finalGoal = path.LastSegment();
					Segment prior = view_as<Segment>(0);
					if (view_as<Address>(seg) != Address_Null)
						prior = path.PriorSegment(seg);
					
					float vecPos[3];
					
					if (view_as<Address>(prior) != Address_Null && prior != finalGoal)
					{
						prior.GetPos(vecPos);
						g_CBaseNPCNextBotInterface[npc.Index].SetPosition(vecPos);
						SDKCall(g_hSDKClearStuckStatus, g_CBaseNPCLocomotionInterface[npc.Index], "Un-Stuck moved to previous segment");
						g_CBaseNPCflLastStuckTime[npc.Index] = 0.0;
					}
					else if (view_as<Address>(seg) != Address_Null && seg != finalGoal)
					{
						seg.GetPos(vecPos);
						g_CBaseNPCNextBotInterface[npc.Index].SetPosition(vecPos);
						SDKCall(g_hSDKClearStuckStatus, g_CBaseNPCLocomotionInterface[npc.Index], "Un-Stuck moved to goal");
						g_CBaseNPCflLastStuckTime[npc.Index] = 0.0;
					}
					else if (view_as<Address>(prior) != Address_Null)
					{
						path.GetPosition(40.0, prior, vecPos);
						g_CBaseNPCNextBotInterface[npc.Index].SetPosition(vecPos);
						SDKCall(g_hSDKClearStuckStatus, g_CBaseNPCLocomotionInterface[npc.Index], "Un-Stuck");
						g_CBaseNPCflLastStuckTime[npc.Index] = 0.0;
					}
					else if (view_as<Address>(seg) != Address_Null)
					{
						path.GetPosition(40.0, seg, vecPos);
						g_CBaseNPCNextBotInterface[npc.Index].SetPosition(vecPos);
						SDKCall(g_hSDKClearStuckStatus, g_CBaseNPCLocomotionInterface[npc.Index], "Un-Stuck");
						g_CBaseNPCflLastStuckTime[npc.Index] = 0.0;
					}
				}
			}
			if (g_CBaseNPCflLastStuckTime[npc.Index] == 0.0)
				g_CBaseNPCflLastStuckTime[npc.Index] = GetGameTime();
		}
	}
}

public int Native_CBaseNPCGetEntity(Handle plugin, int numParams)
{
	return EntRefToEntIndex(g_CBaseNPCEntityRef[GetNativeCell(1)]);
}

public int Native_CBaseNPCTeleport(Handle plugin, int numParams)
{
	int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[GetNativeCell(1)]);
	if (iEntity > MaxClients)
	{
		float vecPos[3], vecAng[3], vecVel[3];
		GetNativeArray(2, vecPos, sizeof(vecPos));
		GetNativeArray(3, vecAng, sizeof(vecAng));
		GetNativeArray(4, vecVel, sizeof(vecVel));
		
		TeleportEntity(iEntity, vecPos, vecAng, vecVel);
	}
}

public int Native_CBaseNPCSpawn(Handle plugin, int numParams)
{
	int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[GetNativeCell(1)]);
	if (iEntity > MaxClients)
	{
		DispatchSpawn(iEntity);
		ActivateEntity(iEntity);
	}
}

public int Native_CBaseNPCSetModel(Handle plugin, int numParams)
{
	int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[GetNativeCell(1)]);
	if (iEntity > MaxClients)
	{
		char sModel[PLATFORM_MAX_PATH];
		GetNativeString(2, sModel, sizeof(sModel));
		SetEntityModel(iEntity, sModel);
	}
}

public int Native_CBaseNPCSetCollisionBounds(Handle plugin, int numParams)
{
	int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[GetNativeCell(1)]);
	if (iEntity > MaxClients)
	{
		float vecMins[3], vecMaxs[3];
		GetNativeArray(2, vecMins, sizeof(vecMins));
		GetNativeArray(3, vecMaxs, sizeof(vecMaxs));
		
		float vecMinsPre[3], vecMaxsPre[3];
		GetEntPropVector(iEntity, Prop_Send, "m_vecMinsPreScaled", vecMinsPre);
		GetEntPropVector(iEntity, Prop_Send, "m_vecMaxsPreScaled", vecMaxsPre);
		
		if ( vecMins[0] != vecMinsPre[0] || vecMins[1] != vecMinsPre[1] || vecMins[2] != vecMinsPre[2] || vecMaxs[0] != vecMaxsPre[0] || vecMaxs[1] != vecMaxsPre[1] || vecMaxs[2] != vecMaxsPre[2] )
		{
			SetEntPropVector(iEntity, Prop_Send, "m_vecMinsPreScaled", vecMins);
			SetEntPropVector(iEntity, Prop_Send, "m_vecMaxsPreScaled", vecMaxs);
		}

		float vecOldMins[3], vecOldMaxs[3];
		GetEntPropVector(iEntity, Prop_Send, "m_vecMins", vecOldMins);
		GetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecOldMaxs);
			
		bool bDirty = false;
		float flModelScale = GetEntPropFloat(iEntity, Prop_Send, "m_flModelScale");
		if ( flModelScale != 1.0 )
		{
			// Do the scaling
			float vecNewMins[3], vecNewMaxs[3];
			for (int i = 0; i < 3; i++) vecNewMins[i] = vecMins[i] * flModelScale;
			for (int i = 0; i < 3; i++) vecNewMaxs[i] = vecMaxs[i] * flModelScale;

			if ( vecOldMins[0] != vecNewMins[0] || vecOldMins[1] != vecNewMins[1] || vecOldMins[2] != vecNewMins[2] || vecOldMaxs[0] != vecNewMaxs[0] || vecOldMaxs[1] != vecNewMaxs[1] || vecOldMaxs[2] != vecNewMaxs[2] )
			{
				SetEntPropVector(iEntity, Prop_Send, "m_vecMins", vecNewMins);
				SetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecNewMaxs);
				bDirty = true;
			}
		}
		else
		{
			// No scaling needed!
			if ( vecOldMins[0] != vecMins[0] || vecOldMins[1] != vecMins[1] || vecOldMins[2] != vecMins[2] || vecOldMaxs[0] != vecMaxs[0] || vecOldMaxs[1] != vecMaxs[1] || vecOldMaxs[2] != vecMaxs[2] )
			{
				SetEntPropVector(iEntity, Prop_Send, "m_vecMins", vecMins);
				SetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecMaxs);
				bDirty = true;
			}
		}
		
		if ( bDirty )
		{
			float vecDirtyMins[3], vecDirtyMaxs[3], vecSize[3];
			GetEntPropVector(iEntity, Prop_Send, "m_vecMins", vecDirtyMins);
			GetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecDirtyMaxs);
			SubtractVectors(vecDirtyMaxs, vecDirtyMins, vecSize);
			SetEntPropFloat(iEntity, Prop_Data, "m_flRadius", GetVectorLength(vecSize, false) * 0.5);

			SetEntProp(iEntity, Prop_Data, "m_iEFlags", GetEntProp(iEntity, Prop_Data, "m_iEFlags")|EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS);
		}
	}
}

public int Native_CBaseNPCGetVectors(Handle plugin, int numParams)
{
	int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[GetNativeCell(1)]);
	if (iEntity < MaxClients) return;
	float vecForward[3], vecRight[3], vecUp[3];
	
	SDK_GetVectors(iEntity, vecForward, vecRight, vecUp);
	
	SetNativeArray(2, vecForward, sizeof(vecForward));
	SetNativeArray(3, vecRight, sizeof(vecRight));
	SetNativeArray(4, vecUp, sizeof(vecUp));
}

public int Native_CBaseNPCGetLastKnownArea(Handle plugin, int numParams)
{
	int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[GetNativeCell(1)]);
	if (iEntity < MaxClients) return 0;
	
	return view_as<int>(SDK_GetLastKnownArea(iEntity));
}

public int Native_CBaseNPCGetBot(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCNextBotInterface[GetNativeCell(1)]);
}

public int Native_CBaseNPCGetLocomotion(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCLocomotionInterface[GetNativeCell(1)]);
}

public int Native_CBaseNPCGetBody(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCBodyInterface[GetNativeCell(1)]);
}

public int Native_CBaseNPCSetType(Handle plugin, int numParams)
{
	GetNativeString(2, g_CBaseNPCType[GetNativeCell(1)], sizeof(g_CBaseNPCType[]));
}

public int Native_CBaseNPCGetType(Handle plugin, int numParams)
{
	SetNativeString(2, g_CBaseNPCType[GetNativeCell(1)], sizeof(g_CBaseNPCType[]));
}

public int Native_CBaseNPCEquipItem(Handle plugin, int numParams)
{
	int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[GetNativeCell(1)]);
	if (iEntity < MaxClients) return -1;
	
	char sAttachement[120], sModel[PLATFORM_MAX_PATH], sAnim[120];
	GetNativeString(2, sAttachement, sizeof(sAttachement));
	GetNativeString(3, sModel, sizeof(sModel));
	GetNativeString(4, sAnim, sizeof(sAnim));
	
	int iItem = CreateEntityByName("prop_dynamic");
	DispatchKeyValue(iItem, "model", sModel);
	DispatchKeyValueFloat(iItem, "modelscale", GetEntPropFloat(iEntity, Prop_Send, "m_flModelScale"));
	DispatchSpawn(iItem);
	
	SetEntProp(iItem, Prop_Send, "m_nSkin", GetNativeCell(5));
	SetEntProp(iItem, Prop_Send, "m_fEffects", EF_BONEMERGE|EF_PARENT_ANIMATES);
	
	if(strcmp(sAnim, "") != 0)
	{
		SetVariantString(sAnim);
		AcceptEntityInput(iItem, "SetAnimation");
	}

	SetVariantString("!activator");
	AcceptEntityInput(iItem, "SetParent", iEntity);
	
	SetVariantString(sAttachement);
	AcceptEntityInput(iItem, "SetParentAttachmentMaintainOffset"); 

	return iItem;
}

public int Native_CBaseNPCApproach(Handle plugin, int numParams)
{
	float vecPos[3];
	GetNativeArray(2, vecPos, 3);
	
	g_CBaseNPCLocomotionInterface[GetNativeCell(1)].Approach(vecPos, 1.0);
}

public int Native_CBaseNPCFaceTowards(Handle plugin, int numParams)
{
	float vecPos[3];
	GetNativeArray(2, vecPos, 3);
	
	g_CBaseNPCLocomotionInterface[GetNativeCell(1)].FaceTowards(vecPos);
}

public int Native_CBaseNPCWalk(Handle plugin, int numParams)
{
	g_CBaseNPCLocomotionInterface[GetNativeCell(1)].Walk();
}

public int Native_CBaseNPCRun(Handle plugin, int numParams)
{
	g_CBaseNPCLocomotionInterface[GetNativeCell(1)].Run();
}

public int Native_CBaseNPCStop(Handle plugin, int numParams)
{
	g_CBaseNPCLocomotionInterface[GetNativeCell(1)].Stop();
}

public int Native_CBaseNPCJump(Handle plugin, int numParams)
{
	g_CBaseNPCLocomotionInterface[GetNativeCell(1)].Jump();
}

public int Native_CBaseNPCIsOnGround(Handle plugin, int numParams)
{
	return g_CBaseNPCLocomotionInterface[GetNativeCell(1)].IsOnGround();
}

public int Native_CBaseNPCIsClimbingOrJumping(Handle plugin, int numParams)
{
	return g_CBaseNPCLocomotionInterface[GetNativeCell(1)].IsClimbingOrJumping();
}

public int Native_CBaseNPCSetVelocity(Handle plugin, int numParams)
{
	float vec[3];
	GetNativeArray(2, vec, 3);
	
	g_CBaseNPCLocomotionInterface[GetNativeCell(1)].SetVelocity(vec);
}

public int Native_CBaseNPCGetVelocity(Handle plugin, int numParams)
{
	float vec[3];
	g_CBaseNPCLocomotionInterface[GetNativeCell(1)].GetVelocity(vec);
	SetNativeArray(2, vec, 3);
}

public int Native_CBaseNPCflStepSizeSet(Handle plugin, int numParams)
{
	g_CBaseNPCflStepSize[GetNativeCell(1)] = view_as<float>(GetNativeCell(2));
}

public int Native_CBaseNPCflStepSizeGet(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCflStepSize[GetNativeCell(1)]);
}

public int Native_CBaseNPCflGravitySet(Handle plugin, int numParams)
{
	g_CBaseNPCflGravity[GetNativeCell(1)] = view_as<float>(GetNativeCell(2));
}

public int Native_CBaseNPCflGravityGet(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCflGravity[GetNativeCell(1)]);
}

public int Native_CBaseNPCflAccelerationSet(Handle plugin, int numParams)
{
	g_CBaseNPCflAcceleration[GetNativeCell(1)] = view_as<float>(GetNativeCell(2));
}

public int Native_CBaseNPCflAccelerationGet(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCflAcceleration[GetNativeCell(1)]);
}

public int Native_CBaseNPCflJumpHeightSet(Handle plugin, int numParams)
{
	g_CBaseNPCflJumpHeight[GetNativeCell(1)] = view_as<float>(GetNativeCell(2));
}

public int Native_CBaseNPCflJumpHeightGet(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCflJumpHeight[GetNativeCell(1)]);
}

public int Native_CBaseNPCflWalkSpeedSet(Handle plugin, int numParams)
{
	g_CBaseNPCflWalkSpeed[GetNativeCell(1)] = view_as<float>(GetNativeCell(2));
}

public int Native_CBaseNPCflWalkSpeedGet(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCflWalkSpeed[GetNativeCell(1)]);
}

public int Native_CBaseNPCflRunSpeedSet(Handle plugin, int numParams)
{
	g_CBaseNPCflRunSpeed[GetNativeCell(1)] = view_as<float>(GetNativeCell(2));
}

public int Native_CBaseNPCflRunSpeedGet(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCflRunSpeed[GetNativeCell(1)]);
}

public int Native_CBaseNPCflFrictionForwardSpeedSet(Handle plugin, int numParams)
{
	g_CBaseNPCflFrictionForward[GetNativeCell(1)] = view_as<float>(GetNativeCell(2));
}

public int Native_CBaseNPCflFrictionForwardSpeedGet(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCflFrictionForward[GetNativeCell(1)]);
}

public int Native_CBaseNPCflFrictionSidewaysSet(Handle plugin, int numParams)
{
	g_CBaseNPCflFrictionSideways[GetNativeCell(1)] = view_as<float>(GetNativeCell(2));
}

public int Native_CBaseNPCflFrictionSidewaysGet(Handle plugin, int numParams)
{
	return view_as<int>(g_CBaseNPCflFrictionSideways[GetNativeCell(1)]);
}

public CBaseNPC CBaseNPC_GiveIDToEntity(int iEntity)
{
	for (int ID = 0; ID < MAX_NPCS; ID++)
	{
		if (EntRefToEntIndex(g_CBaseNPCEntityRef[ID]) <= MaxClients)
		{
			g_CBaseNPCEntityRef[ID] = EntIndexToEntRef(iEntity);
			return view_as<CBaseNPC>(ID);
		}
	}
	return INVALID_NPC;
}

public CBaseNPC NPCFindByEntityIndex(int iEntity)
{
	for (int ID = 0; ID < MAX_NPCS; ID++)
	{
		if (EntRefToEntIndex(g_CBaseNPCEntityRef[ID]) == iEntity)
		{
			return view_as<CBaseNPC>(ID);
		}
	}
	return INVALID_NPC;
}

public CBaseNPC NPCGetFromLocomotion(NextBotGroundLocomotion locomotion)
{
	for (int ID = 0; ID < MAX_NPCS; ID++)
	{
		if (g_CBaseNPCLocomotionInterface[ID] == locomotion)
		{
			return view_as<CBaseNPC>(ID);
		}
	}
	return INVALID_NPC;
}





public int Native_CNPCsFindNPCByEntIndex(Handle plugin, int numParams)
{
	return view_as<int>(NPCFindByEntityIndex(GetNativeCell(2)));
}

public int Native_CNPCsIsValidNPC(Handle plugin, int numParams)
{
	int iIndex = GetNativeCell(2);
	if (iIndex < 0 || iIndex >= MAX_NPCS) return false;
	if (EntRefToEntIndex(g_CBaseNPCEntityRef[iIndex]) <= MaxClients) return false;
	
	return true;
}



public int Native_CBaseAnimatingLookupSequence(Handle plugin, int numParams)
{
	if (g_hSDKLookupSequence != INVALID_HANDLE)
	{
		char sAnim[120];
		GetNativeString(2, sAnim, sizeof(sAnim));
		return SDKCall(g_hSDKLookupSequence, GetNativeCell(1), sAnim);
	}
	return -1;
}

public int Native_CBaseAnimatingSequenceDuration(Handle plugin, int numParams)
{
	if (g_hSDKSequenceDuration != INVALID_HANDLE)
		return SDKCall(g_hSDKSequenceDuration, GetNativeCell(1), GetNativeCell(2), GetNativeCell(3));
	return view_as<int>(0.0);
}

public int Native_CBaseAnimatingSelectWeightedSequence(Handle plugin, int numParams)
{
	if (g_hSDKSelectWeightedSequence != INVALID_HANDLE)
	{
		return SDKCall(g_hSDKSelectWeightedSequence, GetEntData(GetNativeCell(1), g_ipStudioHdrOffset * 4), GetNativeCell(2), GetEntProp(GetNativeCell(1), Prop_Send, "m_nSequence"));
	}
	return -1;
}

public int Native_CBaseAnimatingResetSequence(Handle plugin, int numParams)
{
	if (g_hSDKResetSequence != INVALID_HANDLE)
	{
		SDKCall(g_hSDKResetSequence, GetNativeCell(1), GetNativeCell(2));
	}
}

public int Native_CBaseAnimatingGetModelPtr(Handle plugin, int numParams)
{
	Address pStudioHdr = view_as<Address>(GetEntData(GetNativeCell(1), g_ipStudioHdrOffset * 4));
	if (!IsValidAddress(pStudioHdr)) return view_as<int>(Address_Null);
	return view_as<int>(pStudioHdr);
}

public int Native_CBaseAnimatingLookupPoseParameter(Handle plugin, int numParams)
{
	if (g_hSDKLookupPoseParameter != INVALID_HANDLE)
	{
		char sParamName[120];
		GetNativeString(3, sParamName, sizeof(sParamName));
		return SDKCall(g_hSDKLookupPoseParameter, GetNativeCell(1), GetNativeCell(2), sParamName);
	}
	return -1;
}

public int Native_CBaseAnimatingSetPoseParameter(Handle plugin, int numParams)
{
	if (g_hSDKSetPoseParameter != INVALID_HANDLE)
	{
		SDKCall(g_hSDKSetPoseParameter, GetNativeCell(1), GetNativeCell(2), GetNativeCell(3), view_as<float>(GetNativeCell(4)));
	}
}

public int Native_CBaseAnimatingOverlayAddGestureSequence(Handle plugin, int numParams)
{
	if (g_hSDKAddGestureSequence != INVALID_HANDLE)
	{
		SDKCall(g_hSDKAddGestureSequence, GetNativeCell(1), GetNativeCell(2), GetNativeCell(3));
	}
}

// SDK Functions
void SDK_Init()
{
	Handle hGameData = LoadGameConfigFile("cbasenpc");
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Virtual, "CBaseCombatCharacter::UpdateLastKnownArea");
	g_hSDKUpdateLastKnownArea = EndPrepSDKCall();
	if (g_hSDKUpdateLastKnownArea == INVALID_HANDLE)
	{
		PrintToServer("Failed to retrieve CBaseCombatCharacter::UpdateLastKnownArea offset!");
	}
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Virtual, "CBaseCombatCharacter::GetLastKnownArea");
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_ByValue);
	g_hSDKGetLastKnownArea = EndPrepSDKCall();
	if (g_hSDKGetLastKnownArea == INVALID_HANDLE)
	{
		PrintToServer("Failed to retrieve CBaseCombatCharacter::GetLastKnownArea offset!");
	}
	
	StartPrepSDKCall(SDKCall_Raw);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Virtual, "ILocomotion::StuckMonitor");
	g_hSDKStuckMonitor = EndPrepSDKCall();
	if (g_hSDKStuckMonitor == INVALID_HANDLE)
	{
		SetFailState("Failed to create Virtual Call for ILocomotion::StuckMonitor!");
	}
	
	StartPrepSDKCall(SDKCall_Raw);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Virtual, "ILocomotion::IsStuck");
	PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
	g_hSDKIsStuck = EndPrepSDKCall();
	if (g_hSDKIsStuck  == INVALID_HANDLE)
	{
		SetFailState("Failed to create Virtual Call for ILocomotion::IsStuck!");
	}
	
	StartPrepSDKCall(SDKCall_Raw);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Virtual, "ILocomotion::ClearStuckStatus");
	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	g_hSDKClearStuckStatus = EndPrepSDKCall();
	if (g_hSDKClearStuckStatus == INVALID_HANDLE) 
	{
		SetFailState("Failed to create Virtual Call for ILocomotion::ClearStuckStatus!");
	}
	
	int iOffset = GameConfGetOffset(hGameData, "NextBotGroundLocomotion::GetGravity"); 
	g_hGetGravity = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetGravity);
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::IsAbleToClimb");
	g_hIsAbleToClimb = DHookCreate(iOffset, HookType_Raw, ReturnType_Bool, ThisPointer_Address, StartActivity);
	if (g_hIsAbleToClimb == null) SetFailState("Failed to create hook for ILocomotion::IsAbleToClimb!");
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::IsAbleToJumpAcrossGaps");
	g_hIsAbleToJump = DHookCreate(iOffset, HookType_Raw, ReturnType_Bool, ThisPointer_Address, StartActivity);
	if (g_hIsAbleToJump == null) SetFailState("Failed to create hook for ILocomotion::IsAbleToJumpAcrossGaps!");
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::ClimbUpToLedge"); 
	g_hClimbUpToLedge = DHookCreate(iOffset, HookType_Raw, ReturnType_Void, ThisPointer_Address, ClimbUpToLedge);
	if (g_hClimbUpToLedge == null) SetFailState("Failed to create hook for ILocomotion::ClimbUpToLedge!");
	DHookAddParam(g_hClimbUpToLedge, HookParamType_VectorPtr);
	DHookAddParam(g_hClimbUpToLedge, HookParamType_VectorPtr);
	DHookAddParam(g_hClimbUpToLedge, HookParamType_CBaseEntity);
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::GetStepHeight"); 
	g_hGetStepHeight = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetStepHeight);
	if (g_hGetStepHeight == null) SetFailState("Failed to create hook for ILocomotion::GetStepHeight!");
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::GetMaxJumpHeight"); 
	g_hGetMaxJumpHeight = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetMaxJumpHeight);
	if (g_hGetMaxJumpHeight == null) SetFailState("Failed to create hook for ILocomotion::GetMaxJumpHeight!");
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::GetMaxAcceleration"); 
	g_hGetAcceleration = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetAcceleration);
	if (g_hGetAcceleration == null) SetFailState("Failed to create hook for ILocomotion::GetMaxAcceleration!");
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::GetRunSpeed"); 
	g_hGetRunSpeed = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetRunSpeed);
	if (g_hGetRunSpeed == null) SetFailState("Failed to create hook for ILocomotion::GetRunSpeed!");
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::GetWalkSpeed"); 
	g_hGetWalkSpeed = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetWalkSpeed);
	if (g_hGetWalkSpeed == null) SetFailState("Failed to create hook for ILocomotion::GetWalkSpeed!");
	
	iOffset = GameConfGetOffset(hGameData, "ILocomotion::ShouldCollideWith");
	g_hShouldCollide = DHookCreate(iOffset, HookType_Raw, ReturnType_Bool, ThisPointer_Address, ShouldCollideWith);
	if (g_hShouldCollide == null) SetFailState("Failed to create hook for ILocomotion::ShouldCollideWith!");
	DHookAddParam(g_hShouldCollide, HookParamType_CBaseEntity);

	iOffset = GameConfGetOffset(hGameData, "IBody::StartActivity");
	g_hStartActivity = DHookCreate(iOffset, HookType_Raw, ReturnType_Bool, ThisPointer_Address, StartActivity);
	if (g_hStartActivity == null) SetFailState("Failed to create hook for IBody::StartActivity!");

	iOffset = GameConfGetOffset(hGameData, "IBody::GetHullWidth");
	if(iOffset == -1) SetFailState("Failed to get offset of IBody::GetHullWidth");
	g_hGetHullWidth = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetHullWidth);
	
	iOffset = GameConfGetOffset(hGameData, "IBody::GetHullHeight");
	if(iOffset == -1) SetFailState("Failed to get offset of IBody::GetHullHeight");
	g_hGetHullHeight = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetHullHeight);

	iOffset = GameConfGetOffset(hGameData, "IBody::GetStandHullHeight");
	if(iOffset == -1) SetFailState("Failed to get offset of IBody::GetStandHullHeight");
	g_hGetStandHullHeight = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetStandHullHeight);

	iOffset = GameConfGetOffset(hGameData, "IBody::GetCrouchHullHeight");
	g_hGetCrouchHullHeight = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetCrouchHullHeight);
	if (g_hGetCrouchHullHeight == null) SetFailState("Failed to create hook for IBody::GetCrouchHullHeight!");
	
	iOffset = GameConfGetOffset(hGameData, "IBody::GetSolidMask");
	g_hGetSolidMask = DHookCreate(iOffset, HookType_Raw, ReturnType_Int, ThisPointer_Address, GetSolidMask);
	if (g_hGetSolidMask == null) SetFailState("Failed to create hook for IBody::GetSolidMask!");
	
	iOffset = GameConfGetOffset(hGameData, "NextBotGroundLocomotion::GetFrictionForward"); 
	g_hGetFrictionForward = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetFrictionForward);
	
	iOffset = GameConfGetOffset(hGameData, "NextBotGroundLocomotion::GetFrictionSideways"); 
	g_hGetFrictionSideways = DHookCreate(iOffset, HookType_Raw, ReturnType_Float, ThisPointer_Address, GetFrictionSideways);
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CBaseAnimating::LookupSequence");
	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_ByValue);
	g_hSDKLookupSequence = EndPrepSDKCall();
	if (g_hSDKLookupSequence == INVALID_HANDLE) PrintToServer("Failed to retrieve CBaseAnimating::LookupSequence signature!");
	
	StartPrepSDKCall(SDKCall_Raw);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CBaseAnimating::SelectWeightedSequence");
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_ByValue);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_ByValue);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_ByValue);
	g_hSDKSelectWeightedSequence = EndPrepSDKCall();
	if (g_hSDKSelectWeightedSequence == INVALID_HANDLE) PrintToServer("Failed to retrieve CBaseAnimating::SelectWeightedSequence signature!");
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CBaseAnimating::SequenceDuration");
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_ByValue);
	PrepSDKCall_SetReturnInfo(SDKType_Float, SDKPass_ByValue);
	g_hSDKSequenceDuration = EndPrepSDKCall();
	if (g_hSDKSequenceDuration == INVALID_HANDLE) PrintToServer("Failed to retrieve CBaseAnimating::SequenceDuration signature!");
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CBaseAnimating::ResetSequence");
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_ByValue);
	g_hSDKResetSequence = EndPrepSDKCall();
	if(g_hSDKResetSequence == INVALID_HANDLE)
	{
		SetFailState("Failed to retrieve CBaseAnimating::ResetSequence signature!");
	}
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CBaseAnimating::LookupPoseParameter");
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_ByValue);
	PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_ByValue);
	g_hSDKLookupPoseParameter = EndPrepSDKCall();
	if (g_hSDKLookupPoseParameter == INVALID_HANDLE) PrintToServer("Failed to retrieve CBaseAnimating::LookupPoseParameter signature!");
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CBaseAnimating::SetPoseParameter");
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
	PrepSDKCall_AddParameter(SDKType_Float, SDKPass_Plain);
	PrepSDKCall_SetReturnInfo(SDKType_Float, SDKPass_Plain);
	g_hSDKSetPoseParameter = EndPrepSDKCall();
	if (g_hSDKSetPoseParameter == INVALID_HANDLE) PrintToServer("Failed to retrieve CBaseAnimating::SetPoseParameter signature!");
	
	g_ipStudioHdrOffset = GameConfGetOffset(hGameData, "CBaseAnimating::m_pStudioHdr");
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "CBaseAnimatingOverlay::AddGestureSequence");
	PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_ByValue);
	PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_ByValue);
	g_hSDKAddGestureSequence = EndPrepSDKCall();
	if (g_hSDKAddGestureSequence == INVALID_HANDLE) PrintToServer("Failed to retrieve CBaseAnimatingOverlay::AddGestureSequence signature!");
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Virtual, "CBaseAnimating::StudioFrameAdvance");
	g_hSDKStudioFrameAdvance = EndPrepSDKCall();
	if (g_hSDKStudioFrameAdvance == INVALID_HANDLE) SetFailState("Failed to retrieve CBaseAnimating::StudioFrameAdvance offset!");
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Virtual, "CBaseAnimating::DispatchAnimEvents");
	PrepSDKCall_AddParameter(SDKType_CBaseEntity, SDKPass_Pointer);
	g_hSDKDispatchAnimEvents = EndPrepSDKCall();
	if (g_hSDKDispatchAnimEvents == INVALID_HANDLE) SetFailState("Failed to retrieve CBaseAnimating::DispatchAnimEvents offset!");
	
	StartPrepSDKCall(SDKCall_Entity);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Virtual, "CBaseEntity::GetVectors");
	PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef, _, VENCODE_FLAG_COPYBACK);
	PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef, _, VENCODE_FLAG_COPYBACK);
	PrepSDKCall_AddParameter(SDKType_Vector, SDKPass_ByRef, _, VENCODE_FLAG_COPYBACK);
	if((g_hSDKGetVectors = EndPrepSDKCall()) == INVALID_HANDLE) SetFailState("Failed to create Virtual Call for CBaseEntity::GetVectors!");
}

stock void SDK_UpdateLastKnownArea(int iEntity)
{
	if (g_hSDKUpdateLastKnownArea != null)
	{
		SDKCall(g_hSDKUpdateLastKnownArea, iEntity);
	}
}

stock Address SDK_GetLastKnownArea(int iEntity)
{
	if (g_hSDKGetLastKnownArea != null)
	{
		return SDKCall(g_hSDKGetLastKnownArea, iEntity);
	}
	return Address_Null;
}

stock void SDK_StudioFrameAdvance(int iEntity)
{
	if (g_hSDKStudioFrameAdvance != INVALID_HANDLE)
	{
		SDKCall(g_hSDKStudioFrameAdvance, iEntity);
	}
}

stock void SDK_DispatchAnimEvents(int iEntity)
{
	if (g_hSDKDispatchAnimEvents != INVALID_HANDLE)
	{
		SDKCall(g_hSDKDispatchAnimEvents, iEntity, iEntity);
	}
}

stock void SDK_GetVectors(int iEntity, float vecForward[3], float vecRight[3], float vecUp[3])
{
	if (g_hSDKGetVectors != INVALID_HANDLE)
	{
		SDKCall(g_hSDKGetVectors, iEntity, vecForward, vecRight, vecUp);
	}
}

//Detours
public Action NextBotGroundLocomotion_UpdatePosition(NextBotGroundLocomotion mover, float vecFromPos[3], float vecToPos[3], float vecAdjustedPos[3], float vecEditedPos[3])
{
	/*if (vecFromPos[0] == vecToPos[0] && vecFromPos[1] == vecToPos[1] && vecFromPos[2] == vecToPos[2]) //Nothing happened
		return Plugin_Continue;
		
	CBaseNPC Npc = NPCGetFromLocomotion(mover);
	if (Npc == INVALID_NPC)
		return Plugin_Continue;
	
	int iEntity = EntRefToEntIndex(g_CBaseNPCEntityRef[Npc.Index]);
	if (iEntity <= MaxClients)
		return Plugin_Continue;
	
	if (vecAdjustedPos[0] == vecFromPos[0] && vecAdjustedPos[1] == vecFromPos[1] && vecAdjustedPos[2] == vecFromPos[2])//we tried to move and failed, let's figure out why
	{
		float flAvoidDist = 5.0;
		
		float flHullWidth = g_CBaseNPCBodyInterface[Npc.Index].GetHullWidth();
		float flHalfHullWidth = flHullWidth/2.0;
		float flHullHeight = g_CBaseNPCBodyInterface[Npc.Index].GetHullHeight();
		
		//vecFromPos[2] -= flHullHeight/2.0;
		vecFromPos[2] += g_CBaseNPCflStepSize[Npc.Index];
		
		float vecXBoxMins[3];
		vecXBoxMins[0] = -flAvoidDist;
		vecXBoxMins[1] = -flHalfHullWidth;
		vecXBoxMins[2] = g_CBaseNPCflStepSize[Npc.Index];
		float vecXBoxMaxs[3];
		vecXBoxMaxs[0] = flAvoidDist;
		vecXBoxMaxs[1] = flHalfHullWidth;
		vecXBoxMaxs[2] = flHullHeight;
		float vecYBoxMins[3];
		vecYBoxMins[0] = -flHalfHullWidth;
		vecYBoxMins[1] = -flAvoidDist;
		vecYBoxMins[2] = g_CBaseNPCflStepSize[Npc.Index];
		float vecYBoxMaxs[3];
		vecYBoxMaxs[0] = flHalfHullWidth;
		vecYBoxMaxs[1] = flAvoidDist;
		vecYBoxMaxs[2] = flHullHeight;
		float vecZBoxMins[3];
		vecZBoxMins[0] = -flHalfHullWidth;
		vecZBoxMins[1] = -flHalfHullWidth;
		vecZBoxMins[2] = 0.0;
		float vecZBoxMaxs[3];
		vecZBoxMaxs[0] = flHalfHullWidth;
		vecZBoxMaxs[1] = flHalfHullWidth;
		vecZBoxMaxs[2] = flAvoidDist;
		
		float vecFrontBoxPos[3], vecBehindBoxPos[3], vecLeftBox[3], vecRightBox[3], vecTopBoxPos[3];
		//float vecFrontHitPos[3], vecBehindHitPos[3], vecLeftHisPos[3], vecRightHitPos[3], vecTopHitPos[3];
		bool bFrontBoxHit, bBehindBoxHit, bLeftBoxHit, bRightBoxHit, bTopBoxHit;
		vecFrontBoxPos = vecFromPos;
		vecBehindBoxPos = vecFromPos;
		
		vecFrontBoxPos[0] += (flHullWidth/2.0+flAvoidDist);
		vecBehindBoxPos[0] -= (flHullWidth/2.0+flAvoidDist);
		
		Handle hTrace = TR_TraceHullFilterEx(vecFrontBoxPos, vecFrontBoxPos, vecXBoxMins, vecXBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bFrontBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecFrontBoxPos, vecXBoxMins, vecXBoxMaxs, bFrontBoxHit);
	#endif
		
		hTrace = TR_TraceHullFilterEx(vecBehindBoxPos, vecBehindBoxPos, vecXBoxMins, vecXBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bBehindBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecBehindBoxPos, vecXBoxMins, vecXBoxMaxs, bBehindBoxHit);
	#endif
		
		vecLeftBox = vecFromPos;
		vecRightBox = vecFromPos;
		
		vecLeftBox[1] += (flHullWidth/2.0+flAvoidDist);
		vecRightBox[1] -= (flHullWidth/2.0+flAvoidDist);
		
		hTrace = TR_TraceHullFilterEx(vecLeftBox, vecLeftBox, vecYBoxMins, vecYBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bLeftBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecLeftBox, vecYBoxMins, vecYBoxMaxs, bLeftBoxHit);
	#endif
		
		hTrace = TR_TraceHullFilterEx(vecRightBox, vecRightBox, vecYBoxMins, vecYBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bRightBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecRightBox, vecYBoxMins, vecYBoxMaxs, bRightBoxHit);
	#endif
		
		vecTopBoxPos = vecFromPos;
		vecTopBoxPos[2] += flHullHeight;
		
		hTrace = TR_TraceHullFilterEx(vecTopBoxPos, vecTopBoxPos, vecZBoxMins, vecZBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bTopBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecTopBoxPos, vecZBoxMins, vecZBoxMaxs, bTopBoxHit);
	#endif
	
		vecEditedPos = vecFromPos;
		vecEditedPos[2] = vecToPos[2];
		if (bFrontBoxHit || bBehindBoxHit)
		{
			if (!bBehindBoxHit)
				vecEditedPos[0] -= 0.1;
			if (!bFrontBoxHit)
				vecEditedPos[0] += 0.1;
		}
		
		if (bRightBoxHit || bLeftBoxHit)
		{
			if (!bRightBoxHit)
				vecEditedPos[1] -= 0.1;
			if (!bLeftBoxHit)
				vecEditedPos[1] += 0.1;
		}
		
		if (bTopBoxHit && !bRightBoxHit && !bLeftBoxHit && !bFrontBoxHit && !bBehindBoxHit)
			vecEditedPos[2] -= 0.1;
		
		if (vecEditedPos[0] != vecFromPos[0] || vecEditedPos[1] != vecFromPos[1] || vecEditedPos[2] != vecToPos[2])
			return Plugin_Changed;
	}
	else if (!mover.IsOnGround() && !mover.IsClimbingOrJumping())//We are in the air perform hull checks and stop our velocity if we collide with a wall
	{
		float flAvoidDist = 2.0;
		
		float flHullWidth = g_CBaseNPCBodyInterface[Npc.Index].GetHullWidth();
		float flHalfHullWidth = flHullWidth/2.0;
		float flHullHeight = g_CBaseNPCBodyInterface[Npc.Index].GetHullHeight();
		
		//vecFromPos[2] -= flHullHeight/2.0;
		vecFromPos[2] += g_CBaseNPCflStepSize[Npc.Index];
		
		float vecXBoxMins[3];
		vecXBoxMins[0] = -flAvoidDist;
		vecXBoxMins[1] = -flHalfHullWidth;
		vecXBoxMins[2] = g_CBaseNPCflStepSize[Npc.Index];
		float vecXBoxMaxs[3];
		vecXBoxMaxs[0] = flAvoidDist;
		vecXBoxMaxs[1] = flHalfHullWidth;
		vecXBoxMaxs[2] = flHullHeight;
		float vecYBoxMins[3];
		vecYBoxMins[0] = -flHalfHullWidth;
		vecYBoxMins[1] = -flAvoidDist;
		vecYBoxMins[2] = g_CBaseNPCflStepSize[Npc.Index];
		float vecYBoxMaxs[3];
		vecYBoxMaxs[0] = flHalfHullWidth;
		vecYBoxMaxs[1] = flAvoidDist;
		vecYBoxMaxs[2] = flHullHeight;
		float vecZBoxMins[3];
		vecZBoxMins[0] = -flHalfHullWidth;
		vecZBoxMins[1] = -flHalfHullWidth;
		vecZBoxMins[2] = 0.0;
		float vecZBoxMaxs[3];
		vecZBoxMaxs[0] = flHalfHullWidth;
		vecZBoxMaxs[1] = flHalfHullWidth;
		vecZBoxMaxs[2] = flAvoidDist;
		
		float vecFrontBoxPos[3], vecBehindBoxPos[3], vecLeftBox[3], vecRightBox[3], vecTopBoxPos[3];
		//float vecFrontHitPos[3], vecBehindHitPos[3], vecLeftHisPos[3], vecRightHitPos[3], vecTopHitPos[3];
		bool bFrontBoxHit, bBehindBoxHit, bLeftBoxHit, bRightBoxHit, bTopBoxHit;
		vecFrontBoxPos = vecFromPos;
		vecBehindBoxPos = vecFromPos;
		
		vecFrontBoxPos[0] += (flHullWidth/2.0+flAvoidDist);
		vecBehindBoxPos[0] -= (flHullWidth/2.0+flAvoidDist);
		
		Handle hTrace = TR_TraceHullFilterEx(vecFrontBoxPos, vecFrontBoxPos, vecXBoxMins, vecXBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bFrontBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecFrontBoxPos, vecXBoxMins, vecXBoxMaxs, bFrontBoxHit);
	#endif
		
		hTrace = TR_TraceHullFilterEx(vecBehindBoxPos, vecBehindBoxPos, vecXBoxMins, vecXBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bBehindBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecBehindBoxPos, vecXBoxMins, vecXBoxMaxs, bBehindBoxHit);
	#endif
		
		vecLeftBox = vecFromPos;
		vecRightBox = vecFromPos;
		
		vecLeftBox[1] += (flHullWidth/2.0+flAvoidDist);
		vecRightBox[1] -= (flHullWidth/2.0+flAvoidDist);
		
		hTrace = TR_TraceHullFilterEx(vecLeftBox, vecLeftBox, vecYBoxMins, vecYBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bLeftBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecLeftBox, vecYBoxMins, vecYBoxMaxs, bLeftBoxHit);
	#endif
		
		hTrace = TR_TraceHullFilterEx(vecRightBox, vecRightBox, vecYBoxMins, vecYBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bRightBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecRightBox, vecYBoxMins, vecYBoxMaxs, bRightBoxHit);
	#endif
		
		vecTopBoxPos = vecFromPos;
		vecTopBoxPos[2] += flHullHeight;
		
		hTrace = TR_TraceHullFilterEx(vecTopBoxPos, vecTopBoxPos, vecZBoxMins, vecZBoxMaxs, MASK_NPCSOLID, TraceRayDontHitEntity, iEntity);
		bTopBoxHit = TR_DidHit(hTrace);
		delete hTrace;
	#if defined DEBUG
		DrawBox(vecTopBoxPos, vecZBoxMins, vecZBoxMaxs, bTopBoxHit);
	#endif
	
		vecEditedPos = vecToPos;
		if (bTopBoxHit)
			vecEditedPos[2] = vecFromPos[2];
		
		if (bFrontBoxHit)
		{
			if (vecEditedPos[0] > vecFromPos[0])
				vecEditedPos[0] = vecFromPos[0];
		}
		else if (bBehindBoxHit)
		{
			if (vecEditedPos[0] < vecFromPos[0])
				vecEditedPos[0] = vecFromPos[0];
		}
		
		if (bLeftBoxHit)
		{
			if (vecEditedPos[1] > vecFromPos[1])
				vecEditedPos[1] = vecFromPos[1];
		}
		else if (bRightBoxHit)
		{
			if (vecEditedPos[1] < vecFromPos[1])
				vecEditedPos[1] = vecFromPos[1];
		}
		
		if (vecEditedPos[0] != vecFromPos[0] || vecEditedPos[1] != vecFromPos[1] || vecEditedPos[2] != vecFromPos[2])
			return Plugin_Changed;
	}
	return Plugin_Continue;*/
}

public bool TraceRayDontHitEntity(int entity, int mask, any data)
{
	if (entity == data) return false;
	if (entity != 0) return false;
	return true;
}

#if defined DEBUG
void DrawBox(float origin[3], float mins[3], float maxs[3], bool bShouldBeRed)
{
	float corners[4][2];
	corners[0][0] = mins[0]; corners[0][1] = mins[1];
	corners[1][0] = mins[0]; corners[1][1] = maxs[1];
	corners[3][0] = maxs[0]; corners[3][1] = mins[1];
	corners[2][0] = maxs[0]; corners[2][1] = maxs[1];

	int color[4];
	if (bShouldBeRed)
		color = {255, 0, 0, 255};
	else
		color = {51, 255, 0, 255};
	
	for(int i=0; i<sizeof(corners); i++)
	{
		float start[3], end[3];
		for(int j=0; j<2; j++)
		{
			start[j] = corners[i][j] + origin[j];
			end[j] = corners[(i+1)%sizeof(corners)][j] + origin[j];
		}
		start[2] = end[2] = maxs[2] + origin[2];
		TE_SetupBeamPoints(start, end, g_iLaserIndex, 0, 0, 15, 0.1, 1.0, 5.0, 5, 0.1, color, 1);
		TE_SendToAll();
	}
	
	for(int i=0; i<sizeof(corners); i++)
	{
		float start[3], end[3];
		for(int j=0; j<2; j++)
		{
			start[j] = end[j] = corners[i][j] + origin[j];
		}
		start[2] = maxs[2] + origin[2];
		end[2] = mins[2] + origin[2];

		TE_SetupBeamPoints(start, end, g_iLaserIndex, 0, 0, 15, 0.1, 1.0, 5.0, 5, 0.1, color, 1);
		TE_SendToAll();
	}	
}
#endif

public void OnEntityCreated(int iEntity, const char[] sClassname)
{
	if (iEntity > MaxClients)
	{
		
	}
}

public void OnEntityDestroyed(int iEntity)
{
	if (iEntity > MaxClients)
	{
		CBaseNPC Npc = NPCFindByEntityIndex(iEntity);
		if (Npc != INVALID_NPC)
		{
			if (g_CBaseNPCHooks[Npc.Index].Length > 0)
			{
				for (int i = 0; i < g_CBaseNPCHooks[Npc.Index].Length; i++)
				{
					int hookID = g_CBaseNPCHooks[Npc.Index].Get(i);
					DHookRemoveHookID(hookID);
				}
			}
			g_CBaseNPCHooks[Npc.Index].Clear();
		}
	}
}

public MRESReturn ClimbUpToLedge(Address pThis, Handle hParams)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	INextBot bot = NpcLocomotion.GetBot();
	float vecMyPos[3], vecJumpPos[3];
	bot.GetPosition(vecMyPos);
	DHookGetParamVector(hParams, 1, vecJumpPos);
	
	float vecJumpVel[3];
	float flActualHeight = vecJumpPos[2] - vecMyPos[2];
	float height = flActualHeight;
	if ( height < 16.0 )
	{
		height = 16.0;
	}
	
	float additionalHeight = 20.0;
	if ( height < 32 )
	{
		additionalHeight += 8.0;
	}

	height += additionalHeight;
	
	float flGravity = NpcLocomotion.GetGravity();
	float speed = SquareRoot( 2.0 * flGravity * height );
	float time = speed / flGravity;
	
	time += SquareRoot( (2.0 * additionalHeight) / flGravity );
	
	SubtractVectors( vecJumpPos, vecMyPos, vecJumpVel );
	vecJumpVel[0] /= time;
	vecJumpVel[1] /= time;
	vecJumpVel[2] /= time;
	
	vecJumpVel[2] = speed;

	float flJumpSpeed = GetVectorLength(vecJumpVel);
	float flMaxSpeed = 650.0;
	if ( flJumpSpeed > flMaxSpeed )
	{
		vecJumpVel[0] *= flMaxSpeed / flJumpSpeed;
		vecJumpVel[1] *= flMaxSpeed / flJumpSpeed;
		vecJumpVel[2] *= flMaxSpeed / flJumpSpeed;
	}

	NpcLocomotion.Jump();
	NpcLocomotion.SetVelocity(vecJumpVel);
	return MRES_Supercede;
}

public MRESReturn GetGravity(Address pThis, Handle hReturn)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	DHookSetReturn(hReturn, g_CBaseNPCflGravity[Npc.Index]);
	return MRES_Supercede;
}

public MRESReturn GetAcceleration(Address pThis, Handle hReturn)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	DHookSetReturn(hReturn, g_CBaseNPCflAcceleration[Npc.Index]);
	return MRES_Supercede;
}

public MRESReturn GetStepHeight(Address pThis, Handle hReturn)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	DHookSetReturn(hReturn, g_CBaseNPCflStepSize[Npc.Index]);
	return MRES_Supercede;
}

public MRESReturn GetMaxJumpHeight(Address pThis, Handle hReturn)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	DHookSetReturn(hReturn, g_CBaseNPCflJumpHeight[Npc.Index]);
	return MRES_Supercede;
}

public MRESReturn GetWalkSpeed(Address pThis, Handle hReturn)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	DHookSetReturn(hReturn, g_CBaseNPCflWalkSpeed[Npc.Index]);
	return MRES_Supercede;
}

public MRESReturn GetRunSpeed(Address pThis, Handle hReturn)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	DHookSetReturn(hReturn, g_CBaseNPCflRunSpeed[Npc.Index]);
	return MRES_Supercede;
}

public MRESReturn ShouldCollideWith(Address pThis, Handle hReturn, Handle hParams)
{
	/*int iEntity = DHookGetParam(hParams, 1);
	if (IsValidEntity(iEntity))
	{
		char strClass[32];
		GetEdictClassname(iEntity, strClass, sizeof(strClass));
		if(strcmp(strClass, "tf_zombie") == 0 || strcmp(strClass, "base_boss") == 0 || strcmp(strClass, "tf_ammo_pack") == 0 || strcmp(strClass, "tf_dropped_weapon") == 0 )
		{
			DHookSetReturn(hReturn, false);
			return MRES_Supercede;
		}
	}
	return MRES_Ignored;*/
	DHookSetReturn(hReturn, false);
	return MRES_Supercede;
}

public MRESReturn GetFrictionForward(Address pThis, Handle hReturn)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	DHookSetReturn(hReturn, g_CBaseNPCflFrictionForward[Npc.Index]);
	return MRES_Supercede;
}

public MRESReturn GetFrictionSideways(Address pThis, Handle hReturn)
{
	NextBotGroundLocomotion NpcLocomotion = view_as<NextBotGroundLocomotion>(pThis);
	CBaseNPC Npc = NPCGetFromLocomotion(NpcLocomotion);
	if (Npc == INVALID_NPC)
	{
		return MRES_Ignored;
	}
	DHookSetReturn(hReturn, g_CBaseNPCflFrictionSideways[Npc.Index]);
	return MRES_Supercede;
}

//IBody
public MRESReturn StartActivity(Address pThis, Handle hReturn, Handle hParams)
{
	DHookSetReturn(hReturn, true);
	return MRES_Supercede;
}

public MRESReturn GetSolidMask(Address pThis, Handle hReturn)
{
	DHookSetReturn(hReturn, MASK_NPCSOLID|MASK_PLAYERSOLID);
	return MRES_Supercede;
}

public MRESReturn GetHullWidth(Address pThis, Handle hReturn, Handle hParams)
{
	IBody pBody = view_as<IBody>(pThis);
	INextBot pNextBot = pBody.GetBot();
	int iEntity = pNextBot.GetEntity();

	float vecMaxs[3];
	GetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecMaxs);
	
	if(vecMaxs[1] > vecMaxs[0])
		DHookSetReturn(hReturn, vecMaxs[1] * 2);
	else
		DHookSetReturn(hReturn, vecMaxs[0] * 2);
	return MRES_Supercede;
}

public MRESReturn GetHullHeight(Address pThis, Handle hReturn, Handle hParams)
{
	IBody pBody = view_as<IBody>(pThis);
	INextBot pNextBot = pBody.GetBot();
	int iEntity = pNextBot.GetEntity();

	float vecMaxs[3];
	GetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecMaxs);
	
	DHookSetReturn(hReturn, vecMaxs[2]);

	return MRES_Supercede;
}

public MRESReturn GetStandHullHeight(Address pThis, Handle hReturn, Handle hParams)
{
	IBody pBody = view_as<IBody>(pThis);
	INextBot pNextBot = pBody.GetBot();
	int iEntity = pNextBot.GetEntity();

	float vecMaxs[3];
	GetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecMaxs);
	
	DHookSetReturn(hReturn, vecMaxs[2]);

	return MRES_Supercede;
}

public MRESReturn GetCrouchHullHeight(Address pThis, Handle hReturn, Handle hParams)
{
	IBody pBody = view_as<IBody>(pThis);
	INextBot pNextBot = pBody.GetBot();
	int iEntity = pNextBot.GetEntity();

	float vecMaxs[3];
	GetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecMaxs);
	
	DHookSetReturn(hReturn, vecMaxs[2] / 2);

	return MRES_Supercede;
}

public MRESReturn GetHullMins(Address pThis, Handle hReturn, Handle hParams)
{
	IBody pBody = view_as<IBody>(pThis);
	INextBot pNextBot = pBody.GetBot();
	int iEntity = pNextBot.GetEntity();

	float vecMins[3];
	GetEntPropVector(iEntity, Prop_Send, "m_vecMins", vecMins);
	
	DHookSetReturnVector(hReturn, vecMins);

	return MRES_Supercede;
}

public MRESReturn GetHullMaxs(Address pThis, Handle hReturn, Handle hParams)
{
	IBody pBody = view_as<IBody>(pThis);
	INextBot pNextBot = pBody.GetBot();
	int iEntity = pNextBot.GetEntity();

	float vecMaxs[3];
	GetEntPropVector(iEntity, Prop_Send, "m_vecMaxs", vecMaxs);
	
	DHookSetReturnVector(hReturn, vecMaxs);

	return MRES_Supercede;
}

#define VALID_MINIMUM_MEMORY_ADDRESS 0x10000
bool IsValidAddress(Address addr)
{
	if (addr == Address_Null) return false;
	if (addr <= view_as<Address>(VALID_MINIMUM_MEMORY_ADDRESS)) return false;
	
	return true;
}
