#pragma once
#include <reaper_plugin.h>
extern reaper_plugin_info_t *g_rec;


struct FXSpec;

extern FXSpec *const g_last_focused_fx;


bool GetLastFocusedFX(int *out_track_id, int *out_item_id, int *out_take_id, int *out_fx_id, int *out_param_id);

bool yh_api_load();
