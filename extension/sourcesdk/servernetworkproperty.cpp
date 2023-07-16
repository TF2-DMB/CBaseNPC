//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include "servernetworkproperty.h"
#include "smsdk_ext.h"
#include "baseentity.h"
#include <string_t.h>

extern CTimedEventMgr g_NetworkPropertyEventMgr;


//-----------------------------------------------------------------------------
// Save/load
//-----------------------------------------------------------------------------
BEGIN_DATADESC_NO_BASE(CServerNetworkProperty)
//	DEFINE_FIELD( m_pOuter, FIELD_CLASSPTR ),
//	DEFINE_FIELD( m_pPev, FIELD_CLASSPTR ),
//	DEFINE_FIELD( m_PVSInfo, PVSInfo_t ),
//	DEFINE_FIELD( m_pServerClass, FIELD_CLASSPTR ),
//  DEFINE_GLOBAL_FIELD(m_hParent, FIELD_EHANDLE),
//	DEFINE_FIELD( m_TimerEvent, CEventRegister ),
//	DEFINE_FIELD( m_bPendingStateChange, FIELD_BOOLEAN ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Initialization
// Edition: The mod doesn't create entity, so we will never to create our own CServerNetworkProperty
// on top of that it avoid unnecessary undefined symbol g_NetworkPropertyEventMgr
//-----------------------------------------------------------------------------
/*
void CServerNetworkProperty::Init(CBaseEntityHack* pEntity)
{
	m_pPev = NULL;
	m_pOuter = pEntity;
	m_pServerClass = NULL;
	//	m_pTransmitProxy = NULL;
	m_bPendingStateChange = false;
	m_PVSInfo.m_nClusterCount = 0;
	m_TimerEvent.Init(&g_NetworkPropertyEventMgr, this);
}
*/

//-----------------------------------------------------------------------------
// Connects, disconnects edicts
//-----------------------------------------------------------------------------
void CServerNetworkProperty::AttachEdict(edict_t* pRequiredEdict)
{
	Assert(!m_pPev);

	// see if there is an edict allocated for it, otherwise get one from the engine
	if (!pRequiredEdict)
	{
		pRequiredEdict = engine->CreateEdict();
	}

	m_pPev = pRequiredEdict;
	m_pPev->SetEdict((IServerUnknown*)GetBaseEntity(), true);
}

void CServerNetworkProperty::DetachEdict()
{
	if (m_pPev)
	{
		m_pPev->SetEdict(NULL, false);
		engine->RemoveEdict(m_pPev);
		m_pPev = NULL;
	}
}


//-----------------------------------------------------------------------------
// Entity handles
//-----------------------------------------------------------------------------
IHandleEntity* CServerNetworkProperty::GetEntityHandle()
{
	return m_pOuter;
}

void CServerNetworkProperty::Release()
{
	delete m_pOuter;
	// Don't zero m_pOuter or reference any member variables after
	// the delete call because the object may be deleted.
	//m_pOuter = NULL;
}


//-----------------------------------------------------------------------------
// Returns the network parent
//-----------------------------------------------------------------------------
CServerNetworkProperty* CServerNetworkProperty::GetNetworkParent()
{
	CBaseEntity* pParent = m_hParent.Get();
	return pParent ? pParent->NetworkProp() : NULL;
}


//-----------------------------------------------------------------------------
// Marks for deletion
//-----------------------------------------------------------------------------
/*void CServerNetworkProperty::MarkForDeletion()
{
	m_pOuter->AddEFlags(EFL_KILLME);
}

bool CServerNetworkProperty::IsMarkedForDeletion() const
{
	return (m_pOuter->GetEFlags() & EFL_KILLME) != 0;
}*/


//-----------------------------------------------------------------------------
// PVS information
//-----------------------------------------------------------------------------
void CServerNetworkProperty::RecomputePVSInformation()
{
	if (m_pPev && ((m_pPev->m_fStateFlags & FL_EDICT_DIRTY_PVS_INFORMATION) != 0))
	{
		m_pPev->m_fStateFlags &= ~FL_EDICT_DIRTY_PVS_INFORMATION;
		engine->BuildEntityClusterList(edict(), &m_PVSInfo);
	}
}


