
static NextBotActionFactory ActionFactory;

static char g_sStartSounds[][] = {
	"vo/taunts/scout/scout_taunt_flip_int_03.mp3",
	"vo/taunts/scout/scout_taunt_flip_int_06.mp3",
	"vo/taunts/scout/scout_taunt_flip_int_07.mp3",
};

void ScoutBaitAction_Init()
{
	ActionFactory = new NextBotActionFactory("ScoutBaitAction");
	ActionFactory.BeginDataMapDesc()
		.DefineFloatField("m_flNextSoundTime")
		.DefineFloatField("m_flFinishTime")
	.EndDataMapDesc();
	ActionFactory.SetCallback( NextBotActionCallbackType_OnStart, ScoutBaitAction_OnStart );
	ActionFactory.SetCallback( NextBotActionCallbackType_Update, ScoutBaitAction_Update );
	ActionFactory.SetCallback( NextBotActionCallbackType_OnEnd, ScoutBaitAction_OnEnd );
	ActionFactory.SetCallback( NextBotActionCallbackType_OnSuspend, ScoutBaitAction_OnSuspend );
	ActionFactory.SetEventCallback( EventResponderType_OnInjured, ScoutBaitAction_OnInjured );
}

NextBotAction ScoutBaitAction_Create()
{
	return ActionFactory.Create();
}

static int ScoutBaitAction_OnStart( NextBotAction action, int actor, NextBotAction prevAction )
{
	for (int i = 0; i < sizeof(g_sStartSounds); i++)
	{
		PrecacheSound( g_sStartSounds[i] );
	}

	CBaseCombatCharacter anim = CBaseCombatCharacter(actor);
	int sequence = anim.LookupSequence( "taunt_flip_start" );

	if (sequence == -1)
		return action.Done();
	
	anim.ResetSequence(sequence);
	SetEntPropFloat( actor, Prop_Data, "m_flCycle", 0.0 );
	SetEntProp( actor, Prop_Data, "m_bSequenceLoops", false );

	action.SetDataFloat("m_flNextSoundTime", GetGameTime());
	action.SetDataFloat("m_flFinishTime", GetGameTime() + 15.0);

	return action.Continue();
}

static int ScoutBaitAction_MakeSound( NextBotAction action )
{
	int actor = action.Actor;
	int randomSound = GetRandomInt(0, sizeof(g_sStartSounds) - 1);

	EmitSoundToAll(g_sStartSounds[randomSound], actor, SNDCHAN_VOICE, SNDLEVEL_SCREAMING);
}

static int ScoutBaitAction_Update( NextBotAction action, int actor, float interval )
{
	float flFinishTime = action.GetDataFloat("m_flFinishTime");
	if (GetGameTime() >= flFinishTime)
	{
		return action.Done( "Bored of waiting" );
	}

	int target = GetEntPropEnt(actor, Prop_Data, "m_Target");
	if (!IsValidEntity(target))
	{
		return action.Done( "No target to bait!" );
	}

	float vecPos[3]; float vecTargetPos[3];
	GetEntPropVector( actor, Prop_Data, "m_vecAbsOrigin", vecPos );
	GetEntPropVector( target, Prop_Data, "m_vecAbsOrigin", vecTargetPos );

	float dist = GetVectorDistance(vecPos, vecTargetPos);
	if (dist > 450.0)
	{
		return action.Done( "Target is too far away" );
	}

	if ( dist < 128.0 )
	{
		if (target > 0 && target <= MaxClients && ( TF2_IsPlayerInCondition( target, TFCond_Taunting ) ))
		{
			return action.ChangeTo( ScoutLaughAction_Create(), "I baited the target! Haha!" );
		}
	}

	if (GetGameTime() >= action.GetDataFloat("m_flNextSoundTime"))
	{
		action.SetDataFloat("m_flNextSoundTime", GetGameTime() + 4.0);
		ScoutBaitAction_MakeSound( action );
	}

	float flCycle = GetEntPropFloat(actor, Prop_Send, "m_flCycle");
	if ((flCycle + interval) >= 1.0)
	{
		SetEntPropFloat(actor, Prop_Send, "m_flCycle", 0.25);
	}

	return action.Continue();
}

static void ScoutBaitAction_OnEnd( NextBotAction action, int actor, NextBotAction nextAction )
{
}

static int ScoutBaitAction_OnSuspend( NextBotAction action, int actor, NextBotAction interruptingAction )
{
	// This is supposed to be a short action and we don't need to go back to it when we're done.
	return action.Done();
}

static int ScoutBaitAction_OnInjured(NextBotAction action, 
	int actor, 
	int attacker, 
	int inflictor, 
	float damage, 
	int damagetype, 
	int weapon, 
	const float damageForce[3],
	const float damagePosition[3], int damageCustom )
{
	return action.TryDone();
}