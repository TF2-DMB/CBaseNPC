#include "sourcesdk/NextBot/NextBotComponentInterface.h"
#include "sourcesdk/NextBot/NextBotInterface.h"

ConVar* NextBotStop = nullptr;

VCall<void> INextBotComponent::vUpdate;

bool INextBotComponent::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	try
	{
		vUpdate.Init(&INextBotComponent::Update);
	}
	catch (const std::exception& e)
	{
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}

	return true;
}

INextBotComponent::INextBotComponent(INextBot *bot)
{
	m_curInterval = TICK_INTERVAL;
	m_lastUpdateTime = 0;
	m_bot = bot;
	bot->RegisterComponent( this );
}