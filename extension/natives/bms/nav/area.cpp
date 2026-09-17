#include "area.hpp"

#include <cstdint>

#include "sourcesdk/bms_nav_area.h"

namespace natives::bms::nav::area {

inline CBlackMesaNavArea* Get(IPluginContext* context, const cell_t param)
{
    auto* area = reinterpret_cast<CBlackMesaNavArea*>(PawnAddressToPtr(param));
    if (!area)
    {
        context->ThrowNativeError("Black Mesa nav area pointer is null!");
        return nullptr;
    }

    return area;
}

cell_t GetGameAttributes(IPluginContext* context, const cell_t* params)
{
    auto* area = Get(context, params[1]);
    return area ? static_cast<cell_t>(area->GetGameAttributes()) : 0;
}

cell_t SetGameAttribute(IPluginContext* context, const cell_t* params)
{
    auto* area = Get(context, params[1]);
    if (area)
    {
        area->SetGameAttribute(static_cast<std::uint32_t>(params[2]));
    }

    return 0;
}

cell_t ClearGameAttribute(IPluginContext* context, const cell_t* params)
{
    auto* area = Get(context, params[1]);
    if (area)
    {
        area->ClearGameAttribute(static_cast<std::uint32_t>(params[2]));
    }

    return 0;
}

cell_t HasGameAttribute(IPluginContext* context, const cell_t* params)
{
    auto* area = Get(context, params[1]);
    return area && area->HasGameAttribute(static_cast<std::uint32_t>(params[2]));
}

void setup(std::vector<sp_nativeinfo_t>& natives)
{
    sp_nativeinfo_t list[] = {
        {"CBlackMesaNavArea.GetGameAttributes", GetGameAttributes},
        {"CBlackMesaNavArea.SetGameAttribute", SetGameAttribute},
        {"CBlackMesaNavArea.ClearGameAttribute", ClearGameAttribute},
        {"CBlackMesaNavArea.HasGameAttribute", HasGameAttribute},
    };

    natives.insert(natives.end(), std::begin(list), std::end(list));
}

} // namespace natives::bms::nav::area
