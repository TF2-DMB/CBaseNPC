#include "serialrefresher.h"	
#include "helpers.h"

#include <unordered_map>
#include <memory>

std::unordered_map<std::uint32_t, std::unique_ptr<ToolsNetworkRefresher>> gRefreshers;

class CFrameSnapshotManager {};
CFrameSnapshotManager* gFrameSnapshot;
MCall<void, int> add_explicit_delete;

ToolsNetworkRefresher::ToolsNetworkRefresher(CBaseEntity* entity) : 
 m_link(entity),
 m_0_63_players(-1),
 m_64_127_players(-1),
 m_old_0_63_players(-1),
 m_old_64_127_players(-1) {
}

#ifdef COMPILER_GCC
__attribute__((noinline))
#endif
bool ToolsNetworkRefresher::Refresh() {
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
	add_explicit_delete(gFrameSnapshot, edict->m_EdictIndex);

	return true;
}

void ToolsNetworkRefresher::UpdateTransmit(int player, bool toggle) {
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
	for (auto it = gRefreshers.begin(); it != gRefreshers.end();) {
		auto& e = *it;
        if (!(e.second->Refresh())) {
            // Can't refresh, entity became invalid
            it = gRefreshers.erase(it);
        } else {
            it++;
        }
	}
}

void Tools_RefreshEntity(CBaseEntity* entity, int player, bool toggle) {
	auto hndl = entity->GetRefEHandle();

	auto it = gRefreshers.find(hndl.ToInt());
	if (it == gRefreshers.end()) {
		auto element = gRefreshers.emplace(hndl.ToInt(), std::move(std::make_unique<ToolsNetworkRefresher>(entity)));
		if (!element.second) {
			return;
		}
		it = element.first;
	}
	it->second->UpdateTransmit(player, toggle);
}

bool Tools_Refresh_Init(SourceMod::IGameConfig* config, char* error, size_t maxlength)
{
	void* addr = nullptr;
	if (!config->GetMemSig("CFrameSnapshotManager::AddExplicitDelete", &addr) || addr == nullptr) {
		snprintf(error, maxlength, "Failed to get CFrameSnapshotManager::AddExplicitDelete");
		return false;
	}
	add_explicit_delete.Init(addr);

	void* manager = nullptr;
	if (!config->GetMemSig("framesnapshotmanager", reinterpret_cast<void**>(&manager)) || manager == nullptr) {
		snprintf(error, maxlength, "Failed to get framesnapshotmanager signature");
		return false;
	}

	int offset = 0;
	if (config->GetOffset("framesnapshotmanager", &offset)) {
		if (offset == 0) {
			snprintf(error, maxlength, "Failed to get framesnapshotmanager offset!");
			return false;
		}
		gFrameSnapshot = reinterpret_cast<CFrameSnapshotManager*>(*reinterpret_cast<std::uint8_t**>(reinterpret_cast<std::uint8_t*>(manager) + offset) + 0x4);
	} else {
		gFrameSnapshot = *reinterpret_cast<CFrameSnapshotManager**>(manager);
	}

	g_pSM->AddGameFrameHook(&Hook_Frame);
	return true;
}

void Tools_Refresh_Shutdown()
{
	g_pSM->RemoveGameFrameHook(&Hook_Frame);
}