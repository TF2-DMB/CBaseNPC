
static NextBotActionFactory ActionFactory;

methodmap TestScoutBotMainAction < NextBotAction
{
	public static void Initialize()
	{
		ActionFactory = new NextBotActionFactory("ScoutMainAction");
		ActionFactory.BeginDataMapDesc()
			.DefineIntField("m_PathFollower")
			.EndDataMapDesc();
		ActionFactory.SetCallback(NextBotActionCallbackType_OnStart, OnStart);
		ActionFactory.SetCallback(NextBotActionCallbackType_Update, Update);
		ActionFactory.SetCallback(NextBotActionCallbackType_OnEnd, OnEnd);
		ActionFactory.SetEventCallback(EventResponderType_OnInjured, OnInjured);
		ActionFactory.SetEventCallback(EventResponderType_OnKilled, OnKilled);
	}

	public static NextBotActionFactory GetFactory()
	{
		return ActionFactory;
	}

	public TestScoutBotMainAction()
	{
		return view_as<TestScoutBotMainAction>(ActionFactory.Create());
	}

	property ChasePath m_PathFollower
	{
		public get()
		{
			return view_as<ChasePath>(this.GetData("m_PathFollower"));
		}

		public set(ChasePath value)
		{
			this.SetData("m_PathFollower", value);
		}
	}
}

static void OnStart(TestScoutBotMainAction action, TestScoutBot actor, NextBotAction prevAction)
{
	action.m_PathFollower = ChasePath(LEAD_SUBJECT, _, Path_FilterIgnoreActors, Path_FilterOnlyActors);
}

static int Update(TestScoutBotMainAction action, TestScoutBot actor, float interval)
{
	float vecPos[3];
	actor.GetAbsOrigin(vecPos);

	CBaseNPC pNPC = TheNPCs.FindNPCByEntIndex(actor.index);
	NextBotGroundLocomotion loco = pNPC.GetLocomotion();
	INextBot bot = pNPC.GetBot();

	bool onGround = !!(actor.GetFlags() & FL_ONGROUND);

	CBaseEntity target = actor.m_Target;
	if (target.IsValid())
	{
		float vecTargetPos[3];
		target.GetAbsOrigin(vecTargetPos);

		float dist = GetVectorDistance(vecTargetPos, vecPos);

		loco.FaceTowards(vecTargetPos);

		if (dist > 250.0)
		{
			ChasePath path = action.GetData("m_PathFollower");
			if (path)
			{
				loco.Run();
				path.Update(bot, target.index);
			}
		}
		else if (onGround)
		{
			return action.SuspendFor(TestScoutBotBaitAction());
		}
	}

	float speed = loco.GetGroundSpeed();

	int sequence = actor.GetProp(Prop_Send, "m_nSequence");

	if (speed < 0.01)
	{
		int idleSequence = actor.GetProp(Prop_Data, "m_idleSequence");
		if (idleSequence != -1 && sequence != idleSequence)
		{
			actor.ResetSequence(idleSequence);
		}
	}
	else
	{
		int runSequence = actor.GetProp(Prop_Data, "m_runSequence");
		int airSequence = actor.GetProp(Prop_Data, "m_airSequence");

		if (!onGround)
		{
			if (airSequence != -1 && sequence != airSequence)
			{
				actor.ResetSequence(airSequence);
			}
		}
		else
		{			
			if (runSequence != -1 && sequence != runSequence)
			{
				actor.ResetSequence(runSequence);
			}
		}
	}

	return action.Continue();
}

static void OnEnd(TestScoutBotMainAction action, TestScoutBot actor, NextBotAction nextAction)
{
	ChasePath path = action.m_PathFollower;
	if (path)
	{
		actor.MyNextBotPointer().NotifyPathDestruction(path);
		path.Destroy();
	}
}

static int OnInjured(TestScoutBotMainAction action, 
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

static int OnKilled(TestScoutBotMainAction action, 
	TestScoutBot actor, 
	CBaseEntity attacker, 
	CBaseEntity inflictor, 
	float damage, 
	int damagetype, 
	CBaseEntity weapon, 
	const float damageForce[3],
	const float damagePosition[3], int damageCustom)
{
	return action.TryChangeTo(TestScoutBotDeathAction(damagetype), RESULT_CRITICAL);
}