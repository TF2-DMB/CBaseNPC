
static NextBotActionFactory ActionFactory;

methodmap TestScoutBotLaughAction < NextBotAction
{
	public static void Initialize()
	{
		ActionFactory = new NextBotActionFactory("ScoutLaughAction");
		ActionFactory.BeginDataMapDesc()
			.DefineIntField("m_iLayerSequence")
			.DefineFloatField("m_flFinishTime")
			.DefineEntityField("m_hLaughAtEntity")
		.EndDataMapDesc();
		ActionFactory.SetCallback( NextBotActionCallbackType_OnStart, OnStart );
		ActionFactory.SetCallback( NextBotActionCallbackType_Update, Update );
		ActionFactory.SetCallback( NextBotActionCallbackType_OnEnd, OnEnd );
		ActionFactory.SetCallback( NextBotActionCallbackType_OnSuspend, OnSuspend );
		ActionFactory.SetEventCallback( EventResponderType_OnInjured, OnInjured );
	}

	property int m_iLayerSequence
	{
		public get()
		{
			return this.GetData("m_iLayerSequence");
		}

		public set(int value)
		{
			this.SetData("m_iLayerSequence", value);
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

	property CBaseEntity m_hLaughAtEntity
	{
		public get()
		{
			return CBaseEntity(this.GetDataEnt("m_hLaughAtEntity"));
		}

		public set(CBaseEntity value)
		{
			this.SetDataEnt("m_hLaughAtEntity", value.index);
		}
	}

	public TestScoutBotLaughAction()
	{
		TestScoutBotLaughAction action = view_as<TestScoutBotLaughAction>(ActionFactory.Create());

		action.m_iLayerSequence = -1;

		return action;
	}
}

static int OnStart(TestScoutBotLaughAction action, TestScoutBot actor, NextBotAction prevAction)
{
	int sequence = actor.LookupSequence( "taunt_laugh" );

	if (sequence == -1)
	{
		return action.Done();
	}

	float duration = actor.SequenceDuration(sequence);
	action.m_flFinishTime = GetGameTime() + duration;

	int layer = actor.AddLayeredSequence(sequence, 1);
	if (!actor.IsValidLayer(layer))
	{
		return action.Done();
	}

	CBaseEntity target = actor.m_Target;
	if (target.IsValid())
	{
		action.m_hLaughAtEntity = target;
	}

	EmitSoundToAll("vo/scout_laughlong02.mp3", actor.index, SNDCHAN_VOICE, SNDLEVEL_SCREAMING);

	actor.SetLayerCycle(layer, 0.0);
	actor.SetLayerPlaybackRate(layer, 1.0);

	action.m_iLayerSequence = layer;

	return action.Continue();
}

static int Update(TestScoutBotLaughAction action, TestScoutBot actor, float interval)
{
	if (GetGameTime() >= action.m_flFinishTime)
	{
		return action.Done();
	}

	CBaseEntity laughAtEntity = action.m_hLaughAtEntity;
	if (laughAtEntity.IsValid())
	{
		INextBot bot = actor.MyNextBotPointer();

		float laughPos[3];
		laughAtEntity.GetAbsOrigin(laughPos);

		bot.GetLocomotionInterface().FaceTowards(laughPos);
	}

	return action.Continue();
}

static void OnEnd(TestScoutBotLaughAction action, TestScoutBot actor, NextBotAction nextAction)
{
	int layer = action.m_iLayerSequence;
	if (layer != -1)
	{
		actor.RemoveLayer(layer);
	}
}

static int OnSuspend(TestScoutBotLaughAction action, TestScoutBot actor, NextBotAction interruptingAction)
{
	// This is supposed to be a short action and we don't need to go back to it when we're done.
	return action.Done();
}

static int OnInjured(TestScoutBotLaughAction action, 
	TestScoutBot actor, 
	CBaseEntity attacker, 
	CBaseEntity inflictor, 
	float damage, 
	int damagetype, 
	CBaseEntity weapon, 
	const float damageForce[3],
	const float damagePosition[3], int damageCustom)
{
	return action.TryContinue();
}