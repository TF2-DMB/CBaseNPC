#ifndef NEXTBOT_NEXTBOTINTERFACE_H
#define NEXTBOT_NEXTBOTINTERFACE_H

#pragma once

#include "NextBotEventResponderInterface.h"
#include "enginecallback.h"
#include "util_shared.h"

class INextBotComponent;
class IIntention;
class ILocomotion;
class IBody;
class IVision;
class PathFollower;
class Path;
class NextBotCombatCharacter;


class INextBot : public INextBotEventResponder
{
public:
	enum NextBotDebugType : unsigned int
	{
		DEBUG_NONE = 0x0000,
		DEBUG_ANY  = 0xffff,

		DEBUG_BEHAVIOR   = (1 << 0), // NextBotBehavior
		DEBUG_LOOK_AT    = (1 << 1), // NextBotBodyInterface
		DEBUG_PATH       = (1 << 2), // NextBotPath, NextBotPathFollow, NextBotChasePath
		DEBUG_ANIMATION  = (1 << 3),
		DEBUG_LOCOMOTION = (1 << 4), // NextBotLocomotionInterface
		DEBUG_VISION     = (1 << 5), // NextBotVisionInterface
		DEBUG_HEARING    = (1 << 6),
		DEBUG_EVENTS     = (1 << 7), // NextBotEventResponderInterface
		DEBUG_ERRORS     = (1 << 8),
	};
	
	struct NextBotDebugLineType
	{
		NextBotDebugType type; // +0x000
		char buf[0x100];       // +0x004
	};
	
	INextBot() {}
	virtual ~INextBot() = 0;

	virtual void Reset() = 0;
	virtual void Update() = 0;
	virtual void Upkeep() = 0;
	
	virtual bool IsRemovedOnReset() const = 0;
	
	virtual CBaseCombatCharacter *GetEntity() const = 0;
	virtual NextBotCombatCharacter *GetNextBotCombatCharacter() const = 0;
	
	virtual ILocomotion *GetLocomotionInterface() const = 0;
	virtual IBody *GetBodyInterface() const = 0;
	virtual IIntention *GetIntentionInterface() const = 0;
	virtual IVision *GetVisionInterface() const = 0;
	
	virtual bool SetPosition(const Vector& pos) = 0;
	virtual Vector& GetPosition() const = 0;
	
	virtual bool IsEnemy(const CBaseEntity *ent) const = 0;
	virtual bool IsFriend(const CBaseEntity *ent) const = 0;
	virtual bool IsSelf(const CBaseEntity *ent) const = 0;
	
	virtual bool IsAbleToClimbOnto(const CBaseEntity *ent) const = 0;
	virtual bool IsAbleToBreak(const CBaseEntity *ent) const = 0;
	virtual bool IsAbleToBlockMovementOf(const INextBot *nextbot) const = 0;
	
	virtual bool ShouldTouch(const CBaseEntity *ent) const = 0;
	
	virtual bool IsImmobile() const = 0;
	virtual float GetImmobileDuration() const = 0;
	virtual void ClearImmobileStatus();
	virtual float GetImmobileSpeedThreshold() const = 0;
	
	virtual PathFollower *GetCurrentPath() const = 0;
	virtual void SetCurrentPath(const PathFollower *follower) = 0;
	virtual void NotifyPathDestruction(const PathFollower *follower) = 0;
	
	virtual bool IsRangeLessThan(CBaseEntity *ent, float dist) const = 0;
	virtual bool IsRangeLessThan(const Vector& vec, float dist) const = 0;
	virtual bool IsRangeGreaterThan(CBaseEntity *ent, float dist) const = 0;
	virtual bool IsRangeGreaterThan(const Vector& vec, float dist) const = 0;
	
	virtual float GetRangeTo(CBaseEntity *ent) const = 0;
	virtual float GetRangeTo(const Vector& vec) const = 0;
	virtual float GetRangeSquaredTo(CBaseEntity *ent) const = 0;
	virtual float GetRangeSquaredTo(const Vector& vec) const = 0;
	
	virtual bool IsDebugging(unsigned int type) const = 0;
	virtual char *GetDebugIdentifier() const = 0;
	virtual bool IsDebugFilterMatch(const char *filter) const = 0;
	virtual void DisplayDebugText(const char *text) const = 0;
	
	bool BeginUpdate() { return false; }
	void EndUpdate() {}
	
	void DebugConColorMessage(NextBotDebugType type, const Color& color, const char *fmt, ...) {}
	
	void GetDebugHistory(unsigned int mask, CUtlVector<const NextBotDebugLineType *> *dst) const {}
	void ResetDebugHistory() {}
	
	void RegisterComponent(INextBotComponent *component);
	void UnRegisterComponent(INextBotComponent *component);
	
private:
	void UpdateImmobileStatus() {}
	
	INextBotComponent *m_ComponentList;              // +0x04
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
	CUtlVector<NextBotDebugLineType *> m_DebugLines; // +0x4c
};


#endif
