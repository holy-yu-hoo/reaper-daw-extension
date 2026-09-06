#include "api.h"
#include "fx_context.h"
#include "misc/utils.h"
#include "utils.h"

#define proj nullptr
using std::operator ""s;
using std::string;
using std::shared_ptr;
using std::weak_ptr;


static std::unordered_map<string, weak_ptr<IFXContext>> contexts;


shared_ptr<IFXContext> IFXContext::get_context(int tr_idx, int it_idx, int tk_idx, int param) {
	shared_ptr<IFXContext> context;
	string key;
	key.resize(64);
	MediaTrack* track = tr_idx == -1 ? GetMasterTrack(proj) : GetTrack(proj, tr_idx);
	if (!track) return nullptr;
	if (it_idx == -1) {
		GetSetMediaTrackInfo_String(track, "GUID", &key.front(), false);
		if (param) key += "_rec";
		key.resize(strlen(key.data()));

		if (contexts.find(key) == contexts.end()) {
			context = std::shared_ptr<TrackFXContext>(
				new TrackFXContext(track, param),
				[key](TrackFXContext* ctx) {
					contexts.erase(key);
					delete ctx;
				}
			);
			contexts[key] = context;
		} else {
			context = contexts[key].lock();
		}

	} else {
		MediaItem* item = GetTrackMediaItem(track, it_idx);
		if (!item) return nullptr;
		MediaItem_Take* take = GetTake(item, tk_idx);
		if (!take) return nullptr;
		GetSetMediaItemTakeInfo_String(take, "GUID", &key.front(), false);
		key.resize(strlen(key.data()));

		if (contexts.find(key) == contexts.end()) {
			context = std::shared_ptr<TakeFXContext>(
				new TakeFXContext(take),
				[key](TakeFXContext* ctx) {
					contexts.erase(key);
					delete ctx;
				}
			);
			contexts[key] = context;
		} else {
			context = contexts[key].lock();
		}
	}
	return context;
}

//<------------------------------TRACK CONTEXT------------------------------<//

inline TrackFXContext::TrackFXContext(int tr_idx, bool rec) {
	m_track = tr_idx == -1 ? GetMasterTrack(proj) : GetTrack(proj, tr_idx);
	m_rec = rec;
}

inline TrackFXContext::TrackFXContext(MediaTrack* track, bool rec): m_track(track), m_rec(rec) {}


