#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <dhooks>
#include <cbasenpc>
#include <cbasenpc/util>
#include <profiler>

#define NPC_TEST_MODEL		"models/player/medic.mdl"
#define NPC_TEST_ITEM		"models/player/items/medic/medic_zombie.mdl"

CBaseNPC g_ControlledNPC[MAXPLAYERS+1] = {INVALID_NPC, ...};
PathFollower pPath[MAX_NPCS];
int g_iSummoner[MAX_NPCS];

float g_flLastAttackTime[MAX_NPCS];

public void OnPluginStart()
{
	RegAdminCmd("spawn_test_npc", Command_SpawnNPC, ADMFLAG_CHEATS);
	RegAdminCmd("teleport_test_npc", Command_TeleportNPC, ADMFLAG_CHEATS);
	RegAdminCmd("set_goal_npc", Command_SetGoalNPC, ADMFLAG_CHEATS);
	RegAdminCmd("set_goal_npc_me", Command_SetGoalNPCMe, ADMFLAG_CHEATS);
	
	for (int i = 0; i < MAX_NPCS; i++) pPath[i] = PathFollower(_, Path_FilterIgnoreActors, Path_FilterOnlyActors);
}

public void OnMapStart()
{
	PrecacheModel(NPC_TEST_MODEL);
	PrecacheModel(NPC_TEST_ITEM);
}

public void OnClientPutInServer(int iClient)
{
	g_ControlledNPC[iClient] = INVALID_NPC;
}

public void Hook_NPCThink(int iEnt)
{
	CBaseNPC npc = TheNPCs.FindNPCByEntIndex(iEnt);
	//int iClient = GetClientOfUserId(g_iSummoner[npc.Index]);
	
	if (npc != INVALID_NPC)
	{
		float vecNPCPos[3], vecNPCAng[3], vecTargetPos[3];
		INextBot bot = npc.GetBot();
		NextBotGroundLocomotion loco = npc.GetLocomotion();
		
		bot.GetPosition(vecNPCPos);
		GetEntPropVector(iEnt, Prop_Data, "m_angAbsRotation", vecNPCAng);
		
		float flMaxDistance = 9999999999999999.0;
		int iBestTarget = -1;
		for (int i = 1; i <= MaxClients; i++)
		{
			if (IsClientInGame(i) && IsPlayerAlive(i))
			{
				float vecBuffer[3];
				GetClientAbsOrigin(i, vecBuffer);
				float flDistance = GetVectorDistance(vecBuffer, vecNPCPos);
				if (flDistance < flMaxDistance)
				{
					flMaxDistance = flDistance;
					iBestTarget = i;
				}
			}
		}
		
		if (iBestTarget == -1) return;
		GetClientAbsOrigin(iBestTarget, vecTargetPos);
		
		CBaseCombatCharacter animationEntity = CBaseCombatCharacter(iEnt);
		
		if (GetVectorDistance(vecNPCPos, vecTargetPos) > 100.0)
		{
			pPath[npc.Index].Update(bot);
			loco.FaceTowards(vecTargetPos);
		}
		else if (g_flLastAttackTime[npc.Index] <= GetGameTime())
		{
			animationEntity.RestartGesture(ACT_MP_ATTACK_STAND_MELEE);
			g_flLastAttackTime[npc.Index] = GetGameTime()+1.0;
			
			loco.FaceTowards(vecTargetPos);
			SlapPlayer(iBestTarget, GetRandomInt(30,50), false);
		}
		loco.Run();
		
		int iSequence = GetEntProp(iEnt, Prop_Send, "m_nSequence");
		
		static int sequence_ilde = -1;
		if (sequence_ilde == -1) sequence_ilde = animationEntity.SelectWeightedSequence(ACT_MP_STAND_MELEE);
		
		static int sequence_air_walk = -1;
		if (sequence_air_walk == -1) sequence_air_walk = animationEntity.SelectWeightedSequence(ACT_MP_JUMP_FLOAT_MELEE);
		
		static int sequence_run = -1;
		if (sequence_run == -1) sequence_run = animationEntity.SelectWeightedSequence(ACT_MP_RUN_MELEE);
		
		
		int iPitch = animationEntity.LookupPoseParameter("body_pitch");
		int iYaw = animationEntity.LookupPoseParameter("body_yaw");
		float vecDir[3], vecAng[3], vecNPCCenter[3], vecPlayerCenter[3];
		animationEntity.WorldSpaceCenter(vecNPCCenter);
		CBaseAnimating(iBestTarget).WorldSpaceCenter(vecPlayerCenter);
		SubtractVectors(vecNPCCenter, vecPlayerCenter, vecDir); 
		NormalizeVector(vecDir, vecDir);
		GetVectorAngles(vecDir, vecAng); 
		
		
		float flPitch = animationEntity.GetPoseParameter(iPitch);
		float flYaw = animationEntity.GetPoseParameter(iYaw);
		
		vecAng[0] = UTIL_Clamp(UTIL_AngleNormalize(vecAng[0]), -44.0, 89.0);
		animationEntity.SetPoseParameter(iPitch, UTIL_ApproachAngle(vecAng[0], flPitch, 1.0));
		vecAng[1] = UTIL_Clamp(-UTIL_AngleNormalize(UTIL_AngleDiff(UTIL_AngleNormalize(vecAng[1]), UTIL_AngleNormalize(vecNPCAng[1]+180.0))), -44.0,  44.0);
		animationEntity.SetPoseParameter(iYaw, UTIL_ApproachAngle(vecAng[1], flYaw, 1.0));
		
		int iMoveX = animationEntity.LookupPoseParameter("move_x");
		int iMoveY = animationEntity.LookupPoseParameter("move_y");
		
		if ( iMoveX < 0 || iMoveY < 0 )
			return;
		
		float flGroundSpeed = loco.GetGroundSpeed();
		if ( flGroundSpeed != 0.0 )
		{
			if (!(GetEntityFlags(iEnt) & FL_ONGROUND))
			{
				if (iSequence != sequence_air_walk)
					animationEntity.ResetSequence(sequence_air_walk);
			}
			else
			{			
				if (iSequence != sequence_run)
					animationEntity.ResetSequence(sequence_run);
			}
			
			float vecForward[3], vecRight[3], vecUp[3], vecMotion[3];
			animationEntity.GetVectors(vecForward, vecRight, vecUp);
			loco.GetGroundMotionVector(vecMotion);
			float newMoveX = (vecForward[1] * vecMotion[1]) + (vecForward[0] * vecMotion[0]) +  (vecForward[2] * vecMotion[2]);
			float newMoveY = (vecRight[1] * vecMotion[1]) + (vecRight[0] * vecMotion[0]) + (vecRight[2] * vecMotion[2]);
			
			animationEntity.SetPoseParameter(iMoveX, newMoveX);
			animationEntity.SetPoseParameter(iMoveY, newMoveY);
		}
		else
		{
			if (iSequence != sequence_ilde)
				animationEntity.ResetSequence(sequence_ilde);
		}
	}
}

