#ifndef _API_H_
#define _API_H_
#include <vector>
#include <string>
#include <unordered_map>
#include <iso646.h>

#include <reaper_plugin_functions.h>
#ifdef REAPERAPI_IMPLEMENT
#define REAPER_EXTRA_API_DECL
#else
#define REAPER_EXTRA_API_DECL extern
#endif

REAPER_EXTRA_API_DECL MediaTrack* (*BR_TrackAtMouseCursor)(int* contextOut, double* positionOut);

REAPER_EXTRA_API_DECL void (*BR_GetMouseCursorContext)(char* windowOut, int windowOut_sz, char* segmentOut, int segmentOut_sz, char* detailsOut, int detailsOut_sz);

REAPER_EXTRA_API_DECL void (*BR_GetMediaTrackGUID)(MediaTrack* track, char* guidStringOut, int guidStringOut_sz);

REAPER_EXTRA_API_DECL void (*BR_GetMediaItemGUID)(MediaItem* item, char* guidStringOut, int guidStringOut_sz);

REAPER_EXTRA_API_DECL void (*BR_GetMediaItemTakeGUID)(MediaItem_Take* take, char* guidStringOut, int guidStringOut_sz);

REAPER_EXTRA_API_DECL HWND(*CF_GetTrackFXChainEx)(ReaProject* project, MediaTrack* track, bool wantInputChain);

REAPER_EXTRA_API_DECL HWND(*CF_GetTakeFXChain)(MediaItem_Take* take);

REAPER_EXTRA_API_DECL HWND(*CF_GetFocusedFXChain)();


constexpr int SECTION_MAIN = 0;
constexpr int SECTION_MAIN_ALT = 100;
constexpr int SECTION_MIDI_EDITOR = 32060;
constexpr int SECTION_MIDI_LIST = 32061;
constexpr int SECTION_MIDI_INLINE = 32062;
constexpr int SECTION_MEDIA_EXPLORER = 32063;


typedef struct COMMAND_T {
	const char* id;
	const char* name;
	const char* menu_text;
	void (*do_command)(COMMAND_T*);
	int unique_section_id;
	void (*on_action)(COMMAND_T*, int, int, int, HWND);
	int cmd_id;
	INT_PTR user;
} COMMAND_T;


bool register_commands(std::vector<COMMAND_T>&); // implement in main.cpp
bool unregister_commands(std::vector<COMMAND_T>& commands); // implement in main.cpp

#endif
