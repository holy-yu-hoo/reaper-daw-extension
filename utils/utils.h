#pragma once
#include <reaper_plugin.h>
#include <string>

bool has_single_selected_track(MediaTrack *&out_track);

bool is_track_under_mouse(MediaTrack *track, int *context_out = nullptr);

double bars_to_seconds(int bars, double bpm, double beats_per_bar);

std::string get_track_fx_chunk(MediaTrack *track, int fx);

std::string set_track_fx_chunk(MediaTrack *track, int fx, const std::string &new_fx_chunk);
