#include "extension.h"
#include <shareddefs.h>
#include <enginecallback.h>
#include <util_shared.h>
#include "sourcesdk/nav.h"
#include "sourcesdk/nav_area.h"
#include "sourcesdk/nav_mesh.h"
CNavMesh *TheNavMesh = nullptr;

DEFINEFUNCTION(CNavMesh, GetNearestNavArea, CNavArea*, (const Vector& pos, bool anyZ, float maxDist, bool checkLOS, bool checkGround, int team), (pos, anyZ, maxDist, checkLOS, checkGround, team));
DEFINEFUNCTION(CNavMesh, GetGroundHeight, bool, (const Vector& pos, float* height, Vector* normal), (pos, height, normal));

bool CNavMesh::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	CNavMesh * *ppTheNavMesh;
	char* addr;
	if (g_pGameConf->GetMemSig("TheNavMesh", (void**)&addr) && addr)
	{
		ppTheNavMesh = reinterpret_cast<CNavMesh**>(addr);
	}
	else if (g_pGameConf->GetMemSig("CNavMesh::SnapToGrid", (void**)&addr) && addr)
	{
		int offset;
		if (!g_pGameConf->GetOffset("TheNavMesh", &offset) || !offset)
		{
			snprintf(error, maxlength, "Couldn't find offset for TheNavMesh ptr!");
			return false;
		}
		ppTheNavMesh = *reinterpret_cast<CNavMesh***>(addr + offset);
	}
	else
	{
		snprintf(error, maxlength, "Failed to retrieve TheNavMesh ptr!");
		return false;
	}

	TheNavMesh = *ppTheNavMesh;

	FINDSIG(config, GetNearestNavArea, "CNavMesh::GetNearestNavArea");
	FINDSIG(config, GetGroundHeight, "CNavMesh::GetGroundHeight");
	return true;
}

//bool (*BuildPath)(CNavArea *, CNavArea *, Vector *, IPathCost&, CNavArea **, float, int, bool) = nullptr;

