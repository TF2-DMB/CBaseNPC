//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// nav.h
// Data structures and constants for the Navigation Mesh system
// Author: Michael S. Booth (mike@turtlerockstudios.com), January 2003

#ifndef _NAV_H_
#define _NAV_H_

#include "extension.h"

class CFuncElevator;
class CNavArea;
class IHandleEntity;

/**
 * Below are several constants used by the navigation system.
 * @todo Move these into TheNavMesh singleton.
 */
const float GenerationStepSize = 25.0f;			///< (30) was 20, but bots can't fit always fit
const float StepHeight = 18.0f;					///< if delta Z is greater than this, we have to jump to get up
const float JumpHeight = 41.8f;					///< if delta Z is less than this, we can jump up on it
const float JumpCrouchHeight = 58.0f;			///< (48) if delta Z is less than or equal to this, we can jumpcrouch up on it

const float BotRadius = 10.0f;					///< circular extent that contains bot
const float DeathDrop = 200.0f;					///< (300) distance at which we will die if we fall - should be about 600, and pay attention to fall damage during pathfind

const float HalfHumanWidth = 16.0f;
const float HalfHumanHeight = 36.0f;
const float HumanHeight = 72.0f;


#define NAV_MAGIC_NUMBER 0xFEEDFACE				///< to help identify nav files

/**
 * A place is a named group of navigation areas
 */
typedef unsigned int Place;
#define UNDEFINED_PLACE 0				// ie: "no place"
#define ANY_PLACE 0xFFFF

enum GetNavAreaFlags_t
{
	GETNAVAREA_CHECK_LOS			= 0x1,
	GETNAVAREA_ALLOW_BLOCKED_AREAS	= 0x2,
	GETNAVAREA_CHECK_GROUND			= 0x4,
};

enum NavErrorType
{
	NAV_OK,
	NAV_CANT_ACCESS_FILE,
	NAV_INVALID_FILE,
	NAV_BAD_FILE_VERSION,
	NAV_FILE_OUT_OF_DATE,
	NAV_CORRUPT_DATA,
};

enum NavAttributeType
{
	NAV_MESH_INVALID		= 0,
	NAV_MESH_CROUCH			= 0x00000001,				// must crouch to use this node/area
	NAV_MESH_JUMP			= 0x00000002,				// must jump to traverse this area (only used during generation)
	NAV_MESH_PRECISE		= 0x00000004,				// do not adjust for obstacles, just move along area
	NAV_MESH_NO_JUMP		= 0x00000008,				// inhibit discontinuity jumping
	NAV_MESH_STOP			= 0x00000010,				// must stop when entering this area
	NAV_MESH_RUN			= 0x00000020,				// must run to traverse this area
	NAV_MESH_WALK			= 0x00000040,				// must walk to traverse this area
	NAV_MESH_AVOID			= 0x00000080,				// avoid this area unless alternatives are too dangerous
	NAV_MESH_TRANSIENT		= 0x00000100,				// area may become blocked, and should be periodically checked
	NAV_MESH_DONT_HIDE		= 0x00000200,				// area should not be considered for hiding spot generation
	NAV_MESH_STAND			= 0x00000400,				// bots hiding in this area should stand
	NAV_MESH_NO_HOSTAGES	= 0x00000800,				// hostages shouldn't use this area
	NAV_MESH_STAIRS			= 0x00001000,				// this area represents stairs, do not attempt to climb or jump them - just walk up
	NAV_MESH_NO_MERGE		= 0x00002000,				// don't merge this area with adjacent areas
	NAV_MESH_OBSTACLE_TOP	= 0x00004000,				// this nav area is the climb point on the tip of an obstacle
	NAV_MESH_CLIFF			= 0x00008000,				// this nav area is adjacent to a drop of at least CliffHeight

	NAV_MESH_FIRST_CUSTOM	= 0x00010000,				// apps may define custom app-specific bits starting with this value
	NAV_MESH_LAST_CUSTOM	= 0x04000000,				// apps must not define custom app-specific bits higher than with this value

	NAV_MESH_FUNC_COST		= 0x20000000,				// area has designer specified cost controlled by func_nav_cost entities
	NAV_MESH_HAS_ELEVATOR	= 0x40000000,				// area is in an elevator's path
	NAV_MESH_NAV_BLOCKER	= 0x80000000				// area is blocked by nav blocker ( Alas, needed to hijack a bit in the attributes to get within a cache line [7/24/2008 tom])
};

enum NavDirType
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,

	NUM_DIRECTIONS
};

/**
 * Defines possible ways to move from one area to another
 */
enum NavTraverseType
{
	// NOTE: First 4 directions MUST match NavDirType
	GO_NORTH = 0,
	GO_EAST,
	GO_SOUTH,
	GO_WEST,

	GO_LADDER_UP,
	GO_LADDER_DOWN,
	GO_JUMP,
	GO_ELEVATOR_UP,
	GO_ELEVATOR_DOWN,

	NUM_TRAVERSE_TYPES
};
enum NavCornerType
{
	NORTH_WEST = 0,
	NORTH_EAST = 1,
	SOUTH_EAST = 2,
	SOUTH_WEST = 3,

	NUM_CORNERS
};

enum NavRelativeDirType
{
	FORWARD = 0,
	RIGHT,
	BACKWARD,
	LEFT,
	UP,
	DOWN,

	NUM_RELATIVE_DIRECTIONS
};

