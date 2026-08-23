#pragma once
#include "utils.h"
#include "fx_observer.h"

typedef void (*ActionFunc)(COMMAND_T *cmd);

void insert_4_bars_midi_item_at_cursor(COMMAND_T *);

void fx_ab_comparer(COMMAND_T *cmd);
