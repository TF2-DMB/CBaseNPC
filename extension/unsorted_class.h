#ifndef _UNSORTED_CLASS_
#define _UNSORTED_CLASS_
	
#include <ihandleentity.h>

#define DECLARE_CLASS_NOBASE( className )					typedef className ThisClass;
#ifndef TRACER_DONT_USE_ATTACHMENT
#define TRACER_DONT_USE_ATTACHMENT -1
#endif
class CNavArea;
class INextBot;

typedef bool (*ShouldHitFunc_t)( IHandleEntity *pHandleEntity, int contentsMask );

//This function is a hack around CTraceFilterSimple do not edit it! you may create segmentation fault
class CTraceFilterSimpleHack : public CTraceFilter
{
public:
	DECLARE_CLASS_NOBASE( CTraceFilterSimpleHack );
	CTraceFilterSimpleHack( const IHandleEntity *passentity, int collisionGroup, ShouldHitFunc_t pExtraShouldHitCheckFn = NULL );
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
	static bool (CTraceFilterSimpleHack::*func_ShouldHitEntity)(IHandleEntity *pHandleEntity, int contentsMask);
	void SetFunctionPtr(IPluginFunction *pFunc)
	{
		m_pFunc = pFunc;
	}
};
#endif