public Action Command_SpawnNPC(int iClient, int iArgs)
{
	if (iClient <= 0 || iClient > MaxClients || !IsClientInGame(iClient))
	{
		ReplyToCommand(iClient, "This command can only be executed by a player!");
		return Plugin_Handled;
	}
	
	if (TheNPCs.IsValidNPC(g_ControlledNPC[iClient]))
	{
		ReplyToCommand(iClient, "Only one test NPC per player!");
		return Plugin_Handled;
	}
	
	float eyePos[3], eyeAng[3], endPos[3];
	GetClientEyePosition(iClient, eyePos);
	GetClientEyeAngles(iClient, eyeAng);
	
	Handle hTrace = TR_TraceRayFilterEx(eyePos, eyeAng, MASK_NPCSOLID, RayType_Infinite, TraceRayDontHitEntity, iClient);
	TR_GetEndPosition(endPos, hTrace);
	delete hTrace;
	
	CBaseNPC npc = new CBaseNPC();
	if (npc == INVALID_NPC)
	{
		ReplyToCommand(iClient, "Failed to create npc!");
		return Plugin_Handled;
	}
	
	CBaseCombatCharacter npcEntity = CBaseCombatCharacter(npc.GetEntity());
	npcEntity.Spawn();
	npcEntity.Teleport(endPos);
	npcEntity.SetModel(NPC_TEST_MODEL);
	
	SetEntProp(npcEntity.iEnt, Prop_Send, "m_nSkin",  4);
	EquipItem(npcEntity.iEnt, 0, "head", NPC_TEST_ITEM);
	SDKHook(npcEntity.iEnt, SDKHook_Think, Hook_NPCThink);
	
	npc.flStepSize = 18.0;
	npc.flGravity = 800.0;
	npc.flAcceleration = 4000.0;
	npc.flJumpHeight = 85.0;
	npc.flWalkSpeed = 300.0;
	npc.flRunSpeed = 300.0;
	npc.flDeathDropHeight = 2000.0;
	
	float vecMins[3] = {-1.0, -1.0, 0.0};
	float vecMaxs[3] = {1.0, 1.0, 90.0};
	npc.SetBodyMins(vecMins);
	npc.SetBodyMaxs(vecMaxs);
	
	int iSequence = npcEntity.SelectWeightedSequence(ACT_MP_STAND_MELEE);
	if (iSequence != -1)
	{
		npcEntity.ResetSequence(iSequence);
		SetEntPropFloat(npcEntity.iEnt, Prop_Data, "m_flCycle", 0.0);
	}
	
	g_iSummoner[npc.Index] = GetClientUserId(iClient);
	g_flLastAttackTime[npc.Index] = 0.0;
	g_ControlledNPC[iClient] = npc;
	
	return Plugin_Handled;
}

