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
