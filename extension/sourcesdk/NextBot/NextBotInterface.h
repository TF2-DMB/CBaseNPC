#ifndef NEXTBOT_NEXTBOTINTERFACE_H
#define NEXTBOT_NEXTBOTINTERFACE_H

#pragma once

#include "NextBotKnownEntity.h"
#include "NextBotEventResponderInterface.h"
#include "NextBotDebug.h"
#include "NextBotBodyInterface.h"
#include "NextBotIntentionInterface.h"
#include "sourcesdk/tracefilter_simple.h"
#include "sourcesdk/basecombatcharacter.h"
#include <enginecallback.h>
#include <util_shared.h>

class INextBotComponent;
class IIntention;
class ILocomotion;
class IVision;
class PathFollower;
class Path;
class NextBotCombatCharacter;
class CBaseEntityHack;
class CBaseCombatCharacterHack;

bool IgnoreActorsTraceFilterFunction( IHandleEntity *pServerEntity, int contentsMask );

class NextBotTraceFilterIgnoreActors : public CTraceFilterSimpleHack
{
public:
	NextBotTraceFilterIgnoreActors(const IHandleEntity *passentity, int collisionGroup) : CTraceFilterSimpleHack(passentity, collisionGroup, IgnoreActorsTraceFilterFunction)
	{
	}
};

class INextBot : public INextBotEventResponder
{
public:
	INextBot();
	virtual ~INextBot();

	virtual void Reset();
	virtual void Update();
	virtual void Upkeep();
	
	virtual bool IsRemovedOnReset() const { return true; }
	
	virtual CBaseCombatCharacterHack* GetEntity() const = 0;
	virtual NextBotCombatCharacter *GetNextBotCombatCharacter() const { return nullptr; };
	
	virtual ILocomotion *GetLocomotionInterface() const = 0;
	virtual IBody *GetBodyInterface() const;
	virtual IIntention *GetIntentionInterface() const;
	virtual IVision *GetVisionInterface() const = 0;
	
	virtual bool SetPosition(const Vector& pos);
	virtual const Vector& GetPosition() const;
	
	virtual bool IsEnemy(const CBaseEntityHack* ent) const;
	virtual bool IsFriend(const CBaseEntityHack* ent) const;
	virtual bool IsSelf(const CBaseEntityHack* ent) const;
	
	virtual bool IsAbleToClimbOnto(const CBaseEntityHack* ent) const;
	virtual bool IsAbleToBreak(const CBaseEntityHack* ent) const;
	virtual bool IsAbleToBlockMovementOf(const INextBot *nextbot) const { return true; }
	
	virtual bool ShouldTouch(const CBaseEntityHack* ent) const { return true; }
	
	virtual bool IsImmobile() const;
	virtual float GetImmobileDuration() const;
	virtual void ClearImmobileStatus();
	virtual float GetImmobileSpeedThreshold() const;
	
	virtual const PathFollower *GetCurrentPath() const;
	virtual void SetCurrentPath(const PathFollower *follower);
	virtual void NotifyPathDestruction(const PathFollower *follower);
	
	virtual bool IsRangeLessThan(CBaseEntityHack*ent, float dist) const;
	virtual bool IsRangeLessThan(const Vector& vec, float dist) const;
	virtual bool IsRangeGreaterThan(CBaseEntityHack*ent, float dist) const;
	virtual bool IsRangeGreaterThan(const Vector& vec, float dist) const;
	
	virtual float GetRangeTo(CBaseEntityHack *ent) const;
	virtual float GetRangeTo(const Vector& vec) const;
	virtual float GetRangeSquaredTo(CBaseEntityHack*ent) const;
	virtual float GetRangeSquaredTo(const Vector& vec) const;
	
