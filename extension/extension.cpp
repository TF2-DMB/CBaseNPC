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
//CDetour *g_pUpdatePosition = NULL;

CNavArea * (CNavMesh:: *CNavMesh::func_GetNearestNavArea)(const Vector &pos, bool anyZ, float maxDist, bool checkLOS, bool checkGround, int team) = nullptr;
bool (CNavMesh:: *CNavMesh::func_GetGroundHeight)(const Vector &pos, float *height, Vector *normal) = nullptr;
bool (CTraceFilterSimpleHack:: *CTraceFilterSimpleHack::func_ShouldHitEntity)(IHandleEntity *pHandleEntity, int contentsMask) = nullptr;

float k_flMaxEntityEulerAngle = 360.0 * 1000.0f;

inline bool IsEntityQAngleReasonable(const QAngle &q)
{
	float r = k_flMaxEntityEulerAngle;
	return
		q.x > -r && q.x < r &&
		q.y > -r && q.y < r &&
		q.z > -r && q.z < r;
}

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
}*/

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
	
	/*g_pUpdatePosition = DETOUR_CREATE_MEMBER(NextBotGroundLocomotion_UpdatePosition, "NextBotGroundLocomotion::UpdatePosition");
	if(g_pUpdatePosition != NULL)
	{
		g_pUpdatePosition->EnableDetour();
		g_pSM->LogMessage(myself, "NextBotGroundLocomotion::UpdatePosition detour enabled.");
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
	//if (g_pUpdatePosition != NULL) g_pUpdatePosition->Destroy();
	
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