#include "extension.h"
#include <NextBot/Path/NextBotPath.h>
#include <sourcesdk/nav_threaded.h>

// Hardcoded the vtable offsets, they're very unlikely to move
// However if I'm proved wrong, well I guess i'll stop being lazy
#ifdef _WINDOWS
SH_DECL_MANUALHOOK0_void(MHook_CleanUpMap, 225, 0, 0);
#else
SH_DECL_MANUALHOOK0_void(MHook_CleanUpMap, 227, 0, 0);
#endif

unsigned int CTNavArea::m_masterMarker = 1;
CTNavArea* CTNavArea::m_openList = NULL;
CTNavArea* CTNavArea::m_openListTail = NULL;
CUtlMap<unsigned int, CTNavArea*> CTNavArea::m_copiedNavAreas;

// Static funcs

void CTNavArea::Init(void)
{
	SetDefLessFunc( m_copiedNavAreas );
}

void CTNavArea::MakeNewMarker( void )
{
	++m_masterMarker;
	if (m_masterMarker == 0) m_masterMarker = 1;
}

void CTNavArea::ClearSearchLists( void )
{
	CTNavArea::MakeNewMarker();

	m_openList = NULL;
	m_openListTail = NULL;
}

// Members func

void CTNavArea::AddToOpenList( void )
{
	if ( IsOpen() )
	{
		// already on list
		return;
	}

	// mark as being on open list for quick check
	m_openMarker = m_masterMarker;

	// if list is empty, add and return
	if ( m_openList == NULL )
	{
		m_openList = this;
		m_openListTail = this;
		this->m_prevOpen = NULL;
		this->m_nextOpen = NULL;
		return;
	}

	// insert self in ascending cost order
	CTNavArea *area, *last = NULL;
	for( area = m_openList; area; area = area->m_nextOpen )
	{
		if ( GetTotalCost() < area->GetTotalCost() )
		{
			break;
		}
		last = area;
	}

	if ( area )
	{
		// insert before this area
		this->m_prevOpen = area->m_prevOpen;

		if ( this->m_prevOpen )
		{
			this->m_prevOpen->m_nextOpen = this;
		}
		else
		{
			m_openList = this;
		}

		this->m_nextOpen = area;
		area->m_prevOpen = this;
	}
	else
	{
		// append to end of list
		last->m_nextOpen = this;
		this->m_prevOpen = last;
	
		this->m_nextOpen = NULL;

		m_openListTail = this;
	}
}

void CTNavArea::UpdateOnOpenList(void)
{
	// since value can only decrease, bubble this area up from current spot
	while (m_prevOpen && this->GetTotalCost() < m_prevOpen->GetTotalCost())
	{
		// swap position with predecessor
		CTNavArea* other = m_prevOpen;
		CTNavArea* before = other->m_prevOpen;
		CTNavArea* after = this->m_nextOpen;

		this->m_nextOpen = other;
		this->m_prevOpen = before;

		other->m_prevOpen = this;
		other->m_nextOpen = after;

		if (before)
		{
			before->m_nextOpen = this;
		}
		else
		{
			m_openList = this;
		}

		if (after)
		{
			after->m_prevOpen = other;
		}
		else
		{
			m_openListTail = this;
		}
	}
}

void CTNavArea::RemoveFromOpenList( void )
{
	if ( m_openMarker == 0 )
	{
		// not on the list
		return;
	}

	if ( m_prevOpen )
	{
		m_prevOpen->m_nextOpen = m_nextOpen;
	}
	else
	{
		m_openList = m_nextOpen;
	}
	
	if ( m_nextOpen )
	{
		m_nextOpen->m_prevOpen = m_prevOpen;
	}
	else
	{
		m_openListTail = m_prevOpen;
	}
	
	// zero is an invalid marker
	m_openMarker = 0;
}

ke::AutoPtr<ke::Thread> CTNavMesh::m_CollectWorker;
ke::ConditionVariable CTNavMesh::m_CollectEvent;
CUtlQueue<CTNavMesh::NavThreadedData*> CTNavMesh::m_QueueCollect;
ke::Mutex CTNavMesh::m_CBLock;
CUtlQueue<CTNavMesh::NavThreadedData*> CTNavMesh::m_QueueCB;
bool CTNavMesh::m_ThreadTerminate = false;
int CTNavMesh::m_hookCleanUpID = 0;

void CTNavMesh::Init(void)
{
	CTNavArea::Init();

	g_pSM->AddGameFrameHook(&CTNavMesh::OnFrame);
	m_CollectWorker = nullptr;
}

