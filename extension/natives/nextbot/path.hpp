#pragma once

#include "NextBotPath.h"

#include "natives.hpp"

namespace natives::nextbot::path {

class SMPathFollowerCost : public IPathCost {
public:
	SMPathFollowerCost( INextBot* bot, IPluginFunction* func = nullptr );
	virtual float operator()(CNavArea* area, CNavArea* fromArea, const CNavLadder* ladder, const CFuncElevator* elevator, float length) const override;
private:
	IPluginFunction* m_pFunc;
	INextBot* m_bot;
};

void setup(std::vector<sp_nativeinfo_t>& natives);

}