#pragma once
bool midi_is_grid_triplet(MediaItem_Take* take);

bool midi_is_grid_dotted(MediaItem_Take* take);

bool midi_is_grid_swing(MediaItem_Take* take);

double midi_get_grid_swing(MediaItem_Take* take);

int midi_get_grid_type(MediaItem_Take* take);

void delete_midi_notes(MediaItem_Take* take);
