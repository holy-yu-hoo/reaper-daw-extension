#include "api.h"
#include "commands.h"
#include "midi/midi.h"
#include "item/item.h"
#include "config.h"
#include "misc/misc.h"
#include "fx/fx.h"
#include "fx/fx_observer.h"
#include "track/track.h"

// @formatter:off
std::vector<COMMAND_T> commands = {
	{ ID_PREFIX "INSERT_4_BARS_MIDI_ITEM_AT_CURSOR",NAME_PREFIX "Insert 4 bars midi item at cursor", nullptr, insert_4_bars_midi_item_at_cursor, SECTION_MAIN, nullptr },
	{ ID_PREFIX "FX_A/B_COMPARER", NAME_PREFIX "FX A/B comparer", nullptr, ab_comparer, SECTION_MAIN, nullptr, 0, 0 },
	{ ID_PREFIX "FX_CHAIN_A/B_COMPARER", NAME_PREFIX "FX chain A/B comparer", nullptr, ab_comparer, SECTION_MAIN, nullptr, 0, 1 },
	{ ID_PREFIX "TOGGLE_SHOW_LAST_FOCUSED_FX",NAME_PREFIX "Toggle show last focused fx", nullptr, toggle_show_last_focused_, SECTION_MAIN, nullptr, 0, 0 },
	{ ID_PREFIX "TOGGLE_SHOW_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Toggle show last focused fx chain", nullptr, toggle_show_last_focused_, SECTION_MAIN, nullptr, 0, 1 },
	{ ID_PREFIX "TOGGLE_BYPASS_LAST_FOCUSED_FX",NAME_PREFIX "Toggle bypass last focused fx", nullptr, toggle_bypass_last_focused_, SECTION_MAIN, nullptr, 0, 0 },
	{ ID_PREFIX "TOGGLE_BYPASS_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Toggle bypass last focused fx chain", nullptr, toggle_bypass_last_focused_, SECTION_MAIN, nullptr, 0, 1 },
	{ ID_PREFIX "TOGGLE_OFFLINE_LAST_FOCUSED_FX",NAME_PREFIX "Toggle offline last focused fx", nullptr, toggle_offline_last_focused_, SECTION_MAIN, nullptr, 0, 0 },
	{ ID_PREFIX "TOGGLE_OFFLINE_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Toggle offline last focused fx chain", nullptr, toggle_offline_last_focused_, SECTION_MAIN, nullptr, 0, 1 },
	{ ID_PREFIX "DELETE_LAST_FOCUSED_FX",NAME_PREFIX "Delete last focused fx", nullptr, delete_last_focused_, SECTION_MAIN, nullptr, 0, 0 },
	{ ID_PREFIX "DELETE_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Delete last focused fx chain", nullptr, delete_last_focused_, SECTION_MAIN, nullptr, 0, 1 },
	{ ID_PREFIX "RESET_LAST_FOCUSED_FX",NAME_PREFIX "Reset last focused fx", nullptr, reset_last_focused_, SECTION_MAIN, nullptr, 0, 0 },
	{ ID_PREFIX "RESET_LAST_FOCUSED_FX_CHAIN",NAME_PREFIX "Reset last focused fx chain", nullptr, reset_last_focused_, SECTION_MAIN, nullptr, 0, 1 },
	{ ID_PREFIX "SAVE_HORIZONTAL_ZOOM_MODE", NAME_PREFIX "Save horizontal zoom mode", nullptr, save_hor_zoom_mode, SECTION_MAIN, },
	{ ID_PREFIX "RESTORE_HORIZONTAL_ZOOM_MODE", NAME_PREFIX "Restore horizontal zoom mode", nullptr, restore_hor_zoom_mode, SECTION_MAIN, },
	{ ID_PREFIX "SAVE_VERTICAL_ZOOM_MODE", NAME_PREFIX "Save vertical zoom mode", nullptr, save_ver_zoom_mode, SECTION_MAIN, },
	{ ID_PREFIX "RESTORE_VERTICAL_ZOOM_MODE", NAME_PREFIX "Restore vertical zoom mode", nullptr, restore_ver_zoom_mode, SECTION_MAIN, },
	{ ID_PREFIX "FILL_EACH_2_STEPS_IN_ACTIVE_ME", NAME_PREFIX "fill each 2 steps", nullptr, fill_each_n_steps, SECTION_MIDI_EDITOR, nullptr, 0, 2 },
	{ ID_PREFIX "FILL_EACH_4_STEPS_IN_ACTIVE_ME", NAME_PREFIX "fill each 4 steps", nullptr, fill_each_n_steps, SECTION_MIDI_EDITOR, nullptr, 0, 4 },
	{ ID_PREFIX "FILL_EACH_8_STEPS_IN_ACTIVE_ME", NAME_PREFIX "fill each 8 steps", nullptr, fill_each_n_steps, SECTION_MIDI_EDITOR, nullptr, 0, 8 },
	{ ID_PREFIX "FILL_EACH_16_STEPS_IN_ACTIVE_ME", NAME_PREFIX "fill each 16 steps", nullptr, fill_each_n_steps, SECTION_MIDI_EDITOR, nullptr, 0, 16 },
	{ ID_PREFIX "FILL_EACH_2_STEPS_IN_SELECTED_ITEMS", NAME_PREFIX "fill each 2 steps in selected items", nullptr, fill_each_n_steps_in_selected_items, SECTION_MAIN, nullptr, 0, 2 },
	{ ID_PREFIX "FILL_EACH_4_STEPS_IN_SELECTED_ITEMS", NAME_PREFIX "fill each 4 steps in selected items", nullptr, fill_each_n_steps_in_selected_items, SECTION_MAIN, nullptr, 0, 4 },
	{ ID_PREFIX "FILL_EACH_8_STEPS_IN_SELECTED_ITEMS", NAME_PREFIX "fill each 8 steps in selected items", nullptr, fill_each_n_steps_in_selected_items, SECTION_MAIN, nullptr, 0, 8 },
	{ ID_PREFIX "FILL_EACH_16_STEPS_IN_SELECTED_ITEMS", NAME_PREFIX "fill each 16 steps in selected items",  nullptr,  fill_each_n_steps_in_selected_items,  SECTION_MAIN,  nullptr,  0,  16 },
	{ ID_PREFIX "FILL_EACH_2_STEPS_WITH_DEL_IN_ACTIVE_ME", NAME_PREFIX "fill each 2 steps (delete before)", nullptr, fill_each_n_steps_with_del, SECTION_MIDI_EDITOR, nullptr, 0, 2 },
	{ ID_PREFIX "FILL_EACH_4_STEPS_WITH_DEL_IN_ACTIVE_ME", NAME_PREFIX "fill each 4 steps (delete before)", nullptr, fill_each_n_steps_with_del, SECTION_MIDI_EDITOR, nullptr, 0, 4 },
	{ ID_PREFIX "FILL_EACH_8_STEPS_WITH_DEL_IN_ACTIVE_ME", NAME_PREFIX "fill each 8 steps (delete before)", nullptr, fill_each_n_steps_with_del, SECTION_MIDI_EDITOR, nullptr, 0, 8 },
	{ ID_PREFIX "FILL_EACH_16_STEPS_WITH_DEL_IN_ACTIVE_ME",  NAME_PREFIX "fill each 16 steps (delete before)",  nullptr,  fill_each_n_steps_with_del,  SECTION_MIDI_EDITOR,  nullptr,  0,  16 },
	{ ID_PREFIX "FILL_EACH_2_STEPS_WITH_DEL_IN_SELECTED_ITEMS",NAME_PREFIX "fill each 2 steps in selected items (delete before)",nullptr,fill_each_n_steps_with_del_in_selected_items,SECTION_MAIN,nullptr,0,2 },
	{ ID_PREFIX "FILL_EACH_4_STEPS_WITH_DEL_IN_SELECTED_ITEMS",  NAME_PREFIX "fill each 4 steps in selected items (delete before)",  nullptr,  fill_each_n_steps_with_del_in_selected_items,  SECTION_MAIN,  nullptr,  0,  4 },
	{ ID_PREFIX "FILL_EACH_8_STEPS_WITH_DEL_IN_SELECTED_ITEMS",  NAME_PREFIX "fill each 8 steps in selected items (delete before)",  nullptr,  fill_each_n_steps_with_del_in_selected_items,  SECTION_MAIN,  nullptr,  0,  8 },
	{ ID_PREFIX "FILL_EACH_16_STEPS_WITH_DEL_IN_SELECTED_ITEMS",  NAME_PREFIX "fill each 16 steps in selected items (delete before)",  nullptr,  fill_each_n_steps_with_del_in_selected_items,  SECTION_MAIN,  nullptr,0,16 },
	{ ID_PREFIX "SAVE_HORIZONTAL_ZOOM_MODE_ME", NAME_PREFIX "Save horizontal zoom mode", nullptr, save_hor_zoom_mode, SECTION_MIDI_EDITOR, },
	{ ID_PREFIX "RESTORE_HORIZONTAL_ZOOM_MODE_ME", NAME_PREFIX "Restore horizontal zoom mode", nullptr, restore_hor_zoom_mode, SECTION_MIDI_EDITOR, },
	{ ID_PREFIX "SAVE_VERTICAL_ZOOM_MODE_ME", NAME_PREFIX "Save vertical zoom mode", nullptr, save_ver_zoom_mode, SECTION_MIDI_EDITOR, },
	{ ID_PREFIX "RESTORE_VERTICAL_ZOOM_MODE_ME", NAME_PREFIX "Restore vertical zoom mode", nullptr, restore_ver_zoom_mode, SECTION_MIDI_EDITOR, },
	{ ID_PREFIX "NOTE_STUTTER_INCR", NAME_PREFIX "Note stutter increment", nullptr, note_stutter_incr, SECTION_MIDI_EDITOR, },
	{ ID_PREFIX "NOTE_STUTTER_DECR", NAME_PREFIX "Note stutter decrement", nullptr, note_stutter_decr, SECTION_MIDI_EDITOR, },
	{ ID_PREFIX "NOTE_STUTTER_MOUSEWHEEL", NAME_PREFIX "Note stutter (Midi relative/mousewheel)", nullptr, nullptr, SECTION_MIDI_EDITOR, note_stutter_mousewheel, },
	{ ID_PREFIX "RIPPLE_PER_TRACK_DELETE_ITEMS", NAME_PREFIX "Ripple per track delete items", nullptr, delete_items_ripple_per_track, SECTION_MAIN, },
	{ ID_PREFIX "RIPPLE_ALL_TRACK_DELETE_ITEMS", NAME_PREFIX "Ripple all track delete items", nullptr, delete_items_ripple_all_track, SECTION_MAIN, },
	{ ID_PREFIX "GO_TO_NEXT_TRACK_CYCLE",NAME_PREFIX "Go to next track (cycle)", nullptr, select_track_cycle, SECTION_MAIN, nullptr,0,1 },
	{ ID_PREFIX "GO_TO_PREVIOUS_TRACK_CYCLE",NAME_PREFIX "Go to previous track (cycle)", nullptr, select_track_cycle, SECTION_MAIN, nullptr,0,-1 },
	{ ID_PREFIX "GO_TO_NEXT_TRACK_CYCLE_KEEP_SELECTION",NAME_PREFIX "Go to next track keep selection (cycle)", nullptr, select_track_cycle_keep_selection, SECTION_MAIN, nullptr,0,1 },
	{ ID_PREFIX "GO_TO_PREVIOUS_TRACK_CYCLE_KEEP_SELECTION",NAME_PREFIX "Go to previous track keep selection (cycle)", nullptr, select_track_cycle_keep_selection, SECTION_MAIN, nullptr,0,-1 },
	{ ID_PREFIX "SELECT_NEXT_TRACKS_CYCLE",NAME_PREFIX "Select next tracks (cycle)", nullptr, select_tracks_cycle, SECTION_MAIN, nullptr,0,1 },
	{ ID_PREFIX "SELECT_PREVIOUS_TRACKS_CYCLE",NAME_PREFIX "Select previous tracks (cycle)", nullptr, select_tracks_cycle, SECTION_MAIN, nullptr,0,-1 },
	{ ID_PREFIX "AUTO_SOLO_LAST_TOUCH_TRACK",NAME_PREFIX "Auto-solo last touched track", nullptr, TrackObserver::set_auto_solo_mode, SECTION_MAIN, nullptr,0,1,TrackObserver::get_auto_solo_mode, },
	{ ID_PREFIX "AUTO_SOLO_SELECTED_TRACKS",NAME_PREFIX "Auto-solo selected tracks", nullptr, TrackObserver::set_auto_solo_mode, SECTION_MAIN, nullptr,0,2,TrackObserver::get_auto_solo_mode, },
	{ ID_PREFIX "AUTO_SOLO_LAST_TOUCH_TRACK_SELECTED",NAME_PREFIX "Auto-solo last touched selected track", nullptr, TrackObserver::set_auto_solo_mode, SECTION_MAIN, nullptr,0,4,TrackObserver::get_auto_solo_mode, },
	{ ID_PREFIX "AUTO_SOLO_DISABLE",NAME_PREFIX "Disable auto-solo", nullptr, TrackObserver::set_auto_solo_mode, SECTION_MAIN, nullptr,0,0,TrackObserver::get_auto_solo_mode, },
	{ ID_PREFIX "SET_LOOP_POINTS_TO_ACTIVE_MIDI_TAKE",  NAME_PREFIX "Set loop points to active midi take",  nullptr,  set_loop_time_range_to_active_take,  SECTION_MIDI_EDITOR,  nullptr,0,1 },
	{ ID_PREFIX "SET_TIME_SELECTION_TO_ACTIVE_MIDI_TAKE",  NAME_PREFIX "Set time selection to active midi take",  nullptr,  set_loop_time_range_to_active_take,  SECTION_MIDI_EDITOR,  nullptr,  0,  0 },
	{ ID_PREFIX "MOVE_LAST_FOCUSED_FX_UP",NAME_PREFIX "Move last focused fx up",nullptr,move_last_focused_fx_,SECTION_MAIN,nullptr,0,-1 },
	{ ID_PREFIX "MOVE_LAST_FOCUSED_FX_DOWN",NAME_PREFIX "Move last focused fx down",nullptr,move_last_focused_fx_,SECTION_MAIN,nullptr,0,1 },
};
//@formatter:on

bool commands_init() {
	if (!register_commands(commands)) return false;
	// LAST_FOCUSED_FX::init();
	plugin_register("csurf_inst", static_cast<void*>(&g_fx_observer));
	plugin_register("timer", static_cast<void*>(&FXObserver::fx_change_observer));
	plugin_register("csurf_inst", static_cast<void*>(&g_track_observer));
	return true;
}

bool commands_exit() {
	unregister_commands(commands);
	plugin_register("-csurf_inst", static_cast<void*>(&g_fx_observer));
	plugin_register("-timer", static_cast<void*>(&FXObserver::fx_change_observer));
	plugin_register("-csurf_inst", static_cast<void*>(&g_track_observer));
	return true;
}
