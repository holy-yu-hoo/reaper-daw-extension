#include "api.h"
#include "fx_context.h"
#include "misc/utils.h"
#include "utils.h"

#define proj nullptr
using std::operator ""s;
using std::string;
using std::shared_ptr;
using std::weak_ptr;

namespace {
	struct FXNameParts {
		string prefix;
		string base;
		string suffix;
		bool has_prefix = false;
		bool has_suffix = false;
	};


	FXNameParts split_fx_name(const string& full_name, const string& fx_type, const string& fx_name) {
		FXNameParts parts;
		parts.base = full_name;

		if (!fx_type.empty()) {
			const string prefix = fx_type + ": ";
			if (parts.base.rfind(prefix, 0) == 0) {
				parts.prefix = prefix;
				parts.base = parts.base.substr(prefix.size());
				parts.has_prefix = true;
			}
		}

		const size_t vendor_start = fx_name.rfind(" (");
		const size_t vendor_end = fx_name.rfind(')');
		if (vendor_start != string::npos && vendor_end != string::npos && vendor_end > vendor_start) {
			parts.suffix = fx_name.substr(vendor_start, vendor_end - vendor_start + 1);
			if (parts.base.size() >= parts.suffix.size() &&
				parts.base.compare(parts.base.size() - parts.suffix.size(), parts.suffix.size(), parts.suffix) == 0) {
				parts.base.resize(parts.base.size() - parts.suffix.size());
				parts.has_suffix = true;
			}
		}

		return parts;
	}
}


static std::unordered_map<string, weak_ptr<IFXContext>> contexts;

#define fx_check(fx_idx) _fx_idx_valid(fx_idx)

shared_ptr<IFXContext> IFXContext::get_context(int tr_idx, int it_idx, int tk_idx, int param) {
	shared_ptr<IFXContext> context;
	string key;
	key.resize(64);
	MediaTrack* track = tr_idx == -1 ? GetMasterTrack(proj) : GetTrack(proj, tr_idx);
	if (!track) return nullptr;
	if (it_idx == -1) {
		return get_context(track, param);
	} else {
		MediaItem* item = GetTrackMediaItem(track, it_idx);
		if (!item) return nullptr;
		MediaItem_Take* take = GetTake(item, tk_idx);
		if (!take) return nullptr;
		return get_context(take);
	}
}

std::shared_ptr<IFXContext> IFXContext::get_context(MediaTrack* track, int param) {
	if (!track) return nullptr;
	shared_ptr<IFXContext> context;
	string key;
	key.resize(64);
	GetSetMediaTrackInfo_String(track, "GUID", &key.front(), false);
	key.resize(strlen(key.data()));
	if (param) key += "_rec";

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
	return context;
}

std::shared_ptr<IFXContext> IFXContext::get_context(MediaItem_Take* take) {
	if (!take) return nullptr;
	shared_ptr<IFXContext> context;
	string key;
	key.resize(64);
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
	return context;
}

//<------------------------------TRACK CONTEXT------------------------------<//

TrackFXContext::TrackFXContext() : m_rec(false) {}

inline TrackFXContext::TrackFXContext(int tr_idx, bool rec) {
	m_track = tr_idx == -1 ? GetMasterTrack(proj) : GetTrack(proj, tr_idx);
	m_rec = rec;
	m_guid = TrackFXContext::get_guid();
}

inline TrackFXContext::TrackFXContext(MediaTrack* track, bool rec) : m_track(track), m_rec(rec) {
	m_guid = TrackFXContext::get_guid();
}


