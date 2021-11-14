
static NextBotActionFactory ActionFactory;

NextBotActionFactory ScoutMainAction_GetFactory()
{
	return ActionFactory;
}

void ScoutMainAction_Init()
{
	ActionFactory = new NextBotActionFactory("ScoutMainAction");
	ActionFactory.BeginDataMapDesc()
		.DefineIntField("m_PathFollower")
	.EndDataMapDesc();
	ActionFactory.SetCallback( NextBotActionCallbackType_OnStart, ScoutMainAction_OnStart );
	ActionFactory.SetCallback( NextBotActionCallbackType_Update, ScoutMainAction_Update );
	ActionFactory.SetCallback( NextBotActionCallbackType_OnEnd, ScoutMainAction_OnEnd );
	ActionFactory.SetEventCallback( EventResponderType_OnInjured, ScoutMainAction_OnInjured );
	ActionFactory.SetEventCallback( EventResponderType_OnKilled, ScoutMainAction_OnKilled );
}

static int ScoutMainAction_OnStart( NextBotAction action, int actor, NextBotAction prevAction )
{
	action.SetData( "m_PathFollower", ChasePath(LEAD_SUBJECT, _, Path_FilterIgnoreActors, Path_FilterOnlyActors) );
}

static int ScoutMainAction_Update( NextBotAction action, int actor, float interval )
{
	float vecPos[3];
	GetEntPropVector(actor, Prop_Data, "m_vecAbsOrigin", vecPos);

	CBaseNPC pNPC = TheNPCs.FindNPCByEntIndex(actor);
	NextBotGroundLocomotion loco = pNPC.GetLocomotion();
	INextBot bot = pNPC.GetBot();
	CBaseCombatCharacter pCC = CBaseCombatCharacter(actor);

	bool onGround = !!(GetEntityFlags(actor) & FL_ONGROUND);

	int target = GetEntPropEnt(actor, Prop_Data, "m_Target");
	if (IsValidEntity(target))
	{
		float vecTargetPos[3];
		GetEntPropVector(target, Prop_Data, "m_vecAbsOrigin", vecTargetPos);

		float dist = GetVectorDistance(vecTargetPos, vecPos);

		loco.FaceTowards(vecTargetPos);

		if (dist > 250.0)
		{
			ChasePath path = action.GetData("m_PathFollower");
			if (path)
			{
				path.Update(bot, target);
				loco.Run();
			}
		}
		else if (onGround)
		{
			return action.SuspendFor( ScoutBaitAction_Create() );
		}
	}

	float speed = loco.GetGroundSpeed();

	int sequence = GetEntProp(actor, Prop_Send, "m_nSequence");

	if (speed < 0.01)
	{
		int idleSequence = GetEntProp(actor, Prop_Data, "m_idleSequence");
		if (idleSequence != -1 && sequence != idleSequence)
		{
			pCC.ResetSequence(idleSequence);
		}
	}
	else
	{
		int runSequence = GetEntProp(actor, Prop_Data, "m_runSequence");
		int airSequence = GetEntProp(actor, Prop_Data, "m_airSequence");

		if (!onGround)
		{
			if (airSequence != -1 && sequence != airSequence)
				pCC.ResetSequence(airSequence);
		}
		else
		{			
			if (runSequence != -1 && sequence != runSequence)
				pCC.ResetSequence(runSequence);
		}
	}

	return action.Continue();
}

static void ScoutMainAction_OnEnd(NextBotAction action, int actor, NextBotAction nextAction)
{
	ChasePath path = action.GetData("m_PathFollower");
	if (path)
	{
		path.Destroy();
	}
}

static int ScoutMainAction_OnInjured(NextBotAction action, 
	int actor, 
	int attacker, 
	int inflictor, 
	float damage, 
	int damagetype, 
	int weapon, 
	const float damageForce[3],
	const float damagePosition[3], int damageCustom )
{
	return action.TryContinue();
}

static int ScoutMainAction_OnKilled(NextBotAction action, 
	int actor, 
	int attacker, 
	int inflictor, 
	float damage, 
	int damagetype, 
	int weapon, 
	const float damageForce[3],
	const float damagePosition[3], int damageCustom )
{
	NextBotAction deathAction = ScoutDeathAction_Create();
	deathAction.SetData("m_iDamageType", damagetype);

	return action.TryChangeTo( deathAction, RESULT_CRITICAL );
}