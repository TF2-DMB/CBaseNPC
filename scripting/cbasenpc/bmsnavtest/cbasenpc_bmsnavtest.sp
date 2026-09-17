//test this on dm_crossfire, it has proper nav mesh (you may need to launch with mp_coop 1)
  
#include <sourcemod>  
#include <cbasenpc>  
#include <cbasenpc/nav>  
#include <cbasenpc/bms/nav>  
  
#pragma semicolon 1  
#pragma newdecls required  
  
public Plugin myinfo =  
{  
    name        = "BMS NavArea Full Demo",  
    author      = "You",  
    description = "Demonstrates base + BMS CBaseNPC nav natives",  
    version     = "1.0",  
    url         = ""  
};  
  
// ─── Plugin load ─────────────────────────────────────────────────────────────  
  
public void OnPluginStart()  
{  
    RegConsoleCmd("sm_bms_navinfo",      Cmd_NavInfo,      "Detailed nav area info under your feet");  
    RegConsoleCmd("sm_bms_markbase",     Cmd_MarkBase,     "Toggle BM_NAV_PLAYER_BASE on area under your feet");  
    RegConsoleCmd("sm_bms_hidingspots",  Cmd_HidingSpots,  "List hiding spots in the area under your feet");  
    RegConsoleCmd("sm_bms_neighbors",    Cmd_Neighbors,    "List adjacent areas in all 4 directions");  
    RegConsoleCmd("sm_bms_path",         Cmd_Path,         "sm_bms_path <target> - Build A* path to target player");  
    RegConsoleCmd("sm_bms_scanall",      Cmd_ScanAll,      "Scan ALL nav areas and count player bases");  
    RegConsoleCmd("sm_bms_hidingall",    Cmd_HidingAll,    "List all global hiding spots with cover flags");  
}  
  
public void OnMapStart()  
{  
    if (!TheNavMesh.IsLoaded())  
    {  
        LogMessage("[BMS Nav] Nav mesh NOT loaded.");  
        return;  
    }  
  
    bool analyzed = TheNavMesh.IsAnalyzed();  
    bool outdated  = TheNavMesh.IsOutOfDate();  
    int  areaCount = TheNavMesh.GetNavAreaCount();  
    int  spotCount = TheHidingSpots.Count;  
  
    LogMessage("[BMS Nav] Loaded. Areas: %d | HidingSpots: %d | Analyzed: %s | OutOfDate: %s",  
        areaCount, spotCount,  
        analyzed ? "yes" : "no",  
        outdated  ? "yes" : "no"  
    );  
}  
  
// ─── Helpers ─────────────────────────────────────────────────────────────────  
  
static bool GetAreaUnderClient(int client, CNavArea &area)  
{  
	int iclient = 1;
    float pos[3];  
    GetClientAbsOrigin(iclient, pos);  
    area = TheNavMesh.GetNavArea(pos, 120.0);  
    return (area != NULL_AREA);  
}  
  
static void NavAttribsToString(int attribs, char[] buf, int maxlen)  
{  
    buf[0] = '\0';  
    if (attribs & NAV_MESH_CROUCH)   StrCat(buf, maxlen, "CROUCH ");  
    if (attribs & NAV_MESH_JUMP)     StrCat(buf, maxlen, "JUMP ");  
    if (attribs & NAV_MESH_PRECISE)  StrCat(buf, maxlen, "PRECISE ");  
    if (attribs & NAV_MESH_NO_JUMP)  StrCat(buf, maxlen, "NO_JUMP ");  
    if (attribs & NAV_MESH_STOP)     StrCat(buf, maxlen, "STOP ");  
    if (attribs & NAV_MESH_RUN)      StrCat(buf, maxlen, "RUN ");  
    if (attribs & NAV_MESH_WALK)     StrCat(buf, maxlen, "WALK ");  
    if (attribs & NAV_MESH_AVOID)    StrCat(buf, maxlen, "AVOID ");  
    if (attribs & NAV_MESH_STAND)    StrCat(buf, maxlen, "STAND ");  
    if (buf[0] == '\0')              StrCat(buf, maxlen, "NONE");  
}  
  
static void HidingFlagsToString(HidingSpotFlags flags, char[] buf, int maxlen)  
{  
    buf[0] = '\0';  
    if (flags & IN_COVER)           StrCat(buf, maxlen, "COVER ");  
    if (flags & GOOD_SNIPER_SPOT)   StrCat(buf, maxlen, "SNIPER ");  
    if (flags & IDEAL_SNIPER_SPOT)  StrCat(buf, maxlen, "IDEAL_SNIPER ");  
    if (flags & EXPOSED)            StrCat(buf, maxlen, "EXPOSED ");  
    if (buf[0] == '\0')             StrCat(buf, maxlen, "NONE");  
}  
  
