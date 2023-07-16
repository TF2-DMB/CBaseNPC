#ifndef H_TRACEFILTER_SIMPLE_CBASENPC_
#define H_TRACEFILTER_SIMPLE_CBASENPC_

#pragma once
#include <ihandleentity.h>
#include <IEngineTrace.h>
#include "sourcesdk/baseentity.h"

typedef bool (*ShouldHitFunc_t)( IHandleEntity *pHandleEntity, int contentsMask );

class ToolsTraceFilterSimple : public CTraceFilter
{
public:
	DECLARE_CLASS_NOBASE( ToolsTraceFilterSimple );
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);
	ToolsTraceFilterSimple( const IHandleEntity *passentity, int collisionGroup, ShouldHitFunc_t pExtraShouldHitCheckFn = NULL );
	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask );
	virtual void SetPassEntity( const IHandleEntity *pPassEntity ) { m_pPassEnt = pPassEntity; }
	virtual void SetCollisionGroup( int iCollisionGroup ) { m_collisionGroup = iCollisionGroup; }

	const IHandleEntity *GetPassEntity( void ){ return m_pPassEnt;}
private:
	const IHandleEntity *m_pPassEnt;
	int m_collisionGroup;
	ShouldHitFunc_t m_pExtraShouldHitCheckFunction;
	IPluginFunction *m_pFunc;
public:
	static bool (ToolsTraceFilterSimple::*func_ShouldHitEntity)(IHandleEntity *pHandleEntity, int contentsMask);
	void SetFunctionPtr(IPluginFunction *pFunc)
	{
		m_pFunc = pFunc;
	}
};

#endif // H_TRACEFILTER_SIMPLE_CBASENPC_
