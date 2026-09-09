#pragma once
#include "fx_target.h"


struct FXPreset {
	friend class FXChainPreset;
	FXPreset(std::string preset = "", std::string fx_name = "")
		: m_preset(std::move(preset)), m_fx_name(std::move(fx_name)) {}

	static FXPreset get(FX* fx);

	static bool set(FX* fx, const FXPreset &preset);

	protected:
	std::string m_preset;
	std::string m_fx_name;
};


struct FXChainPreset {
	friend class FXPreset;

	FXChainPreset(std::vector<FXPreset> preset_list = {})
		: m_preset_list(std::move(preset_list)) {}

	static FXChainPreset get(FXChain* fx_chain);

	static bool set(FXChain* fx_chain, const FXChainPreset &preset);

	protected:
	std::vector<FXPreset> m_preset_list;
};
