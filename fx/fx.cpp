#include "api.h"
#include "fx.h"
#include "utils.h"
#include "fx_observer.h"

#define proj nullptr
using std::operator ""s;
using std::string;

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

void move_last_focused_fx_(COMMAND_T* cmd) {
	const FX* fx = FXObserver::get_last_focused_fx();
	const FXChain* fx_chain = FXObserver::get_last_focused_fx_chain();
	if (!*fx) return;
	int idx = fx->get_fx_idx();
	idx += cmd->user;
	int cnt_fx = fx_chain->get_count();
	if (idx < 0) { idx = cnt_fx + idx; } else if (idx >= cnt_fx) { idx = idx % cnt_fx; }
	fx->move(fx->get_context(), idx);
}

static std::unordered_map<string, FX::Preset> fx_presets;
static std::unordered_map<string, FXChain::Preset> fx_chain_presets;

void ab_comparer(COMMAND_T* cmd) {
	const IFXTarget* target = FXObserver::get_last_focused(cmd->user);
	if (!target || !*target) return;
	IFXContext* ctx = target->get_context();
	ctx->_undo_begin_block(proj);
	string undo_str;
	prevent_autofloat_created_fx_windows_save();
	if (const FX* fx = dynamic_cast<const FX*>(target)) {
		const string key = fx->get_key();
		const auto preset = fx_presets.find(key);
		if (preset == fx_presets.end()) {
			fx_presets[key] = fx->get_preset();
			fx->reset();
		} else {
			FX::Preset pres = fx->get_preset();
			fx->set_preset(preset->second);
			fx_presets[key] = pres;
		}

		undo_str = "FX AB comparer: "s + ctx->get_undo_str();
	} else if (const FXChain* fx_chain = dynamic_cast<const FXChain*>(target)) {
		const string key = fx_chain->get_key();
		auto preset = fx_chain_presets.find(key);
		if (preset == fx_chain_presets.end()) {
			fx_chain_presets[key] = fx_chain->get_preset();
			fx_chain->reset();
		} else {
			FXChain::Preset pres = fx_chain->get_preset();
			fx_chain->set_preset(preset->second);
			fx_chain_presets[key] = pres;
		}


		undo_str = "FX chain AB comparer: "s + ctx->get_undo_str();
	}
	prevent_autofloat_created_fx_windows_restore();
	ctx->_undo_end_block(proj, undo_str);
	return;
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
			names[name] = { fx };
		} else {
			names[name].push_back(fx);
		}
	}


	for (const auto& n : names) {
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
