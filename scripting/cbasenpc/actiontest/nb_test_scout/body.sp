
methodmap ScoutBody < IBody
{
	public void UpdateAnimation()
	{
		INextBot bot = this.GetBot();
		int ent = bot.GetEntity();
		CBaseNPC npc = TheNPCs.FindNPCByEntIndex(ent);
		CBaseCombatCharacter pCC = CBaseCombatCharacter(ent);
		NextBotGroundLocomotion loco = npc.GetLocomotion();

		int m_moveXPoseParameter = GetEntProp(ent, Prop_Data, "m_moveXPoseParameter");
		if ( m_moveXPoseParameter < 0 )
		{
			m_moveXPoseParameter = pCC.LookupPoseParameter( "move_x" );
			SetEntProp(ent, Prop_Data, "m_moveXPoseParameter", m_moveXPoseParameter);
		}

		int m_moveYPoseParameter = GetEntProp(ent, Prop_Data, "m_moveYPoseParameter");
		if ( m_moveYPoseParameter < 0 )
		{
			m_moveYPoseParameter = pCC.LookupPoseParameter( "move_y" );
			SetEntProp(ent, Prop_Data, "m_moveYPoseParameter", m_moveYPoseParameter);
		}

		int m_idleSequence = GetEntProp(ent, Prop_Data, "m_idleSequence");
		if (m_idleSequence < 0)
		{
			m_idleSequence = pCC.SelectWeightedSequence(ACT_MP_STAND_MELEE);
			SetEntProp(ent, Prop_Data, "m_idleSequence", m_idleSequence);
		}

		int m_runSequence = GetEntProp(ent, Prop_Data, "m_runSequence");
		if (m_runSequence < 0)
		{
			m_runSequence = pCC.SelectWeightedSequence(ACT_MP_RUN_MELEE);
			SetEntProp(ent, Prop_Data, "m_runSequence", m_runSequence);
		}

		int m_airSequence = GetEntProp(ent, Prop_Data, "m_airSequence");
		if (m_airSequence < 0)
		{
			m_airSequence = pCC.SelectWeightedSequence(ACT_MP_JUMP_FLOAT_MELEE);
			SetEntProp(ent, Prop_Data, "m_airSequence", m_airSequence);
		}

		float speed = loco.GetGroundSpeed();

		if ( speed < 0.01 )
		{
			// stopped
			if ( m_moveXPoseParameter >= 0 )
			{
				pCC.SetPoseParameter( m_moveXPoseParameter, 0.0 );
			}

			if ( m_moveYPoseParameter >= 0 )
			{
				pCC.SetPoseParameter( m_moveYPoseParameter, 0.0 );
			}
		}
		else
		{
			float fwd[3], right[3], up[3];
			pCC.GetVectors( fwd, right, up );

			float motionVector[3];
			loco.GetGroundMotionVector(motionVector);

			// move_x == 1.0 at full forward motion and -1.0 in full reverse
			if ( m_moveXPoseParameter >= 0 )
			{
				float forwardVel = GetVectorDotProduct( motionVector, fwd );

				pCC.SetPoseParameter( m_moveXPoseParameter, forwardVel );
			}

			if ( m_moveYPoseParameter >= 0 )
			{
				float sideVel = GetVectorDotProduct( motionVector, right );

				pCC.SetPoseParameter( m_moveYPoseParameter, sideVel );
			}
		}
	}
}
