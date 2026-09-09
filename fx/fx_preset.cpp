#include "api.h"
#include "fx_preset.h"


FXPreset FXPreset::get(FX* fx) {
	if (!fx) return FXPreset();
	return FXPreset(fx->get_chunk(), fx->get_config_param("fx_name"));
}

bool FXPreset::set(FX* fx, const FXPreset &preset) {
	if (!fx || !fx->is_valid()) return false;
	if (preset.m_fx_name != fx->get_config_param("fx_name")) return false;
	fx->set_chunk(preset.m_preset);
	return true;
}


FXChainPreset FXChainPreset::get(FXChain* fx_chain) {
	FXChainPreset chain_preset{};
	if (!fx_chain) return chain_preset;
	for (int f = 0; f < fx_chain->get_count(); f++) {
		FX fx = fx_chain->get_fx(f);
		chain_preset.m_preset_list.push_back(FXPreset::get(&fx));
	}
	return chain_preset;
}

bool FXChainPreset::set(FXChain* fx_chain, const FXChainPreset &preset) {
	if (!fx_chain) return false;

	fx_chain->remove();
	for (int i = 0; i < preset.m_preset_list.size(); ++i) {
		const FXPreset &item = preset.m_preset_list[i];
		const int pos = fx_chain->add_fx_by_name(item.m_fx_name);
		if (pos < 0) continue;
		FX fx = fx_chain->get_fx(pos);
		FXPreset::set(&fx, item);
	}
	return true;
}