// ─── sm_bms_navinfo ──────────────────────────────────────────────────────────  
  
Action Cmd_NavInfo(int client, int args)  
{  
    int iclient = 1;
    if (!TheNavMesh.IsLoaded()) { ReplyToCommand(iclient, "[BMS Nav] Mesh not loaded."); return Plugin_Handled; }  
  
    CNavArea baseArea;  
    if (!GetAreaUnderClient(iclient, baseArea))  
    {  
        ReplyToCommand(iclient, "[BMS Nav] No area found at your position.");  
        return Plugin_Handled;  
    }  
  
    CBlackMesaNavArea area = view_as<CBlackMesaNavArea>(baseArea);  
  
    float center[3];  
    area.GetCenter(center);  
  
    float nwCorner[3], seCorner[3];  
    area.GetExtent(nwCorner, seCorner);  
  
    float normal[3];  
    area.ComputeNormal(normal);  
  
    char attribStr[128];  
    NavAttribsToString(area.GetAttributes(), attribStr, sizeof(attribStr));  
  
    BlackMesaNavAttributeType gameAttribs = area.GetGameAttributes();  
    char isBase[4];  
    isBase = area.HasGameAttribute(BM_NAV_PLAYER_BASE) ? "YES" : "NO";  
  
    int adjTotal = 0;  
    for (int d = 0; d < view_as<int>(NUM_DIRECTIONS); d++)  
        adjTotal += area.GetAdjacentCount(view_as<NavDirType>(d));  
  
    ReplyToCommand(iclient, "--- Area %d ---", area.GetID());  
    ReplyToCommand(iclient, "  Center:      (%.1f, %.1f, %.1f)", center[0], center[1], center[2]);  
    ReplyToCommand(iclient, "  Size:        %.1f x %.1f", area.GetSizeX(), area.GetSizeY());  
    ReplyToCommand(iclient, "  Light:       %.2f", area.GetLightIntensity());  
    ReplyToCommand(iclient, "  Normal:      (%.2f, %.2f, %.2f)", normal[0], normal[1], normal[2]);  
    ReplyToCommand(iclient, "  NavAttribs:  %s (0x%X)", attribStr, area.GetAttributes());  
    ReplyToCommand(iclient, "  GameAttribs: 0x%X | PlayerBase: %s", gameAttribs, isBase);  
    ReplyToCommand(iclient, "  HidingSpots: %d | Neighbors: %d", area.GetHidingSpotCount(), adjTotal);  
    ReplyToCommand(iclient, "  Blocked:     %s", area.IsBlocked(TEAM_ANY) ? "YES" : "NO");  
  
    return Plugin_Handled;  
}  
  
// ─── sm_bms_markbase ─────────────────────────────────────────────────────────  
  
Action Cmd_MarkBase(int client, int args)  
{  
    int iclient = 1;
    if (!TheNavMesh.IsLoaded()) { ReplyToCommand(iclient, "[BMS Nav] Mesh not loaded."); return Plugin_Handled; }  
  
    CNavArea baseArea;  
    if (!GetAreaUnderClient(iclient, baseArea))  
    {  
        ReplyToCommand(iclient, "[BMS Nav] No area found at your position.");  
        return Plugin_Handled;  
    }  
  
    CBlackMesaNavArea area = view_as<CBlackMesaNavArea>(baseArea);  
  
    if (area.HasGameAttribute(BM_NAV_PLAYER_BASE))  
    {  
        area.ClearGameAttribute(BM_NAV_PLAYER_BASE);  
        ReplyToCommand(iclient, "[BMS Nav] Area %d: BM_NAV_PLAYER_BASE cleared.", area.GetID());  
    }  
    else  
    {  
        area.SetGameAttribute(BM_NAV_PLAYER_BASE);  
        ReplyToCommand(iclient, "[BMS Nav] Area %d: BM_NAV_PLAYER_BASE set.", area.GetID());  
    }  
  
    return Plugin_Handled;  
}  
  
// ─── sm_bms_hidingspots ──────────────────────────────────────────────────────  
  
