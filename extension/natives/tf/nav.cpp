#include "nav.hpp"
#include "nav/area.hpp"

namespace natives::tf::nav {

void setup(std::vector<sp_nativeinfo_t>& natives) {
	area::setup(natives);
}

}