#include "api.h"
#include "fx.h"
#include "misc.h"


// bool GetFxByGUID(GUID *guid, int *track_id_out, int *item_id_out, int *take_id_out, int *fx_id_out) {
// 	MediaTrack *track = GetMasterTrack(nullptr);
// 	for (int i = 0; i < TrackFX_GetCount(track); i++) {
// 		if (*TrackFX_GetFXGUID(track, i) == *guid) {
// 			*track_id_out = -1;
// 			*item_id_out = -1;
// 			*take_id_out = -1;
// 			*fx_id_out = i;
// 			return true;
// 		}
// 	}
//
// 	for (int i = 0; i < TrackFX_GetRecCount(track); i++) {
// 		if (*TrackFX_GetFXGUID(track, rec_fx + i) == *guid) {
// 			*track_id_out = -1;
// 			*item_id_out = -1;
// 			*take_id_out = -1;
// 			*fx_id_out = rec_fx + i;
// 			return true;
// 		}
// 	}
//
// 	for (int j = 0; j < CountTracks(nullptr); j++) {
// 		track = GetTrack(nullptr, j);
// 		for (int i = 0; i < TrackFX_GetCount(track); i++) {
// 			if (*TrackFX_GetFXGUID(track, i) == *guid) {
// 				*track_id_out = j;
// 				*item_id_out = -1;
// 				*take_id_out = -1;
// 				*fx_id_out = i;
// 				return true;
// 			}
// 		}
//
//
// 		for (int i = 0; i < TrackFX_GetRecCount(track); i++) {
// 			if (*TrackFX_GetFXGUID(track,rec_fx + i) == *guid) {
// 				*track_id_out = j;
// 				*item_id_out = -1;
// 				*take_id_out = -1;
// 				*fx_id_out = rec_fx + i;
// 				return true;
// 			}
// 		}
// 		MediaItem *item;
// 		MediaItem_Take *take;
// 		for (int i = 0; i < CountTrackMediaItems(track); i++) {
// 			item = GetTrackMediaItem(track, i);
// 			for (int k = 0; k < CountTakes(item); k++) {
// 				take = GetMediaItemTake(item, k);
// 				for (int w = 0; w < TakeFX_GetCount(take); w++) {
// 					if (*TakeFX_GetFXGUID(take, w) == *guid) {
// 						*track_id_out = j;
// 						*item_id_out = i;
// 						*take_id_out = k;
// 						*fx_id_out = w;
// 						return true;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return false;
// }