Action Cmd_HidingSpots(int client, int args)  
{  
    int iclient = 1;  
    if (!TheNavMesh.IsLoaded()) { ReplyToCommand(iclient, "[BMS Nav] Mesh not loaded."); return Plugin_Handled; }  
  
    CNavArea baseArea;  
    if (!GetAreaUnderClient(iclient, baseArea))  
    {  
        ReplyToCommand(iclient, "[BMS Nav] No area found at your position.");  
        return Plugin_Handled;  
    }  
  
    int count = baseArea.GetHidingSpotCount();  
    ReplyToCommand(iclient, "[BMS Nav] Area %d has %d hiding spot(s):", baseArea.GetID(), count);  
  
    for (int i = 0; i < count; i++)  
    {  
        HidingSpot spot = baseArea.GetHidingSpot(i);  
        float pos[3];  
        spot.GetPosition(pos);  
  
        char flagStr[64];  
        HidingFlagsToString(spot.GetFlags(), flagStr, sizeof(flagStr));  
  
        ReplyToCommand(iclient, "  [%d] ID:%d pos:(%.1f,%.1f,%.1f) flags:%s",  
            i, spot.GetID(), pos[0], pos[1], pos[2], flagStr);  
    }  
  
    return Plugin_Handled;  
}  
  
// ─── sm_bms_neighbors ────────────────────────────────────────────────────────  
  
Action Cmd_Neighbors(int client, int args)  
{  
    int iclient = 1;
    if (!TheNavMesh.IsLoaded()) { ReplyToCommand(iclient, "[BMS Nav] Mesh not loaded."); return Plugin_Handled; }  
  
    CNavArea baseArea;  
    if (!GetAreaUnderClient(iclient, baseArea))  
    {  
        ReplyToCommand(iclient, "[BMS Nav] No area found at your position.");  
        return Plugin_Handled;  
    }  
  
    static const char dirNames[4][] = { "NORTH", "EAST", "SOUTH", "WEST" };  
  
    ReplyToCommand(iclient, "[BMS Nav] Neighbors of area %d:", baseArea.GetID());  
  
    for (int d = 0; d < view_as<int>(NUM_DIRECTIONS); d++)  
    {  
        NavDirType dir = view_as<NavDirType>(d);  
        int count = baseArea.GetAdjacentCount(dir);  
  
        for (int i = 0; i < count; i++)  
        {  
            CBlackMesaNavArea neighbor = view_as<CBlackMesaNavArea>(baseArea.GetAdjacentArea(dir, i));  
            float neighborCenter[3];  
            neighbor.GetCenter(neighborCenter);  
  
            char isBase[4];  
            isBase = neighbor.HasGameAttribute(BM_NAV_PLAYER_BASE) ? "YES" : "NO";  
  
            ReplyToCommand(iclient, "  %s[%d] -> Area %d at (%.1f,%.1f,%.1f) len:%.1f base:%s",  
                dirNames[d], i,  
                neighbor.GetID(),  
                neighborCenter[0], neighborCenter[1], neighborCenter[2],  
                baseArea.GetAdjacentLength(dir, i),  
                isBase  
            );  
        }  
    }  
  
    return Plugin_Handled;  
}  
  
// ─── sm_bms_path ─────────────────────────────────────────────────────────────  
  
