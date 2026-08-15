#define REAPERAPI_IMPLEMENT
#include "commands.h"

MediaTrack* (*BR_TrackAtMouseCursor)(int* contextOut, double* positionOut) = nullptr;
reaper_plugin_info_t* g_rec = nullptr;

bool onAction(KbdSectionInfo* sec, int command, int val, int val2, int relmode, HWND hwnd) {
	auto it = actions.find(command);
	if (it == actions.end()) return false;
	it->second();
	return true;
}

void load_sws() { // load BR_*
	BR_TrackAtMouseCursor =
		static_cast<MediaTrack * (*)(int*, double*)>(g_rec->GetFunc("BR_TrackAtMouseCursor"));

	if (!BR_TrackAtMouseCursor) {
		ShowConsoleMsg("BR_TrackAtMouseCursor not loaded yet, retrying...\n");
		return;
	}

	ShowConsoleMsg("SWS/BR functions resolved!\n");
	g_rec->Register("-timer", static_cast<void*>(&load_sws));  // disable timer
}

extern "C" REAPER_PLUGIN_DLL_EXPORT int ReaperPluginEntry(
	HINSTANCE hInst, reaper_plugin_info_t* rec) {
	if (!rec || rec->caller_version != REAPER_PLUGIN_VERSION) return 0;
	if (REAPERAPI_LoadAPI(rec->GetFunc)) return 0;

	g_rec = rec;

	// Сначала вся синхронная инициализация
	if (!init_actions(rec)) return 0;
	rec->Register("hookcommand2", static_cast<void*>(onAction));

	// Потом запускаем таймер для отложенного резолва
	rec->Register("timer", static_cast<void*>(&load_sws));

	ShowConsoleMsg("My C++ Extension loaded successfully!\n");
	return 1;
}