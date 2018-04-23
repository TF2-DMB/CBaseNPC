#include <sourcemod>
#include <sdktools>
#include <sdkhooks>
#include <dhooks>
#include <cbasenpc>
#include <profiler>

#define NPC_TEST_MODEL		"models/player/sniper.mdl"
#define NPC_TEST_ITEM		"models/player/items/sniper/sniper_zombie.mdl"

CBaseNPC g_ControlledNPC = INVALID_NPC;
PathFollower pPath;
int g_iSummoner;
float flLastAttackTime;

public void OnPluginStart()
{
	RegAdminCmd("spawn_test_npc", Command_SpawnNPC, ADMFLAG_CHEATS);
	RegAdminCmd("teleport_test_npc", Command_TeleportNPC, ADMFLAG_CHEATS);
	RegAdminCmd("set_goal_npc", Command_SetGoalNPC, ADMFLAG_CHEATS);//Change path to pathfollower if you use this command
	RegAdminCmd("set_goal_npc_me", Command_SetGoalNPCMe, ADMFLAG_CHEATS);//Change path to pathfollower if you use this command
	
	pPath = PathFollower(_, Path_FilterIgnoreActors, Path_FilterOnlyActors);
}

public void OnMapStart()
{
	PrecacheModel(NPC_TEST_MODEL);
	PrecacheModel(NPC_TEST_ITEM);
	flLastAttackTime = 0.0;
}

