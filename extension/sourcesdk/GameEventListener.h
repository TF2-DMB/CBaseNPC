#include <igameevents.h>

class CGameEventListener : public IGameEventListener2
{
public:
	CGameEventListener() : m_bRegisteredForEvents(false)
	{
	}

#if SOURCE_ENGINE == SE_CSGO || SOURCE_ENGINE == SE_LEFT4DEAD || SOURCE_ENGINE == SE_LEFT4DEAD2
	int m_nDebugID;
#endif

private:
	// Have we registered for any events?
	bool m_bRegisteredForEvents;
};