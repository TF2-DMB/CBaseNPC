#include "extension.h"
#include "CDetour/detours.h"
#include "helpers.h"
#include "sourcesdk/nav_mesh.h"
#include "natives.h"
#include <ihandleentity.h>

CGlobalVars *gpGlobals = nullptr;
IGameConfig *g_pGameConf = nullptr;
IBinTools *g_pBinTools = nullptr;
IServerGameEnts *gameents = nullptr;
IEngineTrace *enginetrace = nullptr;
IdentityType_t g_CoreIdent;
CUtlVector<PathFunctions> g_PathFunctions;
CBaseEntityList *g_pEntityList = nullptr;

ConVar NextBotPathDrawIncrement("cnb_path_draw_inc", "0", 0, "");                     
ConVar NextBotPathSegmentInfluenceRadius("cnb_path_segment_influence", "0", 0, "");

int g_iLastKnownAreaOffset = -1;
int g_iMyNextBotPointerOffset = -1;

HandleType_t g_CellArrayHandle;
HandleType_t g_KeyValueType;

CBaseNPCExt g_CBaseNPCExt;
SMEXT_LINK(&g_CBaseNPCExt);


IForward *g_pForwardSetLocalAngles = NULL;
IForward *g_pForwardUpdateLoco = NULL;

CDetour *g_pSetLocalAngles = NULL;
CDetour *g_pUpdatePosition = NULL;
CDetour *g_pUpdateGroundConstraint = NULL;

CNavArea * (CNavMesh:: *CNavMesh::func_GetNearestNavArea)(const Vector &pos, bool anyZ, float maxDist, bool checkLOS, bool checkGround, int team) = nullptr;
bool (CNavMesh:: *CNavMesh::func_GetGroundHeight)(const Vector &pos, float *height, Vector *normal) = nullptr;
bool (CTraceFilterSimpleHack:: *CTraceFilterSimpleHack::func_ShouldHitEntity)(IHandleEntity *pHandleEntity, int contentsMask) = nullptr;

DETOUR_DECL_MEMBER1(CBaseEntity_SetLocalAngles, void, QAngle&, angles)
{
	if (g_pForwardSetLocalAngles != NULL)
	{
		cell_t iEntity = gamehelpers->EntityToBCompatRef(reinterpret_cast<CBaseEntity*>(this));
		
		cell_t vector[3];
		vector[0] = sp_ftoc(angles.x);
		vector[1] = sp_ftoc(angles.y);
		vector[2] = sp_ftoc(angles.z);

		cell_t result = Pl_Continue;
		g_pForwardSetLocalAngles->PushCell(iEntity);
		g_pForwardSetLocalAngles->PushArray(vector, 3, SM_PARAM_COPYBACK);
		
		g_pForwardSetLocalAngles->Execute(&result);
		if (result == Pl_Changed)
		{
			angles.x = sp_ctof(vector[0]);
			angles.y = sp_ctof(vector[1]);
			angles.z = sp_ctof(vector[2]);
		}
	}
	DETOUR_MEMBER_CALL(CBaseEntity_SetLocalAngles)(angles);
}
/*
class GroundLocomotionCollisionTraceFilter : public CTraceFilterSimpleHack
{
public:
	GroundLocomotionCollisionTraceFilter( INextBot *me, const IHandleEntity *passentity, int collisionGroup ) : CTraceFilterSimpleHack( passentity, collisionGroup )
	{
		m_me = me;
	}

	virtual bool ShouldHitEntity( IHandleEntity *pServerEntity, int contentsMask )
	{
		if ( CTraceFilterSimpleHack::ShouldHitEntity( pServerEntity, contentsMask ) )
		{
			IServerUnknown *unKnown = (IServerUnknown *)pServerEntity;
			CBaseEntity *entity = unKnown->GetBaseEntity();

			// don't collide with ourself
			if ( entity && m_me->IsSelf( entity ) )
				return false;

			return true;
		}

		return false;
	}

	INextBot *m_me;
};

int ClipVelocity( Vector& in, Vector& normal, Vector& out, float overbounce )
{
	float	backoff;
	float	change;
	float angle;
	int		i, blocked;
	
	angle = normal[ 2 ];

	blocked = 0x00;         // Assume unblocked.
	if (angle > 0)			// If the plane that is blocking us has a positive z component, then assume it's a floor.
		blocked |= 0x01;	// 
	if (!angle)				// If the plane has no Z, it is vertical (wall/step)
		blocked |= 0x02;	// 
	

	// Determine how far along plane to slide based on incoming direction.
	backoff = DotProduct (in, normal) * overbounce;

	for (i=0 ; i<3 ; i++)
	{
		change = normal[i]*backoff;
		out[i] = in[i] - change; 
	}
	
	// iterate once to make sure we aren't still moving through the plane
	float adjust = DotProduct( out, normal );
	if( adjust < 0.0f )
	{
		out -= ( normal * adjust );
//		Msg( "Adjustment = %lf\n", adjust );
	}

	// Return blocking flags.
	return blocked;
}*/

