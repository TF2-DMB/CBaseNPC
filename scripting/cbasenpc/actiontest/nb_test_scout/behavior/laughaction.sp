
static NextBotActionFactory ActionFactory;

void ScoutLaughAction_Init()
{
	ActionFactory = new NextBotActionFactory("ScoutLaughAction");
	ActionFactory.BeginDataMapDesc()
		.DefineIntField("m_iLayerSequence")
		.DefineFloatField("m_flFinishTime")
	.EndDataMapDesc();
	ActionFactory.SetCallback( NextBotActionCallbackType_OnStart, ScoutLaughAction_OnStart );
	ActionFactory.SetCallback( NextBotActionCallbackType_Update, ScoutLaughAction_Update );
	ActionFactory.SetCallback( NextBotActionCallbackType_OnEnd, ScoutLaughAction_OnEnd );
	ActionFactory.SetCallback( NextBotActionCallbackType_OnSuspend, ScoutLaughAction_OnSuspend );
	ActionFactory.SetEventCallback( EventResponderType_OnInjured, ScoutLaughAction_OnInjured );
}

NextBotAction ScoutLaughAction_Create()
{
	return ActionFactory.Create();
}

static int ScoutLaughAction_OnStart( NextBotAction action, int actor, NextBotAction prevAction )
{
	CBaseCombatCharacter anim = CBaseCombatCharacter(actor);
	int sequence = anim.LookupSequence( "taunt_laugh" );

	if (sequence == -1)
		return action.Done();
	
	float duration = anim.SequenceDuration(sequence);
	action.SetDataFloat("m_flFinishTime", GetGameTime() + duration);

	int layer = anim.AddLayeredSequence(sequence, 1);
	if (!anim.IsValidLayer(layer))
	{
		action.SetData("m_iLayerSequence", -1);
		return action.Done();
	}

	EmitSoundToAll("vo/scout_laughlong02.mp3", actor, SNDCHAN_VOICE, SNDLEVEL_SCREAMING);

	anim.SetLayerCycle(layer, 0.0);
	anim.SetLayerPlaybackRate(layer, 1.0);

	action.SetData("m_iLayerSequence", layer);
	
	return action.Continue();
}

static int ScoutLaughAction_Update( NextBotAction action, int actor, float interval )
{
	float flFinishTime = action.GetDataFloat("m_flFinishTime");
	if (GetGameTime() >= flFinishTime)
	{
		return action.Done();
	}

	return action.Continue();
}

static void ScoutLaughAction_OnEnd( NextBotAction action, int actor, NextBotAction nextAction )
{
	CBaseCombatCharacter anim = CBaseCombatCharacter(actor);

	int layer = action.GetData("m_iLayerSequence");
	if (layer != -1)
	{
		anim.RemoveLayer(layer);
	}
}

static int ScoutLaughAction_OnSuspend( NextBotAction action, int actor, NextBotAction interruptingAction )
{
	// This is supposed to be a short action and we don't need to go back to it when we're done.
	return action.Done();
}

static int ScoutLaughAction_OnInjured(NextBotAction action, 
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