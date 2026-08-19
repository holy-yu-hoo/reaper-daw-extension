#include "utils.h"
#include "../core/api.h"

std::string guid_to_string(GUID *guid) {
	std::string ret;
	ret.resize(38);
	guidToString(guid, ret.data());
	return ret;
}

bool has_single_selected_track(MediaTrack *&out_track) {
	if (CountSelectedTracks(nullptr) != 1) {
		return false;
	}
	out_track = GetSelectedTrack(nullptr, 0);
	return out_track != nullptr;
}

bool is_track_under_mouse(MediaTrack *track, int *context_out) {
	if (!BR_TrackAtMouseCursor) {
		return false;
	}
	int context = 0;
	const MediaTrack *hovered = BR_TrackAtMouseCursor(&context, nullptr);
	if (context_out) {
		*context_out = context;
	}
	return hovered == track;
}


double bars_to_seconds(int bars, double bpm, double beats_per_bar) {
	if (bars <= 0) {
		return 0.0;
	}
	if (bpm <= 0.0 || beats_per_bar <= 0.0) {
		bpm = 120.0;
		beats_per_bar = 4.0;
	}
	return (bars * beats_per_bar * 60.0) / bpm;
}

bool reset_fx_preset(MediaTrack *track, int fx) {
	if (!TrackFX_SetPresetByIndex(track, fx, -1)) {
		return TrackFX_SetPresetByIndex(track, fx, -2);
	}
	return true;
}

bool reset_fx_preset(MediaItem_Take *take, int fx) {
	if (!TakeFX_SetPresetByIndex(take, fx, -1)) {
		return TakeFX_SetPresetByIndex(take, fx, -2);
	}
	return true;
}


bool get_track_state_chunk(MediaTrack *track, std::string &chunk) {
	chunk.clear();
	chunk.resize(4096);
	bool ret = GetTrackStateChunk(track, chunk.data(), static_cast<int>(chunk.size()), false);
	while (ret && (strlen(chunk.data()) >= (chunk.size() - 1))) {
		chunk.resize(chunk.size() * 2);
		ret = GetTrackStateChunk(track, chunk.data(), static_cast<int>(chunk.size()), false);
	}
	chunk.resize(strlen(chunk.data()));
	return ret;
}


std::pair<int, int> get_track_fx_chunk_borders(const std::string &chunk, const std::string &fx_guid) {
	int i = chunk.find(fx_guid); // guid position
	while (chunk[i] != '>') i--;
	int e = --i;

	int c = 1;
	while (c > 0 && i > 0) {
		if (chunk[i] == '<') {
			c--;
		} else if (chunk[i] == '>') {
			c++;
		}
		i--;
	}
	i++;
	while (chunk[i] != '\n') i++;
	int s = ++i;
	return {s, e};
}


std::string get_track_fx_chunk(MediaTrack *track, int fx) {
	std::string chunk;
	bool ret = get_track_state_chunk(track, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get track chunk\n");
		#endif
		return "";
	}

	std::string guid = guid_to_string(TrackFX_GetFXGUID(track, fx));

	if (guid.empty()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
		#endif
		return "";
	}
	std::pair<int, int> fx_chunk_borders = get_track_fx_chunk_borders(chunk, guid);
	std::string fx_chunk = chunk.substr(fx_chunk_borders.first, fx_chunk_borders.second - fx_chunk_borders.first);

	return fx_chunk;
}

std::string set_track_fx_chunk(MediaTrack *track, int fx, const std::string &new_fx_chunk) { // return new track chunk, NOT change track
	std::string chunk;
	bool ret = get_track_state_chunk(track, chunk);
	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get track chunk\n");
		#endif
		return "";
	}

	std::string guid = guid_to_string(TrackFX_GetFXGUID(track, fx));

	if (guid.empty()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
		#endif
		return "";
	}
	std::pair<int, int> fx_chunk_borders = get_track_fx_chunk_borders(chunk, guid);
	std::string new_chunk = chunk.substr(0, fx_chunk_borders.first) + new_fx_chunk + chunk.substr(fx_chunk_borders.second);
	return new_chunk;
}
