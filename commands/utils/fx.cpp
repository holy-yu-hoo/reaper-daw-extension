#include "fx.h"
#include "misc.h"
#include "api.h"


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


std::pair<int, int> get_fx_chunk_borders(const std::string &chunk, const std::string &fx_guid) {
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


std::string get_fx_chunk(MediaTrack *track, int fx) {
	std::string chunk;
	bool ret = get_state_chunk(track, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
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
	std::pair<int, int> fx_chunk_borders = get_fx_chunk_borders(chunk, guid);
	std::string fx_chunk = chunk.substr(fx_chunk_borders.first, fx_chunk_borders.second - fx_chunk_borders.first);

	return fx_chunk;
}

std::string get_fx_chunk(MediaItem_Take *take, int fx) {
	MediaItem *item = GetMediaItemTake_Item(take);
	std::string chunk;
	bool ret = get_state_chunk(item, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return "";
	}

	std::string guid = guid_to_string(TakeFX_GetFXGUID(take, fx));

	if (guid.empty()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
		#endif
		return "";
	}
	std::pair<int, int> fx_chunk_borders = get_fx_chunk_borders(chunk, guid);
	std::string fx_chunk = chunk.substr(fx_chunk_borders.first, fx_chunk_borders.second - fx_chunk_borders.first);

	return fx_chunk;
}

bool set_fx_chunk(MediaTrack *track, int fx, const std::string &new_fx_chunk) { // return new track chunk, NOT change track
	std::string chunk;
	bool ret = get_state_chunk(track, chunk);
	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
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
	std::pair<int, int> fx_chunk_borders = get_fx_chunk_borders(chunk, guid);
	std::string new_chunk = chunk.substr(0, fx_chunk_borders.first) + new_fx_chunk + chunk.substr(fx_chunk_borders.second);
	return SetTrackStateChunk(track, new_chunk.data(), false);
}

bool set_fx_chunk(MediaItem_Take *take, int fx, const std::string &new_fx_chunk) {
	MediaItem *item = GetMediaItemTake_Item(take);
	std::string chunk;
	bool ret = get_state_chunk(item, chunk);
	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return "";
	}

	std::string guid = guid_to_string(TakeFX_GetFXGUID(take, fx));

	if (guid.empty()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
		#endif
		return "";
	}
	std::pair<int, int> fx_chunk_borders = get_fx_chunk_borders(chunk, guid);
	std::string new_chunk = chunk.substr(0, fx_chunk_borders.first) + new_fx_chunk + chunk.substr(fx_chunk_borders.second);
	return SetItemStateChunk(item, new_chunk.data(), false);

}