	virtual bool IsDebugging(unsigned int type) const;
	virtual const char *GetDebugIdentifier() const;
	virtual bool IsDebugFilterMatch(const char *filter) const;
	virtual void DisplayDebugText(const char *text) const;
	void DebugConColorMsg( NextBotDebugType debugType, const Color &color, const char *fmt, va_list arglist );
	void ResetDebugHistory( );
	void DebugConColorMsg( NextBotDebugType debugType, const Color &color, const char *fmt, ... )
	{ 
		va_list argptr;
		va_start(argptr, fmt);
		DebugConColorMsg( debugType, color, fmt, argptr); 
		va_end(argptr);
	}

	enum
	{
		MAX_NEXTBOT_DEBUG_HISTORY = 100,
		MAX_NEXTBOT_DEBUG_LINE_LENGTH = 256,
	};

	struct NextBotDebugLineType
	{
		NextBotDebugType debugType; // +0x000
		char data[MAX_NEXTBOT_DEBUG_LINE_LENGTH];       // +0x004
	};
	
	friend class INextBotComponent;
	void RegisterComponent(INextBotComponent *component);
	
public:
	void UpdateImmobileStatus() {}
	
	INextBotComponent *m_componentList;              // +0x04
	const PathFollower *m_CurrentPath;               // +0x08
	int m_iManagerIndex;                             // +0x0c
	bool m_bScheduledForNextTick;                    // +0x10
	int m_iLastUpdateTick;                           // +0x14
	int m_Dword18;                                   // +0x18 (reset to 0 in INextBot::Reset)
	int m_iDebugTextOffset;                          // +0x1c
	Vector m_vecLastPosition;                        // +0x20
	CountdownTimer m_ctImmobileCheck;                // +0x2c
	IntervalTimer m_itImmobileEpoch;                 // +0x38
	mutable ILocomotion *m_LocoInterface;            // +0x3c
	mutable IBody *m_BodyInterface;                  // +0x40
	mutable IIntention *m_IntentionInterface;        // +0x44
	IVision *m_VisionInterface;                      // +0x48
	CUtlVector<NextBotDebugLineType *> m_debugHistory; // +0x4c
};

inline const PathFollower *INextBot::GetCurrentPath( void ) const
{
	return m_CurrentPath;
}

inline void INextBot::SetCurrentPath( const PathFollower *path )
{
	m_CurrentPath = path;
}

inline void INextBot::NotifyPathDestruction( const PathFollower *path )
{
	if ( m_CurrentPath == path )
		m_CurrentPath = nullptr;
}

/*inline ILocomotion *INextBot::GetLocomotionInterface( void ) const
{
	if ( m_LocoInterface == nullptr )
	{
		m_LocoInterface = new ILocomotion( const_cast< INextBot * >( this ) );
	}

	return m_LocoInterface;
}*/

inline IBody *INextBot::GetBodyInterface( void ) const
{
	if ( m_BodyInterface == nullptr )
	{
		m_BodyInterface = new IBody( const_cast< INextBot * >( this ) );
	}

	return m_BodyInterface;
}

inline IIntention *INextBot::GetIntentionInterface( void ) const
{
	if ( m_IntentionInterface == nullptr )
	{
		m_IntentionInterface = new IIntention( const_cast< INextBot * >( this ) );
	}

	return m_IntentionInterface;
}

/*inline IVision *INextBot::GetVisionInterface( void ) const
{
	if ( m_VisionInterface == nullptr )
	{
		m_VisionInterface = new IVision( const_cast< INextBot * >( this ) );
	}

	return m_VisionInterface;
}*/

inline bool INextBot::IsImmobile( void ) const
{
	return m_itImmobileEpoch.HasStarted();
}

inline float INextBot::GetImmobileDuration( void ) const
{
	return m_itImmobileEpoch.GetElapsedTime();
}

inline void INextBot::ClearImmobileStatus( void )
{
	m_itImmobileEpoch.Invalidate();
	m_vecLastPosition = GetEntity()->GetAbsOrigin();
}

inline float INextBot::GetImmobileSpeedThreshold( void ) const
{
	return 30.0f;
}

#endif
