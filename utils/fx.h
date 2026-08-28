#pragma once
#include "api.h"

#define rec_fx 0x1000000
#define cont_fx 0x2000000

bool GetFxByGUID(GUID *guid, int *track_id_out, int *item_id_out, int *take_id_out, int *fx_id_out);

bool GetFxChainByHWND(HWND hwnd, int *track_id_out, int *item_id_out, int *take_id_out, int *is_rec);

bool reset_fx_preset(MediaTrack *track, int fx);

bool reset_fx_preset(MediaItem_Take *take, int fx);

bool reset_fx_chain_preset(MediaTrack *track, int fx);

bool reset_fx_chain_preset(MediaItem_Take *take, int fx);


std::string get_fx_chunk(MediaTrack *track, int fx);

std::string get_fx_chunk(MediaItem_Take *take, int fx);

bool set_fx_chunk(MediaTrack *track, int fx, const std::string &new_fx_chunk);

bool set_fx_chunk(MediaItem_Take *take, int fx, const std::string &new_fx_chunk);


std::string get_fx_chain_chunk(MediaTrack *track, int fx);

std::string get_fx_chain_chunk(MediaItem_Take *take, int fx);

bool set_fx_chain_chunk(MediaTrack *track, int fx, const std::string &new_fx_chunk);

bool set_fx_chain_chunk(MediaItem_Take *take, int fx, const std::string &new_fx_chunk);
