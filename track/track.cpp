#include "api.h"
#include "track.h"

void select_track_cycle(COMMAND_T* cmd) {
	PreventUIRefresh(1);
	MediaTrack* last_track = GetLastTouchedTrack();
	if (!last_track) return;
	if (last_track == GetMasterTrack(nullptr)) return;
	int n = GetMediaTrackInfo_Value(last_track, "IP_TRACKNUMBER") - 1 + (cmd->user);
	int track_cnt = CountTracks(nullptr);
	if (n < 0) { n = track_cnt + n; } else { n = n % track_cnt; }
	MediaTrack* track = GetTrack(nullptr, n);
	SetOnlyTrackSelected(track);
	Main_OnCommandEx(40913, 0, nullptr);
	PreventUIRefresh(-1);
}

void select_track_cycle_keep_selection(COMMAND_T* cmd) {
	PreventUIRefresh(1);
	std::vector<MediaTrack*>tracks;
	for (int i = 0;i < CountSelectedTracks2(nullptr, false);i++) { tracks.push_back(GetSelectedTrack2(nullptr, i, false)); }
	MediaTrack* last_track = GetLastTouchedTrack();
	if (!last_track) return;
	if (last_track == GetMasterTrack(nullptr)) return;
	int n = GetMediaTrackInfo_Value(last_track, "IP_TRACKNUMBER") - 1 + (cmd->user);
	int track_cnt = CountTracks(nullptr);
	if (n < 0) { n = track_cnt + n; } else { n = n % track_cnt; }
	MediaTrack* track = GetTrack(nullptr, n);
	SetOnlyTrackSelected(track);
	for (MediaTrack* tr : tracks) SetTrackSelected(tr, true);
	Main_OnCommandEx(40913, 0, nullptr);
	PreventUIRefresh(-1);
}

void select_tracks_cycle(COMMAND_T* cmd) {
	PreventUIRefresh(1);
	MediaTrack* last_track = GetLastTouchedTrack(), * track;
	if (!last_track) return;
	std::vector<MediaTrack*>tracks;
	int track_cnt = CountTracks(nullptr);
	for (int i = 0;i < CountSelectedTracks2(nullptr, false);i++) {
		track = GetSelectedTrack2(nullptr, i, false);
		int n = GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER") - 1 + (cmd->user);
		if (n < 0) { n = track_cnt + n; } else { n = n % track_cnt; }
		tracks.push_back(GetTrack(nullptr, n));
	}
	if (last_track != GetMasterTrack(nullptr)) {
		int n = GetMediaTrackInfo_Value(last_track, "IP_TRACKNUMBER") - 1 + (cmd->user);
		if (n < 0) { n = track_cnt + n; } else { n = n % track_cnt; }
		track = GetTrack(nullptr, n);
		SetOnlyTrackSelected(track);
	} else {
		for (int i = CountSelectedTracks2(nullptr, false) - 1;i >= 0;i--) SetTrackSelected(GetSelectedTrack2(nullptr, i, false), false);
	}
	for (MediaTrack* tr : tracks) SetTrackSelected(tr, true);
	Main_OnCommandEx(40913, 0, nullptr);
	PreventUIRefresh(-1);
}


int TrackOSC::mode = 0; // 0 - off; 2 - last touched; 4 - seleceted; 6 - last touched while selected

void TrackOSC::SetSurfaceSelected(MediaTrack* trackid, bool selected) {
	selected_handler(trackid, selected);
}

int TrackOSC::Extended(int call, void* parm1, void* parm2, void* parm3) {
	switch (call) {
	case (CSURF_EXT_SETLASTTOUCHEDTRACK): {
		MediaTrack* track = (MediaTrack*)parm1;
		return last_touched_handler(track);
		break;
	}
	}
	return 0;
}

TrackOSC g_track_osc{};