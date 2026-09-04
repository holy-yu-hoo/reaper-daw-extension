#include "api.h"
#include "commands.h"
#include "actions/actions.h"
#include "midi/midi.h"
#include "config.h"


std::vector<COMMAND_T> commands = {
	{ID_PREFIX "INSERT_4_BARS_MIDI_ITEM_AT_CURSOR",NAME_PREFIX "Insert 4 bars midi item at cursor", nullptr, insert_4_bars_midi_item_at_cursor, SECTION_MAIN, nullptr},
	{ID_PREFIX "FX_A/B_COMPARER", NAME_PREFIX "FX A/B comparer", nullptr, fx_ab_comparer, SECTION_MAIN, nullptr},
	{ID_PREFIX "FX_CHAIN_A/B_COMPARER", NAME_PREFIX "FX chain A/B comparer", nullptr, fx_chain_ab_comparer, SECTION_MAIN, nullptr},
	{ID_PREFIX "TOGGLE_SHOW_LAST_FOCUSED_FX",NAME_PREFIX "Toggle show last focused fx", nullptr, toggle_show_last_focused_fx, SECTION_MAIN, nullptr},
	{ID_PREFIX "TOGGLE_SHOW_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Toggle show last focused fx chain", nullptr, toggle_show_last_focused_fx_chain, SECTION_MAIN, nullptr},
	{ID_PREFIX "TOGGLE_BYPASS_LAST_FOCUSED_FX",NAME_PREFIX "Toggle bypass last focused fx", nullptr, toggle_bypass_last_focused_fx, SECTION_MAIN, nullptr},
	{ID_PREFIX "TOGGLE_BYPASS_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Toggle bypass last focused fx chain", nullptr, toggle_bypass_last_focused_fx_chain, SECTION_MAIN, nullptr},
	{ID_PREFIX "TOGGLE_OFFLINE_LAST_FOCUSED_FX",NAME_PREFIX "Toggle offline last focused fx", nullptr, toggle_offline_last_focused_fx, SECTION_MAIN, nullptr},
	{ID_PREFIX "TOGGLE_OFFLINE_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Toggle offline last focused fx chain", nullptr, toggle_offline_last_focused_fx_chain, SECTION_MAIN, nullptr},
	{ID_PREFIX "DELETE_LAST_FOCUSED_FX",NAME_PREFIX "Delete last focused fx", nullptr, delete_last_focused_fx, SECTION_MAIN, nullptr},
	{ID_PREFIX "DELETE_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Delete last focused fx chain", nullptr, delete_last_focused_fx_chain, SECTION_MAIN, nullptr},
	{ID_PREFIX "SAVE_HORIZONTAL_ZOOM_MODE", NAME_PREFIX "Save horizontal zoom mode", nullptr, save_hor_zoom_mode, SECTION_MAIN,},
	{ID_PREFIX "RESTORE_HORIZONTAL_ZOOM_MODE", NAME_PREFIX "Restore horizontal zoom mode", nullptr, restore_hor_zoom_mode, SECTION_MAIN,},
	{ID_PREFIX "SAVE_VERTICAL_ZOOM_MODE", NAME_PREFIX "Save vertical zoom mode", nullptr, save_ver_zoom_mode, SECTION_MAIN,},
	{ID_PREFIX "RESTORE_VERTICAL_ZOOM_MODE", NAME_PREFIX "Restore vertical zoom mode", nullptr, restore_ver_zoom_mode, SECTION_MAIN,},
	{ID_PREFIX "FILL_EACH_2_STEPS", NAME_PREFIX "fill each 2 steps", nullptr, fill_each_n_steps, SECTION_MIDI_EDITOR,nullptr,0,2},
	{ID_PREFIX "FILL_EACH_4_STEPS", NAME_PREFIX "fill each 4 steps", nullptr, fill_each_n_steps, SECTION_MIDI_EDITOR,nullptr,0,4},
	{ID_PREFIX "FILL_EACH_8_STEPS", NAME_PREFIX "fill each 8 steps", nullptr, fill_each_n_steps, SECTION_MIDI_EDITOR,nullptr,0,8},
	{ID_PREFIX "FILL_EACH_16_STEPS", NAME_PREFIX "fill each 16 steps", nullptr, fill_each_n_steps, SECTION_MIDI_EDITOR,nullptr,0,16},
};

bool commands_init() {
	if (!register_commands(commands)) return false;
	// LAST_FOCUSED_FX::init();
	plugin_register("timer", static_cast<void*>(LAST_FOCUSED_FX::last_focused_fx_observer));
	return true;
}

bool commands_exit() {
	unregister_commands(commands);
	plugin_register("-timer", static_cast<void*>(LAST_FOCUSED_FX::last_focused_fx_observer));
	return true;
}
