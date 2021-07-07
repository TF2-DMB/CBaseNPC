#ifndef VARIANT_T_HACK_H
#define VARIANT_T_HACK_H

#include <datamap.h>
#include <string_t.h>
#include <basehandle.h>

class variant_t
{
public:
	union
	{
		bool bVal;
		string_t iszVal;
		int iVal;
		float flVal;
		float vecVal[3];
		color32 rgbaVal;
	};
	
	CBaseHandle eVal;
	fieldtype_t fieldType;
};

#endif