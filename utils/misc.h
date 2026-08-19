#include <reaper_plugin.h>
#include <string>

std::string guid_to_string(GUID *guid);

bool get_state_chunk(MediaTrack *track, std::string &chunk);

bool get_state_chunk(MediaItem *item, std::string &chunk);
