#include <string>
#include <core/api.h>

bool get_state_chunk(MediaTrack *track, std::string &chunk) {
	chunk.clear();
	chunk.resize(4096);
	bool ret = GetTrackStateChunk(track, chunk.data(), static_cast<int>(chunk.size()), false);
	while (ret && (strlen(chunk.data()) >= (chunk.size() - 1))) {
		chunk.resize(chunk.size() * 2);
		ret = GetTrackStateChunk(track, chunk.data(), static_cast<int>(chunk.size()), false);
	}
	chunk.resize(strlen(chunk.data()));
	return ret;
}

bool get_state_chunk(MediaItem *item, std::string &chunk) {
	chunk.clear();
	chunk.resize(4096);
	bool ret = GetItemStateChunk(item, chunk.data(), static_cast<int>(chunk.size()), false);
	while (ret && (strlen(chunk.data()) >= (chunk.size() - 1))) {
		chunk.resize(chunk.size() * 2);
		ret = GetItemStateChunk(item, chunk.data(), static_cast<int>(chunk.size()), false);
	}
	chunk.resize(strlen(chunk.data()));
	return ret;
}
