#ifndef _TF_NAV_AREA_H_
#define _TF_NAV_AREA_H_

#include "sourcesdk/nav_area.h"

class CTFNavArea : public CNavArea
{
public:
	DECLARE_CLASS(CTFNavArea, CNavArea)

	int GetAttributesTF() const;
	void SetAttributeTF( int flags );
	void ClearAttributeTF( int flags );
	bool HasAttributeTF( int flags ) const;

private:
	float m_distanceFromSpawnRoom[ 4 ];
	CUtlVector< CTFNavArea * > m_invasionAreaVector[ 4 ];
	unsigned int m_invasionSearchMarker;
	unsigned int m_attributeFlags;
};

inline int CTFNavArea::GetAttributesTF() const
{
	return m_attributeFlags;
}

inline void CTFNavArea::SetAttributeTF( int flags )
{
	m_attributeFlags |= flags;
}

inline void CTFNavArea::ClearAttributeTF( int flags )
{
	m_attributeFlags &= ~flags;
}

inline bool CTFNavArea::HasAttributeTF( int flags ) const
{
	return ( m_attributeFlags & flags ) ? true : false;
}

#endif