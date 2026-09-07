#include  "api.h"
#include "utils.h"
#include "misc/utils.h"

int get_selected_fx(MediaTrack* track, bool rec) {
	std::string chunk;
	get_state_chunk(track, chunk);
	int s = rec ? chunk.find("<FXCHAIN_REC") : chunk.find("<FXCHAIN");
	if (s == std::string::npos) return -1;
	s = chunk.find("LASTSEL ", s) + 8;
	int e = chunk.find('\n', s);
	std::string str = chunk.substr(s, e - s);
	return std::stoi(str);
}


int get_selected_fx(MediaItem_Take* take) {
	std::string chunk;
	int n = GetMediaItemTakeInfo_Value(take, "IP_TAKENUMBER");
	MediaItem*item=GetMediaItemTake_Item(take);
	get_state_chunk(item, chunk);
	int s = chunk.find("<TAKEFX");
	if (s == std::string::npos) return -1;
	s = chunk.find("LASTSEL ", s) + 8;
	int e = chunk.find('\n', s);
	std::string str = chunk.substr(s, e - s);
	return std::stoi(str);
}
