#include <igameevents.h>

class CGameEventListener : public IGameEventListener2
{
public:
	CGameEventListener() : m_bRegisteredForEvents(false)
	{
	}

private:
	// Have we registered for any events?
	bool m_bRegisteredForEvents;
};