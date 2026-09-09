#include "api.h"
#include "fx.h"
#include "fx_observer.h"
#include "fx_preset.h"

#define proj nullptr
using std::operator ""s;
using std::string;

static std::unordered_map<string, string> presets;

void toggle_show_last_focused_(COMMAND_T* cmd) {
	const IFXTarget* fx = FXObserver::get_last_focused(cmd->user);
	if (!*fx) return;
	fx->toggle_show();
}


void toggle_bypass_last_focused_(COMMAND_T* cmd) {
	const IFXTarget* fx = FXObserver::get_last_focused(cmd->user);
	if (!*fx) return;
	fx->toggle_bypass();
}


void toggle_offline_last_focused_(COMMAND_T* cmd) {
	const IFXTarget* fx = FXObserver::get_last_focused(cmd->user);
	if (!*fx) return;
	fx->toggle_offline();
}

void delete_last_focused_(COMMAND_T* cmd) {
	const IFXTarget* fx = FXObserver::get_last_focused(cmd->user);
	if (!*fx) return;
	fx->remove();
}

void reset_last_focused_(COMMAND_T* cmd) {
	const IFXTarget* fx = FXObserver::get_last_focused(cmd->user);
	if (!*fx) return;
	fx->reset();
}

static std::unordered_map<string, FXPreset> fx_presets;
static std::unordered_map<string, FXChainPreset> fx_chain_presets;

void ab_comparer(COMMAND_T* cmd) {
	const IFXTarget* target = FXObserver::get_last_focused(cmd->user);
	if (!target || !*target) return;

	if (const FX* fx = dynamic_cast<const FX*>(target)) {
		const string key = fx->get_key();
		const auto it = fx_presets.find(key);
		if (it == fx_presets.end()) {
			fx_presets[key] = FXPreset::get(const_cast<FX*>(fx));
			fx->reset();
			return;
		}
		FXPreset::set(const_cast<FX*>(fx), it->second);
		fx_presets.erase(it);
		return;
	} else if (const FXChain* fx_chain = dynamic_cast<const FXChain*>(target)) {
		const string key = fx_chain->get_guid();
		auto it = fx_chain_presets.find(key);
		if (it == fx_chain_presets.end()) {
			fx_chain_presets[key] = FXChainPreset::get(const_cast<FXChain*>(fx_chain));
			fx_chain->reset();
			return;
		}
		FXChainPreset::set(const_cast<FXChain*>(fx_chain), it->second);
		fx_chain_presets.erase(it);
		return;
	}
}

void fx_renamer(IFXContext* ctx) {
	if (!ctx || !ctx->is_valid()) return;
	std::unordered_map<string, std::vector<int>> names;
	string name;
	for (int fx = 0; fx < ctx->get_count(); fx++) {
		name = ctx->get_name(fx);
		int s = name.find(" #");
		if (s != string::npos) {
			name = name.substr(0, s);
		}
		if (names.find(name) == names.end()) {
			names[name] = {fx};
		} else {
			names[name].push_back(fx);
		}
	}


	for (const auto &n: names) {
		if (n.second.size() == 1) {
			const int fx_idx = n.second[0];
			if (ctx->get_name(fx_idx) != n.first) {
				ctx->set_name(fx_idx, n.first);
			}
		} else {
			for (int i = 0; i < n.second.size(); i++) {
				const string new_name = n.first + " #" + std::to_string(i + 1);
				const int fx_idx = n.second[i];
				if (ctx->get_name(fx_idx) != new_name) {
					ctx->set_name(fx_idx, new_name);
				}
			}
		}
	}
}
