#include "nav.hpp"
#include "nav/area.hpp"

namespace natives::bms::nav {

void setup(std::vector<sp_nativeinfo_t>& natives)
{
    area::setup(natives);
}

} // namespace natives::bms::nav
