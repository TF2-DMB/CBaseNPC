#include "serialrefresher.h"

#include <unordered_map>
#include <memory>

std::unordered_map<std::uint32_t, std::unique_ptr<ToolsSerialRefresher>> gRefreshers;

ToolsSerialRefresher::ToolsSerialRefresher(CBaseEntity* entity) : 
 m_link(entity),
 m_original_serial(entity->GetNetworkable()->GetEdict()->m_NetworkSerialNumber),
 m_0_63_players(-1),
 m_64_127_players(-1),
 m_old_0_63_players(-1),
 m_old_64_127_players(-1) {
}

bool ToolsSerialRefresher::Refresh() {
	auto entity = m_link.Get();
	if (!entity) {
		return false;
	}

	if (m_64_127_players == m_old_64_127_players
	&& m_0_63_players == m_old_0_63_players) {
		// No transmission update
		return true;
	}

	m_old_0_63_players = m_0_63_players;
	m_old_64_127_players = m_64_127_players;

	auto edict = entity->GetNetworkable()->GetEdict();
	edict->m_NetworkSerialNumber = edict->m_NetworkSerialNumber + 2;
	if (edict->m_NetworkSerialNumber == m_original_serial || edict->m_NetworkSerialNumber == (m_original_serial + 1)) {
		// This ensures the next entity that occupies this edict index doesn't cause server/client crash
		edict->m_NetworkSerialNumber = edict->m_NetworkSerialNumber + 2;
	}

	return true;
}

void ToolsSerialRefresher::UpdateTransmit(int player, bool toggle) {
	if (player <= 0 || player > 128) {
		return;
	}

	auto index = player - 1;
	if (index <= 63) {
		m_0_63_players = (m_0_63_players & ~(1 << index)) | (toggle << index);
	} else {
		index -= 64;
		m_64_127_players = (m_64_127_players & ~(1 << index)) | (toggle << index);
	}
}

void Hook_Frame(bool simulating) {
	for (auto it = gRefreshers.begin(); it != gRefreshers.end(); it++) {
		if (!it->second->Refresh()) {
			// Can't refresh, entity became invalid
			it = gRefreshers.erase(it);
		}
	}
}

void Tools_RefreshEntity(CBaseEntity* entity, int player, bool toggle) {
	auto hndl = entity->GetRefEHandle();

	auto it = gRefreshers.find(hndl.ToInt());
	if (it == gRefreshers.end()) {
		auto element = gRefreshers.emplace(hndl.ToInt(), std::move(std::make_unique<ToolsSerialRefresher>(entity)));
		if (!element.second) {
			return;
		}
		it = element.first;
	}
	it->second->UpdateTransmit(player, toggle);
}

void Tools_Refresh_Init()
{
	g_pSM->AddGameFrameHook(&Hook_Frame);
}

void Tools_Refresh_Shutdown()
{
	g_pSM->RemoveGameFrameHook(&Hook_Frame);
}