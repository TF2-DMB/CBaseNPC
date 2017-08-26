#ifndef _NAV_LADDER_H_
#define _NAV_LADDER_H_

#include "sourcesdk/nav.h"
#include "extension.h"
#include "entitylist_base.h"
#include "ehandle.h"
#include "shareddefs.h"
#include "enginecallback.h"
#include "util_shared.h"

class CNavArea;

//--------------------------------------------------------------------------------------------------------------
/**
 * The NavLadder represents ladders in the Navigation Mesh, and their connections to adjacent NavAreas
 * @todo Deal with ladders that allow jumping off to areas in the middle
 */
class CNavLadder
{
public:
	CNavLadder( void )
	{
		m_topForwardArea = NULL;
		m_topRightArea = NULL;
		m_topLeftArea = NULL;
		m_topBehindArea = NULL;
		m_bottomArea = NULL;

		// set an ID for interactive editing - loads will overwrite this
		m_id = m_nextID++;
	}

	~CNavLadder() {};
	
	enum LadderDirectionType
	{
		LADDER_UP = 0,
		LADDER_DOWN,

		NUM_LADDER_DIRECTIONS
	};

	Vector m_top;									///< world coords of the top of the ladder
	Vector m_bottom;								///< world coords of the top of the ladder
	float m_length;									///< the length of the ladder
	float m_width;

	CNavArea *m_topForwardArea;						///< the area at the top of the ladder
	CNavArea *m_topLeftArea;
	CNavArea *m_topRightArea;
	CNavArea *m_topBehindArea;						///< area at top of ladder "behind" it - only useful for descending
	CNavArea *m_bottomArea;							///< the area at the bottom of the ladder
	
	NavDirType GetDir( void ) const;
	const Vector &GetNormal( void ) const;

private:

	CHandle<CBaseEntity> m_ladderEntity;

	NavDirType m_dir;								///< which way the ladder faces (ie: surface normal of climbable side)
	Vector m_normal;								///< surface normal of the ladder surface (or Vector-ized m_dir, if the traceline fails)

	enum LadderConnectionType						///< Ladder connection directions, to facilitate iterating over connections
	{
		LADDER_TOP_FORWARD = 0,
		LADDER_TOP_LEFT,
		LADDER_TOP_RIGHT,
		LADDER_TOP_BEHIND,
		LADDER_BOTTOM,

		NUM_LADDER_CONNECTIONS
	};

	CNavArea ** GetConnection( LadderConnectionType dir );

	static unsigned int m_nextID;					///< used to allocate unique IDs
	unsigned int m_id;								///< unique area ID
};
typedef CUtlVector< CNavLadder * > NavLadderVector;

//--------------------------------------------------------------------------------------------------------------
inline NavDirType CNavLadder::GetDir( void ) const
{
	return m_dir;
}


//--------------------------------------------------------------------------------------------------------------
inline const Vector &CNavLadder::GetNormal( void ) const
{
	return m_normal;
}

#endif