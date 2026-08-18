#include "commands.h"
#include "utils/utils.h"
std::map<int, void (*)()> actions;

bool init_actions(const reaper_plugin_info_t *rec) {
	for (int i = 0; i < actions_count; ++i) {
		int action_id = rec->Register("custom_action", static_cast<void *>(&commands[i].action));
		if (!action_id) return false;
		actions[action_id] = commands[i].func;
	}
	return true;
}

void Hello() {
	ShowConsoleMsg("My C++ Extension loaded successfully!\n");
}

void insert_4_bars_midi_item_at_cursor() {
	int count_sel_tracks = CountSelectedTracks(nullptr);
	if (count_sel_tracks != 1) return;
	MediaTrack *track = GetSelectedTrack(nullptr, 0);
	if (track == GetMasterTrack(nullptr)) return;
	int ctx = 0;
	if (!BR_TrackAtMouseCursor) {
		ShowConsoleMsg("Need SWS extension");
		return;
	}
	if (track != BR_TrackAtMouseCursor(&ctx, nullptr)) return;
	if (ctx != 2) return;
	double curpos = GetCursorPositionEx(nullptr);

	double bpm = 0.0;
	double beats_per_bar = 4; // дефолт 4/4
	GetProjectTimeSignature2(nullptr, &bpm, &beats_per_bar);
	if (bpm <= 0.0) bpm = 120.0;
	constexpr int BARS = 4;
	double length = (BARS * beats_per_bar * 60.0) / bpm;

	bool qn_optional = false;
	MediaItem *item = CreateNewMIDIItemInProj(track, curpos, curpos + length, &qn_optional);


	if (item) {
		UpdateArrange();
	} else {
		ShowConsoleMsg("Failed to create MIDI item!\n");
	}
}

void show_track_fx_chunk() {
	int tracknumberOut, fxnumberOut, paramnumberOut;
	bool ret = GetLastTouchedFX(&tracknumberOut, &fxnumberOut, &paramnumberOut);
	if (!ret || tracknumberOut <= 0) {
		ShowConsoleMsg("No last touched FX\n");
		return;
	}
	MediaTrack *track = GetTrack(nullptr, tracknumberOut - 1);
	if (!track) {
		ShowConsoleMsg("Track not found\n");
		return;
	}

	std::string fx_chunk = get_track_fx_chunk(track, fxnumberOut);
	if (fx_chunk.empty()) {
		ShowConsoleMsg("FX GUID not found in track chunk\n");
		return;
	}
	ShowConsoleMsg(fx_chunk.c_str());
}
