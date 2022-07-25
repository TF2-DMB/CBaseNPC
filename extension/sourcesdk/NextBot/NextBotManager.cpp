#include "NextBotManager.h"

ConVar* nb_update_framelimit = nullptr;
ConVar* nb_update_maxslide = nullptr;

FCall<NextBotManager&> TheNextBots;

//static int g_nRun;
//static int g_nSlid;
//static int g_nBlockedSlides;

bool NextBotManager::Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	try
	{
		TheNextBots.Init(config, "TheNextBots");
	}
	catch (const std::exception& e)
	{
		snprintf(error, maxlength, "%s", e.what());
		return false;
	}

	return true;
}

bool NextBotManager::ShouldUpdate(INextBot* bot)
{
	if (m_iUpdateTickrate < 1)
	{
		return true;
	}

	float frameLimit = nb_update_framelimit->GetFloat();
	float sumFrameTime = 0;
	if (bot->IsFlaggedForUpdate())
	{
		bot->FlagForUpdate(false);
		sumFrameTime = m_SumFrameTime * 1000.0;
		if (frameLimit > 0.0f)
		{
			if (sumFrameTime < frameLimit)
			{
				return true;
			}
			/*else if ( nb_update_debug.GetBool() )
			{
				Msg( "Frame %8d/tick %8d: frame out of budget (%.2fms > %.2fms)\n", gpGlobals->framecount, gpGlobals->tickcount, sumFrameTime, frameLimit );
			}*/
		}
	}

	int nTicksSlid = (gpGlobals->tickcount - bot->GetTickLastUpdate()) - m_iUpdateTickrate;

	if (nTicksSlid >= nb_update_maxslide->GetInt())
	{
		if (frameLimit == 0.0 || sumFrameTime < nb_update_framelimit->GetFloat() * 2.0)
		{
			//g_nBlockedSlides++;
			return true;
		}
	}

	/*if (nb_update_debug.GetBool())
	{
		if (nTicksSlid > 0)
		{
			g_nSlid++;
		}
	}*/

	return false;
}

void NextBotManager::NotifyBeginUpdate(INextBot* bot)
{
	/*if (nb_update_debug.GetBool())
	{
		g_nRun++;
	}*/

	m_botList.Unlink(bot->GetBotId());
	m_botList.LinkToTail(bot->GetBotId());
	bot->SetTickLastUpdate(gpGlobals->tickcount);

	m_CurUpdateStartTime = Plat_FloatTime();
}

void NextBotManager::NotifyEndUpdate(INextBot* bot)
{
	m_SumFrameTime += Plat_FloatTime() - m_CurUpdateStartTime;
}

int NextBotManager::Register(INextBot* bot)
{
	return m_botList.AddToHead(bot);
}

void NextBotManager::UnRegister(INextBot* bot)
{
	m_botList.Remove(bot->GetBotId());

	if (bot == m_selectedBot)
	{
		m_selectedBot = nullptr;
	}
}