
#include "nb_test_scout/body.sp"
#include "nb_test_scout/behavior.sp"

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

methodmap TestScoutBot < CBaseCombatCharacter
{
	public TestScoutBot(int entIndex)
	{
		return view_as<TestScoutBot>(entIndex);
	}

	public static void Initialize()
	{
		InitBehavior();

		EntityFactory = new CEntityFactory("nb_test_scout", OnCreate, OnRemove);
		EntityFactory.DeriveFromNPC();
		EntityFactory.SetInitialActionFactory(TestScoutBotMainAction.GetFactory());
		EntityFactory.BeginDataMapDesc()
			.DefineIntField("m_moveXPoseParameter")
			.DefineIntField("m_moveYPoseParameter")
			.DefineIntField("m_idleSequence")
			.DefineIntField("m_runSequence")
			.DefineIntField("m_airSequence")
			.DefineEntityField("m_Target")
			.DefineFloatField("m_flNextPainSound")
			.DefineInputFunc("Explode", InputFuncValueType_Void, InputExplode)
			.DefineInputFunc("Say", InputFuncValueType_String, InputSay)
			.DefineInputFunc("SayNumber", InputFuncValueType_Integer, InputSayNumber)
		.EndDataMapDesc();

		EntityFactory.Install();
	}

	property CBaseEntity m_Target
	{
		public get()
		{
			return CBaseEntity(this.GetPropEnt(Prop_Data, "m_Target"));
		}

		public set(CBaseEntity value)
		{
			this.SetPropEnt(Prop_Data, "m_Target", value.index);
		}
	}
}

static void InitBehavior()
{
	TestScoutBotMainAction.Initialize();
	TestScoutBotDeathAction.Initialize();
	TestScoutBotLaughAction.Initialize();
	TestScoutBotBaitAction.Initialize();
}

static void Precache()
{
	for (int i = 0; i < sizeof(g_sPainSounds); i++)
	{
		PrecacheSound(g_sPainSounds[i]);
	}

	for (int i = 0; i < sizeof(g_sPainSevereSounds); i++)
	{
		PrecacheSound(g_sPainSevereSounds[i]);
	}

	PrecacheSound("vo/scout_laughlong02.mp3");
	PrecacheSound("vo/scout_paincrticialdeath01.mp3");
}

static void OnCreate(TestScoutBot ent)
{
	Precache();

	ent.m_Target = CBaseEntity(-1);

	ent.SetModel("models/player/scout.mdl");

	ent.SetProp(Prop_Data, "m_iHealth", 125);
	ent.SetProp(Prop_Data, "m_moveXPoseParameter", -1);
	ent.SetProp(Prop_Data, "m_moveYPoseParameter", -1);
	ent.SetProp(Prop_Data, "m_idleSequence", -1);
	ent.SetProp(Prop_Data, "m_runSequence", -1);
	ent.SetProp(Prop_Data, "m_airSequence", -1);
	ent.SetPropFloat(Prop_Data, "m_flNextPainSound", GetGameTime());

	CBaseNPC npc = TheNPCs.FindNPCByEntIndex(ent.index);

	npc.flStepSize = 18.0;
	npc.flGravity = 800.0;
	npc.flAcceleration = 2000.0;
	npc.flJumpHeight = 85.0;
	npc.flWalkSpeed = 400.0;
	npc.flRunSpeed = 400.0;
	npc.flDeathDropHeight = 2000.0;
	npc.flMaxYawRate = 250.0;

	SDKHook(ent.index, SDKHook_SpawnPost, SpawnPost);
	SDKHook(ent.index, SDKHook_Think, Think);
	SDKHook(ent.index, SDKHook_OnTakeDamageAlivePost, OnTakeDamageAlivePost);

	CBaseNPC_Locomotion loco = npc.GetLocomotion();
	loco.SetCallback(LocomotionCallback_ClimbUpToLedge, LocomotionClimbUpToLedge);
	loco.SetCallback(LocomotionCallback_ShouldCollideWith, LocomotionShouldCollideWith);
	loco.SetCallback(LocomotionCallback_IsEntityTraversable, LocomotionIsEntityTraversable);
}

static bool LocomotionClimbUpToLedge(CBaseNPC_Locomotion loco, const float goal[3], const float fwd[3], int entity)
{
	float feet[3];
	loco.GetFeet(feet);

	if (GetVectorDistance(feet, goal) > loco.GetDesiredSpeed())
	{
		return false;
	}

	return loco.CallBaseFunction(goal, fwd, entity);
}

static bool LocomotionShouldCollideWith(CBaseNPC_Locomotion loco, CBaseEntity other)
{
	if (other.index > 0 && other.index <= MaxClients)
	{
		return true;
	}

	if (CEntityFactory.GetFactoryOfEntity(other.index) == EntityFactory)
	{
		return true;
	}

	return loco.CallBaseFunction(other);
}