bool GetFxByGUID(GUID *guid, int *track_id_out, int *item_id_out, int *take_id_out, int *fx_id_out) {
	MediaTrack *track = GetMasterTrack(nullptr);

	auto check_container_track = [&](MediaTrack *t, int fx_idx, bool is_rec, int track_id) -> bool {
		int chain_count = is_rec ? TrackFX_GetRecCount(t) : TrackFX_GetCount(t);
		int base_flag = is_rec ? rec_fx : 0;
		int current_idx = base_flag + fx_idx;

		char buf[64];
		if (TrackFX_GetNamedConfigParm(t, current_idx, "container_count", buf, sizeof(buf))) {
			int container_count = atoi(buf);
			if (container_count > 0) {
				int subitem_base = chain_count + 1;
				for (int c = 0; c < container_count; c++) {
					int container_idx = cont_fx + (c + 1) * subitem_base + (fx_idx + 1);
					GUID *container_guid = TrackFX_GetFXGUID(t, container_idx);
					if (container_guid && *container_guid == *guid) {
						*track_id_out = track_id;
						*item_id_out = -1;
						*take_id_out = -1;
						*fx_id_out = container_idx;
						return true;
					}
				}
			}
		}
		return false;
	};

	auto check_container_take = [&](MediaItem_Take *take, int fx_idx, int track_id, int item_id, int take_id) -> bool {
		int chain_count = TakeFX_GetCount(take);
		char buf[64];
		if (TakeFX_GetNamedConfigParm(take, fx_idx, "container_count", buf, sizeof(buf))) {
			int container_count = atoi(buf);
			if (container_count > 0) {
				int subitem_base = chain_count + 1;
				for (int c = 0; c < container_count; c++) {
					int container_idx = cont_fx + (c + 1) * subitem_base + (fx_idx + 1);
					GUID *container_guid = TakeFX_GetFXGUID(take, container_idx);
					if (container_guid && *container_guid == *guid) {
						*track_id_out = track_id;
						*item_id_out = item_id;
						*take_id_out = take_id;
						*fx_id_out = container_idx;
						return true;
					}
				}
			}
		}
		return false;
	};

	for (int i = 0; i < TrackFX_GetCount(track); i++) {
		GUID *current_guid = TrackFX_GetFXGUID(track, i);
		if (current_guid && *current_guid == *guid) {
			*track_id_out = -1;
			*item_id_out = -1;
			*take_id_out = -1;
			*fx_id_out = i;
			return true;
		}
		if (check_container_track(track, i, false, -1)) return true;
	}

	for (int i = 0; i < TrackFX_GetRecCount(track); i++) {
		GUID *current_guid = TrackFX_GetFXGUID(track, rec_fx + i);
		if (current_guid && *current_guid == *guid) {
			*track_id_out = -1;
			*item_id_out = -1;
			*take_id_out = -1;
			*fx_id_out = rec_fx + i;
			return true;
		}
		if (check_container_track(track, i, true, -1)) return true;
	}

	for (int j = 0; j < CountTracks(nullptr); j++) {
		track = GetTrack(nullptr, j);

		for (int i = 0; i < TrackFX_GetCount(track); i++) {
			GUID *current_guid = TrackFX_GetFXGUID(track, i);
			if (current_guid && *current_guid == *guid) {
				*track_id_out = j;
				*item_id_out = -1;
				*take_id_out = -1;
				*fx_id_out = i;
				return true;
			}
			if (check_container_track(track, i, false, j)) return true;
		}

		for (int i = 0; i < TrackFX_GetRecCount(track); i++) {
			GUID *current_guid = TrackFX_GetFXGUID(track, rec_fx + i);
			if (current_guid && *current_guid == *guid) {
				*track_id_out = j;
				*item_id_out = -1;
				*take_id_out = -1;
				*fx_id_out = rec_fx + i;
				return true;
			}
			if (check_container_track(track, i, true, j)) return true;
		}

		for (int i = 0; i < CountTrackMediaItems(track); i++) {
			MediaItem *item = GetTrackMediaItem(track, i);
			for (int k = 0; k < CountTakes(item); k++) {
				MediaItem_Take *take = GetMediaItemTake(item, k);
				for (int w = 0; w < TakeFX_GetCount(take); w++) {
					GUID *current_guid = TakeFX_GetFXGUID(take, w);
					if (current_guid && *current_guid == *guid) {
						*track_id_out = j;
						*item_id_out = i;
						*take_id_out = k;
						*fx_id_out = w;
						return true;
					}
					if (check_container_take(take, w, j, i, k)) return true;
				}
			}
		}
	}

	return false;
}

