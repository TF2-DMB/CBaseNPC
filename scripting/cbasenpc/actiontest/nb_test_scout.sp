
#include "cbasenpc/actiontest/nb_test_scout/body.sp"
#include "cbasenpc/actiontest/nb_test_scout/behavior/mainaction.sp"
#include "cbasenpc/actiontest/nb_test_scout/behavior/deathaction.sp"
#include "cbasenpc/actiontest/nb_test_scout/behavior/laughaction.sp"
#include "cbasenpc/actiontest/nb_test_scout/behavior/baitaction.sp"

static CEntityFactory EntityFactory;

static const char g_sPainSounds[][] = {
	"vo/scout_painsharp01.mp3",
	"vo/scout_painsharp02.mp3",
	"vo/scout_painsharp03.mp3",
	"vo/scout_painsharp04.mp3",
	"vo/scout_painsharp05.mp3",
	"vo/scout_painsharp06.mp3",
	"vo/scout_painsharp07.mp3"
};

static const char g_sPainSevereSounds[][] = {
	"vo/scout_painsevere01.mp3",
	"vo/scout_painsevere02.mp3",
	"vo/scout_painsevere03.mp3",
	"vo/scout_painsevere04.mp3",
	"vo/scout_painsevere05.mp3"
};

void ScoutNextBot_OnPluginStart()
{
	ScoutNextBot_InitBehavior();

	EntityFactory = new CEntityFactory("nb_test_scout", ScoutNextBot_Create, ScoutNextBot_OnRemove);
	EntityFactory.DeriveFromNPC();
	EntityFactory.SetInitialActionFactory(ScoutMainAction_GetFactory());
	EntityFactory.BeginDataMapDesc()
		.DefineIntField("m_moveXPoseParameter")
		.DefineIntField("m_moveYPoseParameter")
		.DefineIntField("m_idleSequence")
		.DefineIntField("m_runSequence")
		.DefineIntField("m_airSequence")
		.DefineEntityField("m_Target")
		.DefineFloatField("m_flNextPainSound")
		.DefineInputFunc("Explode", InputFuncValueType_Void, ScoutNextBot_InputExplode)
		.DefineInputFunc("Say", InputFuncValueType_String, ScoutNextBot_InputSay)
		.DefineInputFunc("SayNumber", InputFuncValueType_Integer, ScoutNextBot_InputSayNumber)
	.EndDataMapDesc();

	EntityFactory.Install();
}

static void ScoutNextBot_InitBehavior()
{
	ScoutMainAction_Init();
	ScoutDeathAction_Init();
	ScoutLaughAction_Init();
	ScoutBaitAction_Init();
}

static void ScoutNextBot_Precache()
{
	for (int i = 0; i < sizeof(g_sPainSounds); i++)
	{
		PrecacheSound( g_sPainSounds[i] );
	}

	for (int i = 0; i < sizeof(g_sPainSevereSounds); i++)
	{
		PrecacheSound( g_sPainSevereSounds[i] );
	}

	PrecacheSound("vo/scout_laughlong02.mp3");
	PrecacheSound("vo/scout_paincrticialdeath01.mp3");
}

static void ScoutNextBot_Create(int ent)
{
	ScoutNextBot_Precache();

	SetEntityModel(ent, "models/player/scout.mdl");

	SetEntProp(ent, Prop_Data, "m_iHealth", 125);
	SetEntPropEnt(ent, Prop_Data, "m_Target", INVALID_ENT_REFERENCE);
	SetEntProp(ent, Prop_Data, "m_moveXPoseParameter", -1);
	SetEntProp(ent, Prop_Data, "m_moveYPoseParameter", -1);
	SetEntProp(ent, Prop_Data, "m_idleSequence", -1);
	SetEntProp(ent, Prop_Data, "m_runSequence", -1);
	SetEntProp(ent, Prop_Data, "m_airSequence", -1);
	SetEntPropFloat(ent, Prop_Data, "m_flNextPainSound", GetGameTime());

	CBaseNPC npc = TheNPCs.FindNPCByEntIndex(ent);

	npc.flStepSize = 18.0;
	npc.flGravity = 800.0;
	npc.flAcceleration = 2000.0;
	npc.flJumpHeight = 85.0;
	npc.flWalkSpeed = 400.0;
	npc.flRunSpeed = 400.0;
	npc.flDeathDropHeight = 2000.0;
	npc.flMaxYawRate = 250.0;

	SDKHook(ent, SDKHook_SpawnPost, ScoutNextBot_SpawnPost);
	SDKHook(ent, SDKHook_Think, ScoutNextBot_Think);
	SDKHook(ent, SDKHook_OnTakeDamageAlivePost, ScoutNextBot_OnTakeDamageAlivePost);
}

