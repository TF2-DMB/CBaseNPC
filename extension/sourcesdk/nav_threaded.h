#ifndef NAV_THREADE_H
#define NAV_THREADE_H

#include "extension.h"
#include "sourcesdk/nav_mesh.h"
#include <NextBot/NextBotInterface.h>
#include <NextBot/NextBotBodyInterface.h>
#include <NextBot/NextBotGroundLocomotion.h>
#include <amtl/am-thread-utils.h>
#include <tier1/utlvector.h>
#include <tier1/utlqueue.h>

#pragma once

class CTNavArea
{
private:
	unsigned int m_marker;
	float m_totalCost;
	float m_costSoFar;
	float m_pathLengthSoFar;
	
	CTNavArea *m_nextOpen, *m_prevOpen;
	unsigned int m_openMarker;
	
	CTNavArea *m_parent;
	NavTraverseType m_parentHow;
	
public:
	CTNavArea(CNavArea* area) : m_realNavArea(area) { m_copiedNavAreas.Insert(area->GetID(), this); };


	static void Init( void );
	static void MakeNewMarker( void );
	static void ClearSearchLists( void );
	static bool IsOpenListEmpty( void );
	static CTNavArea *PopOpenList( void );
	static CTNavArea* Find( CNavArea* area );
	
	void SetCostSoFar( float value )	{ m_costSoFar = value; }
	float GetCostSoFar( void ) const	{ return m_costSoFar; }
	
	void SetTotalCost( float value )	{ m_totalCost = value; }
	float GetTotalCost( void ) const	{ return m_totalCost; }

	void SetPathLengthSoFar(float value) { m_pathLengthSoFar = value; }
	float GetPathLengthSoFar(void) const { return m_pathLengthSoFar; }
	
	void SetParent(CTNavArea* parent, NavTraverseType how = NUM_TRAVERSE_TYPES) { m_parent = parent; m_parentHow = how; }
	CTNavArea *GetParent( void ) const	{ return m_parent; }
	NavTraverseType GetParentHow(void) const { return m_parentHow; }
	
	void AddToOpenList( void );
	void RemoveFromOpenList( void );
	void UpdateOnOpenList(void);
	bool IsOpen( void ) const;
	bool IsClosed(void) const;
	void AddToClosedList(void);
	void RemoveFromClosedList(void);

	void Mark( void )					{ m_marker = m_masterMarker; };
	BOOL IsMarked(void) const { return (m_marker == m_masterMarker) ? true : false; };
	
	CNavArea *GetRealNavArea( void ) const { return m_realNavArea; };
	
public:
	CNavArea *m_realNavArea;
	static CUtlMap<unsigned int, CTNavArea*> m_copiedNavAreas;
private:
	static unsigned int m_masterMarker;
	static CTNavArea* m_openList;
	static CTNavArea* m_openListTail;
};

// Static funcs

inline bool CTNavArea::IsOpenListEmpty( void )
{
	return (m_openList) ? false : true;
}

inline CTNavArea *CTNavArea::PopOpenList( void )
{
	if ( m_openList )
	{
		CTNavArea *area = m_openList;
	
		// disconnect from list
		area->RemoveFromOpenList();
		area->m_prevOpen = NULL;
		area->m_nextOpen = NULL;

		return area;
	}

	return nullptr;
}

inline CTNavArea *CTNavArea::Find( CNavArea *area )
{
	if (!area) return nullptr;

	unsigned short index = m_copiedNavAreas.Find( area->GetID() );
	if (m_copiedNavAreas.IsValidIndex(index))
		return m_copiedNavAreas.Element(index);
	return new CTNavArea(area);
}

// Member funcs

inline bool CTNavArea::IsOpen( void ) const
{
	return (m_openMarker == m_masterMarker) ? true : false;
}


inline bool CTNavArea::IsClosed(void) const
{
	if (IsMarked() && !IsOpen())
		return true;

	return false;
}

inline void CTNavArea::AddToClosedList(void)
{
	Mark();
}

inline void CTNavArea::RemoveFromClosedList(void)
{

}

class CTNavMesh
{
public:
	class NavThreadedData
	{
	public:
		NavThreadedData(IPluginFunction* callback, cell_t data) : m_pFunction(callback), m_data(data)
		{

		}


		enum ThreadedDataType
		{
			NAVPATH = 0,
			COLLECTNAV
		};

		virtual ThreadedDataType GetDataType() = 0;
		IPluginFunction* m_pFunction;
		cell_t m_data;
	};

	class NavPathThreadedData : public IPathCost, public NavThreadedData
	{
	public:
		virtual ThreadedDataType GetDataType() override { return NAVPATH; };