Action Cmd_Path(int client, int args)  
{  
	int iclient = 1;
    if (!TheNavMesh.IsLoaded()) { ReplyToCommand(iclient, "[BMS Nav] Mesh not loaded."); return Plugin_Handled; }  
  
    if (args < 1)  
    {  
        ReplyToCommand(iclient, "Usage: sm_bms_path <target name>");  
        return Plugin_Handled;  
    }  
  
    char targetName[64];  
    GetCmdArg(1, targetName, sizeof(targetName));  
  
	int target = -1;  
	for (int i = 1; i <= MaxClients; i++)  
	{  
		if (!IsClientInGame(i) || !IsPlayerAlive(i))  
			continue;  
	  
		char name[MAX_NAME_LENGTH];  
		GetClientName(i, name, sizeof(name));  
	  
		if (StrContains(name, targetName, false) != -1)  
		{  
			target = i;  
			break;  
		}  
	}  
	  
	if (target == -1)  
	{  
		ReplyToCommand(client, "[BMS Nav] Target '%s' not found or not alive.", targetName);  
		return Plugin_Handled;  
	} 
  
    float startPos[3], goalPos[3];  
    GetClientAbsOrigin(iclient, startPos);  
    GetClientAbsOrigin(target, goalPos);  
  
    CNavArea startArea = TheNavMesh.GetNavArea(startPos, 120.0);  
    CNavArea goalArea  = TheNavMesh.GetNavArea(goalPos,  120.0);  
  
    if (startArea == NULL_AREA || goalArea == NULL_AREA)  
    {  
        ReplyToCommand(iclient, "[BMS Nav] Could not find nav areas for one or both positions.");  
        return Plugin_Handled;  
    }  
  
    CNavArea closestArea;  
    bool found = TheNavMesh.BuildPath(startArea, goalArea, goalPos, Path_Cost, closestArea);  
  
    if (!found)  
    {  
        ReplyToCommand(iclient, "[BMS Nav] No path found to %N (closest area: %d).", target, closestArea.GetID());  
        return Plugin_Handled;  
    }  
  
    // Walk path backwards from goal to start via GetParent()  
    int stepCount = 0;  
    float totalDist = 0.0;  
    CNavArea step = closestArea;  
  
    while (step != NULL_AREA && step != startArea)  
    {  
        CNavArea parent = step.GetParent();  
        if (parent != NULL_AREA)  
        {  
            float stepCenter[3], parentCenter[3];  
            step.GetCenter(stepCenter);  
            parent.GetCenter(parentCenter);  
            totalDist += GetVectorDistance(stepCenter, parentCenter);  
        }  
        stepCount++;  
        step = parent;  
    }  
  
    ReplyToCommand(iclient, "[BMS Nav] Path to %N: %d area(s), ~%.1f units.", target, stepCount, totalDist);  
  
    // Print first 5 steps from start  
    // Collect path into array first (reverse order)  
    CNavArea pathArr[256];  
    int pathLen = 0;  
    step = closestArea;  
    while (step != NULL_AREA && pathLen < 256)  
    {  
        pathArr[pathLen++] = step;  
        step = step.GetParent();  
    }  
  
    int printCount = pathLen < 5 ? pathLen : 5;  
    ReplyToCommand(iclient, "  First %d steps (from goal):", printCount);  
    for (int i = 0; i < printCount; i++)  
    {  
        CBlackMesaNavArea bmsStep = view_as<CBlackMesaNavArea>(pathArr[i]);  
        float c[3];  
        bmsStep.GetCenter(c);  
        char isBase[8];   // was [4] — needs room for "[BASE]" + null  
        isBase = bmsStep.HasGameAttribute(BM_NAV_PLAYER_BASE) ? "[BASE]" : "";  
        ReplyToCommand(client, "  [%d] Area %d at (%.1f,%.1f,%.1f) %s",  
            i, bmsStep.GetID(), c[0], c[1], c[2], isBase);  
    } 
  
    return Plugin_Handled;  
}  
  
// ─── sm_bms_scanall ──────────────────────────────────────────────────────────  
  
Action Cmd_ScanAll(int client, int args)  
{  
	int iclient = 1;
    if (!TheNavMesh.IsLoaded()) { ReplyToCommand(iclient, "[BMS Nav] Mesh not loaded."); return Plugin_Handled; }  
  
    int total     = TheNavAreas.Count;  
    int baseCount = 0;  
    int blocked   = 0;  
  
    for (int i = 0; i < total; i++)  
    {  
        CBlackMesaNavArea area = view_as<CBlackMesaNavArea>(TheNavAreas.Get(i));  
        if (area.HasGameAttribute(BM_NAV_PLAYER_BASE))  
            baseCount++;  
        if (area.IsBlocked(TEAM_ANY))  
            blocked++;  
    }  
  
    ReplyToCommand(iclient, "[BMS Nav] Total areas: %d | Player bases: %d | Blocked: %d",  
        total, baseCount, blocked);  
  
    return Plugin_Handled;  
}  
  
// ─── sm_bms_hidingall ────────────────────────────────────────────────────────  
  
Action Cmd_HidingAll(int client, int args)  
{  
	int iclient = 1;
    if (!TheNavMesh.IsLoaded()) { ReplyToCommand(iclient, "[BMS Nav] Mesh not loaded."); return Plugin_Handled; }  
  
    int total  = TheHidingSpots.Count;  
    int cover  = 0;  
    int sniper = 0;  
    int exposed = 0;  
  
    for (int i = 0; i < total; i++)  
    {  
        HidingSpot spot = TheHidingSpots.Get(i);  
        if (spot.HasGoodCover())      cover++;  
        if (spot.IsGoodSniperSpot())  sniper++;  
        if (spot.IsExposed())         exposed++;  
    }  
  
    ReplyToCommand(iclient, "[BMS Nav] Global hiding spots: %d | Cover: %d | Sniper: %d | Exposed: %d",  
        total, cover, sniper, exposed);  
  
    return Plugin_Handled;  
}