#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>
#include "yh_api.h"
#include "api_load.h"
#include "api.h"

MediaTrack * (*BR_TrackAtMouseCursor)(int *, double *) = nullptr;

void (*BR_GetMouseCursorContext)(char *, int, char *, int, char *, int) = nullptr;


static const struct {
	void **dest;
	const char *name;
} table[] = {
	{reinterpret_cast<void **>(&BR_TrackAtMouseCursor), "BR_TrackAtMouseCursor"},
	{reinterpret_cast<void **>(&BR_GetMouseCursorContext), "BR_GetMouseCursorContext"},
	{nullptr, nullptr}
};


void load_sws() {
	for (int i = 0; table[i].name != nullptr; ++i) {
		*table[i].dest = g_rec->GetFunc(table[i].name);
	}
	g_rec->Register("-timer", static_cast<void *>(&load_sws));

}

bool api_load() {
	if (REAPERAPI_LoadAPI(g_rec->GetFunc)) {
		return 0;
	}
	g_rec->Register("timer", static_cast<void *>(&load_sws));
	yh_api_load();
	return true;
}
