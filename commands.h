#pragma once
#include <map>
#include <reaper_plugin_functions.h>
#include "properties.h"

extern MediaTrack* (*BR_TrackAtMouseCursor)(int* contextOut, double* positionOut);

struct Command {
	custom_action_register_t action;

	void (*func)();
};


bool init_actions(const reaper_plugin_info_t* rec);

void Hello();

void insert_4_bars_midi_item_at_cursor();

extern std::map<int, void (*)()> actions;

static Command commands[] = {
	{{0, ID_PREFIX "Hello", NAME_PREFIX "YH Hello", nullptr}, Hello},
	{{0,ID_PREFIX "INSERT_4_BARS_MIDI_ITEM_AT_CURSOR", NAME_PREFIX "insert 4 bars midi item at cursor", nullptr}, insert_4_bars_midi_item_at_cursor},
};

static int actions_count = static_cast<int>(std::size(commands));
