#pragma once
#include "fx.h"
#include "misc.h"

typedef void (*ActionFunc)(COMMAND_T* cmd);

void insert_4_bars_midi_item_at_cursor(COMMAND_T*);