DETOUR_DECL_MEMBER1(NextBotGroundLocomotion_UpdatePosition, void, Vector&, newPos)
{
	NextBotGroundLocomotion *mover = reinterpret_cast<NextBotGroundLocomotion*>(this);
	INextBot *bot = mover->GetBot();

	Vector vecFromPos = bot->GetPosition();
	DETOUR_MEMBER_CALL(NextBotGroundLocomotion_UpdatePosition)(newPos);
	Vector vecAdjustedPos = bot->GetPosition();

	if (g_pForwardUpdateLoco != NULL)
	{
		cell_t fromPos[3];
		fromPos[0] = sp_ftoc(vecFromPos.x);
		fromPos[1] = sp_ftoc(vecFromPos.y);
		fromPos[2] = sp_ftoc(vecFromPos.z);

		cell_t toPos[3];
		toPos[0] = sp_ftoc(newPos.x);
		toPos[1] = sp_ftoc(newPos.y);
		toPos[2] = sp_ftoc(newPos.z);

		cell_t adjustedPos[3];
		adjustedPos[0] = sp_ftoc(vecAdjustedPos.x);
		adjustedPos[1] = sp_ftoc(vecAdjustedPos.y);
		adjustedPos[2] = sp_ftoc(vecAdjustedPos.z);

		cell_t editedPos[3];

		cell_t result = Pl_Continue;
		g_pForwardUpdateLoco->PushCell((cell_t)this);
		g_pForwardUpdateLoco->PushArray(fromPos, 3);
		g_pForwardUpdateLoco->PushArray(toPos, 3);
		g_pForwardUpdateLoco->PushArray(adjustedPos, 3);
		g_pForwardUpdateLoco->PushArray(editedPos, 3, SM_PARAM_COPYBACK);

		g_pForwardUpdateLoco->Execute(&result);
		if (result == Pl_Changed)
		{
			newPos.x = sp_ctof(editedPos[0]);
			newPos.y = sp_ctof(editedPos[1]);
			newPos.z = sp_ctof(editedPos[2]);
			DETOUR_MEMBER_CALL(NextBotGroundLocomotion_UpdatePosition)(newPos);
		}
	}

	/*NextBotGroundLocomotion *mover = reinterpret_cast<NextBotGroundLocomotion*>(this);
	INextBot *bot = mover->GetBot();
	IBody *body = bot->GetBodyInterface();
	trace_t pm;
	CTraceFilterSimpleHack filter(bot->GetEntity(), COLLISION_GROUP_PLAYER_MOVEMENT);
	mover->TraceHull(bot->GetPosition(),newPos,body->GetHullMins(),body->GetHullMaxs(),body->GetSolidMask(),&filter,&pm);
	
	if (!pm.DidHit() && !pm.startsolid && pm.fraction == 1.0)
	{
		trace_t stuck;
		mover->TraceHull(newPos,newPos,body->GetHullMins(),body->GetHullMaxs(),body->GetSolidMask(),&filter,&stuck);
		if (!stuck.DidHit() && !stuck.startsolid && stuck.fraction == 1.0)
		{
			bot->SetPosition(newPos);
			return;
		}
	}

	if (!mover->IsOnGround())
		return;
	
	mover->StepMove(newPos, pm);*/
}
/*
#define COORD_FRACTIONAL_BITS		5
#define COORD_DENOMINATOR			(1<<(COORD_FRACTIONAL_BITS))
#define COORD_RESOLUTION			(1.0/(COORD_DENOMINATOR))

DETOUR_DECL_MEMBER0(NextBotGroundLocomotion_UpdateGroundConstraint, void)
{
	NextBotGroundLocomotion *mover = reinterpret_cast<NextBotGroundLocomotion*>(this);
	INextBot *bot = mover->GetBot();
	IBody *body = bot->GetBodyInterface();
	
	trace_t trace;
	Vector start( bot->GetPosition() );
	Vector end( bot->GetPosition() );
	start.z += 2;
	end.z -= mover->GetStepHeight();

	// See how far up we can go without getting stuck
	const float stickToGroundTolerance = mover->GetStepHeight() + 0.01f;

	float halfWidth = body->GetHullWidth()/2.0f;
	
	// since we only care about ground collisions, keep hull short to avoid issues with low ceilings
	/// @TODO: We need to also check actual hull height to avoid interpenetrating the world
	float hullHeight = mover->GetStepHeight();
	
	trace_t ground;
	CTraceFilterSimpleHack filter(bot->GetEntity(), COLLISION_GROUP_PLAYER_MOVEMENT);
	mover->TraceHull(bot->GetPosition() + Vector( 0, 0, mover->GetStepHeight() + 0.001f ), bot->GetPosition() + Vector( 0, 0, -stickToGroundTolerance ), Vector( -halfWidth, -halfWidth, 0 ), Vector( halfWidth, halfWidth, hullHeight ), body->GetSolidMask(), &filter, &ground );
	start = trace.endpos;

	// using trace.startsolid is unreliable here, it doesn't get set when
	// tracing bounding box vs. terrain

	// Now trace down from a known safe position
	mover->TraceHull(start,end,body->GetHullMins(),body->GetHullMaxs(),body->GetSolidMask(),&filter,&trace);
	if ( trace.fraction > 0.0f &&			// must go somewhere
		trace.fraction < 1.0f &&			// must hit something
		!trace.startsolid &&				// can't be embedded in a solid
		trace.plane.normal[2] >= 0.7 )		// can't hit a steep slope that we can't stand on anyway
	{
		float flDelta = fabs(bot->GetPosition().z - trace.endpos.z);

		//This is incredibly hacky. The real problem is that trace returning that strange value we can't network over.
		if ( flDelta > 0.5f * COORD_RESOLUTION)
		{
			bot->SetPosition( trace.endpos );
		}
	}
}

#define	MAX_CLIP_PLANES	5

int NextBotGroundLocomotion::TryNextBotMove(Vector *pFirstDest, trace_t *pFirstTrace)
{
	INextBot *bot = GetBot();
	IBody *body = bot->GetBodyInterface();
	
	int			bumpcount, numbumps;
	Vector		dir;
	float		d;
	int			numplanes;
	Vector		planes[MAX_CLIP_PLANES];
	Vector		primal_velocity, original_velocity;
	Vector      new_velocity;
	int			i, j;
	trace_t	pm;
	Vector		end;
	float		time_left, allFraction;
	int			blocked;

	numbumps = 4;           // Bump up to four times

	blocked = 0;           // Assume not blocked
	numplanes = 0;           //  and not sliding along any planes

	VectorCopy(m_velocity, original_velocity);  // Store original velocity
	VectorCopy(m_velocity, primal_velocity);

	allFraction = 0;
	time_left = gpGlobals->frametime;   // Total time for this movement operation.

	new_velocity.Init();

	CTraceFilterSimpleHack filter(bot->GetEntity(), COLLISION_GROUP_PLAYER_MOVEMENT);
	for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		if (m_velocity.Length() == 0.0)
			break;

		// Assume we can move all the way from the current origin to the
		//  end point.
		VectorMA(bot->GetPosition(), time_left, m_velocity, end);

		// See if we can make it from origin to end point.
		// If their velocity Z is 0, then we can avoid an extra trace here during WalkMove.
		if (pFirstDest && end == *pFirstDest)
			pm = *pFirstTrace;
		else
		{
			TraceHull(bot->GetPosition(),end,body->GetHullMins(),body->GetHullMaxs(),body->GetSolidMask(),&filter,&pm);
		}

		allFraction += pm.fraction;

		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity and return that we
		//  are blocked by floor and wall.
		if (pm.allsolid)
		{
			// entity is trapped in another solid
			VectorCopy(vec3_origin, m_velocity);
			return 4;
		}

		// If we moved some portion of the total distance, then
		//  copy the end position into the pmove.origin and 
		//  zero the plane counter.
		if (pm.fraction > 0)
		{
			if (numbumps > 0 && pm.fraction == 1)
			{
				// There's a precision issue with terrain tracing that can cause a swept box to successfully trace
				// when the end position is stuck in the triangle.  Re-run the test with an uswept box to catch that
				// case until the bug is fixed.
				// If we detect getting stuck, don't allow the movement
				trace_t stuck;
				TraceHull(pm.endpos,pm.endpos,body->GetHullMins(),body->GetHullMaxs(),body->GetSolidMask(),&filter,&stuck);
				if (stuck.startsolid || stuck.fraction != 1.0f)
				{
					//Msg( "Player will become stuck!!!\n" );
					VectorCopy(vec3_origin, m_velocity);
					break;
				}
			}

			// actually covered some distance
			bot->SetPosition(pm.endpos);
			VectorCopy(m_velocity, original_velocity);
			numplanes = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (pm.fraction == 1)
		{
			break;		// moved the entire distance
		}

		// If the plane we hit has a high z component in the normal, then
		//  it's probably a floor
		if (pm.plane.normal[2] > 0.7)
		{
			blocked |= 1;		// floor
		}
		// If the plane has a zero z component in the normal, then it's a 
		//  step or wall
		if (!pm.plane.normal[2])
		{
			blocked |= 2;		// step / wall
		}

		// Reduce amount of m_flFrameTime left by total time left * fraction
		//  that we covered.
		time_left -= time_left * pm.fraction;

		// Did we run out of planes to clip against?
		if (numplanes >= MAX_CLIP_PLANES)
		{
			// this shouldn't really happen
			//  Stop our movement if so.
			VectorCopy(vec3_origin, m_velocity);
			//Con_DPrintf("Too many planes 4\n");

			break;
		}

		// Set up next clipping plane
		VectorCopy(pm.plane.normal, planes[numplanes]);
		numplanes++;

		// modify original_velocity so it parallels all of the clip planes
		//

		// reflect player velocity 
		// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
		if (numplanes == 1 && !IsOnGround())
		{
			for (i = 0; i < numplanes; i++)
			{
				if (planes[i][2] > 0.7)
				{
					// floor or slope
					ClipVelocity(original_velocity, planes[i], new_velocity, 1.0);
					VectorCopy(new_velocity, original_velocity);
				}
				else
				{
					ClipVelocity(original_velocity, planes[i], new_velocity, 1.0);
				}
			}

			VectorCopy(new_velocity, m_velocity);
			VectorCopy(new_velocity, original_velocity);
		}
		else
		{
			for (i = 0; i < numplanes; i++)
			{
				ClipVelocity(
					original_velocity,
					planes[i],
					m_velocity,
					1);

				for (j = 0; j < numplanes; j++)
					if (j != i)
					{
						// Are we now moving against this plane?
						if (m_velocity.Dot(planes[j]) < 0)
							break;	// not ok
					}
				if (j == numplanes)  // Didn't have to clip, so we're ok
					break;
			}

			// Did we go all the way through plane set
			if (i != numplanes)
			{	// go along this plane
				// pmove.velocity is set in clipping call, no need to set again.
				;
			}
			else
			{	// go along the crease
				if (numplanes != 2)
				{
					VectorCopy(vec3_origin, m_velocity);
					break;
				}
				CrossProduct(planes[0], planes[1], dir);
				dir.NormalizeInPlace();
				d = dir.Dot(m_velocity);
				VectorScale(dir, d, m_velocity);
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			d = m_velocity.Dot(primal_velocity);
			if (d <= 0)
			{
				//Con_DPrintf("Back\n");
				VectorCopy(vec3_origin, m_velocity);
				break;
			}
		}
	}

	if (allFraction == 0)
	{
		VectorCopy(vec3_origin, m_velocity);
	}

	return blocked;
}

void NextBotGroundLocomotion::StepMove(Vector &vecDestination, trace_t &trace)
{
	INextBot *bot = GetBot();
	IBody *body = bot->GetBodyInterface();

	Vector vecEndPos;
	VectorCopy(vecDestination, vecEndPos);

	// Try sliding forward both on ground and up 16 pixels
	//  take the move that goes farthest
	Vector vecPos, vecVel;
	VectorCopy(bot->GetPosition(), vecPos);
	VectorCopy(m_velocity, vecVel);

	// Slide move down.
	TryNextBotMove(&vecEndPos, &trace);

	// Down results.
	Vector vecDownPos, vecDownVel;
	VectorCopy(bot->GetPosition(), vecDownPos);
	VectorCopy(m_velocity, vecDownVel);

	// Reset original values.
	bot->SetPosition(vecPos);
	VectorCopy(vecVel, m_velocity);

	// Move up a stair height.
	VectorCopy(bot->GetPosition(), vecEndPos);
	CTraceFilterSimpleHack filter(bot->GetEntity(), COLLISION_GROUP_PLAYER_MOVEMENT);
	TraceHull(bot->GetPosition(),vecEndPos,body->GetHullMins(),body->GetHullMaxs(),body->GetSolidMask(),&filter,&trace);
	if (!trace.startsolid && !trace.allsolid)
	{
		bot->SetPosition(trace.endpos);
	}

	// Slide move up.
	TryNextBotMove();

	// Move down a stair (attempt to).
	VectorCopy(bot->GetPosition(), vecEndPos);
	TraceBotBBox(bot, bot->GetPosition(), vecEndPos, body->GetSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace);

	// If we are not on the ground any more then use the original movement attempt.
	if (trace.plane.normal[2] < 0.7)
	{
		bot->SetPosition(vecDownPos);
		VectorCopy(vecDownVel, m_velocity);
		return;
	}

	// If the trace ended up in empty space, copy the end over to the origin.
	if (!trace.startsolid && !trace.allsolid)
	{
		bot->SetPosition(trace.endpos);
	}

	// Copy this origin to up.
	Vector vecUpPos;
	VectorCopy(bot->GetPosition(), vecUpPos);

	// decide which one went farther
	float flDownDist = (vecDownPos.x - vecPos.x) * (vecDownPos.x - vecPos.x) + (vecDownPos.y - vecPos.y) * (vecDownPos.y - vecPos.y);
	float flUpDist = (vecUpPos.x - vecPos.x) * (vecUpPos.x - vecPos.x) + (vecUpPos.y - vecPos.y) * (vecUpPos.y - vecPos.y);
	if (flDownDist > flUpDist)
	{
		bot->SetPosition(vecDownPos);
		VectorCopy(vecDownVel, m_velocity);
	}
	else
	{
		// copy z value from slide move
		m_velocity.z = vecDownVel.z;
	}

	float flStepDist = bot->GetPosition().z - vecPos.z;
}
*/
bool CBaseNPCExt::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	char conf_error[255];
	if(!gameconfs->LoadGameConfigFile("nextbot_pathing", &g_pGameConf, conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "FAILED TO LOAD GAMEDATA ERROR: %s", conf_error);
		return false;
	}
	
	CNavMesh::Init();
	if (TheNavMesh)
		g_pSM->LogMessage(myself, "Found TheNavMesh pointer: 0x%08x", TheNavMesh);
	
	if (g_pGameConf->GetMemSig("CNavMesh::GetNearestNavArea", reinterpret_cast<void **>(&CNavMesh::func_GetNearestNavArea))) {
		g_pSM->LogMessage(myself, "Got function: 0x%08x CNavMesh::GetNearestNavArea", *reinterpret_cast<uintptr_t *>(&CNavMesh::func_GetNearestNavArea));
	} else {
		g_pSM->LogMessage(myself, "Couldn't locate function CNavMesh::GetNearestNavArea!");
		return false;
	}
	
	if (g_pGameConf->GetMemSig("CNavMesh::GetGroundHeight", reinterpret_cast<void **>(&CNavMesh::func_GetGroundHeight))) {
		g_pSM->LogMessage(myself, "Got function: 0x%08x CNavMesh::GetGroundHeight", *reinterpret_cast<uintptr_t *>(&CNavMesh::func_GetGroundHeight));
	} else {
		g_pSM->LogMessage(myself, "Couldn't locate function CNavMesh::GetGroundHeight!");
		return false;
	}
	
	if (g_pGameConf->GetMemSig("CTraceFilterSimple::ShouldHitEntity", reinterpret_cast<void **>(&CTraceFilterSimpleHack::func_ShouldHitEntity))) {
		g_pSM->LogMessage(myself, "Got function: 0x%08x CTraceFilterSimple::ShouldHitEntity", *reinterpret_cast<uintptr_t *>(&CTraceFilterSimpleHack::func_ShouldHitEntity));
	} else {
		g_pSM->LogMessage(myself, "Couldn't locate function CTraceFilterSimple::ShouldHitEntity!");
		return false;
	}
	
	CDetourManager::Init(g_pSM->GetScriptingEngine(), g_pGameConf);

	g_pSetLocalAngles = DETOUR_CREATE_MEMBER(CBaseEntity_SetLocalAngles, "CBaseEntity::SetLocalAngles");
	if(g_pSetLocalAngles != NULL)
	{
		g_pSetLocalAngles->EnableDetour();
		g_pSM->LogMessage(myself, "CBaseEntity::SetLocalAngles detour enabled.");
	}
	
	g_pUpdatePosition = DETOUR_CREATE_MEMBER(NextBotGroundLocomotion_UpdatePosition, "NextBotGroundLocomotion::UpdatePosition");
	if(g_pUpdatePosition != NULL)
	{
		g_pUpdatePosition->EnableDetour();
		g_pSM->LogMessage(myself, "NextBotGroundLocomotion::UpdatePosition detour enabled.");
	}
	
	/*g_pUpdateGroundConstraint = DETOUR_CREATE_MEMBER(NextBotGroundLocomotion_UpdateGroundConstraint, "NextBotGroundLocomotion::UpdateGroundConstraint");
	if (g_pUpdateGroundConstraint != NULL)
	{
		g_pUpdateGroundConstraint->EnableDetour();
		g_pSM->LogMessage(myself, "NextBotGroundLocomotion::UpdateGroundConstraint detour enabled.");
	}*/

	g_pForwardSetLocalAngles = forwards->CreateForward("CBaseEntity_SetLocalAngles", ET_Event, 2, NULL, Param_Cell, Param_Array);
	g_pForwardUpdateLoco = forwards->CreateForward("NextBotGroundLocomotion_UpdatePosition", ET_Event, 5, NULL, Param_Cell, Param_Array, Param_Array, Param_Array, Param_Array);
	GETGAMEDATAOFFSET("CBaseCombatCharacter::GetLastKnownArea", g_iLastKnownAreaOffset);
	GETGAMEDATAOFFSET("CBaseEntity::MyNextBotPointer", g_iMyNextBotPointerOffset);

	sharesys->AddDependency(myself, "bintools.ext", true, true);
	sharesys->AddNatives(myself, g_NativesInfo);
	sharesys->RegisterLibrary(myself, "nextbot_pathing");

	return true;
}

