
static NextBotActionFactory ActionFactory;

static char g_sStartSounds[][] = {
	"vo/taunts/scout/scout_taunt_flip_int_03.mp3",
	"vo/taunts/scout/scout_taunt_flip_int_06.mp3",
	"vo/taunts/scout/scout_taunt_flip_int_07.mp3",
};

methodmap TestScoutBotBaitAction < NextBotAction
{
	public static void Initialize()
	{
		ActionFactory = new NextBotActionFactory("ScoutBaitAction");
		ActionFactory.BeginDataMapDesc()
			.DefineFloatField("m_flNextSoundTime")
			.DefineFloatField("m_flFinishTime")
			.EndDataMapDesc();
		ActionFactory.SetCallback(NextBotActionCallbackType_OnStart, OnStart);
		ActionFactory.SetCallback(NextBotActionCallbackType_Update, Update);
		ActionFactory.SetCallback(NextBotActionCallbackType_OnEnd, OnEnd);
		ActionFactory.SetCallback(NextBotActionCallbackType_OnSuspend, OnSuspend);
		ActionFactory.SetEventCallback(EventResponderType_OnInjured, OnInjured);
	}

	public TestScoutBotBaitAction()
	{
		return view_as<TestScoutBotBaitAction>(ActionFactory.Create());
	}

	property float m_flNextSoundTime
	{
		public get()
		{
			return this.GetDataFloat("m_flNextSoundTime");
		}

		public set(float value)
		{
			this.SetDataFloat("m_flNextSoundTime", value);
		}
	}
	
	property float m_flFinishTime
	{
		public get()
		{
			return this.GetDataFloat("m_flFinishTime");
		}

		public set(float value)
		{
			this.SetDataFloat("m_flFinishTime", value);
		}
	}
}

static int OnStart(TestScoutBotBaitAction action, TestScoutBot actor, NextBotAction prevAction)
{
	for (int i = 0; i < sizeof(g_sStartSounds); i++)
	{
		PrecacheSound(g_sStartSounds[i]);
	}

	int sequence = actor.LookupSequence("taunt_flip_start");
	if (sequence == -1)
	{
		return action.Done();
	}

	actor.ResetSequence(sequence);
	actor.SetPropFloat(Prop_Data, "m_flCycle", 0.0);
	actor.SetProp(Prop_Data, "m_bSequenceLoops", false);

	action.m_flNextSoundTime = GetGameTime();
	action.m_flFinishTime = GetGameTime() + 15.0;

	return action.Continue();
}

static void MakeSound(TestScoutBotBaitAction action)
{
	int actor = action.Actor;
	int randomSound = GetRandomInt(0, sizeof(g_sStartSounds) - 1);

	EmitSoundToAll(g_sStartSounds[randomSound], actor, SNDCHAN_VOICE, SNDLEVEL_SCREAMING);
}

static int Update(TestScoutBotBaitAction action, TestScoutBot actor, float interval)
{
	float finishTime = action.GetDataFloat("m_flFinishTime");
	if (GetGameTime() >= finishTime)
	{
		return action.Done("Bored of waiting");
	}

	CBaseEntity target = CBaseEntity(actor.GetPropEnt(Prop_Data, "m_Target"));
	if (!target.IsValid())
	{
		return action.Done("No target to bait!");
	}

	float vecPos[3]; float vecTargetPos[3];
	actor.GetAbsOrigin(vecPos);
	target.GetAbsOrigin(vecTargetPos);

	float dist = GetVectorDistance(vecPos, vecTargetPos);
	if (dist > 450.0)
	{
		return action.Done("Target is too far away");
	}

	if (dist < 128.0)
	{
		if (target.index > 0 && target.index <= MaxClients && (TF2_IsPlayerInCondition(target.index, TFCond_Taunting)))
		{
			return action.ChangeTo(TestScoutBotLaughAction(), "I baited the target! Haha!");
		}
	}

	if (GetGameTime() >= action.m_flNextSoundTime)
	{
		action.m_flNextSoundTime = GetGameTime() + 4.0;
		MakeSound(action);
	}

	float cycle = actor.GetPropFloat(Prop_Send, "m_flCycle");
	if ((cycle + interval) >= 1.0)
	{
		actor.SetPropFloat(Prop_Send, "m_flCycle", 0.25);
	}

	return action.Continue();
}

static void OnEnd(TestScoutBotBaitAction action, TestScoutBot actor, NextBotAction nextAction)
{
}

static int OnSuspend(TestScoutBotBaitAction action, TestScoutBot actor, NextBotAction interruptingAction)
{
	// This is supposed to be a short action and we don't need to go back to it when we're done.
	return action.Done();
}

static int OnInjured(TestScoutBotBaitAction action, 
	TestScoutBot actor, 
	CBaseEntity attacker, 
	CBaseEntity inflictor, 
	float damage, 
	int damagetype, 
	CBaseEntity weapon, 
	const float damageForce[3],
	const float damagePosition[3], int damageCustom )
{
	return action.TryDone();
}