bool GetFxChainByHWND(HWND hwnd, int *track_id_out, int *item_id_out, int *take_id_out, int *is_rec) {
	MediaTrack *track = GetMasterTrack(nullptr);
	if (CF_GetTrackFXChainEx(nullptr, track, false) == hwnd) {
		*track_id_out = -1;
		*item_id_out = -1;
		*take_id_out = -1;
		*is_rec = 0;
		return true;
	}

	if (CF_GetTrackFXChainEx(nullptr, track, true) == hwnd) {
		*track_id_out = -1;
		*item_id_out = -1;
		*take_id_out = -1;
		*is_rec = 1;
		return true;
	}

	for (int j = 0; j < CountTracks(nullptr); j++) {
		track = GetTrack(nullptr, j);

		if (CF_GetTrackFXChainEx(nullptr, track, false) == hwnd) {
			*track_id_out = j;
			*item_id_out = -1;
			*take_id_out = -1;
			*is_rec = 0;
			return true;
		}

		if (CF_GetTrackFXChainEx(nullptr, track, true) == hwnd) {
			*track_id_out = j;
			*item_id_out = -1;
			*take_id_out = -1;
			*is_rec = 1;
			return true;
		}

		MediaItem *item;
		MediaItem_Take *take;
		for (int i = 0; i < CountTrackMediaItems(track); i++) {
			item = GetTrackMediaItem(track, i);
			for (int k = 0; k < CountTakes(item); k++) {
				take = GetMediaItemTake(item, k);
				if (CF_GetTakeFXChain(take) == hwnd) {
					*track_id_out = j;
					*item_id_out = i;
					*take_id_out = k;
					*is_rec = 0;
					return true;
				}
			}
		}
	}
	return false;
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

bool reset_fx_chain_preset(MediaTrack *track, int fx) {
	if (fx & rec_fx) {
		for (int i = TrackFX_GetRecCount(track) - 1; i >= 0; i--) {
			TrackFX_Delete(track, 0x1000000 + i);
		}
	} else {
		for (int i = TrackFX_GetCount(track) - 1; i >= 0; i--) {
			TrackFX_Delete(track, i);
		}
	}
	return true;
}

bool reset_fx_chain_preset(MediaItem_Take *take, int fx) {
	for (int i = TakeFX_GetCount(take) - 1; i >= 0; i--) {
		TakeFX_Delete(take, i);
	}
	return true;
}


std::pair<int, int> get_fx_chunk_boundaries(const std::string &chunk, const std::string &fx_guid) {
	int i = chunk.find(fx_guid); // guid position
	if (i == std::string::npos) return {0, 0};
	while (i >= 0 and chunk[i] != '>') i--;
	if (i < 0) return {0, 0};
	int e = --i;

	int c = 1;
	while (c > 0 and i > 0) {
		if (chunk[i] == '<') {
			c--;
		} else if (chunk[i] == '>') {
			c++;
		}
		i--;
	}
	if (c > 0) return {0, 0};
	i++;
	while (i < chunk.length() and chunk[i] != '\n') i++;
	if (i >= chunk.length()) return {0, 0};
	int s = ++i;
	return {s, e};
}

std::pair<int, int> get_fx_chain_chunk_boundaries(const std::string &chunk, const std::string &targ, int count = 1) { // targ = "<FXCHAIN" or "<FXCHAIN_REC" or "<TAKEFX"
	int s = 0;
	for (int i = 0; i < count; i++) {
		s = chunk.find(targ, s);
		if (s == std::string::npos) return {0, 0};
		else s += targ.length();
	}

	s = chunk.find("<", s);
	if (s == std::string::npos) return {0, 0};
	int c = 2;
	int i = ++s;
	while (c > 0 and i < chunk.length()) {
		if (chunk[i] == '>') {
			c--;
		} else if (chunk[i] == '<') {
			c++;
		}
		i++;
	}

	if (c > 0) return {0, 0};
	int e = i - 2;
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
	std::pair<int, int> fx_chunk_boundaries = get_fx_chunk_boundaries(chunk, guid);
	std::string fx_chunk = chunk.substr(fx_chunk_boundaries.first, fx_chunk_boundaries.second - fx_chunk_boundaries.first);

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
	std::pair<int, int> fx_chunk_boundaries = get_fx_chunk_boundaries(chunk, guid);
	std::string fx_chunk = chunk.substr(fx_chunk_boundaries.first, fx_chunk_boundaries.second - fx_chunk_boundaries.first);

	return fx_chunk;
}

bool set_fx_chunk(MediaTrack *track, int fx, const std::string &new_fx_chunk) {
	std::string chunk;
	bool ret = get_state_chunk(track, chunk);
	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return false;
	}

	std::string guid = guid_to_string(TrackFX_GetFXGUID(track, fx));

	if (guid.empty()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
		#endif
		return false;
	}
	std::pair<int, int> fx_chunk_boundaries = get_fx_chunk_boundaries(chunk, guid);
	if (fx_chunk_boundaries == std::pair<int, int>(0, 0)) return false;
	std::string new_chunk = chunk.substr(0, fx_chunk_boundaries.first) + new_fx_chunk + chunk.substr(fx_chunk_boundaries.second);
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
		return false;
	}

	std::string guid = guid_to_string(TakeFX_GetFXGUID(take, fx));

	if (guid.empty()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
		#endif
		return false;
	}
	std::pair<int, int> fx_chunk_boundaries = get_fx_chunk_boundaries(chunk, guid);
	if (fx_chunk_boundaries == std::pair<int, int>(0, 0)) return false;

	std::string new_chunk = chunk.substr(0, fx_chunk_boundaries.first) + new_fx_chunk + chunk.substr(fx_chunk_boundaries.second);
	return SetItemStateChunk(item, new_chunk.data(), false);
}


