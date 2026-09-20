#include "api.h"
#include "utils.h"


bool midi_is_grid_triplet(MediaItem_Take* take) { // how in SWS
	double grid = MIDI_GetGrid(take, nullptr, nullptr);
	if (grid < 1e8) return false;
	double n = 1.0 / grid;

	while (n < 3.0) { n *= 2.0; }

	double r = fmod(n, 3.0);
	return r < 0.000001 || r > 2.99999;
}

bool midi_is_grid_dotted(MediaItem_Take* take) { // how in SWS
	double grid = MIDI_GetGrid(take, nullptr, nullptr);
	if (grid < 1e8) return false;
	double n = 1.0 / grid;

	while (n < (2.0 / 3.0)) { n *= 2.0; }
	while (n > (4.0 / 3.0)) { n *= 0.5; }

	double r = fmod(n, (2.0 / 3.0));
	return r < 0.000001 || r > 0.66666;
}

bool midi_is_grid_swing(MediaItem_Take* take) { // how in SWS
	return GetToggleCommandStateEx(SECTION_MIDI_EDITOR, 41006);
}

double midi_get_grid_swing(MediaItem_Take* take) {
	double swing;
	MIDI_GetGrid(take, &swing, nullptr);
	return swing;
}

int midi_get_grid_type(MediaItem_Take* take) {
	if (midi_is_grid_triplet(take)) {
		return 2;
	} else if (midi_is_grid_dotted(take)) {
		return 4;
	} else if (midi_is_grid_swing(take)) {
		return 8;
	} else {
		return 0;
	}
}


void delete_midi_notes(MediaItem_Take* take) {
	int notes_cnt, ccevt_cnt, tsevt_cnt;
	MIDI_CountEvts(take, &notes_cnt, &ccevt_cnt, &tsevt_cnt);
	for (int i = notes_cnt - 1;i >= 0;i--) {
		MIDI_DeleteNote(take, i);
	}
}