
#include "serverclass.h"

void ServerClassHack::Init( const char *pNetworkName, SendTable *pTable )
{
	m_pNetworkName = pNetworkName;
	m_pTable = pTable;
	m_InstanceBaselineIndex = INVALID_STRING_INDEX;
}

void ServerClassHack::AddToGlobalList()
{
}

void ServerClassHack::RemoveFromGlobalList()
{
}