#include "extension.h"
#include <sourcesdk/nav_threaded.h>

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

ke::AutoPtr<ke::Thread> CTNavMesh::m_CollectWorker = nullptr;
ke::ConditionVariable CTNavMesh::m_CollectEvent;
CUtlQueue<CTNavMesh::CollectNavThreadedData*> CTNavMesh::m_QueueCollect;
ke::Mutex CTNavMesh::m_CBLock;
CUtlQueue<CTNavMesh::CollectNavThreadedData*> CTNavMesh::m_QueueCB;
bool CTNavMesh::m_CollectTerminate = false;

void CTNavMesh::Init(void)
{
	CTNavArea::Init();
}

void CTNavMesh::CleanUp(void)
{
	CTNavMesh::KillCollectThread();
	FOR_EACH_MAP_FAST(CTNavArea::m_copiedNavAreas, areaID)
		delete CTNavArea::m_copiedNavAreas.Element(areaID);

	while (!m_QueueCollect.IsEmpty())
	{
		CollectNavThreadedData* pData = m_QueueCollect.RemoveAtHead();
		delete pData->m_pCollector;
		delete pData;
	}

	while (!m_QueueCB.IsEmpty())
	{
		CollectNavThreadedData* pData = m_QueueCB.RemoveAtHead();
		delete pData->m_pCollector;
		delete pData;
	}
}

void CTNavMesh::Add(CNavArea* area)
{
	new CTNavArea(area);
}

void CTNavMesh::CollectSurroundingAreas(CollectNavThreadedData* pData)
{
	if (!m_CollectWorker)
	{
		m_CollectWorker = new ke::Thread([]() -> void {CTNavMesh::RunCollectThread();}, "CBaseNPC Nav Thread");
		if (!m_CollectWorker->Succeeded())
		{
			m_CollectWorker = nullptr;
			m_CollectTerminate = false;
			return;
		}
	}

	ke::AutoLock lock(&m_CollectEvent);
	m_QueueCollect.Insert(pData);
	m_CollectEvent.Notify();
}

void CTNavMesh::RunCollectThread(void)
{
	ke::AutoLock lock(&m_CollectEvent);
	while (true)
	{
		if (m_CollectTerminate) return;

		while (!m_QueueCollect.IsEmpty()) // Always process the queue as long as we have data waiting
		{
			if (m_CollectTerminate) return;

			CollectNavThreadedData* pData = m_QueueCollect.RemoveAtHead();
			ke::AutoUnlock unlock(&m_CollectEvent); // Free queue ownership so plugins can continue to add more collect request

			CTNavMesh::CollectSurroundingAreas_Threaded(pData); // Run the collection

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

			nearbyAreaVector->AddToTail( area->GetRealNavArea() );

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

void CTNavMesh::OnFrame(void)
{
	for (size_t i = 0; i < 5 && m_QueueCB.IsEmpty(); i++) // TO-DO: Implement a ConVar to control callback fire rate
	{
		CollectNavThreadedData* pData = nullptr;
		{
			ke::AutoLock lock(&m_CBLock);
			pData = m_QueueCollect.RemoveAtHead();
		}

		// Fire the plugin callback
		IPluginFunction* pCallback = pData->m_pFunction;
		if (pCallback->IsRunnable())
		{
			IPluginContext* pContext = pCallback->GetParentRuntime()->GetDefaultContext();
			pCallback->PushCell(CREATEHANDLE(SurroundingAreasCollector, pData->m_pCollector));
			pCallback->PushCell(pData->m_data);
			pCallback->Execute(0);
		}
		else
		{
			delete pData->m_pCollector;
		}
		delete pData;
	}
}

void CTNavMesh::KillCollectThread(void)
{
	if (m_CollectWorker)
	{
		{
			ke::AutoLock lock(&m_CollectEvent);
			m_CollectTerminate = true;
			m_CollectEvent.Notify();
		}
		m_CollectWorker->Join();
		m_CollectWorker = nullptr;
		m_CollectTerminate = false;
	}
}