bool CBaseNPCExt::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_ANY(GetServerFactory, gameents, IServerGameEnts, INTERFACEVERSION_SERVERGAMEENTS);
	GET_V_IFACE_ANY(GetEngineFactory, enginetrace, IEngineTrace, INTERFACEVERSION_ENGINETRACE_SERVER);
	
	gpGlobals = ismm->GetCGlobals();
	return true;
}

void CBaseNPCExt::OnPluginUnloaded(IPlugin *plugin)
{
	IPluginContext *pCtx = plugin->GetBaseContext();
	for (int i = 0; i < g_PathFunctions.Count(); i++)
	{
		if (g_PathFunctions[i].pCostFunction->GetParentContext() == pCtx)
		{
			delete g_PathFunctions[i].pPath;
			g_PathFunctions.Remove(i);
			i--;
		}
	}
}

void CBaseNPCExt::OnCoreMapStart(edict_t *pEdictList, int edictCount, int clientMax)
{
	CNavMesh::Init();
}

void CBaseNPCExt::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(BINTOOLS, g_pBinTools);

	handlesys->FindHandleType("CellArray", &g_CellArrayHandle);
	handlesys->FindHandleType("KeyValues", &g_KeyValueType);
	g_CoreIdent = sharesys->FindIdentType("CORE");
	
	g_pEntityList = (CBaseEntityList *)gamehelpers->GetGlobalEntityList();
}

