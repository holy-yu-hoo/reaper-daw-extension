#include "api.h"
#include "utils.h"

bool get_state_chunk(MediaTrack* track, std::string &chunk) {
	chunk.clear();
	chunk.resize(4096);
	bool ret = GetTrackStateChunk(track, &chunk.front(), static_cast<int>(chunk.size()), false);
	while (ret && (strlen(chunk.data()) >= (chunk.size() - 1))) {
		chunk.resize(chunk.size() * 2);
		ret = GetTrackStateChunk(track, &chunk.front(), static_cast<int>(chunk.size()), false);
	}
	chunk.resize(strlen(chunk.data()));
	return ret;
}

bool get_state_chunk(MediaItem* item, std::string &chunk) {
	chunk.clear();
	chunk.resize(4096);
	bool ret = GetItemStateChunk(item, &chunk.front(), static_cast<int>(chunk.size()), false);
	while (ret && (strlen(chunk.data()) >= (chunk.size() - 1))) {
		chunk.resize(chunk.size() * 2);
		ret = GetItemStateChunk(item, &chunk.front(), static_cast<int>(chunk.size()), false);
	}
	chunk.resize(strlen(chunk.data()));
	return ret;
}


std::string guid_to_string(const GUID* guid) {
	std::string ret;
	ret.resize(64);
	guidToString(guid, &ret.front());
	ret.resize(strlen(ret.data()));
	return ret;
}

void string_to_guid(const std::string &p_guid, GUID* guid) {
	stringToGuid(p_guid.data(), guid);
}

bool has_single_selected_track(MediaTrack*&out_track) {
	if (CountSelectedTracks(nullptr) != 1) {
		return false;
	}
	out_track = GetSelectedTrack(nullptr, 0);
	return out_track != nullptr;
}

bool is_track_under_mouse(MediaTrack* track, int* context_out) {
	if (!BR_TrackAtMouseCursor) {
		return false;
	}
	int context = 0;
	const MediaTrack* hovered = BR_TrackAtMouseCursor(&context, nullptr);
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

std::string get_set_media_track_info_string(MediaTrack* tr, const char* parmname, bool setNewValue) {
	std::string str;
	str.resize(4096);
	GetSetMediaTrackInfo_String(tr, parmname, &str.front(), setNewValue);
	str.resize((strlen(str.data())));
	return str;
}

bool is_grid_triplet() { // from SWS
	int size = 0;
	int offset = projectconfig_var_getoffs("projgriddiv", &size);
	void* addr = projectconfig_var_addr(nullptr, offset);
	double grid = *static_cast<double*>(addr);
	if (grid < 1e8) return false;
	double n = 1.0 / grid;

	while (n < 3.0) { n *= 2.0; }

	double r = fmod(n, 3.0);
	return r < 0.000001 || r > 2.99999;

}

bool is_grid_dotted() { // from SWS
	int size = 0;
	int offset = projectconfig_var_getoffs("projgriddiv", &size);
	void* addr = projectconfig_var_addr(nullptr, offset);
	double grid = *static_cast<double*>(addr);
	if (grid < 1e8) return false;
	double n = 1.0 / grid;

	while (n < (2.0 / 3.0)) { n *= 2.0; }
	while (n > (4.0 / 3.0)) { n *= 0.5; }

	double r = fmod(n, (2.0 / 3.0));
	return r < 0.000001 || r > 0.66666;

}

bool is_grid_swing() { // from SWS
	int size = 0;
	int offset = projectconfig_var_getoffs("projgridframe", &size);
	void* addr = projectconfig_var_addr(nullptr, offset);
	int frame = *static_cast<int*>(addr);
	return ((frame & 8) == 1);

}

double get_grid_swing() {
	int size = 0;
	int offset = projectconfig_var_getoffs("projgridswing", &size);
	void* addr = projectconfig_var_addr(nullptr, offset);
	double swing = *static_cast<double*>(addr);
	return swing;

}

int get_grid_type() {
	if (is_grid_triplet()) {
		return 2;

	} else if (is_grid_dotted()) {
		return 4;

	} else if (is_grid_swing()) {
		return 8;

	} else {
		return 0;

	}

}

MediaItem_Take* get_media_item_take_by_guid(ReaProject* proj, std::string &p_guid) {
	if (p_guid.empty()) return nullptr;
	GUID guid;
	string_to_guid(p_guid, &guid);
	MediaItem* item;
	MediaItem_Take* take;
	for (int i = 0; i < CountMediaItems(proj); i++) {
		item = GetMediaItem(proj, i);
		for (int j = 0; j < CountTakes(item); j++) {
			take = GetTake(item, j);
			if (guid == *static_cast<GUID*>(GetSetMediaItemTakeInfo(take, "GUID", nullptr))) {
				return take;
			}
		}
	}
	return nullptr;
}