bool NavAreaBuildPath( CNavArea *startArea, CNavArea *goalArea, Vector *goalPos, IPathCost &costFunc, CNavArea **closestArea, float maxPathLength, int teamID, bool ignoreNavBlockers)
{
	if (closestArea)
	{
		*closestArea = startArea;
	}

#ifdef STAGING_ONLY
	bool isDebug = (g_DebugPathfindCounter-- > 0);
#endif

	if (startArea == NULL)
		return false;

	startArea->SetParent(NULL);

	if (goalArea != NULL && goalArea->IsBlocked(teamID, ignoreNavBlockers))
		goalArea = NULL;

	if (goalArea == NULL && goalPos == NULL)
		return false;

	// if we are already in the goal area, build trivial path
	if (startArea == goalArea)
	{
		return true;
	}

	// determine actual goal position
	Vector actualGoalPos = (goalPos) ? *goalPos : goalArea->GetCenter();

	// start search
	CNavArea::ClearSearchLists();

	// compute estimate of path length
	/// @todo Cost might work as "manhattan distance"
	startArea->SetTotalCost((startArea->GetCenter() - actualGoalPos).Length());

	float initCost = costFunc(startArea, NULL, NULL, NULL, -1.0f);
	if (initCost < 0.0f)
		return false;
	startArea->SetCostSoFar(initCost);
	startArea->SetPathLengthSoFar(0.0);

	startArea->AddToOpenList();

	// keep track of the area we visit that is closest to the goal
	float closestAreaDist = startArea->GetTotalCost();

	// do A* search
	while (!CNavArea::IsOpenListEmpty())
	{
		// get next area to check
		CNavArea* area = CNavArea::PopOpenList();

#ifdef STAGING_ONLY
		if (isDebug)
		{
			area->DrawFilled(0, 255, 0, 128, 30.0f);
		}
#endif

		// don't consider blocked areas
		if (area->IsBlocked(teamID, ignoreNavBlockers))
			continue;

		// check if we have found the goal area or position
		if (area == goalArea || (goalArea == NULL && goalPos && area->Contains(*goalPos)))
		{
			if (closestArea)
			{
				*closestArea = area;
			}

			return true;
		}

		// search adjacent areas
		enum SearchType
		{
			SEARCH_FLOOR, SEARCH_LADDERS, SEARCH_ELEVATORS
		};
		SearchType searchWhere = SEARCH_FLOOR;
		int searchIndex = 0;

		int dir = NORTH;
		const NavConnectVector* floorList = area->GetAdjacentAreas(NORTH);

		bool ladderUp = true;
		const NavLadderConnectVector* ladderList = NULL;
		enum { AHEAD = 0, LEFT, RIGHT, BEHIND, NUM_TOP_DIRECTIONS };
		int ladderTopDir = AHEAD;
		bool bHaveMaxPathLength = (maxPathLength > 0.0f);
		float length = -1;

		while (true)
		{
			CNavArea* newArea = NULL;
			NavTraverseType how;
			const CNavLadder* ladder = NULL;
			const CFuncElevator* elevator = NULL;

			//
			// Get next adjacent area - either on floor or via ladder
			//
			if (searchWhere == SEARCH_FLOOR)
			{
				// if exhausted adjacent connections in current direction, begin checking next direction
				if (searchIndex >= floorList->Count())
				{
					++dir;

					if (dir == NUM_DIRECTIONS)
					{
						// checked all directions on floor - check ladders next
						searchWhere = SEARCH_LADDERS;

						ladderList = area->GetLadders(CNavLadder::LADDER_UP);
						searchIndex = 0;
						ladderTopDir = AHEAD;
					}
					else
					{
						// start next direction
						floorList = area->GetAdjacentAreas((NavDirType)dir);
						searchIndex = 0;
					}

					continue;
				}

				const NavConnect& floorConnect = floorList->Element(searchIndex);
				newArea = floorConnect.area;
				length = floorConnect.length;
				how = (NavTraverseType)dir;
				++searchIndex;

				if (IsX360() && searchIndex < floorList->Count())
				{
					PREFETCH360(floorList->Element(searchIndex).area, 0);
				}
			}
			else if (searchWhere == SEARCH_LADDERS)
			{
				if (searchIndex >= ladderList->Count())
				{
					if (!ladderUp)
					{
						// checked both ladder directions - check elevators next
						searchWhere = SEARCH_ELEVATORS;
						searchIndex = 0;
						ladder = NULL;
					}
					else
					{
						// check down ladders
						ladderUp = false;
						ladderList = area->GetLadders(CNavLadder::LADDER_DOWN);
						searchIndex = 0;
					}
					continue;
				}

				if (ladderUp)
				{
					ladder = ladderList->Element(searchIndex).ladder;

					// do not use BEHIND connection, as its very hard to get to when going up a ladder
					if (ladderTopDir == AHEAD)
					{
						newArea = ladder->m_topForwardArea;
					}
					else if (ladderTopDir == LEFT)
					{
						newArea = ladder->m_topLeftArea;
					}
					else if (ladderTopDir == RIGHT)
					{
						newArea = ladder->m_topRightArea;
					}
					else
					{
						++searchIndex;
						ladderTopDir = AHEAD;
						continue;
					}

					how = GO_LADDER_UP;
					++ladderTopDir;
				}
				else
				{
					newArea = ladderList->Element(searchIndex).ladder->m_bottomArea;
					how = GO_LADDER_DOWN;
					ladder = ladderList->Element(searchIndex).ladder;
					++searchIndex;
				}

				if (newArea == NULL)
					continue;

				length = -1.0f;
			}
			else // if ( searchWhere == SEARCH_ELEVATORS )
			{
				const NavConnectVector& elevatorAreas = area->GetElevatorAreas();

				elevator = area->GetElevator();

				if (elevator == NULL || searchIndex >= elevatorAreas.Count())
				{
					// done searching connected areas
					elevator = NULL;
					break;
				}

				newArea = elevatorAreas[searchIndex++].area;
				if (newArea->GetCenter().z > area->GetCenter().z)
				{
					how = GO_ELEVATOR_UP;
				}
				else
				{
					how = GO_ELEVATOR_DOWN;
				}

				length = -1.0f;
			}


			// don't backtrack
			Assert(newArea);
			if (newArea == area->GetParent())
				continue;
			if (newArea == area) // self neighbor?
				continue;

			// don't consider blocked areas
			if (newArea->IsBlocked(teamID, ignoreNavBlockers))
				continue;

			float newCostSoFar = costFunc(newArea, area, ladder, elevator, length);

			// NaNs really mess this function up causing tough to track down hangs. If
			//  we get inf back, clamp it down to a really high number.
			DebuggerBreakOnNaN_StagingOnly(newCostSoFar);
			if (IS_NAN(newCostSoFar))
				newCostSoFar = 1e30f;

			// check if cost functor says this area is a dead-end
			if (newCostSoFar < 0.0f)
				continue;

			// Safety check against a bogus functor.  The cost of the path
			// A...B, C should always be at least as big as the path A...B.
			Assert(newCostSoFar >= area->GetCostSoFar());

			// And now that we've asserted, let's be a bit more defensive.
			// Make sure that any jump to a new area incurs some pathfinsing
			// cost, to avoid us spinning our wheels over insignificant cost
			// benefit, floating point precision bug, or busted cost functor.
			float minNewCostSoFar = area->GetCostSoFar() * 1.00001f + 0.00001f;
			newCostSoFar = Max(newCostSoFar, minNewCostSoFar);

			// stop if path length limit reached
			if (bHaveMaxPathLength)
			{
				// keep track of path length so far
				float deltaLength = (newArea->GetCenter() - area->GetCenter()).Length();
				float newLengthSoFar = area->GetPathLengthSoFar() + deltaLength;
				if (newLengthSoFar > maxPathLength)
					continue;

				newArea->SetPathLengthSoFar(newLengthSoFar);
			}

			if ((newArea->IsOpen() || newArea->IsClosed()) && newArea->GetCostSoFar() <= newCostSoFar)
			{
				// this is a worse path - skip it
				continue;
			}
			else
			{
				// compute estimate of distance left to go
				float distSq = (newArea->GetCenter() - actualGoalPos).LengthSqr();
				float newCostRemaining = (distSq > 0.0) ? FastSqrt(distSq) : 0.0;

				// track closest area to goal in case path fails
				if (closestArea && newCostRemaining < closestAreaDist)
				{
					*closestArea = newArea;
					closestAreaDist = newCostRemaining;
				}

				newArea->SetCostSoFar(newCostSoFar);
				newArea->SetTotalCost(newCostSoFar + newCostRemaining);

				if (newArea->IsClosed())
				{
					newArea->RemoveFromClosedList();
				}

				if (newArea->IsOpen())
				{
					// area already on open list, update the list order to keep costs sorted
					newArea->UpdateOnOpenList();
				}
				else
				{
					newArea->AddToOpenList();
				}

				newArea->SetParent(area, how);
			}
		}

		// we have searched this area
		area->AddToClosedList();
	}

	return false;
	/*if (!BuildPath)
	{
		if (!g_pGameConf->GetMemSig("NavAreaBuildPath", reinterpret_cast<void **>(&BuildPath)) || !BuildPath)
		{
			BuildPath = nullptr;
			return false;
		}
	}
	return (*BuildPath)(startArea, goalArea, goalPos, costFunc, closestArea, maxPathLength, teamID, ignoreNavBlockers);*/
}