std::string get_fx_chain_chunk(MediaTrack *track, int fx) {
	std::string chunk;
	bool ret = get_state_chunk(track, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return "";
	}

	std::pair<int, int> fx_chain_chunk_boundaries = get_fx_chain_chunk_boundaries(chunk, fx & rec_fx ? "<FXCHAIN_REC" : "<FXCHAIN");
	if (fx_chain_chunk_boundaries == std::pair<int, int>(0, 0)) return "";
	std::string fx_chain_chunk = chunk.substr(fx_chain_chunk_boundaries.first, fx_chain_chunk_boundaries.second - fx_chain_chunk_boundaries.first);

	return fx_chain_chunk;
}

std::string get_fx_chain_chunk(MediaItem_Take *take, int fx) {
	MediaItem *item = GetMediaItemTake_Item(take);
	int tk = 0;
	while (tk < GetMediaItemNumTakes(item) and take != GetMediaItemTake(item, tk)) tk++;
	std::string chunk;
	bool ret = get_state_chunk(item, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return "";
	}

	std::pair<int, int> fx_chain_chunk_boundaries = get_fx_chain_chunk_boundaries(chunk, "<TAKEFX", tk + 1);
	if (fx_chain_chunk_boundaries == std::pair<int, int>(0, 0)) return "";
	std::string fx_chain_chunk = chunk.substr(fx_chain_chunk_boundaries.first, fx_chain_chunk_boundaries.second - fx_chain_chunk_boundaries.first);

	return fx_chain_chunk;
}


bool set_fx_chain_chunk(MediaTrack *track, int fx, const std::string &new_fx_chunk) {
	std::string chunk;
	bool ret = get_state_chunk(track, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return false;
	}

	std::pair<int, int> fx_chain_chunk_boundaries = get_fx_chain_chunk_boundaries(chunk, fx & rec_fx ? "<FXCHAIN_REC" : "<FXCHAIN");
	if (fx_chain_chunk_boundaries == std::pair<int, int>(0, 0)) return false;

	std::string new_chunk = chunk.substr(0, fx_chain_chunk_boundaries.first) + new_fx_chunk + chunk.substr(fx_chain_chunk_boundaries.second);
	return SetTrackStateChunk(track, new_chunk.data(), false);
}

bool set_fx_chain_chunk(MediaItem_Take *take, int fx, const std::string &new_fx_chunk) {
	MediaItem *item = GetMediaItemTake_Item(take);
	int tk = 0;
	while (tk < GetMediaItemNumTakes(item) and take != GetMediaItemTake(item, tk)) tk++;
	std::string chunk;
	bool ret = get_state_chunk(item, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return false;
	}

	std::pair<int, int> fx_chain_chunk_boundaries = get_fx_chain_chunk_boundaries(chunk, "<TAKEFX", tk + 1);
	if (fx_chain_chunk_boundaries == std::pair<int, int>(0, 0)) return false;

	std::string new_chunk = chunk.substr(0, fx_chain_chunk_boundaries.first) + new_fx_chunk + chunk.substr(fx_chain_chunk_boundaries.second);
	return SetItemStateChunk(item, new_chunk.data(), false);
}
