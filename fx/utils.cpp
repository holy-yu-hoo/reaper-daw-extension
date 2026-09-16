#include  "api.h"
#include "utils.h"
#include "misc/utils.h"

static int parse_fx_index(const std::string& value) {
	if (value.empty()) return -1;
	char* end = nullptr;
	const long parsed = std::strtol(value.c_str(), &end, 10);
	if (end == value.c_str() || *end != '\0') return -1;
	if (parsed < -1) return -1;
	return static_cast<int>(parsed);
}

int get_selected_fx(MediaTrack* track, bool rec) {
	std::string chunk;
	if (!track || !get_state_chunk(track, chunk)) return -1;
	int s = rec ? chunk.find("<FXCHAIN_REC") : chunk.find("<FXCHAIN");
	if (s == std::string::npos) return -1;
	s = chunk.find("LASTSEL ", s);
	if (s == std::string::npos) return -1;
	s += 8;
	int e = chunk.find('\n', s);
	if (e == std::string::npos) return -1;
	std::string str = chunk.substr(s, e - s);
	return parse_fx_index(str);
}


int get_selected_fx(MediaItem_Take* take) {
	std::string chunk;
	if (!take) return -1;
	MediaItem* item = GetMediaItemTake_Item(take);
	if (!item || !get_state_chunk(item, chunk)) return -1;
	int s = chunk.find("<TAKEFX");
	if (s == std::string::npos) return -1;
	s = chunk.find("LASTSEL ", s);
	if (s == std::string::npos) return -1;
	s += 8;
	int e = chunk.find('\n', s);
	if (e == std::string::npos) return -1;
	std::string str = chunk.substr(s, e - s);
	return parse_fx_index(str);
}

int autofloat_created_fx_windows_val;
void prevent_autofloat_created_fx_windows_save() {
	int size = 0;
	int* addr = static_cast<int*>(get_config_var("fxfloat_focus", &size));
	autofloat_created_fx_windows_val = *addr & 4;
	*addr &= (~4);
}

void prevent_autofloat_created_fx_windows_restore() {
	int size = 0;
	int* addr = static_cast<int*>(get_config_var("fxfloat_focus", &size));
	*addr |= autofloat_created_fx_windows_val;
}
