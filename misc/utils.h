#pragma once
#include "api.h"


void string_to_guid(const std::string &p_guid, GUID* guid);

std::string guid_to_string(const GUID* guid);

bool get_state_chunk(MediaTrack* track, std::string &chunk);

bool get_state_chunk(MediaItem* item, std::string &chunk);

bool has_single_selected_track(MediaTrack* &out_track);

bool is_track_under_mouse(MediaTrack* track, int* context_out = nullptr);

double bars_to_seconds(int bars, double bpm, double beats_per_bar);

std::string get_set_media_track_info_string(MediaTrack* tr, const char* parmname, bool setNewValue);

MediaItem_Take* get_media_item_take_by_guid(ReaProject* proj, std::string &p_guid);


bool is_grid_triplet();

bool is_grid_dotted();

bool is_grid_swing();

double get_grid_swing();

int get_grid_type();
