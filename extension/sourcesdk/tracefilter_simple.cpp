#include "smsdk_ext.h"
#include "sourcesdk/tracefilter_simple.h"
#include <IGameHelpers.h>

extern SourceMod::IGameHelpers* gamehelpers;

bool ToolsTraceFilterSimple::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	if (!config->GetMemSig("CTraceFilterSimple::ShouldHitEntity", reinterpret_cast<void**>(&ToolsTraceFilterSimple::func_ShouldHitEntity)))
	{
		snprintf(error, maxlength, "Couldn't locate function CTraceFilterSimple::ShouldHitEntity!");
		return false;
	}
	
	return true;
}

ToolsTraceFilterSimple::ToolsTraceFilterSimple(const IHandleEntity *passedict, int collisionGroup, ShouldHitFunc_t pExtraShouldHitFunc)
{
	m_pPassEnt = passedict;
	m_collisionGroup = collisionGroup;
	m_pExtraShouldHitCheckFunction = pExtraShouldHitFunc;
	m_pFunc = NULL;
}

bool ToolsTraceFilterSimple::ShouldHitEntity(IHandleEntity *pHandleEntity, int contentsMask)
{
	bool bResult = (this->*func_ShouldHitEntity)(pHandleEntity, contentsMask);
	if (bResult)
	{
		if (m_pFunc)
		{
			cell_t action;
			m_pFunc->PushCell(gamehelpers->EntityToBCompatRef(reinterpret_cast<CBaseEntity *>(pHandleEntity)));
			m_pFunc->PushCell(contentsMask);
			m_pFunc->PushCell(m_collisionGroup);
			m_pFunc->Execute(&action);
			
			return (action) ? true : false;
		}
		return bResult;
	}
	return bResult;
}