//-----------------------------------------------------------------------------
// Serverclass
//-----------------------------------------------------------------------------
ServerClass* CServerNetworkProperty::GetServerClass()
{
	if (!m_pServerClass && m_pPev)
		m_pServerClass = m_pPev->GetNetworkable()->GetServerClass();// m_pOuter->GetServerClass();
	return m_pServerClass;
}

//-----------------------------------------------------------------------------
// Transmit proxies
/*-----------------------------------------------------------------------------
void CServerNetworkProperty::SetTransmitProxy( CBaseTransmitProxy *pProxy )
{
	if ( m_pTransmitProxy )
	{
		m_pTransmitProxy->Release();
	}

	m_pTransmitProxy = pProxy;

	if ( m_pTransmitProxy )
	{
		m_pTransmitProxy->AddRef();
	}
}*/

//-----------------------------------------------------------------------------
// PVS rules
//-----------------------------------------------------------------------------
bool CServerNetworkProperty::IsInPVS(const edict_t* pRecipient, const void* pvs, int pvssize)
{
	RecomputePVSInformation();

	// ignore if not touching a PV leaf
	// negative leaf count is a node number
	// If no pvs, add any entity

	Assert(pvs && (edict() != pRecipient));

	unsigned char* pPVS = (unsigned char*)pvs;

	if (m_PVSInfo.m_nClusterCount < 0)   // too many clusters, use headnode
	{
		return (engine->CheckHeadnodeVisible(m_PVSInfo.m_nHeadNode, pPVS, pvssize) != 0);
	}

	for (int i = m_PVSInfo.m_nClusterCount; --i >= 0; )
	{
		if (pPVS[m_PVSInfo.m_pClusters[i] >> 3] & (1 << (m_PVSInfo.m_pClusters[i] & 7)))
			return true;
	}

	return false;		// not visible
}


//-----------------------------------------------------------------------------
// PVS: this function is called a lot, so it avoids function calls
//-----------------------------------------------------------------------------
bool CServerNetworkProperty::IsInPVS(const CCheckTransmitInfo* pInfo)
{
	// PVS data must be up to date
	Assert(!m_pPev || ((m_pPev->m_fStateFlags & FL_EDICT_DIRTY_PVS_INFORMATION) == 0));

	int i;

	// Early out if the areas are connected
	if (!m_PVSInfo.m_nAreaNum2)
	{
		for (i = 0; i < pInfo->m_AreasNetworked; i++)
		{
			int clientArea = pInfo->m_Areas[i];
			if (clientArea == m_PVSInfo.m_nAreaNum || engine->CheckAreasConnected(clientArea, m_PVSInfo.m_nAreaNum))
				break;
		}
	}
	else
	{
		// doors can legally straddle two areas, so
		// we may need to check another one
		for (i = 0; i < pInfo->m_AreasNetworked; i++)
		{
			int clientArea = pInfo->m_Areas[i];
			if (clientArea == m_PVSInfo.m_nAreaNum || clientArea == m_PVSInfo.m_nAreaNum2)
				break;

			if (engine->CheckAreasConnected(clientArea, m_PVSInfo.m_nAreaNum))
				break;

			if (engine->CheckAreasConnected(clientArea, m_PVSInfo.m_nAreaNum2))
				break;
		}
	}

	if (i == pInfo->m_AreasNetworked)
	{
		// areas not connected
		return false;
	}

	// ignore if not touching a PV leaf
	// negative leaf count is a node number
	// If no pvs, add any entity

	Assert(edict() != pInfo->m_pClientEnt);

	unsigned char* pPVS = (unsigned char*)pInfo->m_PVS;

	if (m_PVSInfo.m_nClusterCount < 0)   // too many clusters, use headnode
	{
		return (engine->CheckHeadnodeVisible(m_PVSInfo.m_nHeadNode, pPVS, pInfo->m_nPVSSize) != 0);
	}

	for (i = m_PVSInfo.m_nClusterCount; --i >= 0; )
	{
		int nCluster = m_PVSInfo.m_pClusters[i];
		if (((int)(pPVS[nCluster >> 3]))& BitVec_BitInByte(nCluster))
			return true;
	}

	return false;		// not visible

}


