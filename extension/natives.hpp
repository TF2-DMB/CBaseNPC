#pragma once

#include <IExtensionSys.h>
#include <ISourceMod.h>
#include <vector>

#include "helpers.h"

namespace natives {

using SourcePawn::IPluginContext;

void setup(std::vector<sp_nativeinfo_t>& natives);

}