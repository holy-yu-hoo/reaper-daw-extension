#pragma once
#include "api.h"

bool reset_fx_preset(MediaTrack *track, int fx);

bool reset_fx_preset(MediaItem_Take *take, int fx);

std::pair<int, int> get_fx_chunk_borders(const std::string &chunk, const std::string &fx_guid);


std::string get_fx_chunk(MediaTrack *track, int fx);

std::string get_fx_chunk(MediaItem_Take *take, int fx);

bool set_fx_chunk(MediaTrack *track, int fx, const std::string &new_fx_chunk);

bool set_fx_chunk(MediaItem_Take *take, int fx, const std::string &new_fx_chunk);
