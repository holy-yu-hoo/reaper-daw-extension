#pragma once

#include "../sdk/reaper_plugin.h"

extern reaper_plugin_info_t* g_rec;

bool init_actions();
bool action_dispatch_hook(KbdSectionInfo* sec, int command, int val, int val2, int relmode, HWND hwnd);