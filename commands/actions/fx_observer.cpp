#include "api.h"
#include "fx_observer.h"


static struct {
	int exist = false, track_id, item_id, take_id, fx_id, param_id;
} g_last_focused_fx;


void last_focused_fx_observer() {
	int track_id, item_id, take_id, fx_id, param_id;
	bool ret = GetTouchedOrFocusedFX(1, &track_id, &item_id, &take_id, &fx_id, &param_id);

	if (ret) {
		g_last_focused_fx.exist = true;
		g_last_focused_fx.track_id = track_id;
		g_last_focused_fx.item_id = item_id;
		g_last_focused_fx.take_id = take_id;
		g_last_focused_fx.fx_id = fx_id;
		g_last_focused_fx.param_id = param_id;
	}
}

bool GetLastFocusedFX(int *out_track_id, int *out_item_id, int *out_take_id, int *out_fx_id, int *out_param_id) {
	if (g_last_focused_fx.exist) {
		*out_track_id = g_last_focused_fx.track_id;
		*out_item_id = g_last_focused_fx.item_id;
		*out_take_id = g_last_focused_fx.take_id;
		*out_fx_id = g_last_focused_fx.fx_id;
		*out_param_id = g_last_focused_fx.param_id;
	} else {
		*out_track_id = -1;
		*out_item_id = -1;
		*out_take_id = -1;
		*out_fx_id = -1;
		*out_param_id = -1;
	}
	return g_last_focused_fx.exist;
}
