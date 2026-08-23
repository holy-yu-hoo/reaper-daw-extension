#pragma once
#include "api.h"

std::string guid_to_string(const GUID *guid);

bool get_state_chunk(MediaTrack *track, std::string &chunk);

bool get_state_chunk(MediaItem *item, std::string &chunk);

bool has_single_selected_track(MediaTrack *&out_track);

bool is_track_under_mouse(MediaTrack *track, int *context_out = nullptr);

double bars_to_seconds(int bars, double bpm, double beats_per_bar);
