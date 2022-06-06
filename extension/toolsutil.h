#pragma once

#include "sourcesdk/tracefilter_simple.h"
#include <IEngineTrace.h>

inline void UTILTools_TraceLine( const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, const IHandleEntity *ignore, int collisionGroup, trace_t *ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilterSimpleHack traceFilter(ignore, collisionGroup);

	enginetrace->TraceRay(ray, mask, &traceFilter, ptr);
}