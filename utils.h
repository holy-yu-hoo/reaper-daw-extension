#pragma once
#include "api.h"
#include <vector>


constexpr int SECTION_MAIN = 0;
constexpr int SECTION_MAIN_ALT = 100;
constexpr int SECTION_MIDI_EDITOR = 32060;
constexpr int SECTION_MIDI_LIST = 32061;
constexpr int SECTION_MIDI_INLINE = 32062;
constexpr int SECTION_MEDIA_EXPLORER = 32063;


typedef struct COMMAND_T {
	const char *id;
	const char *name;
	const char *menu_text;

	void (*do_command)(COMMAND_T *);

	int unique_section_id;

	void (*on_action)(COMMAND_T *, int, int, int, HWND);

	int cmd_id;
} COMMAND_T;


bool register_commands(std::vector<COMMAND_T> &); // implement in main.cpp
bool unregister_commands(std::vector<COMMAND_T> &commands); // implement in main.cpp
