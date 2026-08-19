#pragma once
#include "actions.h"
#include "../core/api.h"
#include "../utils/fx.h"
#include "../utils/utils.h"
#include "../utils/misc.h"
#include <unordered_map>

void insert_4_bars_midi_item_at_cursor() {
	MediaTrack *track = nullptr;
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
	MediaItem *item = CreateNewMIDIItemInProj(track, curpos, curpos + length, &qn_optional);
	if (!item) {
		ShowConsoleMsg("Failed to create MIDI item!\n");
		return;
	} else {
		Undo_EndBlock2(nullptr, "Add media item", 4);
	}

	UpdateArrange();
}


void fx_ab_comparer() {
	static std::unordered_map<std::string, std::string> presets;
	int track_id, item_id, take_id, fx_id, param_id;
	bool ret = GetLastFocusedFX(&track_id, &item_id, &take_id, &fx_id, &param_id);
	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Not focused plugin\n");
		#endif
		return;
	} else {
		#if defined(_DEBUG)
		ShowConsoleMsg("true\n");
		#endif
	}
	MediaTrack *track = (track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, track_id); // Master : regular
	std::string guid, fx_chunk;


	if (item_id == -1) { // track fx
		guid = guid_to_string(TrackFX_GetFXGUID(track, fx_id));
		auto preset = presets.find(guid);
		fx_chunk = get_fx_chunk(track, fx_id);
		if (preset == presets.end()) {
			presets[guid] = fx_chunk;
			reset_fx_preset(track, fx_id);
		} else {
			set_fx_chunk(track, fx_id, preset->second);
			presets[guid] = fx_chunk;
		}
	} else { // item fx
		MediaItem *item = GetTrackMediaItem(track, item_id);
		MediaItem_Take *take = GetMediaItemTake(item, take_id);
		std::string chunk;
		guid = guid_to_string(TakeFX_GetFXGUID(take, fx_id));
		auto preset = presets.find(guid);
		fx_chunk = get_fx_chunk(take, fx_id);
		if (preset == presets.end()) {
			presets[guid] = fx_chunk;
			reset_fx_preset(take, fx_id);
		} else {
			set_fx_chunk(take, fx_id, preset->second);
			presets[guid] = fx_chunk;
		}
		
	}
};
