
static NextBotActionFactory ActionFactory;

methodmap TestScoutBotDeathAction < NextBotAction
{
	public static void Initialize()
	{
		ActionFactory = new NextBotActionFactory("ScoutDeathAction");
		ActionFactory.BeginDataMapDesc()
			.DefineIntField("m_iDamageType")
			.DefineFloatField("m_flDieTime")
		.EndDataMapDesc();
		ActionFactory.SetCallback( NextBotActionCallbackType_OnStart, OnStart );
		ActionFactory.SetCallback( NextBotActionCallbackType_Update, Update );
	}

	property int m_iDamageType
	{
		public get()
		{
			return this.GetData("m_iDamageType");
		}

		public set(int value)
		{
			this.SetData("m_iDamageType", value);
		}
	}

	property float m_flDieTime
	{
		public get()
		{
			return this.GetDataFloat("m_flDieTime");
		}

		public set(float value)
		{
			this.SetDataFloat("m_flDieTime", value);
		}
	}

	public TestScoutBotDeathAction(int damageType)
	{
		TestScoutBotDeathAction action = view_as<TestScoutBotDeathAction>(ActionFactory.Create());

		action.m_iDamageType = damageType;

		return action;
	}
}

static int OnStart(TestScoutBotDeathAction action, TestScoutBot actor, NextBotAction prevAction)
{
	EmitSoundToAll("vo/scout_paincrticialdeath01.mp3", actor.index, SNDCHAN_VOICE, SNDLEVEL_SCREAMING);

	int sequence = -1;

	if ((actor.GetFlags() & FL_ONGROUND))
	{
		if (action.m_iDamageType & DMG_PLASMA)
		{
			sequence = actor.LookupSequence("primary_death_burning");
		}
	}
	
	if (sequence == -1)
	{
		actor.AcceptInput("BecomeRagdoll");
		return action.Done();
	}

	action.m_flDieTime = GetGameTime() + actor.SequenceDuration(sequence);

	// Play animation
	actor.ResetSequence(sequence);
	actor.SetPropFloat(Prop_Data, "m_flCycle", 0.0);

	return action.Continue();
}

static int Update(TestScoutBotDeathAction action, TestScoutBot actor, float interval)
{
	if (GetGameTime() >= action.m_flDieTime)
	{
		actor.AcceptInput("BecomeRagdoll");
		return action.Done();
	}

	return action.Continue();
}