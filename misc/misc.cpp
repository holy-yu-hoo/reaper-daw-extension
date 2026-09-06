#include "api.h"
#include "misc.h"
#include "utils.h"

#define zoom_out_hor 1011
#define zoom_in_hor 1012

int g_h_zoom_mode;
int g_v_zoom_mode;

void save_hor_zoom_mode(COMMAND_T* cmd) {

	int sz = 0;
	void* ptr = get_config_var("zoommode", &sz);
	if (!ptr || sz != sizeof(int)) return;

	g_h_zoom_mode = *(int *) ptr;
}

void restore_hor_zoom_mode(COMMAND_T* cmd) {

	int sz = 0;
	void* ptr = get_config_var("zoommode", &sz);
	if (!ptr || sz != sizeof(int)) return;

	*(int *) ptr = g_h_zoom_mode;
}


void save_ver_zoom_mode(COMMAND_T* cmd) {

	int sz = 0;
	void* ptr = get_config_var("vzoommode", &sz);
	if (!ptr || sz != sizeof(int)) return;

	g_v_zoom_mode = *(int *) ptr;
}

void restore_ver_zoom_mode(COMMAND_T* cmd) {

	int sz = 0;
	void* ptr = get_config_var("vzoommode", &sz);
	if (!ptr || sz != sizeof(int)) return;

	*(int *) ptr = g_v_zoom_mode;
}


void insert_4_bars_midi_item_at_cursor(COMMAND_T* cmd) {
	MediaTrack* track = nullptr;
	if (!has_single_selected_track(track)) {
		return;
	}
	if (track == GetMasterTrack(nullptr)) {
		return;
	}

	int context = 0;
	if (!BR_TrackAtMouseCursor) {
		ShowConsoleMsg("Need SWS extension\n");
		return;
	}
	if (!is_track_under_mouse(track, &context) || context != 2) {
		return;
	}

	Undo_BeginBlock2(nullptr);
	const double curpos = GetCursorPositionEx(nullptr);
	double bpm = 0.0;
	double beats_per_bar = 4.0;
	GetProjectTimeSignature2(nullptr, &bpm, &beats_per_bar);
	if (bpm <= 0.0) {
		bpm = 120.0;
	}

	const double length = bars_to_seconds(4, bpm, beats_per_bar);
	bool qn_optional = false;
	MediaItem* item = CreateNewMIDIItemInProj(track, curpos, curpos + length, &qn_optional);
	if (!item) {
		ShowConsoleMsg("Failed to create MIDI item!\n");
		return;
	} else {
		Undo_EndBlock2(nullptr, "Add media item", 4);
	}

	UpdateArrange();
}
