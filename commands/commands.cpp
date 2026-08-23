#include "commands.h"
#include <reaper_plugin_functions.h>
#include "actions/actions.h"
#include "utils.h"
#include "config.h"


std::vector<COMMAND_T> commands = {
	{
		ID_PREFIX "INSERT_4_BARS_MIDI_ITEM_AT_CURSOR",
		NAME_PREFIX "Insert 4 bars midi item at cursor",
		nullptr,
		insert_4_bars_midi_item_at_cursor,
		SECTION_MAIN,
		nullptr
	},
	{ID_PREFIX "FX A/B COMPARER", NAME_PREFIX "FX A/B comparer", nullptr, fx_ab_comparer, SECTION_MAIN, nullptr}
};

bool commands_init() {
	if (!register_commands(commands)) return false;
	plugin_register("timer", static_cast<void *>(last_focused_fx_observer));
	return true;
}

bool commands_exit() {
	unregister_commands(commands);
	plugin_register("-timer", static_cast<void *>(last_focused_fx_observer));
	return true;
}
