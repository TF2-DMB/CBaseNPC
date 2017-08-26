#ifndef NAV_MESH_H
#define NAV_MESH_H

#include "sourcesdk/nav_area.h"

#pragma once

#ifdef _X360
#define PREFETCH360(address, offset) __dcbt(offset,address)
#else
#define PREFETCH360(x,y) // nothing
#endif

class CNavMesh;
class IPathCost
{
public:
	virtual float operator()( CNavArea *area, CNavArea *fromArea, const CNavLadder *ladder, const CFuncElevator *elevator, float length ) const = 0;
};


extern CNavMesh *TheNavMesh;

class CNavMesh
{
	public:
		static void Init()
		{
			//To-do add win support
			CNavMesh **ppTheNavMesh;
			char *addr;
			if (g_pGameConf->GetMemSig("TheNavMesh", (void **)&addr) && addr)
			{
				ppTheNavMesh = reinterpret_cast<CNavMesh **>(addr);
			}
			else
				g_pSM->LogMessage(myself, "Couldn't locate TheNavMesh pointer!", *TheNavMesh);
			TheNavMesh = *ppTheNavMesh;
		};
		static CNavArea * (CNavMesh::*func_GetNearestNavArea)(const Vector &pos, bool anyZ, float maxDist, bool checkLOS, bool checkGround, int team);
		CNavArea *GetNearestNavArea(const Vector &pos, bool anyZ = false, float maxDist = 10000.0f, bool checkLOS = false, bool checkGround = true, int team = -2)//-2 stands for TEAM_ANY
		{
			return (this->*func_GetNearestNavArea)(pos, anyZ, maxDist, checkLOS, checkGround, team);
		};
		static bool (CNavMesh::*func_GetGroundHeight)(const Vector &pos, float *height, Vector *normal);
		bool GetGroundHeight(const Vector &pos, float *height, Vector *normal = NULL)
		{
			return (this->*func_GetGroundHeight)(pos,height,normal);
		};
		bool IsAuthoritative()
		{
			return true; //TF2 has simple geometry
		}
};

#define IGNORE_NAV_BLOCKERS true
bool NavAreaBuildPath(CNavArea *startArea, CNavArea *goalArea, Vector *goalPos, IPathCost &costFunc, CNavArea **closestArea = NULL, float maxPathLength = 0.0f, int teamID = TEAM_ANY, bool ignoreNavBlockers = false);
#endif