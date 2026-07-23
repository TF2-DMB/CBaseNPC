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
	gFrameSnapshot = nullptr;

	void* addExplicitDeleteAddress = nullptr;

	if (!config->GetMemSig("CFrameSnapshotManager::AddExplicitDelete", &addExplicitDeleteAddress) || !addExplicitDeleteAddress)
	{
		snprintf(error, maxlength, "Failed to get CFrameSnapshotManager::AddExplicitDelete");

		return false;
	}

	add_explicit_delete.Init(addExplicitDeleteAddress);

	std::uint8_t* managerAddress = nullptr;

	if (!config->GetMemSig("framesnapshotmanager",reinterpret_cast<void**>(&managerAddress)) || !managerAddress)
	{
		snprintf(error, maxlength, "Failed to get framesnapshotmanager signature");

		return false;
	}

	int operandOffset = 0;

	if (config->GetOffset("framesnapshotmanager", &operandOffset))
	{
		if (operandOffset <= 0)
		{
			snprintf(error, maxlength, "Invalid framesnapshotmanager operand offset: %d", operandOffset);

			return false;
		}

		int indirectCount = 0;

		if (!config->GetOffset("framesnapshotmanager_indirect", &indirectCount))
		{
			snprintf(error, maxlength, "Failed to get framesnapshotmanager_indirect offset!");
			return false;
		}

		if (indirectCount < 0 || indirectCount > 4)
		{
			snprintf(error, maxlength, "Invalid framesnapshotmanager indirect count: %d", indirectCount);
			return false;
		}

		int pointerOffset = 0;
		if (!config->GetOffset("framesnapshotmanager_ptr_offset", &pointerOffset))
		{
			snprintf(error, maxlength, "Failed to get framesnapshotmanager_ptr_offset!");

			return false;
		}

		std::uint8_t* resolvedAddress = *reinterpret_cast<std::uint8_t**>(managerAddress + operandOffset);

		if (!resolvedAddress)
		{
			snprintf(error, maxlength, "framesnapshotmanager operand resolved to null!");
			return false;
		}

		for (int i = 0; i < indirectCount; ++i)
		{
			resolvedAddress = *reinterpret_cast<std::uint8_t**>(resolvedAddress);

			if (!resolvedAddress)
			{
				snprintf(error, maxlength, "framesnapshotmanager indirection %d resolved to null!", i + 1);
				return false;
			}
		}

		gFrameSnapshot = reinterpret_cast<CFrameSnapshotManager*>(resolvedAddress + pointerOffset);
	}
	else
	{
		gFrameSnapshot = *reinterpret_cast<CFrameSnapshotManager**>(managerAddress);
	}

	if (!gFrameSnapshot)
	{
		snprintf(error, maxlength, "Failed to resolve CFrameSnapshotManager!");
		return false;
	}

	g_pSM->LogMessage(myself, "CFrameSnapshotManager: %p", static_cast<void*>(gFrameSnapshot));
	g_pSM->AddGameFrameHook(&Hook_Frame);

	return true;
}

void Tools_Refresh_Shutdown()
{
	g_pSM->RemoveGameFrameHook(&Hook_Frame);
	
	gRefreshers.clear();
	gFrameSnapshot = nullptr;
}