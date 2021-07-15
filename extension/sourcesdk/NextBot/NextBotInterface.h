#ifndef NEXTBOT_NEXTBOTINTERFACE_H
#define NEXTBOT_NEXTBOTINTERFACE_H

#pragma once

#include "NextBotKnownEntity.h"
#include "NextBotEventResponderInterface.h"
#include "NextBotDebug.h"
#include "sourcesdk/tracefilter_simple.h"
#include <enginecallback.h>
#include <util_shared.h>

class INextBotComponent;
class IIntention;
class ILocomotion;
class IBody;
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
	void Destroy();

	INextBot() {}
	virtual ~INextBot() = 0;

	virtual void Reset() = 0;
	virtual void Update() = 0;
	virtual void Upkeep() = 0;
	
	virtual bool IsRemovedOnReset() const = 0;
	
	virtual CBaseCombatCharacterHack* GetEntity() const = 0;
	virtual NextBotCombatCharacter *GetNextBotCombatCharacter() const = 0;
	
	virtual ILocomotion *GetLocomotionInterface() const = 0;
	virtual IBody *GetBodyInterface() const = 0;
	virtual IIntention *GetIntentionInterface() const = 0;
	virtual IVision *GetVisionInterface() const = 0;
	
	virtual bool SetPosition(const Vector& pos) = 0;
	virtual Vector& GetPosition() const = 0;
	
	virtual bool IsEnemy(const CBaseEntityHack* ent) const = 0;
	virtual bool IsFriend(const CBaseEntityHack* ent) const = 0;
	virtual bool IsSelf(const CBaseEntityHack* ent) const = 0;
	
	virtual bool IsAbleToClimbOnto(const CBaseEntityHack* ent) const = 0;
	virtual bool IsAbleToBreak(const CBaseEntityHack* ent) const = 0;
	virtual bool IsAbleToBlockMovementOf(const INextBot *nextbot) const = 0;
	
	virtual bool ShouldTouch(const CBaseEntityHack* ent) const = 0;
	
	virtual bool IsImmobile() const = 0;
	virtual float GetImmobileDuration() const = 0;
	virtual void ClearImmobileStatus() = 0;
	virtual float GetImmobileSpeedThreshold() const = 0;
	
	virtual PathFollower *GetCurrentPath() const = 0;
	virtual void SetCurrentPath(const PathFollower *follower) = 0;
	virtual void NotifyPathDestruction(const PathFollower *follower) = 0;
	
	virtual bool IsRangeLessThan(CBaseEntityHack*ent, float dist) const = 0;
	virtual bool IsRangeLessThan(const Vector& vec, float dist) const = 0;
	virtual bool IsRangeGreaterThan(CBaseEntityHack*ent, float dist) const = 0;
	virtual bool IsRangeGreaterThan(const Vector& vec, float dist) const = 0;
	
	virtual float GetRangeTo(CBaseEntityHack *ent) const = 0;
	virtual float GetRangeTo(const Vector& vec) const = 0;
	virtual float GetRangeSquaredTo(CBaseEntityHack*ent) const = 0;
	virtual float GetRangeSquaredTo(const Vector& vec) const = 0;
	
	virtual bool IsDebugging(unsigned int type) const = 0;
	virtual const char *GetDebugIdentifier() const;
	virtual bool IsDebugFilterMatch(const char *filter) const = 0;
	virtual void DisplayDebugText(const char *text) const = 0;
	void DebugConColorMsg( NextBotDebugType debugType, const Color &color, const char *fmt, ... );
	void ResetDebugHistory( );

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
	PathFollower *m_CurrentPath;                     // +0x08
	int m_iManagerIndex;                             // +0x0c
	bool m_bScheduledForNextTick;                    // +0x10
	int m_iLastUpdateTick;                           // +0x14
	int m_Dword18;                                   // +0x18 (reset to 0 in INextBot::Reset)
	int m_iDebugTextOffset;                          // +0x1c
	Vector m_vecLastPosition;                        // +0x20
	CountdownTimer m_ctImmobileCheck;                // +0x2c
	IntervalTimer m_itImmobileEpoch;                 // +0x38
	ILocomotion *m_LocoInterface;                    // +0x3c
	IBody *m_BodyInterface;                          // +0x40
	IIntention *m_IntentionInterface;                // +0x44
	IVision *m_VisionInterface;                      // +0x48
	CUtlVector<NextBotDebugLineType *> m_debugHistory; // +0x4c
};

#endif
