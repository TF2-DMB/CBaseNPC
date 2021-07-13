#ifndef NEXT_BOT_KNOWN_ENTITY_H
#define NEXT_BOT_KNOWN_ENTITY_H

#include "NextBotInterface.h"
#include <mathlib/vector.h>
#include <itoolentity.h>

class CBaseEntity;
class CNavArea;

class CKnownEntity
{
public:
	// constructing assumes we currently know about this entity
	CKnownEntity( CBaseEntity *who ) { };

	virtual ~CKnownEntity() = 0;
	virtual void Destroy( void ) = 0;
	virtual void UpdatePosition( void ) = 0;
	virtual CBaseEntity *GetEntity( void ) const = 0;
	virtual const Vector &GetLastKnownPosition( void ) const = 0;
	virtual bool HasLastKnownPositionBeenSeen( void ) const = 0;
	virtual void MarkLastKnownPositionAsSeen( void ) = 0;
	virtual const CNavArea *GetLastKnownArea( void ) const = 0;
	virtual float GetTimeSinceLastKnown( void ) const = 0;
	virtual float GetTimeSinceBecameKnown( void ) const = 0;
	virtual void UpdateVisibilityStatus( bool visible ) = 0;
	virtual bool IsVisibleInFOVNow( void ) const = 0;
	virtual bool IsVisibleRecently( void ) const = 0;
	virtual float GetTimeSinceBecameVisible( void ) const = 0;
	virtual float GetTimeWhenBecameVisible( void ) const = 0;
	virtual float GetTimeSinceLastSeen( void ) const = 0;
	virtual bool WasEverVisible( void ) const = 0;
	virtual bool IsObsolete( void ) const = 0;
	virtual bool operator==( const CKnownEntity &other ) const = 0;
	virtual bool Is( CBaseEntity *who ) const = 0;

private:
	CBaseHandle m_who;
	Vector m_lastKnownPostion;
	bool m_hasLastKnownPositionBeenSeen;
	CNavArea *m_lastKnownArea;
	float m_whenLastSeen;
	float m_whenLastBecameVisible;
	float m_whenLastKnown;			// last seen or heard, confirming its existance
	float m_whenBecameKnown;
	bool m_isVisible;				// flagged by IVision update as visible or not
};


#endif