inline bool TrackFXContext::get_fx_open(int fx_idx) {
	if (!fx_check(fx_idx)) return false;
	return TrackFX_GetOpen(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

inline void TrackFXContext::set_fx_open(int fx_idx, bool open) {
	if (!fx_check(fx_idx)) return;
	TrackFX_SetOpen(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, open);
}

inline bool TrackFXContext::get_fx_chain_open() {
	return (m_rec ? TrackFX_GetRecChainVisible(m_track) : TrackFX_GetChainVisible(m_track)) != -1;
}


inline int TrackFXContext::get_fx_chain_open(bool more_specifically) {
	return m_rec ? TrackFX_GetRecChainVisible(m_track) : TrackFX_GetChainVisible(m_track);
}


inline void TrackFXContext::set_fx_chain_open(bool open) {
	int fx_idx = get_selected_fx(m_track, m_rec);
	if (!fx_check(fx_idx)) return;
	TrackFX_Show(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, open);
}

inline void TrackFXContext::set_fx_chain_open(bool open, int fx_idx) {
	if (!fx_check(fx_idx)) return;
	TrackFX_Show(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, open);
}

inline bool TrackFXContext::get_fx_enabled(int fx_idx) {
	if (!fx_check(fx_idx)) return false;
	return TrackFX_GetEnabled(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

inline void TrackFXContext::set_fx_enabled(int fx_idx, bool enabled) {
	if (!fx_check(fx_idx)) return;
	TrackFX_SetEnabled(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, enabled);
}

inline bool TrackFXContext::get_fx_offline(int fx_idx) {
	if (!fx_check(fx_idx)) return false;
	return TrackFX_GetOffline(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

inline void TrackFXContext::set_fx_offline(int fx_idx, bool offline) {
	if (!fx_check(fx_idx)) return;
	TrackFX_SetOffline(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, offline);
}

inline void TrackFXContext::delete_fx(int fx_idx) {
	if (!fx_check(fx_idx)) return;
	TrackFX_Delete(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

inline bool TrackFXContext::reset_preset(int fx_idx) {
	if (!fx_check(fx_idx)) return false;
	if (!TrackFX_SetPresetByIndex(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, -1)) {
		return TrackFX_SetPresetByIndex(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, -2);
	}
	return true;
}

inline int TrackFXContext::get_count() {
	return m_rec ? TrackFX_GetRecCount(m_track) : TrackFX_GetCount(m_track);
}

inline string TrackFXContext::get_fx_guid(int fx_idx) {
	if (!fx_check(fx_idx)) return "";
	GUID* guid = TrackFX_GetFXGUID(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
	return guid_to_string(guid);
}

inline string TrackFXContext::get_guid() {
	string guid;
	guid.resize(64);
	GetSetMediaTrackInfo_String(m_track, "GUID", &guid.front(), false);
	guid.resize(strlen(guid.data()));
	return guid;
}

string TrackFXContext::get_fx_chunk(int fx_idx) {
	if (!fx_check(fx_idx)) return "";
	string chunk;
	bool ret = get_state_chunk(m_track, chunk);

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


bool TrackFXContext::set_fx_chunk(int fx_idx, string fx_chunk) {
	if (!fx_check(fx_idx)) return false;
	string chunk;
	bool ret = get_state_chunk(m_track, chunk);

	if (!ret) {
#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
#endif
		return false;
	}

	string guid = get_fx_guid(fx_idx);

	if (guid.empty()) {
#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
#endif
		return false;
	}

	int i = chunk.find(guid); // guid position
	if (i == string::npos) return false;
	while (i >= 0 and chunk[i] != '>') i--;
	if (i < 0) return false;
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

	if (c > 0) return false;
	i++;
	while (i < chunk.length() and chunk[i] != '\n') i++;
	if (i >= chunk.length()) return false;
	int s = ++i;
	string new_chunk = chunk.substr(0, s) + fx_chunk + chunk.substr(e);
	return SetTrackStateChunk(m_track, new_chunk.data(), false);
}

string TrackFXContext::get_undo_str() { // "Master/Track track_num track_name"
	string undo_str;
	if (m_track == GetMasterTrack(nullptr)) {
		undo_str += "Master";
	} else {
		int track_num = GetMediaTrackInfo_Value(m_track, "IP_TRACKNUMBER");
		string track_name = get_set_media_track_info_string(m_track, "P_NAME", false);
		undo_str += "Track " + std::to_string(track_num);
		if (!track_name.empty()) {
			undo_str += (" " + track_name);
		}
	}

	return undo_str;
}

string TrackFXContext::get_config_param(int fx_idx, const string& param_name) {
	if (!fx_check(fx_idx)) return "";
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

bool TrackFXContext::set_config_param(int fx_idx, const std::string& param_name, const std::string& param_value) {
	if (!fx_check(fx_idx)) return false;
	return TrackFX_SetNamedConfigParm(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, param_name.data(), param_value.data());
}

inline string TrackFXContext::get_full_name(int fx_idx) {
	if (!fx_check(fx_idx)) return "";
	string name = get_config_param(fx_idx, "renamed_name");
	if (name.empty()) name = get_config_param(fx_idx, "fx_name");
	return name;
}

inline string TrackFXContext::get_name(int fx_idx) {
	if (!fx_check(fx_idx)) return "";
	const string full_name = get_full_name(fx_idx);
	const string fx_type = get_config_param(fx_idx, "fx_type");
	const string fx_name = get_config_param(fx_idx, "fx_name");
	return split_fx_name(full_name, fx_type, fx_name).base;
}

bool TrackFXContext::set_full_name(int fx_idx, std::string name) {
	return set_config_param(fx_idx, "renamed_name", name);
}

bool TrackFXContext::set_name(int fx_idx, std::string name) {
	if (!fx_check(fx_idx)) return false;
	const string full_name = get_full_name(fx_idx);
	const string fx_type = get_config_param(fx_idx, "fx_type");
	const string fx_name = get_config_param(fx_idx, "fx_name");
	const FXNameParts parts = split_fx_name(full_name, fx_type, fx_name);
	string new_name;
	if (parts.has_prefix) new_name += parts.prefix;
	new_name += name;
	if (parts.has_suffix) new_name += parts.suffix;

	return set_config_param(fx_idx, "renamed_name", new_name);
}

inline void TrackFXContext::_undo_begin_block(ReaProject* project) {
	Undo_BeginBlock2(project);
}

inline void TrackFXContext::_undo_end_block(ReaProject* project, std::string undo_str) {
	Undo_EndBlock2(project, undo_str.data(), UNDO_STATE_FX | UNDO_STATE_TRACKCFG);
}

bool TrackFXContext::operator==(const IFXContext& other) {
	const TrackFXContext* p_other = dynamic_cast<const TrackFXContext*>(&other);
	if (!p_other) return false;
	return (m_track == p_other->m_track) and (m_rec == p_other->m_rec);
}

bool TrackFXContext::operator!=(const IFXContext& other) {
	const TrackFXContext* p_other = dynamic_cast<const TrackFXContext*>(&other);
	if (!p_other) return true;
	return (m_track != p_other->m_track) or (m_rec != p_other->m_rec);
}

bool TrackFXContext::operator!() {
	return !m_track;
}

TrackFXContext::operator bool() {
	return static_cast<bool>(m_track);
}

inline std::string TrackFXContext::get_key() {
	string key = get_guid();
	if (m_rec) key += "_rec";
	return key;
}

bool TrackFXContext::is_valid() {
	if (m_guid.empty()) return false;
	GUID* guid = GetTrackGUID(GetMasterTrack(proj));
	return m_guid == guid_to_string(guid) or BR_GetMediaTrackByGUID(proj, m_guid.data()) != nullptr;
}

int TrackFXContext::add_fx_by_name(std::string name) {
	return TrackFX_AddByName(m_track, name.data(), m_rec, -1);
}

void TrackFXContext::copy_fx(int fx_idx, IFXContext* dest, int dest_fx_idx) {
	if (TrackFXContext* ctx = dynamic_cast<TrackFXContext*>(dest)) {
		TrackFX_CopyToTrack(m_track, fx_idx, ctx->m_track, dest_fx_idx, false);
	} else if (TakeFXContext* ctx = dynamic_cast<TakeFXContext*>(dest)) {
		TrackFX_CopyToTake(m_track, fx_idx, ctx->m_take, dest_fx_idx, false);
	}
}

void TrackFXContext::move_fx(int fx_idx, IFXContext* dest, int dest_fx_idx) {
	if (TrackFXContext* ctx = dynamic_cast<TrackFXContext*>(dest)) {
		TrackFX_CopyToTrack(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, ctx->m_track, ctx->m_rec ? MAKE_FX_REC(dest_fx_idx) : dest_fx_idx, true);

	} else if (TakeFXContext* ctx = dynamic_cast<TakeFXContext*>(dest)) {
		TrackFX_CopyToTake(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, ctx->m_take, dest_fx_idx, true);
	}
}

inline bool TrackFXContext::_fx_idx_valid(int fx_idx) {
	return 0 <= fx_idx and fx_idx < get_count();
}

//>------------------------------TRACK CONTEXT------------------------------>//


//<------------------------------TAKE CONTEXT------------------------------<//

TakeFXContext::TakeFXContext() {}

inline TakeFXContext::TakeFXContext(int tr_idx, int it_idx, int tk_idx) {
	MediaTrack* track = tr_idx == -1 ? GetMasterTrack(proj) : GetTrack(proj, tr_idx);
	if (!track) return;
	MediaItem* item = GetTrackMediaItem(track, it_idx);
	if (!item) return;
	MediaItem_Take* take = GetTake(item, tk_idx);
	if (!take) return;
	m_take = take;

	m_guid = TakeFXContext::get_guid();
}

inline TakeFXContext::TakeFXContext(MediaItem_Take* take) : m_take(take) {
	m_guid = TakeFXContext::get_guid();
}


inline bool TakeFXContext::get_fx_open(int fx_idx) {
	if (!fx_check(fx_idx)) return false;
	return TakeFX_GetOpen(m_take, fx_idx);
}

inline void TakeFXContext::set_fx_open(int fx_idx, bool open) {
	if (!fx_check(fx_idx)) return;
	TakeFX_SetOpen(m_take, fx_idx, open);
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
	if (!fx_check(fx_idx)) return false;
	return TakeFX_GetEnabled(m_take, fx_idx);
}

inline void TakeFXContext::set_fx_enabled(int fx_idx, bool enabled) {
	if (!fx_check(fx_idx)) return;
	TakeFX_SetEnabled(m_take, fx_idx, enabled);
}

inline bool TakeFXContext::get_fx_offline(int fx_idx) {
	if (!fx_check(fx_idx)) return false;
	return TakeFX_GetOffline(m_take, fx_idx);
}

inline void TakeFXContext::set_fx_offline(int fx_idx, bool offline) {
	if (!fx_check(fx_idx)) return;
	TakeFX_SetOffline(m_take, fx_idx, offline);
}

inline void TakeFXContext::delete_fx(int fx_idx) {
	if (!fx_check(fx_idx)) return;
	TakeFX_Delete(m_take, fx_idx);
}

inline bool TakeFXContext::reset_preset(int fx_idx) {
	if (!fx_check(fx_idx)) return false;
	if (!TakeFX_SetPresetByIndex(m_take, fx_idx, -1)) {
		return TakeFX_SetPresetByIndex(m_take, fx_idx, -2);
	}
	return true;
}

inline int TakeFXContext::get_count() {
	return TakeFX_GetCount(m_take);
}

inline string TakeFXContext::get_fx_guid(int fx_idx) {
	if (!fx_check(fx_idx)) return "";
	GUID* guid = TakeFX_GetFXGUID(m_take, fx_idx);
	return guid_to_string(guid);
}

inline string TakeFXContext::get_guid() {
	string guid;
	guid.resize(64);
	GetSetMediaItemTakeInfo_String(m_take, "GUID", &guid.front(), false);
	guid.resize(strlen(guid.data()));
	return guid;
}

string TakeFXContext::get_fx_chunk(int fx_idx) {
	if (!fx_check(fx_idx)) return "";
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

bool TakeFXContext::set_fx_chunk(int fx_idx, std::string fx_chunk) {
	if (!fx_check(fx_idx)) return false;
	MediaItem* item = GetMediaItemTake_Item(m_take);
	string chunk;
	bool ret = get_state_chunk(item, chunk);

	if (!ret) {
#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get chunk\n");
#endif
		return false;
	}

	string guid = get_fx_guid(fx_idx);

	if (guid.empty()) {
#if defined(_DEBUG)
		ShowConsoleMsg("Failed to get fx guid\n");
#endif
		return false;
	}

	int i = chunk.find(guid); // guid position
	if (i == string::npos) return false;
	while (i >= 0 and chunk[i] != '>') i--;
	if (i < 0) return false;
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

	if (c > 0) return false;
	i++;
	while (i < chunk.length() and chunk[i] != '\n') i++;
	if (i >= chunk.length()) return false;
	int s = ++i;

	string new_chunk = chunk.substr(0, s) + fx_chunk + chunk.substr(e);
	return SetItemStateChunk(item, new_chunk.data(), false);
}

string TakeFXContext::get_undo_str() {
	string undo_str = "Item on track ";
	MediaTrack* track = GetMediaItem_Track(GetMediaItemTake_Item(m_take));
	int take_num = GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER");
	string take_name = get_set_media_track_info_string(track, "P_NAME", false);
	undo_str += std::to_string(take_num) + " " + take_name;
	return undo_str;
}

string TakeFXContext::get_config_param(int fx_idx, const string& param_name) {
	if (!fx_check(fx_idx)) return "";
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

bool TakeFXContext::set_config_param(int fx_idx, const std::string& param_name, const std::string& param_value) {
	if (!fx_check(fx_idx)) return false;
	return TakeFX_SetNamedConfigParm(m_take, fx_idx, param_name.data(), param_value.data());
}

inline string TakeFXContext::get_full_name(int fx_idx) {
	if (!fx_check(fx_idx)) return "";
	string name = get_config_param(fx_idx, "renamed_name");
	if (name.empty()) name = get_config_param(fx_idx, "fx_name");
	return name;
}

inline string TakeFXContext::get_name(int fx_idx) {
	if (!fx_check(fx_idx)) return "";
	const string full_name = get_full_name(fx_idx);
	const string fx_type = get_config_param(fx_idx, "fx_type");
	const string fx_name = get_config_param(fx_idx, "fx_name");
	return split_fx_name(full_name, fx_type, fx_name).base;
}

bool TakeFXContext::set_full_name(int fx_idx, std::string name) {
	return set_config_param(fx_idx, "renamed_name", name);
}

bool TakeFXContext::set_name(int fx_idx, std::string name) {
	if (!fx_check(fx_idx)) return false;
	const string full_name = get_full_name(fx_idx);
	const string fx_type = get_config_param(fx_idx, "fx_type");
	const string fx_name = get_config_param(fx_idx, "fx_name");
	const FXNameParts parts = split_fx_name(full_name, fx_type, fx_name);
	string new_name;
	if (parts.has_prefix) new_name += parts.prefix;
	new_name += name;
	if (parts.has_suffix) new_name += parts.suffix;

	return set_config_param(fx_idx, "renamed_name", new_name);
}

inline void TakeFXContext::_undo_begin_block(ReaProject* project) {
	Undo_BeginBlock2(project);
}

inline void TakeFXContext::_undo_end_block(ReaProject* project, std::string undo_str) {
	Undo_EndBlock2(project, undo_str.data(), UNDO_STATE_ITEMS);
}

bool TakeFXContext::operator==(const IFXContext& other) {
	const TakeFXContext* p_other = dynamic_cast<const TakeFXContext*>(&other);
	if (!p_other) return false;
	return (m_take == p_other->m_take);
}

bool TakeFXContext::operator!=(const IFXContext& other) {
	const TakeFXContext* p_other = dynamic_cast<const TakeFXContext*>(&other);
	if (!p_other) return true;
	return (m_take != p_other->m_take);
}

bool TakeFXContext::operator!() {
	return !m_take;
}

TakeFXContext::operator bool() {
	return static_cast<bool>(m_take);
}

inline std::string TakeFXContext::get_key() {
	return get_guid();
}

bool TakeFXContext::is_valid() {
	MediaItem_Take* take = get_media_item_take_by_guid(proj, m_guid);
	return take != nullptr;
}

int TakeFXContext::add_fx_by_name(std::string name) {
	return TakeFX_AddByName(m_take, name.data(), -1);
}

void TakeFXContext::copy_fx(int fx_idx, IFXContext* dest, int dest_fx_idx) {
	if (TrackFXContext* ctx = dynamic_cast<TrackFXContext*>(dest)) {
		TakeFX_CopyToTrack(m_take, fx_idx, ctx->m_track, dest_fx_idx, false);

	} else if (TakeFXContext* ctx = dynamic_cast<TakeFXContext*>(dest)) {
		TakeFX_CopyToTake(m_take, fx_idx, ctx->m_take, dest_fx_idx, false);
	}
}

void TakeFXContext::move_fx(int fx_idx, IFXContext* dest, int dest_fx_idx) {
	if (TrackFXContext* ctx = dynamic_cast<TrackFXContext*>(dest)) {
		TakeFX_CopyToTrack(m_take, fx_idx, ctx->m_track, ctx->m_rec ? MAKE_FX_REC(dest_fx_idx) : dest_fx_idx, true);

	} else if (TakeFXContext* ctx = dynamic_cast<TakeFXContext*>(dest)) {
		TakeFX_CopyToTake(m_take, fx_idx, ctx->m_take, dest_fx_idx, true);
	}
}

inline bool TakeFXContext::_fx_idx_valid(int fx_idx) {
	return 0 <= fx_idx and fx_idx < get_count();
}

//>------------------------------TAKE CONTEXT------------------------------>//
