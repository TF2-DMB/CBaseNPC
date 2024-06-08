#pragma once

#include <IExtensionSys.h>
#include <ISourceMod.h>
#include <vector>

#include "helpers.h"

namespace natives {

using SourcePawn::IPluginContext;

void setup(std::vector<sp_nativeinfo_t>& natives);

// Helper functions
void ptr_register_navmesh();
void ptr_unregister_navmesh();

void ptr_setup(SourceMod::IGameConfig* config);

// Until there's a proper address type, we use these
std::uint32_t ptr_toPtrIndex(const void* ptr);
void* ptrIndex_toPtr(std::uint32_t index);
std::uint32_t add_ptr(const void* ptr);
void erase_ptrIndex(std::uint32_t index);
void erase_ptr(const void* ptr);
}