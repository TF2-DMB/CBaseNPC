#include "sourcesdk/NextBot/NextBotComponentInterface.h"
#include "sourcesdk/NextBot/NextBotInterface.h"

INextBotComponent::INextBotComponent(INextBot *bot)
{
	m_curInterval = TICK_INTERVAL;
	m_lastUpdateTime = 0;
	m_bot = bot;
	bot->RegisterComponent( this );
}