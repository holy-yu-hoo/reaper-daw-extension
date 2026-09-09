#include "api.h"
#include "midi.h"
#include "utils.h"
#include "misc/utils.h"

using std::operator""s;
using std::vector;
using std::unordered_map;

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

		}
		case 4: {
			len *= 1.5;
			step_len = len * step;
			break;

		}
		case 8: {
			double swing = midi_get_grid_swing(take);
			step_len = len * step;
			len *= (1 + (swing / 2));
			break;

		}
		case 0: {
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

		}
		case 4: {
			len *= 1.5;
			step_len = len * step;
			break;

		}
		case 8: {
			double swing = get_grid_swing();
			step_len = len * step;
			len *= (1 + (swing / 2));
			break;

		}
		case 0: {
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
	for (int i = 0; i < CountSelectedMediaItems(nullptr); i++) {
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
	for (int i = 0; i < CountSelectedMediaItems(nullptr); i++) {
		MediaItem_Take* take = GetActiveTake(GetSelectedMediaItem(nullptr, i));
		delete_midi_notes(take);
		fill_each_n_steps_in_take(take, cmd->user, note_len);

	}
	std::string undo_str = "Fill each "s + std::to_string(cmd->user) + " steps in takes (delete before)";
	Undo_EndBlock2(nullptr, undo_str.data(), 4);

}


struct MidiNote {
	int idx;
	bool mute;
	double start_pos, end_pos;
	int chan, pitch, vel;
};


constexpr double epsilon = 1e-8; // for small diff between notes

bool generate_data(MediaItem_Take* take, unordered_map<int, vector<vector<MidiNote>>>* data) {
	int n = MIDI_EnumSelNotes(take, -1);
	if (n < 0) return false;
	bool sel;
	MidiNote note;
	while (n >= 0) {
		note.idx = n;
		MIDI_GetNote(take, n, &sel, &note.mute, &note.start_pos, &note.end_pos, &note.chan, &note.pitch, &note.vel);
		if ((*data)[note.pitch].empty()) {
			(*data)[note.pitch] = {{note}};
		} else {
			vector<vector<MidiNote>> &line = (*data)[note.pitch];
			if (note.start_pos - line.back().back().end_pos <= epsilon) {
				line.back().push_back(note);
			} else {
				line.push_back({note});
			}
		}

		n = MIDI_EnumSelNotes(take, n);
	}
	return true;
}

void note_stutter_incr(COMMAND_T* cmd) {
	HWND hwnd = MIDIEditor_GetActive();
	if (!hwnd) return;
	MediaItem_Take* take = MIDIEditor_GetTake(hwnd);
	if (!take) return;
	unordered_map<int, vector<vector<MidiNote>>> data;
	if (!generate_data(take, &data)) return;

	MIDI_DisableSort(take);
	int new_count;
	double new_start, new_end, new_len;
	bool sel = true;
	MidiNote note;
	Undo_BeginBlock2(nullptr);
	for (const auto &b: data) {
		for (const auto &v: b.second) {
			new_count = v.size();
			new_start = v.front().start_pos, new_end = v.back().end_pos;
			new_len = (new_end - new_start) / (new_count + 1);
			for (int i = 0; i < new_count; i++) {
				new_end = new_start + new_len;
				note = v[i];
				MIDI_SetNote(take, note.idx, &sel, &note.mute, &new_start, &new_end, &note.chan, &note.pitch, &note.vel, nullptr);
				new_start = new_start + new_len;
			}
			MIDI_InsertNote(take, true, note.mute, new_end, v.back().end_pos, note.chan, note.pitch, note.vel, nullptr);
		}
	}
	Undo_EndBlock2(nullptr, "Midi note stutter increment", 2);
	MIDI_Sort(take);
}

void note_stutter_decr(COMMAND_T* cmd) {
	HWND hwnd = MIDIEditor_GetActive();
	if (!hwnd) return;
	MediaItem_Take* take = MIDIEditor_GetTake(hwnd);
	if (!take) return;
	int n = MIDI_EnumSelNotes(take, -1);
	if (n < 0) return;
	unordered_map<int, vector<vector<MidiNote>>> data;
	if (!generate_data(take, &data)) return;

	vector<vector<MidiNote>> s_data;
	for (const auto &p_line: data) {
		for (const auto &line: p_line.second) {
			if (line.size() > 1) {
				s_data.push_back(move(line));
			}
		}
	}
	if (s_data.empty()) return;
	std::sort(s_data.begin(), s_data.end(), [](const std::vector<MidiNote> &a, const std::vector<MidiNote> &b) { return a.back().idx < b.back().idx; });

	MIDI_DisableSort(take);
	int new_count;
	double new_start, new_end, new_len;
	bool sel = true;
	MidiNote note;
	Undo_BeginBlock2(nullptr);
	for (auto line = s_data.rbegin(); line != s_data.rend(); ++line) {
		if ((new_count = line->size()) == 1) continue;
		MIDI_DeleteNote(take, (*line)[new_count - 1].idx);
		new_start = line->front().start_pos, new_end = line->back().end_pos;
		new_len = (new_end - new_start) / (new_count - 1);
		for (int i = 0; i < new_count - 1; i++) {
			new_end = new_start + new_len;
			note = (*line)[i];
			MIDI_SetNote(take, note.idx, &sel, &note.mute, &new_start, &new_end, &note.chan, &note.pitch, &note.vel, nullptr);
			new_start = new_start + new_len;
		}
	}
	Undo_EndBlock2(nullptr, "Midi note stutter decrement", 2);
	MIDI_Sort(take);
}


void note_stutter_mousewheel(COMMAND_T* cmd, int val, int val2, int relmode, HWND hwnd) {
	if (relmode <= 0) return;
	if (val < 64) {
		note_stutter_incr(cmd);
	} else if (val > 64) {
		note_stutter_decr(cmd);
	}
}


void set_loop_time_range_to_active_take(COMMAND_T* cmd) {
	HWND hwnd = MIDIEditor_GetActive();
	if (!hwnd) return;
	MediaItem_Take* take = MIDIEditor_GetTake(hwnd);
	if (!take) return;
	MediaItem* item = GetMediaItemTake_Item(take);
	double start = GetMediaItemInfo_Value(item, "D_POSITION"), end = start + GetMediaItemInfo_Value(item, "D_LENGTH");
	GetSet_LoopTimeRange2(nullptr, true, cmd->user, &start, &end, true);
}
