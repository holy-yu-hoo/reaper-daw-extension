#pragma once
#include "fx_context.h"

void toggle_show_last_focused_(COMMAND_T* cmd);

void toggle_bypass_last_focused_(COMMAND_T* cmd);

void toggle_offline_last_focused_(COMMAND_T* cmd);

void delete_last_focused_(COMMAND_T* cmd);

void reset_last_focused_(COMMAND_T* cmd);

void ab_comparer(COMMAND_T* cmd);

void fx_renamer(IFXContext* ctx);