void CTNavMesh::RefreshHooks(void)
{
	if (!g_pSDKTools) return;

	if (m_hookCleanUpID)
		SH_REMOVE_HOOK_ID(m_hookCleanUpID);

	void* g_pGameRules = g_pSDKTools->GetGameRules();
	if (!g_pGameRules)
	{
		g_pSM->LogMessage(myself, "Failed to hook CTeamplayRoundBasedRules::CleanUpMap");
		return;
	}
	m_hookCleanUpID = SH_ADD_MANUALHOOK(MHook_CleanUpMap, g_pSDKTools->GetGameRules(), &CTNavMesh::CleanUpMap, false);
	g_pSM->LogMessage(myself, "Hooking CTeamplayRoundBasedRules::CleanUpMap");
}

void CTNavMesh::CleanUpMap(void)
{
	CTNavMesh::CleanUp();
	RETURN_META(MRES_IGNORED);
}

void CTNavMesh::CleanUp(void)
{
	CTNavMesh::KillCollectThread();
	CTNavArea::m_copiedNavAreas.PurgeAndDeleteElements();
	
	while (!m_QueueCollect.IsEmpty())
	{
		NavThreadedData* pData = m_QueueCollect.RemoveAtHead();

		if (pData->GetDataType() == NavThreadedData::COLLECTNAV)
		{
			CollectNavThreadedData* pCollect = (CollectNavThreadedData*)pData;
			delete pCollect->m_pCollector;
			delete pCollect;
		}
		else
		{
			NavPathThreadedData* pCompute = (NavPathThreadedData*)pData;
			delete pCompute;
		}
	}

	while (!m_QueueCB.IsEmpty())
	{
		NavThreadedData* pData = m_QueueCB.RemoveAtHead();

		if (pData->GetDataType() == NavThreadedData::COLLECTNAV)
			delete ((CollectNavThreadedData*)pData)->m_pCollector;

		delete pData;
	}
}

void CTNavMesh::Add(CNavArea* area)
{
	new CTNavArea(area);
}

void CTNavMesh::Compute(NavThreadedData* pData)
{
	if (!m_CollectWorker)
	{
		m_CollectWorker = new ke::Thread([]() -> void {CTNavMesh::RunThread(); }, "CBaseNPC Nav Thread");
		if (!m_CollectWorker->Succeeded())
		{
			m_CollectWorker = nullptr;
			m_ThreadTerminate = false;
			return;
		}
	}

	ke::AutoLock lock(&m_CollectEvent);
	m_QueueCollect.Insert(pData);
	m_CollectEvent.Notify();
}

void CTNavMesh::RunThread(void)
{
	ke::AutoLock lock(&m_CollectEvent);
	while (true)
	{
		if (m_ThreadTerminate) return;

		while (!m_QueueCollect.IsEmpty()) // Always process the queue as long as we have data waiting
		{
			if (m_ThreadTerminate) return;

			NavThreadedData* pData = m_QueueCollect.RemoveAtHead();
			ke::AutoUnlock unlock(&m_CollectEvent); // Free queue ownership so plugins can continue to add more collect request

			if (pData->GetDataType() == NavThreadedData::COLLECTNAV)
				CTNavMesh::CollectSurroundingAreas_Threaded((CollectNavThreadedData*)pData); // Run the collection
			else
			{
				NavPathThreadedData* compute = (NavPathThreadedData*)pData;
				compute->m_bSuccess = compute->m_path->ComputeT(compute);
			}

			ke::AutoLock lock(&m_CBLock);
			m_QueueCB.Insert(pData);
		}

		// Unlock and wait for a notification
		m_CollectEvent.Wait();
	}
}
 
