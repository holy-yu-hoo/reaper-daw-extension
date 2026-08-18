#include "utils.h"
#include <reaper_plugin_functions.h>

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


std::pair<int, int> get_track_fx_chunk_borders(std::string chunk, std::string fx_guid) {
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

	std::string guid;
	guid.resize(38);
	guidToString(TrackFX_GetFXGUID(track, fx), guid.data());

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

std::string set_track_fx_chunk(MediaTrack *track, int fx, std::string new_fx_chunk) { // return new track chunk, NOT change track
	std::string chunk;
	bool ret = get_track_state_chunk(track, chunk);
	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get track chunk\n");
		#endif
		return "";
	}

	std::string guid;
	guid.resize(38);
	guidToString(TrackFX_GetFXGUID(track, fx), guid.data());

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
