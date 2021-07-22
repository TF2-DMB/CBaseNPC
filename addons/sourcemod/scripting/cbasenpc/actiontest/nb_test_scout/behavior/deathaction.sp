
static NextBotActionFactory ActionFactory;

void ScoutDeathAction_Init()
{
	ActionFactory = new NextBotActionFactory("ScoutDeathAction");
	ActionFactory.BeginDataMapDesc()
		.DefineIntField("m_iDamageType")
		.DefineFloatField("m_flDieTime")
	.EndDataMapDesc();
	ActionFactory.SetCallback( NextBotActionCallbackType_OnStart, ScoutDeathAction_OnStart );
	ActionFactory.SetCallback( NextBotActionCallbackType_Update, ScoutDeathAction_Update );
}

NextBotAction ScoutDeathAction_Create()
{
	return ActionFactory.Create();
}

static int ScoutDeathAction_OnStart( NextBotAction action, int actor, NextBotAction prevAction )
{
	EmitSoundToAll("vo/scout_paincrticialdeath01.mp3", actor, SNDCHAN_VOICE, SNDLEVEL_SCREAMING);

	CBaseCombatCharacter anim = CBaseCombatCharacter(actor);
	int sequence = -1;

	if ((GetEntityFlags(actor) & FL_ONGROUND))
	{
		int damageType = action.GetData("m_iDamageType");
		if (damageType & DMG_PLASMA)
		{
			sequence = anim.LookupSequence("primary_death_burning");
		}
	}
	
	if (sequence == -1)
	{
		AcceptEntityInput(actor, "BecomeRagdoll");
		return action.Done();
	}

	action.SetDataFloat("m_flDieTime", GetGameTime() + anim.SequenceDuration(sequence));

	// Play animation
	anim.ResetSequence( sequence );
	SetEntPropFloat( actor, Prop_Data, "m_flCycle", 0.0 );

	return action.Continue();
}

static int ScoutDeathAction_Update( NextBotAction action, int actor, float interval )
{
	float flDieTime = action.GetDataFloat("m_flDieTime");
	if (GetGameTime() >= flDieTime)
	{
		AcceptEntityInput(actor, "BecomeRagdoll");
		return action.Done();
	}

	return action.Continue();
}