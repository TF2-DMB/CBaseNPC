#pragma once

#include <IExtensionSys.h>
#include <ISourceMod.h>
#include <vector>

#include "helpers.h"

namespace natives {

using SourcePawn::IPluginContext;

void setup(std::vector<sp_nativeinfo_t>& natives);

// Until there's a proper address type, we use these
std::uint32_t ptr_toPtrIndex(void* ptr);
void* ptrIndex_toPtr(std::uint32_t index);
void erase_ptrIndex(std::uint32_t index);
void erase_ptr(void* ptr);
}