struct Extent
{
	Vector lo, hi;

	void Init( void )
	{
		lo.Init();
		hi.Init();
	}

	float SizeX( void ) const	{ return hi.x - lo.x; }
	float SizeY( void ) const	{ return hi.y - lo.y; }
	float SizeZ( void ) const	{ return hi.z - lo.z; }
	float Area( void ) const	{ return SizeX() * SizeY(); }

	// Increase bounds to contain the given point
	void Encompass( const Vector &pos )
	{
		for ( int i=0; i<3; ++i )
		{
			if ( pos[i] < lo[i] )
			{
				lo[i] = pos[i];
			}
			else if ( pos[i] > hi[i] )
			{
				hi[i] = pos[i];
			}
		}
	}

	/// return true if 'pos' is inside of this extent
	bool Contains( const Vector &pos ) const
	{
		return (pos.x >= lo.x && pos.x <= hi.x &&
				pos.y >= lo.y && pos.y <= hi.y &&
				pos.z >= lo.z && pos.z <= hi.z);
	}

	// return true if this extent overlaps the given one
	bool IsOverlapping( const Extent &other ) const
	{
		return (lo.x <= other.hi.x && hi.x >= other.lo.x &&
				lo.y <= other.hi.y && hi.y >= other.lo.y &&
				lo.z <= other.hi.z && hi.z >= other.lo.z);
	}
};

struct Ray
{
	Vector from, to;
};


class CNavArea;
class CNavNode;


//--------------------------------------------------------------------------------------------------------------
inline NavDirType OppositeDirection( NavDirType dir )
{
	switch( dir )
	{
		case NORTH: return SOUTH;
		case SOUTH: return NORTH;
		case EAST:	return WEST;
		case WEST:	return EAST;
		default: break;
	}

	return NORTH;
}

//--------------------------------------------------------------------------------------------------------------
inline NavDirType DirectionLeft( NavDirType dir )
{
	switch( dir )
	{
		case NORTH: return WEST;
		case SOUTH: return EAST;
		case EAST:	return NORTH;
		case WEST:	return SOUTH;
		default: break;
	}

	return NORTH;
}

//--------------------------------------------------------------------------------------------------------------
inline NavDirType DirectionRight( NavDirType dir )
{
	switch( dir )
	{
		case NORTH: return EAST;
		case SOUTH: return WEST;
		case EAST:	return SOUTH;
		case WEST:	return NORTH;
		default: break;
	}

	return NORTH;
}

//--------------------------------------------------------------------------------------------------------------
inline void AddDirectionVector( Vector *v, NavDirType dir, float amount )
{
	switch( dir )
	{
		case NORTH: v->y -= amount; return;
		case SOUTH: v->y += amount; return;
		case EAST:  v->x += amount; return;
		case WEST:  v->x -= amount; return;
		default: break;
	}
}

//--------------------------------------------------------------------------------------------------------------
inline float DirectionToAngle( NavDirType dir )
{
	switch( dir )
	{
		case NORTH:	return 270.0f;
		case SOUTH:	return 90.0f;
		case EAST:	return 0.0f;
		case WEST:	return 180.0f;
		default: break;
	}

	return 0.0f;
}

//--------------------------------------------------------------------------------------------------------------
inline NavDirType AngleToDirection( float angle )
{
	while( angle < 0.0f )
		angle += 360.0f;

	while( angle > 360.0f )
		angle -= 360.0f;

	if (angle < 45 || angle > 315)
		return EAST;

	if (angle >= 45 && angle < 135)
		return SOUTH;

	if (angle >= 135 && angle < 225)
		return WEST;

	return NORTH;
}

//--------------------------------------------------------------------------------------------------------------
inline void DirectionToVector2D( NavDirType dir, Vector2D *v )
{
	switch( dir )
	{
		case NORTH: v->x =  0.0f; v->y = -1.0f; break;
		case SOUTH: v->x =  0.0f; v->y =  1.0f; break;
		case EAST:  v->x =  1.0f; v->y =  0.0f; break;
		case WEST:  v->x = -1.0f; v->y =  0.0f; break;
		default: break;
	}
}


//--------------------------------------------------------------------------------------------------------------
inline void CornerToVector2D( NavCornerType dir, Vector2D *v )
{
	switch( dir )
	{
		case NORTH_WEST: v->x = -1.0f; v->y = -1.0f; break;
		case NORTH_EAST: v->x =  1.0f; v->y = -1.0f; break;
		case SOUTH_EAST: v->x =  1.0f; v->y =  1.0f; break;
		case SOUTH_WEST: v->x = -1.0f; v->y =  1.0f; break;
		default: break;
	}

	v->NormalizeInPlace();
}


//--------------------------------------------------------------------------------------------------------------
/**
 * Return true if given entity can be ignored when moving
 */
#define WALK_THRU_PROP_DOORS		0x01
#define WALK_THRU_FUNC_DOORS		0x02
#define WALK_THRU_DOORS				(WALK_THRU_PROP_DOORS | WALK_THRU_FUNC_DOORS)
#define WALK_THRU_BREAKABLES		0x04
#define WALK_THRU_TOGGLE_BRUSHES	0x08
#define WALK_THRU_EVERYTHING		(WALK_THRU_DOORS | WALK_THRU_BREAKABLES | WALK_THRU_TOGGLE_BRUSHES)

#endif // _NAV_H_