void CTNavMesh::CollectSurroundingAreas_Threaded( CollectNavThreadedData* pData )
{
	auto nearbyAreaVector = pData->m_pCollector;
	CTNavArea* startArea = CTNavArea::Find(pData->m_pStartArea);

	float travelDistanceLimit = pData->m_flTravelDistance;
	float maxStepUpLimit = pData->m_flStepUpLimit;
	float maxDropDownLimit = pData->m_flMaxDropDown;

	nearbyAreaVector->RemoveAll();

	if ( startArea )
	{
		CTNavArea::MakeNewMarker();
		CTNavArea::ClearSearchLists();
		
		startArea->AddToOpenList();
		startArea->SetTotalCost( 0.0f );
		startArea->SetCostSoFar( 0.0f );
		startArea->SetParent( NULL );
		startArea->Mark();

		CUtlVector< CTNavArea * > adjVector;

		while( !CTNavArea::IsOpenListEmpty() )
		{
			// get next area to check
			CTNavArea *area = CTNavArea::PopOpenList();

			if ( travelDistanceLimit > 0.0f && area->GetCostSoFar() > travelDistanceLimit )
				continue;

			if ( area->GetParent() )
			{
				float deltaZ = area->GetParent()->GetRealNavArea()->ComputeAdjacentConnectionHeightChange( area->GetRealNavArea() );

				if ( deltaZ > maxStepUpLimit )
					continue;

				if ( deltaZ < -maxDropDownLimit )
					continue;
			}

			nearbyAreaVector->AddToTail( *area );

			// mark here to ensure all marked areas are also valid areas that are in the collection
			area->Mark();

			// search adjacent outgoing connections
			for( int dir=0; dir<NUM_DIRECTIONS; ++dir )
			{
				int count = area->GetRealNavArea()->GetAdjacentCount( (NavDirType)dir );
				for( int i=0; i<count; ++i )
				{
					CTNavArea *adjArea = CTNavArea::Find(area->GetRealNavArea()->GetAdjacentArea( (NavDirType)dir, i ));

					if ( adjArea->GetRealNavArea()->IsBlocked( TEAM_ANY ) )
					{
						continue;
					}

					if ( !adjArea->IsMarked() )
					{
						adjArea->SetTotalCost( 0.0f );
						adjArea->SetParent( area );

						// compute approximate travel distance from start area of search
						float distAlong = area->GetCostSoFar();
						distAlong += ( adjArea->GetRealNavArea()->GetCenter() - area->GetRealNavArea()->GetCenter() ).Length();
						adjArea->SetCostSoFar( distAlong );
						adjArea->AddToOpenList();
					}
				}
			}
		}
	}
}

void CTNavMesh::OnFrame(bool simulating)
{
	for (size_t i = 0; i < 5 && !m_QueueCB.IsEmpty(); i++) // TO-DO: Implement a ConVar to control callback fire rate
	{
		NavThreadedData* pData = nullptr;
		{
			ke::AutoLock lock(&m_CBLock);
			pData = m_QueueCB.RemoveAtHead();
		}

		IPluginFunction* pCallback = pData->m_pFunction;
		if (pData->GetDataType() == NavThreadedData::COLLECTNAV)
		{
			CollectNavThreadedData* pCollect = (CollectNavThreadedData *)pData;
			// Fire the plugin callback
			if (pCallback->IsRunnable())
			{
				IPluginContext* pContext = pCallback->GetParentRuntime()->GetDefaultContext();
				pCallback->PushCell(CREATEHANDLE(TSurroundingAreasCollector, pCollect->m_pCollector));
				pCallback->PushCell(pCollect->m_data);
				pCallback->Execute(0);
			}
			else
			{
				delete pCollect->m_pCollector;
			}

			delete pCollect;
		}
		else
		{
			NavPathThreadedData* pCompute = (NavPathThreadedData*)pData;
			pCompute->m_path->OnPathChanged(pCompute->m_bot, (pCompute->m_bSuccess) ? Path::COMPLETE_PATH : Path::NO_PATH);

			if (pCallback->IsRunnable())
			{
				pCallback->PushCell((cell_t)pCompute->m_path);
				pCallback->PushCell(pCompute->m_bSuccess);
				pCallback->PushCell(pCompute->m_data);
				pCallback->Execute(0);
			}

			delete pCompute;
		}
	}
}

void CTNavMesh::KillCollectThread(void)
{
	if (m_CollectWorker)
	{
		{
			ke::AutoLock lock(&m_CollectEvent);
			m_ThreadTerminate = true;
			m_CollectEvent.Notify();
		}
		m_CollectWorker->Join();
		m_CollectWorker = nullptr;
		m_ThreadTerminate = false;
	}
}