void CServerNetworkProperty::SetUpdateInterval(float val)
{
	if (val == 0)
		m_TimerEvent.StopUpdates();
	else
		m_TimerEvent.SetUpdateInterval(val);
}


void CServerNetworkProperty::FireEvent()
{
	// Our timer went off. If our state has changed in the background, then 
	// trigger a state change in the edict.
	if (m_bPendingStateChange)
	{
		m_pPev->StateChanged();
		m_bPendingStateChange = false;
	}
}


IChangeInfoAccessor* CBaseEdict::GetChangeAccessor()
{
	return engine->GetChangeAccessor((const edict_t*)this);
}

const IChangeInfoAccessor* CBaseEdict::GetChangeAccessor() const
{
	return engine->GetChangeAccessor((const edict_t*)this);
}

// ------------------------------------------------------------------------------------------ //
// CEventRegister.
// ------------------------------------------------------------------------------------------ //

CEventRegister::CEventRegister()
{
	m_bRegistered = false;
	m_pEventMgr = NULL;
}


CEventRegister::~CEventRegister()
{
	Term();
}


void CEventRegister::Init(CTimedEventMgr* pMgr, IEventRegisterCallback* pCallback)
{
	Term();
	m_pEventMgr = pMgr;
	m_pCallback = pCallback;
}


void CEventRegister::Term()
{
	// Unregister.
	if (m_pEventMgr && m_bRegistered)
	{
		m_pEventMgr->RemoveEvent(this);
	}
}


void CEventRegister::SetUpdateInterval(float interval)
{
	Assert(m_pEventMgr);

	if (m_pEventMgr)
	{
		// Register for this event.
		m_flUpdateInterval = interval;
		m_flNextEventTime = gpGlobals->curtime + m_flUpdateInterval;

		m_pEventMgr->RegisterForNextEvent(this);
	}
}


void CEventRegister::StopUpdates()
{
	if (m_pEventMgr)
	{
		// Unregister our next event.
		m_pEventMgr->RemoveEvent(this);
	}
}


void CEventRegister::Reregister()
{
	if (m_flUpdateInterval > 1e-6 && m_pEventMgr)
	{
		while (m_flNextEventTime <= gpGlobals->curtime)
		{
			m_flNextEventTime += m_flUpdateInterval;
		}

		m_pEventMgr->RegisterForNextEvent(this);
	}
}


// ------------------------------------------------------------------------------------------ //
// CTimedEventMgr.
// ------------------------------------------------------------------------------------------ //

bool TimedEventMgr_LessFunc(CEventRegister* const& a, CEventRegister* const& b)
{
	return a->m_flNextEventTime > b->m_flNextEventTime;
}


CTimedEventMgr::CTimedEventMgr()
{
	m_Events.SetLessFunc(TimedEventMgr_LessFunc);
}


void CTimedEventMgr::FireEvents()
{
	while (m_Events.Count())
	{
		// Fire the top element, then break out.
		CEventRegister* pEvent = m_Events.ElementAtHead();
		if (gpGlobals->curtime >= pEvent->m_flNextEventTime)
		{
			// Reregister for the timed event, then fire the callback for the event.
			m_Events.RemoveAtHead();
			pEvent->m_bRegistered = false;
			pEvent->Reregister();

			pEvent->m_pCallback->FireEvent();
		}
		else
		{
			break;
		}
	}
}


void CTimedEventMgr::RegisterForNextEvent(CEventRegister* pEvent)
{
	RemoveEvent(pEvent);
	m_Events.Insert(pEvent);
	pEvent->m_bRegistered = true;
}


void CTimedEventMgr::RemoveEvent(CEventRegister* pEvent)
{
	if (pEvent->m_bRegistered)
	{
		// Find the event in the list and remove it.
		int cnt = m_Events.Count();
		for (int i = 0; i < cnt; i++)
		{
			if (m_Events.Element(i) == pEvent)
			{
				m_Events.RemoveAt(i);
				break;
			}
		}
	}
}