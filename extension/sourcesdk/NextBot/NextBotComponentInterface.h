#ifndef _NEXT_BOT_COMPONENT_INTERFACE_H_
#define _NEXT_BOT_COMPONENT_INTERFACE_H_

#include "helpers.h"
#include "NextBotEventResponderInterface.h"
#include <cstdint>

class INextBot;
class Path;
class CGameTrace;
class CTakeDamageInfo;

extern ConVar* NextBotStop;

class INextBotReply
{
public:
	virtual void OnSuccess(INextBot *bot)  { };

	enum FailureReason
	{
		DENIED,
		INTERRUPTED,
		FAILED
	};
	virtual void OnFail(INextBot *bot, FailureReason reason) { };
};

class INextBotComponent : public INextBotEventResponder
{
public:
	static bool Init(SourceMod::IGameConfig* config, char* error, size_t maxlength);

	INextBotComponent(INextBot *bot);
	virtual ~INextBotComponent() { };

	virtual void Reset(void) 
	{ 
		m_lastUpdateTime = 0.0;
		m_curInterval = TICK_INTERVAL;
	}

	virtual void Update(void) = 0;
	virtual void Upkeep(void) { };
	virtual INextBot *GetBot(void) const  { return m_bot; }

#if SOURCE_ENGINE == SE_TF2
	class CUnknown;
	virtual CUnknown *GetScriptDesc(void) const { return nullptr; }
#endif

	inline bool ComputeUpdateInterval(void);
	inline float GetUpdateInterval(void);

private:
	float m_lastUpdateTime;
	float m_curInterval;

	friend class INextBot;
	
	INextBot *m_bot;
	INextBotComponent *m_nextComponent;

#if SOURCE_ENGINE == SE_TF2
	std::int32_t m_scriptInstance;
#endif

public:
	static VCall<void> vUpdate;
};

inline bool INextBotComponent::ComputeUpdateInterval(void) 
{ 
	if (m_lastUpdateTime) 
	{ 
		float interval = gpGlobals->curtime - m_lastUpdateTime;

		const float minInterval = 0.0001f;
		if (interval > minInterval)
		{
			m_curInterval = interval;
			m_lastUpdateTime = gpGlobals->curtime;
			return true;
		}

		return false;
	}

	m_curInterval = 0.033f;
	m_lastUpdateTime = gpGlobals->curtime - m_curInterval;
	return true;
}

inline float INextBotComponent::GetUpdateInterval(void)
{
	return m_curInterval;
}
#endif // _NEXT_BOT_COMPONENT_INTERFACE_H_