/*bool CTNavMesh::NavAreaBuildPath_Threaded(CTNavArea* startArea, CTNavArea* goalArea, const Vector* goalPos, NavPathThreadedData* costFunc, CTNavArea** closestArea, float maxPathLength, int teamID, bool ignoreNavBlockers)
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
	CNavArea *realGoalArea = goalArea->GetRealNavArea();
	CNavArea *realStartArea = startArea->GetRealNavArea();

	if (goalArea != NULL && realGoalArea->IsBlocked(teamID, ignoreNavBlockers))
		goalArea = NULL;

	if (goalArea == NULL && goalPos == NULL)
		return false;

	// if we are already in the goal area, build trivial path
	if (startArea == goalArea)
	{
		return true;
	}

	// determine actual goal position
	Vector actualGoalPos = (goalPos) ? *goalPos : realGoalArea->GetCenter();

	// start search
	CTNavArea::ClearSearchLists();

	// compute estimate of path length
	/// @todo Cost might work as "manhattan distance"
	startArea->SetTotalCost((realStartArea->GetCenter() - actualGoalPos).Length());

	float initCost = costFunc->operator()(startArea, NULL, NULL, NULL, -1.0f);
	if (initCost < 0.0f)
		return false;
	startArea->SetCostSoFar(initCost);
	startArea->SetPathLengthSoFar(0.0);

	startArea->AddToOpenList();

	// keep track of the area we visit that is closest to the goal
	float closestAreaDist = startArea->GetTotalCost();

	// do A* search
	while (!CTNavArea::IsOpenListEmpty())
	{
		// get next area to check
		CTNavArea *area = CTNavArea::PopOpenList();
		CNavArea *realArea = area->GetRealNavArea();
#ifdef STAGING_ONLY
		if (isDebug)
		{
			area->DrawFilled(0, 255, 0, 128, 30.0f);
		}
#endif

		// don't consider blocked areas
		if (realArea->IsBlocked(teamID, ignoreNavBlockers))
			continue;

		// check if we have found the goal area or position
		if (area == goalArea || (goalArea == NULL && goalPos && realArea->Contains(*goalPos)))
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
		const NavConnectVector* floorList = realArea->GetAdjacentAreas(NORTH);

		bool ladderUp = true;
		const NavLadderConnectVector* ladderList = NULL;
		enum { AHEAD = 0, LEFT, RIGHT, BEHIND, NUM_TOP_DIRECTIONS };
		int ladderTopDir = AHEAD;
		bool bHaveMaxPathLength = (maxPathLength > 0.0f);
		float length = -1;

		while (true)
		{
			CNavArea* realNewArea = NULL;

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

						ladderList = realArea->GetLadders(CNavLadder::LADDER_UP);
						searchIndex = 0;
						ladderTopDir = AHEAD;
					}
					else
					{
						// start next direction
						floorList = realArea->GetAdjacentAreas((NavDirType)dir);
						searchIndex = 0;
					}

					continue;
				}

				const NavConnect& floorConnect = floorList->Element(searchIndex);
				realNewArea = floorConnect.area;
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
						ladderList = realArea->GetLadders(CNavLadder::LADDER_DOWN);
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
						realNewArea = ladder->m_topForwardArea;
					}
					else if (ladderTopDir == LEFT)
					{
						realNewArea = ladder->m_topLeftArea;
					}
					else if (ladderTopDir == RIGHT)
					{
						realNewArea = ladder->m_topRightArea;
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
					realNewArea = ladderList->Element(searchIndex).ladder->m_bottomArea;
					how = GO_LADDER_DOWN;
					ladder = ladderList->Element(searchIndex).ladder;
					++searchIndex;
				}

				if (realNewArea == NULL)
					continue;

				length = -1.0f;
			}
			else // if ( searchWhere == SEARCH_ELEVATORS )
			{
				const NavConnectVector& elevatorAreas = realArea->GetElevatorAreas();

				elevator = realArea->GetElevator();

				if (elevator == NULL || searchIndex >= elevatorAreas.Count())
				{
					// done searching connected areas
					elevator = NULL;
					break;
				}

				realNewArea = elevatorAreas[searchIndex++].area;
				if (realNewArea->GetCenter().z > realArea->GetCenter().z)
				{
					how = GO_ELEVATOR_UP;
				}
				else
				{
					how = GO_ELEVATOR_DOWN;
				}

				length = -1.0f;
			}

			CTNavArea* newArea = CTNavArea::Find(realNewArea);
			// don't backtrack
			Assert(newArea);
			if (newArea == area->GetParent())
				continue;
			if (newArea == area) // self neighbor?
				continue;

			// don't consider blocked areas
			if (realNewArea->IsBlocked(teamID, ignoreNavBlockers))
				continue;

			float newCostSoFar = costFunc->operator()(newArea, area, ladder, elevator, length);

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
				float deltaLength = (realNewArea->GetCenter() - realArea->GetCenter()).Length();
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
				float distSq = (realNewArea->GetCenter() - actualGoalPos).LengthSqr();
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
}*/