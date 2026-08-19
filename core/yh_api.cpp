#include "yh_api.h"
#include "api.h"
#include "../config.h"
#include <cstdio>


struct FXSpec {
	bool exist = false;
	int track_id, item_id, take_id, fx_id, param_id;
};


static FXSpec last_fx{};
FXSpec *const g_last_focused_fx = &last_fx;

void last_focused_fx_observer() {
	int track_id, item_id, take_id, fx_id, param_id;
	bool ret = GetTouchedOrFocusedFX(1, &track_id, &item_id, &take_id, &fx_id, &param_id);

	if (ret) {
		g_last_focused_fx->exist = true;
		g_last_focused_fx->track_id = track_id;
		g_last_focused_fx->item_id = item_id;
		g_last_focused_fx->take_id = take_id;
		g_last_focused_fx->fx_id = fx_id;
		g_last_focused_fx->param_id = param_id;
	}
}

bool GetLastFocusedFX(int *out_track_id, int *out_item_id, int *out_take_id, int *out_fx_id, int *out_param_id) {
	if (g_last_focused_fx->exist) {
		*out_track_id = g_last_focused_fx->track_id;
		*out_item_id = g_last_focused_fx->item_id;
		*out_take_id = g_last_focused_fx->take_id;
		*out_fx_id = g_last_focused_fx->fx_id;
		*out_param_id = g_last_focused_fx->param_id;
	} else {
		*out_track_id = -1;
		*out_item_id = -1;
		*out_take_id = -1;
		*out_fx_id = -1;
		*out_param_id = -1;
	}
	return g_last_focused_fx->exist;
}

void *varargGetLastFocusedFX(void **args, int numparms) {
	int a0, a1, a2, a3, a4;
	const bool ret = GetLastFocusedFX(&a0, &a1, &a2, &a3, &a4);

	if (numparms > 0 && args[0]) *static_cast<int *>(args[0]) = a0;
	if (numparms > 1 && args[1]) *static_cast<int *>(args[1]) = a1;
	if (numparms > 2 && args[2]) *static_cast<int *>(args[2]) = a2;
	if (numparms > 3 && args[3]) *static_cast<int *>(args[3]) = a3;
	if (numparms > 4 && args[4]) *static_cast<int *>(args[4]) = a4;

	// ЗНАЧЕНИЕ, приведённое к void*, а не указатель!
	return reinterpret_cast<void *>(static_cast<intptr_t>(ret));
}

char Def_varargGetLastFocusedFX[] =
"bool\r"
"int*,int*,int*,int*,int*\r"
"track_idOut,item_idOut,take_idOut,fx_idOut,param_idOut\r"
"Returns info about last focused FX. Returns false if no FX was focused.";


bool yh_api_load() {
	g_rec->Register("timer", static_cast<void *>(last_focused_fx_observer));
	g_rec->Register(
		"API_" API_PREFIX "GetLastFocusedFX",
		static_cast<void *>(GetLastFocusedFX)
	);
	g_rec->Register(
		"APIvararg_" API_PREFIX "GetLastFocusedFX",
		reinterpret_cast<void *>(varargGetLastFocusedFX)
	);

	char *p = Def_varargGetLastFocusedFX;
	while (*p) {
		if (*p == '\r') *p = '\0';
		p++;
	}

	g_rec->Register("APIdef_" API_PREFIX "GetLastFocusedFX", Def_varargGetLastFocusedFX);

	return true;
}
