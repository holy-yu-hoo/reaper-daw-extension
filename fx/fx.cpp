#include "api.h"
#include "fx.h"
#include "fx_observer.h"

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

void ab_comparer(COMMAND_T* cmd) {
	const IFXTarget* fx = FXObserver::get_last_focused(cmd->user);
	if (!*fx) return;
	if (not fx->is_visible()) return;
	string key = fx->get_guid();
	string chunk = fx->get_chunk();
	if (presets.find(key) == presets.end()) {
		presets[key] = chunk;
		fx->reset();
	} else {
		fx->set_chunk(presets[key]);
		presets[key] = chunk;
	}
}