static bool LocomotionIsEntityTraversable(CBaseNPC_Locomotion loco, CBaseEntity obstacle, TraverseWhenType when)
{
	if (CEntityFactory.GetFactoryOfEntity(obstacle.index) == EntityFactory)
	{
		return false;
	}

	return loco.CallBaseFunction(obstacle, when);
}

static void OnRemove(TestScoutBot ent)
{
}

static void SpawnPost(int entIndex)
{
	TestScoutBot ent = TestScoutBot(entIndex);

	ent.SetProp(Prop_Data, "m_moveXPoseParameter", ent.LookupPoseParameter("move_x"));
	ent.SetProp(Prop_Data, "m_moveYPoseParameter", ent.LookupPoseParameter("move_y"));
	ent.SetProp(Prop_Data, "m_idleSequence", ent.SelectWeightedSequence(ACT_MP_STAND_PRIMARY));
	ent.SetProp(Prop_Data, "m_runSequence", ent.SelectWeightedSequence(ACT_MP_RUN_PRIMARY));
	ent.SetProp(Prop_Data, "m_airSequence", ent.SelectWeightedSequence(ACT_MP_JUMP_FLOAT_PRIMARY));
}

static void UpdateTarget(TestScoutBot ent)
{
	float pos[3];
	ent.GetAbsOrigin(pos);

	float maxDist = 1200.0;
	int target = INVALID_ENT_REFERENCE;

	for (int i = 1; i <= MaxClients; i++)
	{
		if (IsClientInGame(i) && IsPlayerAlive(i))
		{
			float otherPos[3];
			GetClientAbsOrigin(i, otherPos);

			float dist = GetVectorDistance(otherPos, pos);
			if (dist < maxDist)
			{
				maxDist = dist;
				target = EntIndexToEntRef(i);
			}
		}
	}

	ent.SetPropEnt(Prop_Data, "m_Target", target);
}

static void Think(int entIndex) 
{
	TestScoutBot ent = TestScoutBot(entIndex);
	INextBot bot = ent.MyNextBotPointer();

	UpdateTarget(ent);

	view_as<ScoutBody>(bot.GetBodyInterface()).UpdateAnimation();
}

static void OnTakeDamageAlivePost(int entIndex, 
	int attacker, 
	int inflictor, 
	float damage, 
	int damagetype, 
	int weapon, 
	const float damageForce[3],
	const float damagePosition[3], int damageCustom)
{
	TestScoutBot ent = TestScoutBot(entIndex);
	int health = ent.GetProp(Prop_Data, "m_iHealth");

	Event event = CreateEvent("npc_hurt");
	if (event) 
	{
		event.SetInt("entindex", entIndex);
		event.SetInt("health", health > 0 ? health : 0);
		event.SetInt("damageamount", RoundToFloor(damage));
		event.SetBool("crit", ( damagetype & DMG_ACID ) ? true : false);

		if (attacker > 0 && attacker <= MaxClients)
		{
			event.SetInt("attacker_player", GetClientUserId(attacker));
			event.SetInt("weaponid", 0);
		}
		else 
		{
			event.SetInt("attacker_player", 0);
			event.SetInt("weaponid", 0);
		}

		event.Fire();
	}

	if (health > 0)
	{
		if (GetGameTime() >= ent.GetPropFloat(Prop_Data, "m_flNextPainSound"))
		{
			ent.SetPropFloat(Prop_Data, "m_flNextPainSound", GetGameTime() + 1.0);

			char painSound[PLATFORM_MAX_PATH];
			if (health > 70)
			{
				strcopy(painSound, sizeof(painSound), g_sPainSounds[GetRandomInt(0, sizeof(g_sPainSounds) - 1)]);
			}
			else 
			{
				strcopy(painSound, sizeof(painSound), g_sPainSevereSounds[GetRandomInt(0, sizeof(g_sPainSevereSounds) - 1)]);
			}

			EmitSoundToAll(painSound, entIndex, SNDCHAN_VOICE, SNDLEVEL_SCREAMING);
		}
	}
}

static void InputExplode(TestScoutBot ent, CBaseEntity activator, CBaseEntity caller)
{
	PrintToChatAll("No way!");
}

static void InputSay(TestScoutBot ent, CBaseEntity activator, CBaseEntity caller, const char[] szValue)
{
	PrintToChatAll("The Scout #%d: %s", ent, szValue);
}

static void InputSayNumber(TestScoutBot ent, CBaseEntity activator, CBaseEntity caller, int value)
{
	PrintToChatAll("The Scout #%d: Number %d!", ent, value);
}