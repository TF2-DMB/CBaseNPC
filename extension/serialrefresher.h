#pragma once

#include "sourcesdk/baseentity.h"

#include <cstdint>

class ToolsSerialRefresher {
public:
	ToolsSerialRefresher(CBaseEntity*);
	bool Refresh();
	void UpdateTransmit(int player, bool toggle);
protected:
	EHANDLE m_link;
	short m_original_serial;

	std::uint64_t m_0_63_players;
	std::uint64_t m_64_127_players;
	std::uint64_t m_old_0_63_players;
	std::uint64_t m_old_64_127_players;
};

void Tools_RefreshEntity(CBaseEntity* entity, int player, bool toggle);
void Tools_Refresh_Init();
void Tools_Refresh_Shutdown();