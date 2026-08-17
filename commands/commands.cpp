#include "commands.h"
#include "../config.h"
#include "actions.h"
#include <unordered_map>


struct Command {
	custom_action_register_t action;
	ActionFunc func;
};

Command commands[] = {
	{{0, ID_PREFIX "INSERT_4_BARS_MIDI_ITEM_AT_CURSOR", NAME_PREFIX "insert 4 bars midi item at cursor", nullptr}, insert_4_bars_midi_item_at_cursor},
};

std::unordered_map<int, ActionFunc> actions;

bool init_actions() {
	actions.clear();

	for (auto& command: commands) {
		const int action_id = g_rec->Register("custom_action", static_cast<void*>(&command.action));
		if (!action_id) {
			return false;
		}
		actions[action_id] = command.func;
	}
	return true;
}

bool action_dispatch_hook(KbdSectionInfo* sec, int command, int val, int val2, int relmode, HWND hwnd) {
	auto it = actions.find(command);
	if (it == actions.end()) return false;
	it->second();
	return true;
}