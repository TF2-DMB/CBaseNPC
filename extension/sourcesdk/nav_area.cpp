#include "extension.h"
#include <enginecallback.h>
#include <shareddefs.h>
#include <util_shared.h>
#include "sourcesdk/nav.h"
#include "sourcesdk/nav_area.h"
#include "sourcesdk/nav_mesh.h"
#include "toolsnav_mesh.h"

NavAreaVector TheNavAreas;

unsigned int CNavArea::m_masterMarker = 1;
CNavArea *CNavArea::m_openList = NULL;
CNavArea *CNavArea::m_openListTail = NULL;

//--------------------------------------------------------------------------------------------------------------
// Return a computed extent (XY is in m_nwCorner and m_seCorner, Z is computed)
void CNavArea::GetExtent( Extent *extent ) const
{
	extent->lo = m_nwCorner;
	extent->hi = m_seCorner;

	extent->lo.z = MIN( extent->lo.z, m_nwCorner.z );
	extent->lo.z = MIN( extent->lo.z, m_seCorner.z );
	extent->lo.z = MIN( extent->lo.z, m_neZ );
	extent->lo.z = MIN( extent->lo.z, m_swZ );

	extent->hi.z = MAX( extent->hi.z, m_nwCorner.z );
	extent->hi.z = MAX( extent->hi.z, m_seCorner.z );
	extent->hi.z = MAX( extent->hi.z, m_neZ );
	extent->hi.z = MAX( extent->hi.z, m_swZ );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if given area is connected in given direction
 * if dir == NUM_DIRECTIONS, check all directions (direction is unknown)
 * @todo Formalize "asymmetric" flag on connections
 */
bool CNavArea::IsConnected( const CNavArea *area, NavDirType dir ) const
{
	// we are connected to ourself
	if (area == this)
		return true;

	if (dir == NUM_DIRECTIONS)
	{
		// search all directions
		for( int d=0; d<NUM_DIRECTIONS; ++d )
		{
			FOR_EACH_VEC( m_connect[ d ], it )
			{
				if (area == m_connect[ d ][ it ].area)
					return true;
			}
		}

		// check ladder connections
		FOR_EACH_VEC( m_ladder[ CNavLadder::LADDER_UP ], it )
		{
			CNavLadder *ladder = m_ladder[ CNavLadder::LADDER_UP ][ it ].ladder;

			if (ladder->m_topBehindArea == area ||
				ladder->m_topForwardArea == area ||
				ladder->m_topLeftArea == area ||
				ladder->m_topRightArea == area)
				return true;
		}

		FOR_EACH_VEC( m_ladder[ CNavLadder::LADDER_DOWN ], dit )
		{
			CNavLadder *ladder = m_ladder[ CNavLadder::LADDER_DOWN ][ dit ].ladder;

			if (ladder->m_bottomArea == area)
				return true;
		}
	}
	else
	{
		// check specific direction
		FOR_EACH_VEC( m_connect[ dir ], it )
		{
			if (area == m_connect[ dir ][ it ].area)
				return true;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if 'pos' is within 2D extents of area.
 */
bool CNavArea::IsOverlapping( const Vector &pos, float tolerance ) const
{
	if (pos.x + tolerance >= m_nwCorner.x && pos.x - tolerance <= m_seCorner.x &&
		pos.y + tolerance >= m_nwCorner.y && pos.y - tolerance <= m_seCorner.y)
		return true;

	return false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if 'area' overlaps our 2D extents
 */
bool CNavArea::IsOverlapping( const CNavArea *area ) const
{
	if (area->m_nwCorner.x < m_seCorner.x && area->m_seCorner.x > m_nwCorner.x && 
		area->m_nwCorner.y < m_seCorner.y && area->m_seCorner.y > m_nwCorner.y)
		return true;

	return false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if 'extent' overlaps our 2D extents
 */
bool CNavArea::IsOverlapping( const Extent &extent ) const
{
	return ( extent.lo.x < m_seCorner.x && extent.hi.x > m_nwCorner.x && 
			 extent.lo.y < m_seCorner.y && extent.hi.y > m_nwCorner.y );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if 'area' overlaps our X extent
 */
bool CNavArea::IsOverlappingX( const CNavArea *area ) const
{
	if (area->m_nwCorner.x < m_seCorner.x && area->m_seCorner.x > m_nwCorner.x)
		return true;

	return false;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if 'area' overlaps our Y extent
 */
bool CNavArea::IsOverlappingY( const CNavArea *area ) const
{
	if (area->m_nwCorner.y < m_seCorner.y && area->m_seCorner.y > m_nwCorner.y)
		return true;

	return false;
}


//--------------------------------------------------------------------------------------------------------------
class COverlapCheck
{
public:
	COverlapCheck( const CNavArea *me, const Vector &pos ) : m_pos( pos )
	{
		m_me = me;
		m_myZ = me->GetZ( pos );
	}

	bool operator() ( CNavArea *area )
	{
		// skip self
		if ( area == m_me )
			return true;

		// check 2D overlap
		if ( !area->IsOverlapping( m_pos ) )
			return true;

		float theirZ = area->GetZ( m_pos );
		if ( theirZ > m_pos.z )
		{
			// they are above the point
			return true;
		}

		if ( theirZ > m_myZ )
		{
			// we are below an area that is beneath the given position
			return false;
		}

		return true;
	}

	const CNavArea *m_me;
	float m_myZ;
	const Vector &m_pos;
};

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if given point is on or above this area, but no others
 */
bool CNavArea::Contains( const Vector &pos ) const
{
	// check 2D overlap
	if (!IsOverlapping( pos ))
		return false;

	// the point overlaps us, check that it is above us, but not above any areas that overlap us
	float myZ = GetZ( pos );

	// if the nav area is above the given position, fail
	// allow nav area to be as much as a step height above the given position
	if (myZ - StepHeight > pos.z)
		return false;

	Extent areaExtent;
	GetExtent( &areaExtent );

	COverlapCheck overlap( this, pos );
	return ToolsNavMesh->ForAllAreasOverlappingExtent( overlap, areaExtent );
}

//--------------------------------------------------------------------------------------------------------------
/**
* Returns true if area completely contains other area
*/
bool CNavArea::Contains( const CNavArea *area ) const
{
	return ( ( m_nwCorner.x <= area->m_nwCorner.x ) && ( m_seCorner.x >= area->m_seCorner.x ) &&
		( m_nwCorner.y <= area->m_nwCorner.y ) && ( m_seCorner.y >= area->m_seCorner.y ) &&
		( m_nwCorner.z <= area->m_nwCorner.z ) && ( m_seCorner.z >= area->m_seCorner.z ) );
}

void CNavArea::ComputeNormal( Vector *normal, bool alternate ) const
{
	if ( !normal )
		return;

	Vector u, v;

	if ( !alternate )
	{
		u.x = m_seCorner.x - m_nwCorner.x;
		u.y = 0.0f;
		u.z = m_neZ - m_nwCorner.z;

		v.x = 0.0f;
		v.y = m_seCorner.y - m_nwCorner.y;
		v.z = m_swZ - m_nwCorner.z;
	}
	else
	{
		u.x = m_nwCorner.x - m_seCorner.x;
		u.y = 0.0f;
		u.z = m_swZ - m_seCorner.z;

		v.x = 0.0f;
		v.y = m_nwCorner.y - m_seCorner.y;
		v.z = m_neZ - m_seCorner.z;
	}

	*normal = CrossProduct( u, v );
	normal->NormalizeInPlace();
}

float CNavArea::GetZ( float x, float y ) const RESTRICT
{
	if (m_invDxCorners == 0.0f || m_invDyCorners == 0.0f)
		return m_neZ;

	float u = (x - m_nwCorner.x) * m_invDxCorners;
	float v = (y - m_nwCorner.y) * m_invDyCorners;

	// clamp Z values to (x,y) volume
	
	u = fsel( u, u, 0 );			// u >= 0 ? u : 0
	u = fsel( u - 1.0f, 1.0f, u );	// u >= 1 ? 1 : u

	v = fsel( v, v, 0 );			// v >= 0 ? v : 0
	v = fsel( v - 1.0f, 1.0f, v );	// v >= 1 ? 1 : v

	float northZ = m_nwCorner.z + u * (m_neZ - m_nwCorner.z);
	float southZ = m_swZ + u * (m_seCorner.z - m_swZ);

	return northZ + v * (southZ - northZ);
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return closest point to 'pos' on 'area'.
 * Returned point is in 'close'.
 */
void CNavArea::GetClosestPointOnArea( const Vector * RESTRICT pPos, Vector *close ) const RESTRICT 
{
	float x, y, z;

	// Using fsel rather than compares, as much faster on 360 [7/28/2008 tom]
	x = fsel( pPos->x - m_nwCorner.x, pPos->x, m_nwCorner.x );
	x = fsel( x - m_seCorner.x, m_seCorner.x, x );

	y = fsel( pPos->y - m_nwCorner.y, pPos->y, m_nwCorner.y );
	y = fsel( y - m_seCorner.y, m_seCorner.y, y );

	z = GetZ( x, y );

	close->Init( x, y, z );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return shortest distance squared between point and this area
 */
float CNavArea::GetDistanceSquaredToPoint( const Vector &pos ) const
{
	if (pos.x < m_nwCorner.x)
	{
		if (pos.y < m_nwCorner.y)
		{
			// position is north-west of area
			return (m_nwCorner - pos).LengthSqr();
		}
		else if (pos.y > m_seCorner.y)
		{
			// position is south-west of area
			Vector d;
			d.x = m_nwCorner.x - pos.x;
			d.y = m_seCorner.y - pos.y;
			d.z = m_swZ - pos.z;
			return d.LengthSqr();
		}
		else
		{
			// position is west of area
			float d = m_nwCorner.x - pos.x;
			return d * d;
		}
	}
	else if (pos.x > m_seCorner.x)
	{
		if (pos.y < m_nwCorner.y)
		{
			// position is north-east of area
			Vector d;
			d.x = m_seCorner.x - pos.x;
			d.y = m_nwCorner.y - pos.y;
			d.z = m_neZ - pos.z;
			return d.LengthSqr();
		}
		else if (pos.y > m_seCorner.y)
		{
			// position is south-east of area
			return (m_seCorner - pos).LengthSqr();
		}
		else
		{
			// position is east of area
			float d = pos.x - m_seCorner.x;
			return d * d;
		}
	}
	else if (pos.y < m_nwCorner.y)
	{
		// position is north of area
		float d = m_nwCorner.y - pos.y;
		return d * d;
	}
	else if (pos.y > m_seCorner.y)
	{
		// position is south of area
		float d = pos.y - m_seCorner.y;
		return d * d;
	}
	else
	{
		// position is inside of 2D extent of area - find delta Z
		float z = GetZ( pos );
		float d = z - pos.z;
		return d * d;
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Compute "portal" between two adjacent areas. 
 * Return center of portal opening, and half-width defining sides of portal from center.
 * NOTE: center->z is unset.
 */
void CNavArea::ComputePortal( const CNavArea *to, NavDirType dir, Vector *center, float *halfWidth ) const
{
	if ( dir == NORTH || dir == SOUTH )
	{
		if ( dir == NORTH )
		{
			center->y = m_nwCorner.y;
		}
		else
		{
			center->y = m_seCorner.y;
		}

		float left = MAX( m_nwCorner.x, to->m_nwCorner.x );
		float right = MIN( m_seCorner.x, to->m_seCorner.x );

		// clamp to our extent in case areas are disjoint
		if ( left < m_nwCorner.x )
		{
			left = m_nwCorner.x;
		}
		else if ( left > m_seCorner.x )
		{
			left = m_seCorner.x;
		}

		if ( right < m_nwCorner.x )
		{
			right = m_nwCorner.x;
		}
		else if ( right > m_seCorner.x )
		{
			right = m_seCorner.x;
		}

		center->x = ( left + right )/2.0f;
		*halfWidth = ( right - left )/2.0f;
	}
	else	// EAST or WEST
	{
		if ( dir == WEST )
		{
			center->x = m_nwCorner.x;
		}
		else
		{
			center->x = m_seCorner.x;
		}

		float top = MAX( m_nwCorner.y, to->m_nwCorner.y );
		float bottom = MIN( m_seCorner.y, to->m_seCorner.y );

		// clamp to our extent in case areas are disjoint
		if ( top < m_nwCorner.y )
		{
			top = m_nwCorner.y;
		}
		else if ( top > m_seCorner.y )
		{
			top = m_seCorner.y;
		}

		if ( bottom < m_nwCorner.y )
		{
			bottom = m_nwCorner.y;
		}
		else if ( bottom > m_seCorner.y )
		{
			bottom = m_seCorner.y;
		}

		center->y = (top + bottom)/2.0f;
		*halfWidth = (bottom - top)/2.0f;
	}

	center->z = GetZ( center->x, center->y );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Compute closest point within the "portal" between to adjacent areas. 
 */
void CNavArea::ComputeClosestPointInPortal( const CNavArea *to, NavDirType dir, const Vector &fromPos, Vector *closePos ) const
{
	const float margin = GenerationStepSize;

	if ( dir == NORTH || dir == SOUTH )
	{
		if ( dir == NORTH )
		{
			closePos->y = m_nwCorner.y;
		}
		else
		{
			closePos->y = m_seCorner.y;
		}

		float left = MAX( m_nwCorner.x, to->m_nwCorner.x );
		float right = MIN( m_seCorner.x, to->m_seCorner.x );
		
		/// @todo Need better check whether edge is outer edge or not - partial overlap is missed
		float leftMargin = ( to->IsEdge( WEST ) ) ? ( left + margin ) : left;
		float rightMargin = ( to->IsEdge( EAST ) ) ? ( right - margin ) : right;
		
		// if area is narrow, margins may have crossed
		if ( leftMargin > rightMargin )
		{
			// use midline
			float mid = ( left + right )/2.0f;
			leftMargin = mid;
			rightMargin = mid;
		}

		// limit x to within portal
		if ( fromPos.x < leftMargin )
		{
			closePos->x = leftMargin;
		}
		else if ( fromPos.x > rightMargin )
		{
			closePos->x = rightMargin;
		}
		else
		{
			closePos->x = fromPos.x;
		}
	}
	else	// EAST or WEST
	{
		if ( dir == WEST )
		{
			closePos->x = m_nwCorner.x;
		}
		else
		{
			closePos->x = m_seCorner.x;
		}

		float top = MAX( m_nwCorner.y, to->m_nwCorner.y );
		float bottom = MIN( m_seCorner.y, to->m_seCorner.y );
		
		// keep margin if against edge
		float topMargin = ( to->IsEdge( NORTH ) ) ? ( top + margin ) : top;
		float bottomMargin = ( to->IsEdge( SOUTH ) ) ? ( bottom - margin ) : bottom;

		// if area is narrow, margins may have crossed
		if ( topMargin > bottomMargin )
		{
			// use midline
			float mid = ( top + bottom )/2.0f;
			topMargin = mid;
			bottomMargin = mid;
		}

		// limit y to within portal
		if ( fromPos.y < topMargin )
		{
			closePos->y = topMargin;
		}
		else if ( fromPos.y > bottomMargin )
		{
			closePos->y = bottomMargin;
		}
		else
		{
			closePos->y = fromPos.y;
		}
	}

	closePos->z = GetZ( closePos->x, closePos->y );
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return height change between edges of adjacent nav areas (not actual underlying ground)
 */
float CNavArea::ComputeAdjacentConnectionHeightChange( const CNavArea *destinationArea ) const
{
	// find which side it is connected on
	int dir;
	for( dir=0; dir<NUM_DIRECTIONS; ++dir )
	{
		if ( this->IsConnected( destinationArea, (NavDirType)dir ) )
			break;
	}

	if ( dir == NUM_DIRECTIONS )
		return FLT_MAX;

	Vector myEdge;
	float halfWidth;
	this->ComputePortal( destinationArea, (NavDirType)dir, &myEdge, &halfWidth );

	Vector otherEdge;
	destinationArea->ComputePortal( this, OppositeDirection( (NavDirType)dir ), &otherEdge, &halfWidth );

	return otherEdge.z - myEdge.z;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if there are no bi-directional links on the given side
 */
bool CNavArea::IsEdge( NavDirType dir ) const
{
	FOR_EACH_VEC( m_connect[ dir ], it )
	{
		const NavConnect connect = m_connect[ dir ][ it ];

		if (connect.area->IsConnected( this, OppositeDirection( dir ) ))
			return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Return direction from this area to the given point
 */
NavDirType CNavArea::ComputeDirection( Vector *point ) const
{
	if (point->x >= m_nwCorner.x && point->x <= m_seCorner.x)
	{
		if (point->y < m_nwCorner.y)
			return NORTH;
		else if (point->y > m_seCorner.y)
			return SOUTH;
	}
	else if (point->y >= m_nwCorner.y && point->y <= m_seCorner.y)
	{
		if (point->x < m_nwCorner.x)
			return WEST;
		else if (point->x > m_seCorner.x)
			return EAST;
	}

	// find closest direction
	Vector to = *point - m_center;

	if (fabs(to.x) > fabs(to.y))
	{
		if (to.x > 0.0f)
			return EAST;
		return WEST;
	}
	else
	{
		if (to.y > 0.0f)
			return SOUTH;
		return NORTH;
	}

	return NUM_DIRECTIONS;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Add to open list in decreasing value order
 */
void CNavArea::AddToOpenList( void )
{
	if ( IsOpen() )
	{
		// already on list
		return;
	}

	// mark as being on open list for quick check
	m_openMarker = m_masterMarker;

	// if list is empty, add and return
	if ( m_openList == NULL )
	{
		m_openList = this;
		m_openListTail = this;
		this->m_prevOpen = NULL;
		this->m_nextOpen = NULL;
		return;
	}

	// insert self in ascending cost order
	CNavArea *area, *last = NULL;
	for( area = m_openList; area; area = area->m_nextOpen )
	{
		if ( GetTotalCost() < area->GetTotalCost() )
		{
			break;
		}
		last = area;
	}

	if ( area )
	{
		// insert before this area
		this->m_prevOpen = area->m_prevOpen;

		if ( this->m_prevOpen )
		{
			this->m_prevOpen->m_nextOpen = this;
		}
		else
		{
			m_openList = this;
		}

		this->m_nextOpen = area;
		area->m_prevOpen = this;
	}
	else
	{
		// append to end of list
		last->m_nextOpen = this;
		this->m_prevOpen = last;
	
		this->m_nextOpen = NULL;

		m_openListTail = this;
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Add to tail of the open list
 */
void CNavArea::AddToOpenListTail( void )
{
	if ( IsOpen() )
	{
		// already on list
		return;
	}

	// mark as being on open list for quick check
	m_openMarker = m_masterMarker;

	// if list is empty, add and return
	if ( m_openList == NULL )
	{
		m_openList = this;
		m_openListTail = this;
		this->m_prevOpen = NULL;
		this->m_nextOpen = NULL;
		return;
	}

	// append to end of list
	m_openListTail->m_nextOpen = this;

	this->m_prevOpen = m_openListTail;
	this->m_nextOpen = NULL;

	m_openListTail = this;
}

//--------------------------------------------------------------------------------------------------------------
void CNavArea::RemoveFromOpenList( void )
{
	if ( m_openMarker == 0 )
	{
		// not on the list
		return;
	}

	if ( m_prevOpen )
	{
		m_prevOpen->m_nextOpen = m_nextOpen;
	}
	else
	{
		m_openList = m_nextOpen;
	}
	
	if ( m_nextOpen )
	{
		m_nextOpen->m_prevOpen = m_prevOpen;
	}
	else
	{
		m_openListTail = m_prevOpen;
	}
	
	// zero is an invalid marker
	m_openMarker = 0;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * A smaller value has been found, update this area on the open list
 * @todo "bubbling" does unnecessary work, since the order of all other nodes will be unchanged - only this node is altered
 */
void CNavArea::UpdateOnOpenList( void )
{
	// since value can only decrease, bubble this area up from current spot
	while( m_prevOpen && this->GetTotalCost() < m_prevOpen->GetTotalCost() )
	{
		// swap position with predecessor
		CNavArea *other = m_prevOpen;
		CNavArea *before = other->m_prevOpen;
		CNavArea *after  = this->m_nextOpen;

		this->m_nextOpen = other;
		this->m_prevOpen = before;

		other->m_prevOpen = this;
		other->m_nextOpen = after;

		if ( before )
		{
			before->m_nextOpen = this;
		}
		else
		{
			m_openList = this;
		}

		if ( after )
		{
			after->m_prevOpen = other;
		}
		else
		{
			m_openListTail = this;
		}
	}
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Clears the open and closed lists for a new search
 */
void CNavArea::ClearSearchLists( void )
{
	// effectively clears all open list pointers and closed flags
	CNavArea::MakeNewMarker();

	m_openList = NULL;
	m_openListTail = NULL;
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Returns a 0..1 light intensity for the given point
 */
float CNavArea::GetLightIntensity( const Vector &pos ) const
{
	Vector testPos;
	testPos.x = clamp( pos.x, m_nwCorner.x, m_seCorner.x );
	testPos.y = clamp( pos.y, m_nwCorner.y, m_seCorner.y );
	testPos.z = pos.z;

	float dX = (testPos.x - m_nwCorner.x) / (m_seCorner.x - m_nwCorner.x);
	float dY = (testPos.y - m_nwCorner.y) / (m_seCorner.y - m_nwCorner.y);

	float northLight = m_lightIntensity[ NORTH_WEST ] * ( 1 - dX ) + m_lightIntensity[ NORTH_EAST ] * dX;
	float southLight = m_lightIntensity[ SOUTH_WEST ] * ( 1 - dX ) + m_lightIntensity[ SOUTH_EAST ] * dX;
	float light = northLight * ( 1 - dY ) + southLight * dY;

	return light;
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Returns a 0..1 light intensity for the given point
 */
float CNavArea::GetLightIntensity( float x, float y ) const
{
	return GetLightIntensity( Vector( x, y, 0 ) );
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Returns a 0..1 light intensity averaged over the whole area
 */
float CNavArea::GetLightIntensity( void ) const
{
	float light = m_lightIntensity[ NORTH_WEST ];
	light += m_lightIntensity[ NORTH_EAST ];
	light += m_lightIntensity[ SOUTH_WEST];
	light += m_lightIntensity[ SOUTH_EAST ];
	return light / 4.0f;
}