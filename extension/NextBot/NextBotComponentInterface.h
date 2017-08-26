#ifndef _NEXT_BOT_COMPONENT_INTERFACE_H_
#define _NEXT_BOT_COMPONENT_INTERFACE_H_

#include "NextBotEventResponderInterface.h"

class INextBot;
class Path;
class CGameTrace;
class CTakeDamageInfo;

class INextBotComponent : public INextBotEventResponder
{
public:
	virtual ~INextBotComponent() { }

	virtual void Reset( void )	{ m_lastUpdateTime = 0; m_curInterval = TICK_INTERVAL; }
	virtual void Update( void ) = 0;
	virtual void Upkeep( void ) { };
	virtual INextBot *GetBot( void ) const  { return m_bot; }
	
private:
	float m_lastUpdateTime;
	float m_curInterval;

	friend class INextBot;
	
	INextBot *m_bot;
	INextBotComponent *m_nextComponent;
};

#endif // _NEXT_BOT_COMPONENT_INTERFACE_H_