inline bool TrackFXContext::get_fx_open(int fx_idx) {
	return TrackFX_GetOpen(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

inline void TrackFXContext::set_fx_open(int fx_idx, bool open) {
	TrackFX_SetEnabled(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, open);
}

inline bool TrackFXContext::get_fx_chain_open() {
	return (m_rec ? TrackFX_GetRecChainVisible(m_track) : TrackFX_GetChainVisible(m_track)) != -1;
}


inline int TrackFXContext::get_fx_chain_open(bool more_specifically) {
	return m_rec ? TrackFX_GetRecChainVisible(m_track) : TrackFX_GetChainVisible(m_track);
}


inline void TrackFXContext::set_fx_chain_open(bool open) {
	TrackFX_Show(m_track, get_selected_fx(m_track, m_rec), open);
}

inline void TrackFXContext::set_fx_chain_open(bool open, int fx_idx) {
	TrackFX_Show(m_track, fx_idx, open);
}

inline bool TrackFXContext::get_fx_enabled(int fx_idx) {
	return TrackFX_GetEnabled(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

inline void TrackFXContext::set_fx_enabled(int fx_idx, bool enabled) {
	TrackFX_SetEnabled(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, enabled);
}

inline bool TrackFXContext::get_fx_offline(int fx_idx) {
	return TrackFX_GetOffline(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

inline void TrackFXContext::set_fx_offline(int fx_idx, bool offline) {
	TrackFX_SetOffline(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, offline);
}

inline void TrackFXContext::delete_fx(int fx_idx) {
	TrackFX_Delete(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

inline bool TrackFXContext::reset_preset(int fx_idx) {
	if (!TrackFX_SetPresetByIndex(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, -1)) {
		return TrackFX_SetPresetByIndex(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, -2);
	}
	return true;
}

inline int TrackFXContext::get_count() {
	return m_rec ? TrackFX_GetRecCount(m_track) : TrackFX_GetCount(m_track);
}

inline string TrackFXContext::get_fx_guid(int fx_idx) {
	GUID* guid = TrackFX_GetFXGUID(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
	return guid_to_string(guid);
}

inline string TrackFXContext::get_guid() {
	string guid;
	guid.resize(64);
	GetSetMediaTrackInfo_String(m_track, "GUID", &guid.front(), false);
	return guid;
}

string TrackFXContext::get_fx_chunk(int fx_idx) {
	string chunk;
	bool ret = get_state_chunk(m_track, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return "";
	}

	string guid = get_fx_guid(m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);

	if (guid.empty()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
		#endif
		return "";
	}

	int i = chunk.find(guid); // guid position
	if (i == string::npos) return "";
	while (i >= 0 and chunk[i] != '>') i--;
	if (i < 0) return "";
	int e = --i;
	int c = 1;

	while (c > 0 and i > 0) {
		if (chunk[i] == '<') {
			c--;
		} else if (chunk[i] == '>') {
			c++;
		}
		i--;
	}

	if (c > 0) return "";
	i++;
	while (i < chunk.length() and chunk[i] != '\n') i++;
	if (i >= chunk.length()) return "";
	int s = ++i;

	return chunk.substr(s, e - s);
}

string TrackFXContext::get_fx_chain_chunk() {
	string chunk;
	bool ret = get_state_chunk(m_track, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return "";
	}


	int s = chunk.find(m_rec ? "<FXCHAIN_REC" : "<FXCHAIN", 0);
	s = chunk.find("<", s);
	if (s == string::npos) return "";
	int c = 2;
	int i = ++s;

	while (c > 0 and i < chunk.length()) {
		if (chunk[i] == '>') {
			c--;
		} else if (chunk[i] == '<') {
			c++;
		}
		i++;
	}

	if (c > 0) return "";
	int e = i - 2;
	if (e >= chunk.length()) return "";

	return chunk.substr(s, e - s);
}


string TrackFXContext::get_undo_str() {
	string undo_str;
	if (m_track == GetMasterTrack(nullptr)) {
		undo_str += "Master";
	} else {
		int track_num = GetMediaTrackInfo_Value(m_track, "IP_TRACKNUMBER");
		string track_name = get_set_media_track_info_string(m_track, "P_NAME", false);
		undo_str += "Track" + std::to_string(track_num) + " " + track_name;
	}

	return undo_str;
}

string TrackFXContext::get_config_param(int fx_idx, const string &param_name) {
	string param;
	if (m_rec) fx_idx = MAKE_FX_REC(fx_idx);
	param.clear();
	param.resize(128);
	bool ret = TrackFX_GetNamedConfigParm(m_track, fx_idx, param_name.data(), &param.front(), param.size());
	while (ret && (strlen(param.data()) >= (param.size() - 1))) {
		param.resize(param.size() * 2);
		ret = TrackFX_GetNamedConfigParm(m_track, fx_idx, param_name.data(), &param.front(), param.size());
	}
	param.resize(strlen(param.data()));
	return param;
}

inline string TrackFXContext::get_full_name(int fx_idx) {
	string name = get_config_param(fx_idx, "renamed_name");
	if (name.empty()) name = get_config_param(fx_idx, "fx_name");
	return name;
}

inline string TrackFXContext::get_name(int fx_idx) {
	string name = get_full_name(fx_idx);
	string type = get_config_param(fx_idx, "fx_type") + ": ";
	string vendor = " ("s + get_config_param(fx_idx, "fx_name") + ")"s;
	int s = 0, e = name.size() - 1;
	if (name.find(type) == 0) {
		s = type.size();
	}

	if (name.rfind(vendor) == name.size() - vendor.size()) {
		e = name.size() - vendor.size();
	}
	return name.substr(s, e - s);
}

inline void TrackFXContext::_undo_begin_block(ReaProject* project) {
	Undo_BeginBlock2(project);
}

inline void TrackFXContext::_undo_end_block(ReaProject* project, std::string undo_str) {
	Undo_EndBlock2(project, undo_str.data(), 2);
}

//>------------------------------TRACK CONTEXT------------------------------>//


//<------------------------------TAKE CONTEXT------------------------------<//

inline TakeFXContext::TakeFXContext(int tr_idx, int it_idx, int tk_idx) {
	MediaTrack* track = tr_idx == -1 ? GetMasterTrack(proj) : GetTrack(proj, tr_idx);
	if (!track) return;
	MediaItem* item = GetTrackMediaItem(track, it_idx);
	if (!item) return;
	MediaItem_Take* take = GetTake(item, tk_idx);
	if (!take) return;
	m_take = take;
}

inline TakeFXContext::TakeFXContext(MediaItem_Take* take): m_take(take) {}


inline bool TakeFXContext::get_fx_open(int fx_idx) {
	return TakeFX_GetOpen(m_take, fx_idx);
}

inline void TakeFXContext::set_fx_open(int fx_idx, bool open) {
	TakeFX_SetEnabled(m_take, fx_idx, open);
}

inline bool TakeFXContext::get_fx_chain_open() {
	return (TakeFX_GetChainVisible(m_take)) != -1;
}


inline int TakeFXContext::get_fx_chain_open(bool more_specifically) {
	return TakeFX_GetChainVisible(m_take);
}


inline void TakeFXContext::set_fx_chain_open(bool open) {
	TakeFX_Show(m_take, get_selected_fx(m_take), open);
}

inline void TakeFXContext::set_fx_chain_open(bool open, int fx_idx) {
	TakeFX_Show(m_take, fx_idx, open);
}

inline bool TakeFXContext::get_fx_enabled(int fx_idx) {
	return TakeFX_GetEnabled(m_take, fx_idx);
}

inline void TakeFXContext::set_fx_enabled(int fx_idx, bool enabled) {
	TakeFX_SetEnabled(m_take, fx_idx, enabled);
}

inline bool TakeFXContext::get_fx_offline(int fx_idx) {
	return TakeFX_GetOffline(m_take, fx_idx);
}

inline void TakeFXContext::set_fx_offline(int fx_idx, bool offline) {
	TakeFX_SetOffline(m_take, fx_idx, offline);
}

inline void TakeFXContext::delete_fx(int fx_idx) {
	TakeFX_Delete(m_take, fx_idx);
}

inline bool TakeFXContext::reset_preset(int fx_idx) {
	if (!TakeFX_SetPresetByIndex(m_take, fx_idx, -1)) {
		return TakeFX_SetPresetByIndex(m_take, fx_idx, -2);
	}
	return true;
}

inline int TakeFXContext::get_count() {
	return TakeFX_GetCount(m_take);
}

inline string TakeFXContext::get_fx_guid(int fx_idx) {
	GUID* guid = TakeFX_GetFXGUID(m_take, fx_idx);
	return guid_to_string(guid);
}

inline string TakeFXContext::get_guid() {
	string guid;
	guid.resize(64);
	GetSetMediaItemTakeInfo_String(m_take, "GUID", &guid.front(), false);
	return guid;
}

string TakeFXContext::get_fx_chunk(int fx_idx) {
	MediaItem* item = GetMediaItemTake_Item(m_take);
	string chunk;
	bool ret = get_state_chunk(item, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return "";
	}

	string guid = get_fx_guid(fx_idx);

	if (guid.empty()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
		#endif
		return "";
	}

	int i = chunk.find(guid); // guid position
	if (i == string::npos) return "";
	while (i >= 0 and chunk[i] != '>') i--;
	if (i < 0) return "";
	int e = --i;
	int c = 1;

	while (c > 0 and i > 0) {
		if (chunk[i] == '<') {
			c--;
		} else if (chunk[i] == '>') {
			c++;
		}
		i--;
	}

	if (c > 0) return "";
	i++;
	while (i < chunk.length() and chunk[i] != '\n') i++;
	if (i >= chunk.length()) return "";
	int s = ++i;

	return chunk.substr(s, e - s);
}

string TakeFXContext::get_fx_chain_chunk() {
	if (TakeFX_GetCount(m_take) <= 0) return "";
	MediaItem* item = GetMediaItemTake_Item(m_take);
	string chunk;
	bool ret = get_state_chunk(item, chunk);

	if (!ret) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
		#endif
		return "";
	}

	string guid = get_guid();

	int s = chunk.find(guid, 0);
	s = chunk.find("<TAKEFX", s);
	s = chunk.find("<", s);
	if (s == string::npos) return "";
	int c = 2;
	int i = ++s;

	while (c > 0 and i < chunk.length()) {
		if (chunk[i] == '>') {
			c--;
		} else if (chunk[i] == '<') {
			c++;
		}
		i++;
	}

	if (c > 0) return "";
	int e = i - 2;
	if (e >= chunk.length()) return "";

	return chunk.substr(s, e - s);
}


string TakeFXContext::get_undo_str() {
	string undo_str = "Item on track ";
	MediaTrack* track = GetMediaItem_Track(GetMediaItemTake_Item(m_take));
	int take_num = GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER");
	string take_name = get_set_media_track_info_string(track, "P_NAME", false);
	undo_str += std::to_string(take_num) + " " + take_name;
	return undo_str;
}

string TakeFXContext::get_config_param(int fx_idx, const string &param_name) {
	string param;
	param.clear();
	param.resize(128);
	bool ret = TakeFX_GetNamedConfigParm(m_take, fx_idx, param_name.data(), &param.front(), param.size());
	while (ret && (strlen(param.data()) >= (param.size() - 1))) {
		param.resize(param.size() * 2);
		ret = TakeFX_GetNamedConfigParm(m_take, fx_idx, param_name.data(), &param.front(), param.size());
	}
	param.resize(strlen(param.data()));
	return param;
}

inline string TakeFXContext::get_full_name(int fx_idx) {
	string name = get_config_param(fx_idx, "renamed_name");
	if (name.empty()) name = get_config_param(fx_idx, "fx_name");
	return name;
}

inline string TakeFXContext::get_name(int fx_idx) {
	string name = get_full_name(fx_idx);
	string type = get_config_param(fx_idx, "fx_type") + ": ";
	string vendor = " ("s + get_config_param(fx_idx, "fx_name") + ")"s;
	int s = 0, e = name.size() - 1;
	if (name.find(type) == 0) {
		s = type.size();
	}

	if (name.rfind(vendor) == name.size() - vendor.size()) {
		e = name.size() - vendor.size();
	}
	return name.substr(s, e - s);
}

inline void TakeFXContext::_undo_begin_block(ReaProject* project) {
	Undo_BeginBlock2(project);
}

inline void TakeFXContext::_undo_end_block(ReaProject* project, std::string undo_str) {
	Undo_EndBlock2(project, undo_str.data(), 4);
}

//>------------------------------TAKE CONTEXT------------------------------>//
