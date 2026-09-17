#ifndef _BMS_NAV_AREA_H_
#define _BMS_NAV_AREA_H_
#pragma once

#include <cstddef>
#include <cstdint>

#include "sourcesdk/nav_area.h"

/**
 * Black Mesa game-specific navigation attributes.
 *
 * These are separate from CNavArea::m_attributeFlags. The current BMS
 * server binary only defines PLAYER_BASE (bit 0).
 */
enum BlackMesaNavAttributeType : std::uint32_t
{
    BM_NAV_NONE = 0,
    BM_NAV_PLAYER_BASE = (1u << 0),
};

/**
 * Runtime view of Black Mesa's CBlackMesaNavArea.
 *
 * We intentionally access m_gameAttributes through a verified binary offset
 * instead of reproducing every private field of CNavArea/CBlackMesaNavArea.
 * The CNavArea layout differs between MSVC and GCC in the supported BMS
 * binaries.
 */
class CBlackMesaNavArea : public CNavArea
{
public:
    std::uint32_t GetGameAttributes() const
    {
        return GameAttributes();
    }

    void SetGameAttribute(std::uint32_t flags)
    {
        GameAttributes() |= SanitizeFlags(flags);
    }

    void ClearGameAttribute(std::uint32_t flags)
    {
        GameAttributes() &= ~SanitizeFlags(flags);
    }

    bool HasGameAttribute(std::uint32_t flags) const
    {
        const std::uint32_t sanitized = SanitizeFlags(flags);
        return sanitized != 0 && (GameAttributes() & sanitized) != 0;
    }

private:
    static constexpr std::uint32_t kKnownGameAttributes = BM_NAV_PLAYER_BASE;

    static constexpr std::uint32_t SanitizeFlags(std::uint32_t flags)
    {
        return flags & kKnownGameAttributes;
    }

    static constexpr std::size_t GameAttributesOffset()
    {
#if defined(_WIN32)
        return 0x160;
#else
        return 0x164;
#endif
    }

    std::uint32_t& GameAttributes()
    {
        auto* bytes = reinterpret_cast<std::uint8_t*>(this);
        return *reinterpret_cast<std::uint32_t*>(bytes + GameAttributesOffset());
    }

    const std::uint32_t& GameAttributes() const
    {
        const auto* bytes = reinterpret_cast<const std::uint8_t*>(this);
        return *reinterpret_cast<const std::uint32_t*>(bytes + GameAttributesOffset());
    }
};

#endif // _BMS_NAV_AREA_H_