bool CBaseNPCExt::QueryRunning(char *error, size_t maxlength)
{
	SM_CHECK_IFACE(BINTOOLS, g_pBinTools);
	return true;
}

bool CBaseNPCExt::QueryInterfaceDrop(SMInterface *pInterface)
{
	if(pInterface == g_pBinTools)
		return false;

	return IExtensionInterface::QueryInterfaceDrop(pInterface);
}

void CBaseNPCExt::NotifyInterfaceDrop(SMInterface *pInterface)
{
	if(pInterface == g_pBinTools)
		g_pBinTools = nullptr;
}

void CBaseNPCExt::SDK_OnUnload()
{
	gameconfs->CloseGameConfigFile(g_pGameConf);

	forwards->ReleaseForward(g_pForwardSetLocalAngles);
	forwards->ReleaseForward(g_pForwardUpdateLoco);

	if (g_pSetLocalAngles != NULL) g_pSetLocalAngles->Destroy();
	if (g_pUpdatePosition != NULL) g_pUpdatePosition->Destroy();
	//if (g_pUpdateGroundConstraint != NULL) g_pUpdateGroundConstraint->Destroy();
	
	gameconfs->CloseGameConfigFile(g_pGameConf);
}

//Fix external stuff error
float IntervalTimer::Now( void ) const
{
	return gpGlobals->curtime;
}

float CountdownTimer::Now( void ) const
{
	return gpGlobals->curtime;
}

bool CGameTrace::DidHitWorld() const
{
	return gamehelpers->EntityToBCompatRef(reinterpret_cast<CBaseEntity *>(m_pEnt)) == 0;
}

bool CGameTrace::DidHitNonWorldEntity() const
{
	return m_pEnt != nullptr && !DidHitWorld();
}