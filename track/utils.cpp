#include "api.h"
#include "utils.h"

void unsolo_all(ReaProject* proj) {
	PreventUIRefresh(1);
	MediaTrack* track;
	for (int i = 0; i < CountTracks(proj); i++) {
		track = GetTrack(proj, i);
		if (GetMediaTrackInfo_Value(track, "I_SOLO") != 0) SetMediaTrackInfo_Value(track, "I_SOLO", 0);
	}
	PreventUIRefresh(-1);
}

int get_cycled_index(int idx, int offset, int total) {
	if (total <= 0) return -1;
	int n = idx + offset;
	return (n < 0) ? (n + total) : (n % total);
}

int get_solo_ip_settings() {
	int sz = 0;
	int* ptr = static_cast<int*>(get_config_var("soloip", &sz));
	return *ptr & 1;
}


void AutoSoloModeLastTouch::OnActivate() {
	MediaTrack* track = GetLastTouchedTrack();
	if (!track) return;
	unsolo_all();
	SetMediaTrackInfo_Value(track, "I_SOLO", 1 + get_solo_ip_settings());
}

void AutoSoloModeLastTouch::OnDeactivate() { unsolo_all(); }

bool AutoSoloModeLastTouch::OnTrackEvent(const TrackEvent &event) {
	if (!(event.type & TrackEvent::LastTouched)) return false;
	unsolo_all();
	return SetMediaTrackInfo_Value(event.track, "I_SOLO", 1 + get_solo_ip_settings());
}


void AutoSoloModeSelected::OnActivate() {
	for (int i = 0; i < CountTracks(nullptr); i++) SetMediaTrackInfo_Value(GetTrack(nullptr, i), "I_SOLO", 0);
	for (int i = 0; i < CountSelectedTracks2(nullptr, false); i++) SetMediaTrackInfo_Value(GetSelectedTrack2(nullptr, i, false), "I_SOLO", 1 + get_solo_ip_settings());
}

void AutoSoloModeSelected::OnDeactivate() {
	unsolo_all();
}

bool AutoSoloModeSelected::OnTrackEvent(const TrackEvent &event) {
	if (!(event.type & TrackEvent::SelectionChanged)) return false;
	return SetMediaTrackInfo_Value(event.track, "I_SOLO", event.state & 1);
}

void AutoSoloModeLastTouchSel::OnDeactivate() {
	unsolo_all();
}


void AutoSoloModeLastTouchSel::OnActivate() {
	MediaTrack* track = GetLastTouchedTrack();
	if (track && IsTrackSelected(track)) {
		unsolo_all();
		SetMediaTrackInfo_Value(track, "I_SOLO", 1 + get_solo_ip_settings());
	} else {
		unsolo_all();
	}
}

bool AutoSoloModeLastTouchSel::OnTrackEvent(const TrackEvent &event) {
	if (!event.track) return false;

	if (event.type == TrackEvent::LastTouched) {
		if (IsTrackSelected(event.track)) {
			unsolo_all();
			return SetMediaTrackInfo_Value(event.track, "I_SOLO", 1 + get_solo_ip_settings());
		}
		return false;
	}

	if (event.type == TrackEvent::SelectionChanged) {
		if (event.state && event.track == GetLastTouchedTrack()) {
			unsolo_all();
			return SetMediaTrackInfo_Value(event.track, "I_SOLO", 1 + get_solo_ip_settings());
		}


		if (!event.state && event.track == GetLastTouchedTrack()) {
			SetMediaTrackInfo_Value(event.track, "I_SOLO", 0);
			return true;
		}
	}

	return false;
}
