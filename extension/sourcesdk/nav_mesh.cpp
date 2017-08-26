#include "extension.h"
#include "shareddefs.h"
#include "enginecallback.h"
#include "util_shared.h"
#include "sourcesdk/nav.h"
#include "sourcesdk/nav_area.h"
#include "sourcesdk/nav_mesh.h"
CNavMesh *TheNavMesh = nullptr;

bool (*BuildPath)(CNavArea *, CNavArea *, Vector *, IPathCost&, CNavArea **, float, int, bool) = nullptr;

bool NavAreaBuildPath( CNavArea *startArea, CNavArea *goalArea, Vector *goalPos, IPathCost &costFunc, CNavArea **closestArea, float maxPathLength, int teamID, bool ignoreNavBlockers)
{
	if (!BuildPath)
	{
		if (!g_pGameConf->GetMemSig("NavAreaBuildPath", reinterpret_cast<void **>(&BuildPath)) || !BuildPath)
		{
			BuildPath = nullptr;
			return false;
		}
	}
	return (*BuildPath)(startArea, goalArea, goalPos, costFunc, closestArea, maxPathLength, teamID, ignoreNavBlockers);
	/*static ICallWrapper *pNavBuildPath = NULL;
	
	if (!pNavBuildPath)
	{
		void *addr;
		if (!g_pGameConf->GetMemSig("NavAreaBuildPath", &addr) || !addr)
		{
			return false;
		}
		PassInfo pass[8];
		pass[0].flags = PASSFLAG_BYVAL;
		pass[0].type = PassType_Basic;
		pass[0].size = sizeof(CNavArea *);
		pass[1].flags = PASSFLAG_BYVAL;
		pass[1].type = PassType_Basic;
		pass[1].size = sizeof(CNavArea *);
		pass[2].flags = PASSFLAG_BYVAL;
		pass[2].type = PassType_Basic;
		pass[2].size = sizeof(Vector *);
		pass[3].flags = PASSFLAG_BYVAL;
		pass[3].type = PassType_Basic;
		pass[3].size = sizeof(IPathCost &);
		pass[4].flags = PASSFLAG_BYVAL;
		pass[4].type = PassType_Basic;
		pass[4].size = sizeof(CNavArea **);
		pass[5].flags = PASSFLAG_BYVAL;
		pass[5].type = PassType_Basic;
		pass[5].size = sizeof(float);
		pass[6].flags = PASSFLAG_BYVAL;
		pass[6].type = PassType_Basic;
		pass[6].size = sizeof(int);
		pass[7].flags = PASSFLAG_BYVAL;
		pass[7].type = PassType_Basic;
		pass[7].size = sizeof(bool);
		
		PassInfo ret;
		ret.flags = PASSFLAG_BYVAL;
		ret.type = PassType_Basic;
		ret.size = sizeof(bool);
		
		if (!(pNavBuildPath = g_pBinTools->CreateCall(addr, CallConv_Cdecl, &ret, pass, 8)))
		{
			return false;
		}
	}
	
	unsigned char vstk[sizeof(CNavArea *)*2 + sizeof(Vector *) + sizeof(IPathCost &) + sizeof(CNavArea **) + sizeof(float) + sizeof(int) + sizeof(bool)];
	unsigned char *vptr = vstk;
	
	*(CNavArea **)vptr = startArea;
	vptr += sizeof(CNavArea *);
	
	*(CNavArea **)vptr = goalArea;
	vptr += sizeof(CNavArea *);
	
	*(Vector **)vptr = goalPos;
	vptr += sizeof(Vector *);
	
	*(IPathCost *)vptr = costFunc;
	vptr += sizeof(IPathCost &);
	
	*(CNavArea ***)vptr = closestArea;
	vptr += sizeof(CNavArea **);
	
	*(float *)vptr = maxPathLength;
	vptr += sizeof(float);
	
	*(int *)vptr = teamID;
	vptr += sizeof(int);
	
	*(bool *)vptr = ignoreNavBlockers;
	vptr += sizeof(bool);
	
	bool bReturn;
	pNavBuildPath->Execute(vstk, &bReturn);
	return bReturn;*/
}