public void Hook_NPCThink(int iEnt)
{
	CBaseNPC npc = TheNPCs.FindNPCByEntIndex(iEnt);
	int iClient = GetClientOfUserId(g_iSummoner);
	if (npc != INVALID_NPC && 0 < iClient <= MaxClients && IsPlayerAlive(iClient))
	{
		float vecNPCPos[3], vecTargetPos[3];
		INextBot bot = npc.GetBot();
		NextBotGroundLocomotion loco = npc.GetLocomotion();
		
		bot.GetPosition(vecNPCPos);
		GetClientAbsOrigin(iClient, vecTargetPos);
		
		CBaseAnimatingOverlay animationEntity = new CBaseAnimatingOverlay(iEnt);
		
		if (GetVectorDistance(vecNPCPos, vecTargetPos) > 100.0)
			pPath.Update(bot);
		else if (flLastAttackTime <= GetGameTime())
		{
			int iSequence;
			int iRandom = GetRandomInt(0,1);
			if (iRandom == 0)
				iSequence = animationEntity.LookupSequence("ITEM1_fire");
			else
				iSequence = animationEntity.LookupSequence("Melee_Swing");
			
			animationEntity.AddGestureSequence(iSequence);
			flLastAttackTime = GetGameTime()+1.0;
			
			loco.FaceTowards(vecTargetPos);
		}
		loco.Run();
		
		int iSequence = GetEntProp(iEnt, Prop_Send, "m_nSequence");
		
		static int sequence_ilde = -1;
		if (sequence_ilde == -1) sequence_ilde = animationEntity.LookupSequence("Stand_MELEE");
		
		static int sequence_air_walk = -1;
		if (sequence_air_walk == -1) sequence_air_walk = animationEntity.LookupSequence("Airwalk_MELEE");
		
		static int sequence_run = -1;
		if (sequence_run == -1) sequence_run = animationEntity.LookupSequence("run_MELEE");
		
		Address pModelptr = animationEntity.GetModelPtr();
		int iMoveX = animationEntity.LookupPoseParameter(pModelptr, "move_x");
		int iMoveY = animationEntity.LookupPoseParameter(pModelptr, "move_y");
		
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
			npc.GetVectors(vecForward, vecRight, vecUp);
			loco.GetGroundMotionVector(vecMotion);
			float newMoveX = (vecForward[1] * vecMotion[1]) + (vecForward[0] * vecMotion[0]) +  (vecForward[2] * vecMotion[2]);
			float newMoveY = (vecRight[1] * vecMotion[1]) + (vecRight[0] * vecMotion[0]) + (vecRight[2] * vecMotion[2]);
			
			animationEntity.SetPoseParameter(pModelptr, iMoveX, newMoveX);
			animationEntity.SetPoseParameter(pModelptr, iMoveY, newMoveY);
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
	
	if (g_ControlledNPC != INVALID_NPC && TheNPCs.IsValidNPC(g_ControlledNPC))
	{
		ReplyToCommand(iClient, "You can't spawn more than one test npc.");
		return Plugin_Handled;
	}
	
	float eyePos[3], eyeAng[3], endPos[3];
	GetClientEyePosition(iClient, eyePos);
	GetClientEyeAngles(iClient, eyeAng);
	
	Handle hTrace = TR_TraceRayFilterEx(eyePos, eyeAng, MASK_NPCSOLID, RayType_Infinite, TraceRayDontHitEntity, iClient);
	TR_GetEndPosition(endPos, hTrace);
	delete hTrace;
	
	CBaseNPC npc = new CBaseNPC();
	npc.Teleport(endPos);
	npc.SetModel(NPC_TEST_MODEL);
	npc.Spawn();
	npc.SetThinkFunction(Hook_NPCThink);
	npc.nSkin = 4;
	npc.EquipItem("head", NPC_TEST_ITEM);
	
	npc.flStepSize = 18.0;
	npc.flGravity = 800.0;
	npc.flAcceleration = 4000.0;
	npc.flJumpHeight = 85.0;
	npc.flWalkSpeed = 300.0;
	npc.flRunSpeed = 300.0;

	npc.iMaxHealth = 9999999;
	npc.iHealth = 99999999;
		
	npc.Run();
	CBaseAnimatingOverlay animationEntity = new CBaseAnimatingOverlay(npc.GetEntity());
	animationEntity.PlayAnimation("Stand_MELEE");
	g_iSummoner = GetClientUserId(iClient);
	
	g_ControlledNPC = npc;
	return Plugin_Handled;
}

public Action Command_TeleportNPC(int iClient, int iArgs)
{
	if (iClient <= 0 || iClient > MaxClients || !IsClientInGame(iClient))
	{
		ReplyToCommand(iClient, "This command can only be executed by a player!");
		return Plugin_Handled;
	}
	
	if (g_ControlledNPC == INVALID_NPC || !TheNPCs.IsValidNPC(g_ControlledNPC))
	{
		ReplyToCommand(iClient, "No valid test npc.");
		return Plugin_Handled;
	}
	
	float eyePos[3], eyeAng[3], endPos[3];
	GetClientEyePosition(iClient, eyePos);
	GetClientEyeAngles(iClient, eyeAng);
	
	Handle hTrace = TR_TraceRayFilterEx(eyePos, eyeAng, MASK_NPCSOLID, RayType_Infinite, TraceRayDontHitEntity, iClient);
	TR_GetEndPosition(endPos, hTrace);
	delete hTrace;
	
	g_ControlledNPC.Teleport(endPos);
	return Plugin_Handled;
}

public Action Command_SetGoalNPC(int iClient, int iArgs)
{
	if (iClient <= 0 || iClient > MaxClients || !IsClientInGame(iClient))
	{
		ReplyToCommand(iClient, "This command can only be executed by a player!");
		return Plugin_Handled;
	}
	
	if (g_ControlledNPC == INVALID_NPC || !TheNPCs.IsValidNPC(g_ControlledNPC))
	{
		ReplyToCommand(iClient, "No valid test npc.");
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
	PrintToChatAll("Path %x", view_as<int>(pPath));
	endPos[2] += 10.0;
	if (pPath.ComputeToPos(g_ControlledNPC.GetBot(), endPos, 9999999999.0))
		PrintToChatAll("Path built!");
	else
		PrintToChatAll("Failed to build path!");
	StopProfiling(hProf);
	PrintToChatAll("Total build time: %f", GetProfilerTime(hProf));
	delete hProf;
	pPath.SetMinLookAheadDistance(300.0);
	return Plugin_Handled;
}

public Action Command_SetGoalNPCMe(int iClient, int iArgs)
{
	if (iClient <= 0 || iClient > MaxClients || !IsClientInGame(iClient))
	{
		ReplyToCommand(iClient, "This command can only be executed by a player!");
		return Plugin_Handled;
	}
	
	if (g_ControlledNPC == INVALID_NPC || !TheNPCs.IsValidNPC(g_ControlledNPC))
	{
		ReplyToCommand(iClient, "No valid test npc.");
		return Plugin_Handled;
	}
	
	Handle hProf = CreateProfiler();
	StartProfiling(hProf);
	PrintToChatAll("Path %x", view_as<int>(pPath));
	if (pPath.ComputeToTarget(g_ControlledNPC.GetBot(), iClient))
		PrintToChatAll("Path built!");
	else
		PrintToChatAll("Failed to build path!");
	StopProfiling(hProf);
	PrintToChatAll("Total build timer: %f", GetProfilerTime(hProf));
	delete hProf;
	pPath.SetMinLookAheadDistance(300.0);
	
	return Plugin_Handled;
}

public bool TraceRayDontHitEntity(int entity,int mask,any data)
{
	if (entity == data) return false;
	if (entity != 0) return false;
	return true;
}