		NavPathThreadedData(INextBot* pBot, Path *path, Vector end, CBaseCombatCharacter* goal, float maxPathLength, bool includeGoalIfPathFails, IPluginFunction* callback, cell_t data) : NavThreadedData(callback, data)
		{
			ILocomotion* mover = pBot->GetLocomotionInterface();
			m_flDeathDropHeight = mover->GetDeathDropHeight();
			m_flStepHeight = mover->GetStepHeight();
			m_flJumpHeight = mover->GetMaxJumpHeight();

			IBody* body = pBot->GetBodyInterface();
			m_flHullWidth = body->GetHullWidth();
			m_flCrouchHullHeight = body->GetCrouchHullHeight();
			m_iSolidMask = body->GetSolidMask();

			m_entity = pBot->GetEntity();
			m_vecStart = pBot->GetPosition();
			m_startArea = pBot->GetEntity()->GetLastKnownArea();
			if (goal)
			{
				m_vecEnd = goal->GetAbsOrigin();
				m_endArea = goal->GetLastKnownArea();
			}
			else
			{
				m_vecEnd = end;
				m_endArea = nullptr;
			}
			m_bot = pBot;
			m_iTeam = pBot->GetEntity()->GetTeamNumber();
			m_flMaxPathLength = maxPathLength;
			m_bIncludeGoal = includeGoalIfPathFails;
			m_pFunction = callback;
			m_data = data;
			m_path = path;
			m_bSuccess = false;
		}

		virtual float operator()(CNavArea* realArea, CNavArea* realFromArea, const CNavLadder* ladder, const CFuncElevator* elevator, float length) const override
		{
			if (realFromArea == NULL)
			{
				return 0.0f;
			}
			else
			{
				if (realArea->IsBlocked(m_iTeam)) return -1.0f;

				float dist;
				if (length > 0.0f) {
					dist = length;
				}
				else {
					dist = (realArea->GetCenter() - realFromArea->GetCenter()).Length();
				}

				/* account for step height, max jump height, death drop height */
				float delta_z = realFromArea->ComputeAdjacentConnectionHeightChange(realArea);
				if (delta_z >= m_flStepHeight) {
					if (delta_z >= m_flJumpHeight) return -1.0f;

					/* cost penalty for going up steps */
					dist *= 2.0f;
				}
				else {
					if (delta_z < -m_flDeathDropHeight) return -1.0f;
				}

				return dist + realFromArea->GetCostSoFar();
			}
		};

		float m_flDeathDropHeight;
		float m_flStepHeight;
		float m_flJumpHeight;

		float m_flHullWidth;
		float m_flCrouchHullHeight;
		unsigned int m_iSolidMask;

		Vector m_vecStart;
		Vector m_vecEnd;
		CNavArea* m_startArea;
		CNavArea* m_endArea;
		INextBot* m_bot;
		IHandleEntity* m_entity;
		Path* m_path;
		int m_iTeam;
		float m_flMaxPathLength;
		bool m_bIncludeGoal;
		bool m_bSuccess;
	};

public :
	class CollectNavThreadedData : public NavThreadedData
	{
	public:
		virtual ThreadedDataType GetDataType() override { return COLLECTNAV; };

		CollectNavThreadedData(CNavArea* startArea, float travelDistanceLimit, float maxStepUpLimit, float maxDropDownLimit, IPluginFunction* callback, cell_t data) :
			NavThreadedData(callback, data),
			m_pStartArea(startArea),
			m_flTravelDistance(travelDistanceLimit),
			m_flStepUpLimit(maxStepUpLimit),
			m_flMaxDropDown(maxDropDownLimit)
		{
			m_pCollector = new CUtlVector< CTNavArea >;
		}

		CUtlVector< CTNavArea >* m_pCollector;
		CNavArea* m_pStartArea;
		float m_flTravelDistance;
		float m_flStepUpLimit;
		float m_flMaxDropDown;
	};

	static void Init(void);
	static void RefreshHooks(void);
	static void CleanUpMap(void);
	static void CleanUp(void);

	static void OnFrame(bool simulating);

	static void Add(CNavArea* area);
	static void Compute(NavThreadedData* pData);
	static void KillCollectThread(void);

private:
	static ke::AutoPtr<ke::Thread> m_CollectWorker;
	static ke::ConditionVariable m_CollectEvent;
	static CUtlQueue<NavThreadedData*> m_QueueCollect;
	static ke::Mutex m_CBLock;
	static CUtlQueue<NavThreadedData*> m_QueueCB;
	static bool m_ThreadTerminate;
	static int m_hookCleanUpID;

	static void RunThread(void);
	static void CollectSurroundingAreas_Threaded(CollectNavThreadedData* pData);
	static bool NavAreaBuildPath_Threaded(CTNavArea* startArea, CTNavArea* goalArea, const Vector* goalPos, NavPathThreadedData *costFunc, CTNavArea** closestArea = NULL, float maxPathLength = 0.0f, int teamID = TEAM_ANY, bool ignoreNavBlockers = false);
};

#endif