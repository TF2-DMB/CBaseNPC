#ifndef _NEXT_BOT_VISION_INTERFACE_H_
#define _NEXT_BOT_VISION_INTERFACE_H_

#include "NextBotComponentInterface.h"
#include "NextBotInterface.h"

class INextBotEntityFilter;
class CBaseCombatCharacterHack;

class IVision : public INextBotComponent
{
public:
	IVision( INextBot *bot ) : INextBotComponent(bot) {};
	virtual ~IVision() = 0;

	virtual void Reset( void ) = 0;
	virtual void Update( void ) = 0;
	
	class IForEachKnownEntity
	{
	public:
		virtual bool Inspect( const CKnownEntity &known ) = 0;
	};
	virtual bool ForEachKnownEntity( IForEachKnownEntity &func ) = 0;
	virtual void CollectKnownEntities( CUtlVector< CKnownEntity > *knownVector ) = 0;
	virtual const CKnownEntity *GetPrimaryKnownThreat( bool onlyVisibleThreats = false ) = 0;
	virtual float GetTimeSinceVisible( int team ) = 0;
	virtual const CKnownEntity *GetClosestKnown( int team = TEAM_ANY ) = 0;
	virtual int GetKnownCount( int team, bool onlyVisible = false, float rangeLimit = -1.0f ) = 0;
	virtual const CKnownEntity *GetClosestKnown( const INextBotEntityFilter &filter ) = 0;
	virtual const CKnownEntity *GetKnown( const CBaseEntity *entity ) = 0;
	virtual void AddKnownEntity( CBaseEntity *entity ) = 0;
	virtual void ForgetEntity( CBaseEntity *forgetMe ) = 0;
	virtual void ForgetAllKnownEntities( void ) = 0;
	virtual void CollectPotentiallyVisibleEntities( CUtlVector< CBaseEntity * > *potentiallyVisible ) = 0;
	virtual float GetMaxVisionRange( void ) = 0;
	virtual float GetMinRecognizeTime( void ) = 0;
	enum FieldOfViewCheckType { USE_FOV, DISREGARD_FOV };
	virtual bool IsAbleToSee( CBaseEntity *subject, FieldOfViewCheckType checkFOV, Vector *visibleSpot = NULL ) = 0;
	virtual bool IsAbleToSee( const Vector &pos, FieldOfViewCheckType checkFOV ) = 0;
	virtual bool IsIgnored( CBaseEntity *subject ) = 0;
	virtual bool IsVisibleEntityNoticed( CBaseEntity *subject ) = 0;
	virtual bool IsInFieldOfView( const Vector &pos ) = 0;
	virtual bool IsInFieldOfView( CBaseEntity *subject ) = 0;
	virtual float GetDefaultFieldOfView( void ) = 0;
	virtual float GetFieldOfView( void ) = 0;
	virtual void SetFieldOfView( float horizAngle ) = 0;
	virtual bool IsLineOfSightClear( const Vector &pos ) = 0;
	virtual bool IsLineOfSightClearToEntity( const CBaseEntity *subject, Vector *visibleSpot = NULL ) = 0;
	virtual bool IsLookingAt( const Vector &pos, float cosTolerance = 0.95f ) = 0;
	virtual bool IsLookingAt( const CBaseCombatCharacterHack *actor, float cosTolerance = 0.95f ) = 0;

private:
	CountdownTimer m_scanTimer;
	
	float m_FOV;
	float m_cosHalfFOV;
	
	CUtlVector< CKnownEntity > m_knownEntityVector;
	mutable CHandle< CBaseEntity > m_primaryThreat;

	float m_lastVisionUpdateTimestamp;
	IntervalTimer m_notVisibleTimer[ MAX_TEAMS ];
};

#endif
