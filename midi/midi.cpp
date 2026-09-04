#include "api.h"
#include "midi.h"
#include "utils.h"
#include "utils/misc.h"

using std::operator""s;

void fill_each_n_steps(MediaItem_Take* take, float step, double len, bool sel = false, bool mute = false, int chan = 0, int pitch = 60, int vel = 127) {
	MediaItem* item = GetMediaItemTake_Item(take);
	double take_start = GetMediaItemInfo_Value(item, "D_POSITION");
	double take_end = MIDI_GetPPQPosFromProjTime(take, take_start + GetMediaItemInfo_Value(item, "D_LENGTH"));
	double pos = MIDI_GetPPQPosFromProjTime(take, take_start);
	double step_len;
	switch (midi_get_grid_type(take)) {
	case 2: {
		len *= 2.0 / 3.0;
		step_len = len * step;
		break;

	} case 4: {
		len *= 1.5;
		step_len = len * step;
		break;

	} case 8: {
		double swing = midi_get_grid_swing(take);
		step_len = len * step;
		len *= (1 + (swing / 2));
		break;

	} case 0: {
		step_len = len * step;

	}

	}
	MIDI_DisableSort(take);
	while (pos + len < take_end) {
		MIDI_InsertNote(take, sel, mute, pos, pos + len, chan, pitch, vel, nullptr);
		pos += step_len;
	};
	MIDI_Sort(take);

}


void fill_each_n_steps_in_take(MediaItem_Take* take, float step, double len, bool sel = false, bool mute = false, int chan = 0, int pitch = 60, int vel = 127) {
	MediaItem* item = GetMediaItemTake_Item(take);
	double take_start = GetMediaItemInfo_Value(item, "D_POSITION");
	double take_end = MIDI_GetPPQPosFromProjTime(take, take_start + GetMediaItemInfo_Value(item, "D_LENGTH"));
	double pos = MIDI_GetPPQPosFromProjTime(take, take_start);
	double step_len;
	switch (get_grid_type()) {
	case 2: {
		len *= 2.0 / 3.0;
		step_len = len * step;
		break;

	} case 4: {
		len *= 1.5;
		step_len = len * step;
		break;

	} case 8: {
		double swing = get_grid_swing();
		step_len = len * step;
		len *= (1 + (swing / 2));
		break;

	} case 0: {
		step_len = len * step;

	}

	}
	MIDI_DisableSort(take);
	while (pos + len < take_end) {
		MIDI_InsertNote(take, sel, mute, pos, pos + len, chan, pitch, vel, nullptr);
		pos += step_len;

	};
	MIDI_Sort(take);
}

void fill_each_n_steps(COMMAND_T* cmd) {
	double note_len = 480; // 1/8
	MediaItem_Take* take = MIDIEditor_GetTake(MIDIEditor_GetActive());
	Undo_BeginBlock2(nullptr);
	fill_each_n_steps(take, cmd->user, note_len);
	std::string undo_str = "Fill each "s + std::to_string(cmd->user) + " steps";
	Undo_EndBlock2(nullptr, undo_str.data(), 4);
}


void fill_each_n_steps_in_selected_items(COMMAND_T* cmd) {
	double note_len = 480; // 1/8
	Undo_BeginBlock2(nullptr);
	for (int i = 0;i < CountSelectedMediaItems(nullptr);i++) {
		MediaItem_Take* take = GetActiveTake(GetSelectedMediaItem(nullptr, i));
		fill_each_n_steps_in_take(take, cmd->user, note_len);

	}
	std::string undo_str = "Fill each "s + std::to_string(cmd->user) + " steps in takes";
	Undo_EndBlock2(nullptr, undo_str.data(), 4);

}


void fill_each_n_steps_with_del(COMMAND_T* cmd) {
	double note_len = 480; // 1/8 
	MediaItem_Take* take = MIDIEditor_GetTake(MIDIEditor_GetActive());
	Undo_BeginBlock2(nullptr);
	delete_midi_notes(take);
	fill_each_n_steps(take, cmd->user, note_len);
	std::string undo_str = "Fill each "s + std::to_string(cmd->user) + " steps (delete before)";
	Undo_EndBlock2(nullptr, undo_str.data(), 4);

}


void fill_each_n_steps_with_del_in_selected_items(COMMAND_T* cmd) {
	double note_len = 480; // 1/8
	Undo_BeginBlock2(nullptr);
	for (int i = 0;i < CountSelectedMediaItems(nullptr);i++) {
		MediaItem_Take* take = GetActiveTake(GetSelectedMediaItem(nullptr, i));
		delete_midi_notes(take);
		fill_each_n_steps_in_take(take, cmd->user, note_len);

	}
	std::string undo_str = "Fill each "s + std::to_string(cmd->user) + " steps in takes (delete before)";
	Undo_EndBlock2(nullptr, undo_str.data(), 4);

}