static void ScoutNextBot_OnRemove(int ent)
{
}

static void ScoutNextBot_SpawnPost(int ent)
{
	CBaseCombatCharacter anim = CBaseCombatCharacter(ent);

	SetEntProp(ent, Prop_Data, "m_moveXPoseParameter", anim.LookupPoseParameter( "move_x" ));
	SetEntProp(ent, Prop_Data, "m_moveYPoseParameter", anim.LookupPoseParameter( "move_y" ));
	SetEntProp(ent, Prop_Data, "m_idleSequence", anim.SelectWeightedSequence( ACT_MP_STAND_PRIMARY ));
	SetEntProp(ent, Prop_Data, "m_runSequence", anim.SelectWeightedSequence( ACT_MP_RUN_PRIMARY ));
	SetEntProp(ent, Prop_Data, "m_airSequence", anim.SelectWeightedSequence( ACT_MP_JUMP_FLOAT_PRIMARY ));
}

static void ScoutNextBot_UpdateTarget(int ent)
{
	float vecPos[3];
	GetEntPropVector(ent, Prop_Data, "m_vecAbsOrigin", vecPos);

	float flMaxDistance = 1200.0;
	int target = INVALID_ENT_REFERENCE;
	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i) && IsPlayerAlive(i))
		{
			float vecBuffer[3];
			GetClientAbsOrigin(i, vecBuffer);
			float flDistance = GetVectorDistance(vecBuffer, vecPos);
			if (flDistance < flMaxDistance)
			{
				flMaxDistance = flDistance;
				target = EntIndexToEntRef(i);
			}
		}
	}

	SetEntPropEnt(ent, Prop_Data, "m_Target", target );
}

static void ScoutNextBot_Think(int ent) 
{
	INextBot bot = CBaseEntity(ent).MyNextBotPointer();
	if (!bot) return;

	ScoutNextBot_UpdateTarget(ent);

	view_as<ScoutBody>(bot.GetBodyInterface()).UpdateAnimation();
}

static void ScoutNextBot_OnTakeDamageAlivePost(int ent, 
	int attacker, 
	int inflictor, 
	float damage, 
	int damagetype, 
	int weapon, 
	const float damageForce[3],
	const float damagePosition[3], int damageCustom)
{
	int health = GetEntProp(ent, Prop_Data, "m_iHealth");

	Event event = CreateEvent("npc_hurt");
	if (event) 
	{
		event.SetInt( "entindex", ent );
		event.SetInt( "health", health > 0 ? health : 0 );
		event.SetInt( "damageamount", RoundToFloor(damage) );
		event.SetBool( "crit", ( damagetype & DMG_ACID ) ? true : false );

		if (attacker > 0 && attacker <= MaxClients)
		{
			event.SetInt( "attacker_player", GetClientUserId(attacker) );
			event.SetInt( "weaponid", 0 );
		}
		else 
		{
			event.SetInt( "attacker_player", 0 );
			event.SetInt( "weaponid", 0 );
		}

		event.Fire();
	}

	if (health > 0)
	{
		if (GetGameTime() >= GetEntPropFloat(ent, Prop_Data, "m_flNextPainSound"))
		{
			SetEntPropFloat(ent, Prop_Data, "m_flNextPainSound", GetGameTime() + 1.0);

			char sPainSound[PLATFORM_MAX_PATH];
			if (health > 70)
			{
				strcopy( sPainSound, sizeof(sPainSound), g_sPainSounds[GetRandomInt(0, sizeof(g_sPainSounds) - 1)] );
			}
			else 
			{
				strcopy( sPainSound, sizeof(sPainSound), g_sPainSevereSounds[GetRandomInt(0, sizeof(g_sPainSevereSounds) - 1)] );
			}

			EmitSoundToAll(sPainSound, ent, SNDCHAN_VOICE, SNDLEVEL_SCREAMING);
		}
	}
}

static void ScoutNextBot_InputExplode(int ent, int activator, int caller)
{
	PrintToChatAll("No way!");
}

static void ScoutNextBot_InputSay(int ent, int activator, int caller, const char[] szValue)
{
	PrintToChatAll("The Scout #%d: %s", ent, szValue);
}

static void ScoutNextBot_InputSayNumber(int ent, int activator, int caller, int value)
{
	PrintToChatAll("The Scout #%d: Number %d!", ent, value);
}