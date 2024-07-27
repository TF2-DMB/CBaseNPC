#include "area.hpp"

#include "sourcesdk/tf_nav_area.h"

namespace natives::tf::nav::area {

inline CTFNavArea* Get(IPluginContext* context, const cell_t param) {
	CTFNavArea* area = (CTFNavArea*)PawnAddressToPtr(param, context);
	if (!area) {
		context->ThrowNativeError("TFNav area ptr is null!");
		return nullptr;
	}
	return area;
}

cell_t GetAttributesTF(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}

	return area->GetAttributesTF();
}

cell_t SetAttributeTF(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}

	area->SetAttributeTF(params[2]);
	return 0;
}

cell_t ClearAttributeTF(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}

	area->ClearAttributeTF(params[2]);
	return 0;
}

cell_t HasAttributeTF(IPluginContext* context, const cell_t* params) {
	auto area = Get(context, params[1]);
	if (!area) {
		return 0;
	}

	return area->HasAttributeTF(params[2]);
}

void setup(std::vector<sp_nativeinfo_t>& natives) {
	sp_nativeinfo_t list[] = {
		{"CTFNavArea.GetAttributesTF", GetAttributesTF},
		{"CTFNavArea.SetAttributeTF", SetAttributeTF},
		{"CTFNavArea.ClearAttributeTF", ClearAttributeTF},
		{"CTFNavArea.HasAttributeTF", HasAttributeTF}
	};

	natives.insert(natives.end(), std::begin(list), std::end(list));
}

}