public Action Command_TeleportNPC(int iClient, int iArgs)
{
	if (iClient <= 0 || iClient > MaxClients || !IsClientInGame(iClient))
	{
		ReplyToCommand(iClient, "This command can only be executed by a player!");
		return Plugin_Handled;
	}
	
	if (!TheNPCs.IsValidNPC(g_ControlledNPC[iClient]))
	{
		ReplyToCommand(iClient, "No test NPC available!");
		return Plugin_Handled;
	}
	
	float eyePos[3], eyeAng[3], endPos[3];
	GetClientEyePosition(iClient, eyePos);
	GetClientEyeAngles(iClient, eyeAng);
	
	Handle hTrace = TR_TraceRayFilterEx(eyePos, eyeAng, MASK_NPCSOLID, RayType_Infinite, TraceRayDontHitEntity, iClient);
	TR_GetEndPosition(endPos, hTrace);
	delete hTrace;
	
	CBaseCombatCharacter(g_ControlledNPC[iClient].GetEntity()).Teleport(endPos);
	return Plugin_Handled;
}

public Action Command_SetGoalNPC(int iClient, int iArgs)
{
	if (iClient <= 0 || iClient > MaxClients || !IsClientInGame(iClient))
	{
		ReplyToCommand(iClient, "This command can only be executed by a player!");
		return Plugin_Handled;
	}
	
	if (!TheNPCs.IsValidNPC(g_ControlledNPC[iClient]))
	{
		ReplyToCommand(iClient, "No test NPC available!");
		return Plugin_Handled;
	}
	
	float eyePos[3], eyeAng[3], endPos[3];
	GetClientEyePosition(iClient, eyePos);
	GetClientEyeAngles(iClient, eyeAng);
	
	Handle hTrace = TR_TraceRayFilterEx(eyePos, eyeAng, MASK_NPCSOLID, RayType_Infinite, TraceRayDontHitEntity, iClient);
	TR_GetEndPosition(endPos, hTrace);
	delete hTrace;
	
	Handle hProf = CreateProfiler();
	StartProfiling(hProf);
	PrintToChatAll("Path %x", view_as<int>(pPath[g_ControlledNPC[iClient].Index]));
	endPos[2] += 10.0;
	if (pPath[g_ControlledNPC[iClient].Index].ComputeToPos(g_ControlledNPC[iClient].GetBot(), endPos, 9999999999.0))
		PrintToChatAll("Path built!");
	else
		PrintToChatAll("Failed to build path!");
	StopProfiling(hProf);
	PrintToChatAll("Total build time: %f", GetProfilerTime(hProf));
	delete hProf;
	pPath[g_ControlledNPC[iClient].Index].SetMinLookAheadDistance(300.0);
	return Plugin_Handled;
}

public Action Command_SetGoalNPCMe(int iClient, int iArgs)
{
	if (iClient <= 0 || iClient > MaxClients || !IsClientInGame(iClient))
	{
		ReplyToCommand(iClient, "This command can only be executed by a player!");
		return Plugin_Handled;
	}
	
	if (!TheNPCs.IsValidNPC(g_ControlledNPC[iClient]))
	{
		ReplyToCommand(iClient, "No test NPC available!");
		return Plugin_Handled;
	}
	
	Handle hProf = CreateProfiler();
	StartProfiling(hProf);
	PrintToChatAll("Path %x", view_as<int>(pPath[g_ControlledNPC[iClient].Index]));
	if (pPath[g_ControlledNPC[iClient].Index].ComputeToTarget(g_ControlledNPC[iClient].GetBot(), iClient))
		PrintToChatAll("Path built!");
	else
		PrintToChatAll("Failed to build path!");
	StopProfiling(hProf);
	PrintToChatAll("Total build timer: %f", GetProfilerTime(hProf));
	delete hProf;
	pPath[g_ControlledNPC[iClient].Index].SetMinLookAheadDistance(300.0);
	
	return Plugin_Handled;
}

public bool TraceRayDontHitEntity(int entity,int mask,any data)
{
	if (entity == data) return false;
	if (entity != 0) return false;
	return true;
}

int EquipItem(const int iEntity, const int nSkin, const char[] sAttachement, const char[] sModel)
{
	int iItem = CreateEntityByName("prop_dynamic");
	DispatchKeyValue(iItem, "model", sModel);
	DispatchKeyValueFloat(iItem, "modelscale", GetEntPropFloat(iEntity, Prop_Send, "m_flModelScale"));
	DispatchSpawn(iItem);
	
	SetEntProp(iItem, Prop_Send, "m_nSkin", nSkin);
	SetEntProp(iItem, Prop_Send, "m_fEffects", EF_BONEMERGE|EF_PARENT_ANIMATES);
	SetVariantString("!activator");
	AcceptEntityInput(iItem, "SetParent", iEntity);
	
	SetVariantString(sAttachement);
	AcceptEntityInput(iItem, "SetParentAttachmentMaintainOffset"); 

	return iItem;
}