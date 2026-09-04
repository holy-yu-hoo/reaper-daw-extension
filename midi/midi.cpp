#include "api.h"
#include "midi.h"


void fill_each_n_steps(MediaItem_Take* take, float step, double len, bool sel = false, bool mute = false, int chan = 0, int pitch = 60, int vel = 127) {
	MediaItem* item = GetMediaItemTake_Item(take);
	double take_start = GetMediaItemInfo_Value(item, "D_POSITION");
	double take_end = MIDI_GetPPQPosFromProjTime(take, take_start + GetMediaItemInfo_Value(item, "D_LENGTH"));
	double pos = MIDI_GetPPQPosFromProjTime(take, take_start);
	double step_len = len * step;
	while (pos + len < take_end) {
		MIDI_InsertNote(take, sel, mute, pos, pos + len, chan, pitch, vel, nullptr);
		pos += step_len;
	};
}

void fill_each_n_steps(COMMAND_T* cmd) {
	double note_len = 480;
	MediaItem_Take* take = MIDIEditor_GetTake(MIDIEditor_GetActive());
	fill_each_n_steps(take, cmd->user, note_len);
}
