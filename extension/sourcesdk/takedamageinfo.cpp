#include <ehandle.h>
#include <isaverestore.h>
#include <takedamageinfo.h>

void CTakeDamageInfo::SetCritType( ECritType eType )
{
	if ( eType == kCritType_None )
	{
		// always let CRIT_NONE override the current setting
		m_eCritType = eType;
	}
	else
	{
		// don't let CRIT_MINI override CRIT_FULL
		m_eCritType = ( eType > m_eCritType ) ? eType : m_eCritType;
	}
}