#include "api.h"
#include "fx.h"
#include "../utils/misc.h"
#include "../utils/fx.h"

bool only_if_open = true; // Applies the action only if the plugin window is open
bool hide_fx_if_chain_not_open = true; // Hides the plugin window instead of the effect chain window if the effect chain window is hidden and fx is visible


void toggle_show_last_focused_fx(COMMAND_T *cmd) {
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	if (fx->is_valid()) {
		MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id);
		if (!fx->is_take_fx()) { // track fx
			TrackFX_SetOpen(track, fx->fx_id, !TrackFX_GetOpen(track, fx->fx_id));
		} else {
			MediaItem_Take *take = GetMediaItemTake(GetTrackMediaItem(track, fx->item_id), fx->take_id);
			TakeFX_SetOpen(take, fx->fx_id, !TakeFX_GetOpen(take, fx->fx_id));
		}
	}
}

void toggle_show_last_focused_fx_chain(COMMAND_T *cmd) {
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	if (fx->is_valid()) {
		MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id);
		if (!fx->is_take_fx()) { // track fx
			if (fx->is_chain_visible()) {
				TrackFX_Show(track, fx->fx_id, 0);
			} else {
				TrackFX_Show(track, fx->fx_id, 1);
			}
		} else {
			MediaItem_Take *take = GetMediaItemTake(GetTrackMediaItem(track, fx->item_id), fx->take_id);
			if (fx->is_chain_visible()) {
				TakeFX_Show(take, fx->fx_id, 0);
			} else {
				TakeFX_Show(take, fx->fx_id, 1);
			}
		}
	}
}

void toggle_bypass_last_focused_fx(COMMAND_T *cmd) {
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	if (fx->is_valid()) {
		MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id);
		if (!fx->is_take_fx()) { // track fx
			if (!only_if_open or fx->is_visible()) {
				TrackFX_SetEnabled(track, fx->fx_id, !TrackFX_GetEnabled(track, fx->fx_id));
			}
		} else {
			MediaItem_Take *take = GetMediaItemTake(GetTrackMediaItem(track, fx->item_id), fx->take_id);
			if (!only_if_open or fx->is_visible()) {
				TakeFX_SetEnabled(take, fx->fx_id, !TakeFX_GetEnabled(take, fx->fx_id));
			}
		}
	}
}

auto toggle_bypass_last_focused_fx_chain_undo_str = [](MediaTrack *track, MediaItem *item = nullptr) {
	if (!item) {
		std::string undo_str = "Toggle bypass all FX: ";
		undo_str += (track == GetMasterTrack(nullptr))
						? "Master"
						: "Track " +
						std::to_string(static_cast<int>(GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER"))) +
						" " +
						get_set_media_track_info_string(track, "P_NAME", false);
		return undo_str;
	} else {
		return std::string("Toggle bypass item FX");
	}
};

void toggle_bypass_last_focused_fx_chain(COMMAND_T *cmd) {
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	bool state = true;
	if (fx->is_valid()) {
		MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id);
		if (!fx->is_take_fx()) { // track fx
			if (!only_if_open or fx->is_chain_visible()) {
				if (fx->is_rec_fx()) {
					Undo_BeginBlock2(nullptr);
					for (int i = TrackFX_GetRecCount(track) - 1; i >= 0; i--) {
						if (!TrackFX_GetEnabled(track, rec_fx + i)) {
							state = false;
							break;
						}
					}
					for (int i = TrackFX_GetRecCount(track) - 1; i >= 0; i--) {
						TrackFX_SetEnabled(track, rec_fx + i, !state);
					}
					std::string undo_str = toggle_bypass_last_focused_fx_chain_undo_str(track);
					Undo_EndBlock2(nullptr, undo_str.data(), 2);

				} else {
					Undo_BeginBlock2(nullptr);
					for (int i = TrackFX_GetCount(track) - 1; i >= 0; i--) {
						if (!TrackFX_GetEnabled(track, i)) {
							state = false;
							break;
						}
					}
					for (int i = TrackFX_GetCount(track) - 1; i >= 0; i--) {
						TrackFX_SetEnabled(track, i, !state);
					}
					std::string undo_str = toggle_bypass_last_focused_fx_chain_undo_str(track);
					Undo_EndBlock2(nullptr, undo_str.data(), 2);
				}
			}
		} else {
			MediaItem *item = GetTrackMediaItem(track, fx->item_id);
			MediaItem_Take *take = GetMediaItemTake(item, fx->take_id);
			if (!only_if_open or fx->is_chain_visible()) {
				Undo_BeginBlock2(nullptr);
				for (int i = TakeFX_GetCount(take) - 1; i >= 0; i--) {
					if (!TakeFX_GetEnabled(take, i)) {
						state = false;
						break;
					}
				}
				for (int i = TakeFX_GetCount(take) - 1; i >= 0; i--) {
					TakeFX_SetEnabled(take, i, !state);
				}
				std::string undo_str = toggle_bypass_last_focused_fx_chain_undo_str(track, item);
				Undo_EndBlock2(nullptr, undo_str.data(), 4);
			}
		}
	}
}

void toggle_offline_last_focused_fx(COMMAND_T *cmd) {
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	if (fx->is_valid()) {
		MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id);
		if (!fx->is_take_fx()) { // track fx
			if (!only_if_open or fx->is_visible()) {
				TrackFX_SetOffline(track, fx->fx_id, !TrackFX_GetOffline(track, fx->fx_id));
			}
		} else {
			MediaItem_Take *take = GetMediaItemTake(GetTrackMediaItem(track, fx->item_id), fx->take_id);
			if (!only_if_open or fx->is_visible()) {
				TakeFX_SetOffline(take, fx->fx_id, !TakeFX_GetOffline(take, fx->fx_id));
			}
		}
	}
}


void delete_last_focused_fx(COMMAND_T *cmd) {
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	if (fx->is_valid()) {
		MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id);
		if (!fx->is_take_fx()) { // track fx
			if (!only_if_open or fx->is_visible()) {
				TrackFX_Delete(track, fx->fx_id);
			}
		} else {
			MediaItem_Take *take = GetMediaItemTake(GetTrackMediaItem(track, fx->item_id), fx->take_id);
			if (!only_if_open or fx->is_visible()) {
				TakeFX_Delete(take, fx->fx_id);
			}
		}
	}
}

auto toggle_offline_last_focused_fx_chain_undo_str = [](MediaTrack *track, MediaItem *item = nullptr) {
	if (!item) {
		std::string undo_str = "Toggle offline FX: ";
		undo_str += (track == GetMasterTrack(nullptr))
						? "Master"
						: "Track " +
						std::to_string(static_cast<int>(GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER"))) +
						" " +
						get_set_media_track_info_string(track, "P_NAME", false);
		return undo_str;
	} else {
		return std::string("Toggle offline item FX");
	}
};

void toggle_offline_last_focused_fx_chain(COMMAND_T *cmd) {
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	bool state = true;
	if (fx->is_valid()) {
		MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id);
		if (!fx->is_take_fx()) { // track fx
			if (!only_if_open or fx->is_chain_visible()) {
				if (fx->is_rec_fx()) {
					Undo_BeginBlock2(nullptr);
					for (int i = TrackFX_GetRecCount(track) - 1; i >= 0; i--) {
						if (!TrackFX_GetOffline(track, rec_fx + i)) {
							state = false;
							break;
						}
					}
					for (int i = TrackFX_GetRecCount(track) - 1; i >= 0; i--) {
						TrackFX_SetOffline(track, rec_fx + i, !state);
					}
					std::string undo_str = toggle_bypass_last_focused_fx_chain_undo_str(track);
					Undo_EndBlock2(nullptr, undo_str.data(), 2);

				} else {
					Undo_BeginBlock2(nullptr);
					for (int i = TrackFX_GetCount(track) - 1; i >= 0; i--) {
						if (!TrackFX_GetOffline(track, i)) {
							state = false;
							break;
						}
					}
					for (int i = TrackFX_GetCount(track) - 1; i >= 0; i--) {
						TrackFX_SetOffline(track, i, !state);
					}
					std::string undo_str = toggle_offline_last_focused_fx_chain_undo_str(track);
					Undo_EndBlock2(nullptr, undo_str.data(), 2);
				}
			}
		} else {
			MediaItem *item = GetTrackMediaItem(track, fx->item_id);
			MediaItem_Take *take = GetMediaItemTake(item, fx->take_id);
			if (!only_if_open or fx->is_chain_visible()) {
				Undo_BeginBlock2(nullptr);
				for (int i = TakeFX_GetCount(take) - 1; i >= 0; i--) {
					if (!TakeFX_GetOffline(take, i)) {
						state = false;
						break;
					}
				}
				for (int i = TakeFX_GetCount(take) - 1; i >= 0; i--) {
					TakeFX_SetOffline(take, i, !state);
				}
				std::string undo_str = toggle_offline_last_focused_fx_chain_undo_str(track, item);
				Undo_EndBlock2(nullptr, undo_str.data(), 4);
			}
		}
	}
}


auto delete_last_focused_fx_chain_undo_str = [](MediaTrack *track, MediaItem *item = nullptr) {
	if (!item) {
		std::string undo_str = "Remove all FX: ";
		undo_str += (track == GetMasterTrack(nullptr))
						? "Master"
						: "Track " +
						std::to_string(static_cast<int>(GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER"))) +
						" " +
						get_set_media_track_info_string(track, "P_NAME", false);
		return undo_str;
	} else {
		return std::string("Remove item FX");
	}
};


void delete_last_focused_fx_chain(COMMAND_T *cmd) {
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	if (fx->is_valid()) {
		MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id);
		if (!fx->is_take_fx()) { // track fx
			if (!only_if_open or fx->is_chain_visible()) {
				if (fx->is_rec_fx()) {
					Undo_BeginBlock2(nullptr);
					for (int i = TrackFX_GetRecCount(track) - 1; i >= 0; i--) {
						TrackFX_Delete(track, 0x1000000 + i);
					}
					std::string undo_str = delete_last_focused_fx_chain_undo_str(track);
					Undo_EndBlock2(nullptr, undo_str.data(), 2);

				} else {
					if (!only_if_open or fx->is_chain_visible()) {
						Undo_BeginBlock2(nullptr);
						for (int i = TrackFX_GetCount(track) - 1; i >= 0; i--) {
							TrackFX_Delete(track, i);
						}
						std::string undo_str = delete_last_focused_fx_chain_undo_str(track);
						Undo_EndBlock2(nullptr, undo_str.data(), 2);
					}
				}
			}
		} else {
			MediaItem *item = GetTrackMediaItem(track, fx->item_id);
			MediaItem_Take *take = GetMediaItemTake(item, fx->take_id);
			if (!only_if_open or fx->is_chain_visible()) {
				Undo_BeginBlock2(nullptr);
				for (int i = TakeFX_GetCount(take) - 1; i >= 0; i--) {
					TakeFX_Delete(take, i);
				}
				std::string undo_str = delete_last_focused_fx_chain_undo_str(track, item);
				Undo_EndBlock2(nullptr, undo_str.data(), 4);
			}
		}
	}
}

auto fx_ab_comparer_undo_str = [](MediaTrack *track, MediaItem *item = nullptr) {
	if (!item) {
		std::string undo_str = "Change FX preset: ";
		undo_str += (track == GetMasterTrack(nullptr))
						? "Master"
						: "Track " +
						std::to_string(static_cast<int>(GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER"))) +
						" " +
						get_set_media_track_info_string(track, "P_NAME", false);
		return undo_str;
	} else {
		return std::string("Change item FX preset");
	}
};

void fx_ab_comparer(COMMAND_T *cmd) {
	static std::unordered_map<std::string, std::string> presets;
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	if (!fx->is_valid()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Not focused plugin\n");
		#endif
		return;
	}
	// I don't know what to do with monitoring fx (the information about them is stored in the project file, not in a separate chunk)
	if (fx->is_rec_fx() and fx->track_id == -1) return;

	MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id); // Master : regular
	std::string guid, fx_chunk;

	if (!fx->is_take_fx()) { // track fx
		if (only_if_open and !fx->is_visible()) return;
		guid = guid_to_string(TrackFX_GetFXGUID(track, fx->fx_id));
		if (guid.empty()) return;
		auto preset = presets.find(guid);
		fx_chunk = get_fx_chunk(track, fx->fx_id);
		if (fx_chunk.empty()) return;
		std::string undo_str = fx_ab_comparer_undo_str(track);
		Undo_BeginBlock2(nullptr);
		if (preset == presets.end()) {
			presets[guid] = fx_chunk;
			reset_fx_preset(track, fx->fx_id);
		} else {
			set_fx_chunk(track, fx->fx_id, preset->second);
			presets[guid] = fx_chunk;
		}
		Undo_EndBlock2(nullptr, undo_str.data(), 2);
	} else { // item fx
		MediaItem *item = GetTrackMediaItem(track, fx->item_id);
		MediaItem_Take *take = GetMediaItemTake(item, fx->take_id);
		if (only_if_open and !fx->is_visible()) return;
		guid = guid_to_string(TakeFX_GetFXGUID(take, fx->fx_id));

		if (guid.empty()) return;
		auto preset = presets.find(guid);
		fx_chunk = get_fx_chunk(take, fx->fx_id);
		if (fx_chunk.empty()) return;
		std::string undo_str = fx_ab_comparer_undo_str(track, item);
		Undo_BeginBlock2(nullptr);
		if (preset == presets.end()) {
			presets[guid] = fx_chunk;
			reset_fx_preset(take, fx->fx_id);
		} else {
			set_fx_chunk(take, fx->fx_id, preset->second);
			presets[guid] = fx_chunk;
		}
		Undo_EndBlock2(nullptr, undo_str.data(), 4);
	}
}


auto fx_chain_ab_comparer_undo_str = [](MediaTrack *track, MediaItem *item = nullptr) {
	if (!item) {
		std::string undo_str = "Change FX chain preset: ";
		undo_str += (track == GetMasterTrack(nullptr))
						? "Master"
						: "Track " +
						std::to_string(static_cast<int>(GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER"))) +
						" " +
						get_set_media_track_info_string(track, "P_NAME", false);
		return undo_str;
	} else {
		return std::string("Change item FX chain preset");
	}
};

void fx_chain_ab_comparer(COMMAND_T *cmd) {
	static std::unordered_map<std::string, std::string> presets;
	const FX *fx = LAST_FOCUSED_FX::get_last_focused_fx();
	if (!fx->is_valid()) {
		#if defined(_DEBUG)
		ShowConsoleMsg("Not focused plugin\n");
		#endif
		return;
	} else {
		#if defined(_DEBUG)
		ShowConsoleMsg("true\n");
		#endif
	}
	// I don't know what to do with monitoring fx (the information about them is stored in the project file, not in a separate chunk)
	if (fx->is_rec_fx() and fx->track_id == -1) return;

	MediaTrack *track = (fx->track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, fx->track_id); // Master : regular
	std::string guid, fx_chain_chunk;


	if (!fx->is_take_fx()) { // track fx
		if (only_if_open and !fx->is_chain_visible()) return;
		guid.resize(64);
		BR_GetMediaTrackGUID(track, &guid[0], 64);
		guid.resize(strlen(guid.c_str()));
		if (guid.empty()) return;

		if (fx->is_rec_fx()) guid += "rec";

		auto preset = presets.find(guid);
		fx_chain_chunk = get_fx_chain_chunk(track, fx->fx_id);
		if (fx_chain_chunk.empty()) return;
		std::string undo_str = fx_chain_ab_comparer_undo_str(track);
		Undo_BeginBlock2(nullptr);
		if (preset == presets.end()) {
			presets[guid] = fx_chain_chunk;
			reset_fx_chain_preset(track, fx->fx_id);
		} else {
			set_fx_chain_chunk(track, fx->fx_id, preset->second);
			presets[guid] = fx_chain_chunk;
		}
		Undo_EndBlock2(nullptr, undo_str.data(), 2);
	} else { // item fx
		MediaItem *item = GetTrackMediaItem(track, fx->item_id);
		MediaItem_Take *take = GetMediaItemTake(item, fx->take_id);
		if (only_if_open and !fx->is_chain_visible()) return;

		guid.resize(64);
		BR_GetMediaItemTakeGUID(take, &guid.front(), 64);
		guid.resize(strlen(guid.data()));
		if (guid.empty()) return;

		auto preset = presets.find(guid);
		fx_chain_chunk = get_fx_chain_chunk(take, fx->fx_id);
		if (fx_chain_chunk.empty()) return;

		std::string undo_str = fx_chain_ab_comparer_undo_str(track, item);
		Undo_BeginBlock2(nullptr);

		if (preset == presets.end()) {
			presets[guid] = fx_chain_chunk;
			reset_fx_chain_preset(take, fx->fx_id);
		} else {
			set_fx_chain_chunk(take, fx->fx_id, preset->second);
			presets[guid] = fx_chain_chunk;
		}
		Undo_EndBlock2(nullptr, undo_str.data(), 4);
	}
}

FX::FX(int tr, int it, int tk, int fx, int pr = 0): track_id(tr), item_id(it), take_id(tk), fx_id(fx), guid(_get_fx_guid()), param(pr) {}

bool FX::is_visible() const {
	MediaTrack *track = (track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, track_id);
	if (!track) return false;
	if (!is_take_fx()) {
		if (param == 2) {
			return (is_rec_fx() and TrackFX_GetRecChainVisible(track) != 1) or (!is_rec_fx() and TrackFX_GetChainVisible(track) != -1);
		}
		return TrackFX_GetOpen(track, fx_id);
	} else {
		MediaItem *item = GetTrackMediaItem(track, item_id);
		if (!item) return false;
		MediaItem_Take *take = GetMediaItemTake(item, take_id);
		if (!take) return false;
		return TakeFX_GetOpen(take, fx_id);
	}
}

bool FX::is_valid() const {
	if (param == 2) return true;
	MediaTrack *track = (track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, track_id);
	if (!track) return false;
	if (!is_take_fx()) {
		return (*TrackFX_GetFXGUID(track, fx_id) == *guid);
	} else {
		MediaItem *item = GetTrackMediaItem(track, item_id);
		if (!item) return false;
		MediaItem_Take *take = GetMediaItemTake(item, take_id);
		if (!take) return false;
		return (guid and *TakeFX_GetFXGUID(take, fx_id) == *guid);
	}
}


bool FX::is_chain_visible() const {
	MediaTrack *track = (track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, track_id);
	if (!track) return false;
	if (!is_take_fx()) {
		return (is_rec_fx() and TrackFX_GetRecChainVisible(track) != -1) or (!is_rec_fx() and TrackFX_GetChainVisible(track) != -1);
	} else {
		MediaItem *item = GetTrackMediaItem(track, item_id);
		if (!item) return false;
		MediaItem_Take *take = GetMediaItemTake(item, take_id);
		if (!take) return false;
		return TakeFX_GetChainVisible(take) != -1;
	}
}

bool FX::is_exist() const {
	int tr_id, it_id, tk_id, f_id;
	return GetFxByGUID(guid, &tr_id, &it_id, &tk_id, &f_id);
}

bool FX::is_exist_2() {
	return GetFxByGUID(guid, &track_id, &item_id, &take_id, &fx_id);
}

bool FX::is_take_fx() const {
	return (item_id != -1);
}

bool FX::is_rec_fx() const {
	return (fx_id & rec_fx);
}

bool FX::is_cont_fx() const {
	return (fx_id & cont_fx);
};

bool FX::is_chain() const {
	return param == 2;
}

bool FX::operator==(const FX &o) const {
	return track_id == o.track_id and item_id == o.item_id and take_id == o.take_id and fx_id == o.fx_id and param == o.param and *guid == *o.guid;
}

bool FX::operator!=(const FX &o) const {
	return !(track_id == o.track_id and item_id == o.item_id and take_id == o.take_id and fx_id == o.fx_id and fx_id == o.fx_id and param == o.param and *guid == *o.guid);
}

GUID *FX::_get_fx_guid() const {
	MediaTrack *track = (track_id == -1) ? GetMasterTrack(nullptr) : GetTrack(nullptr, track_id); // Master : regular
	if (!track) return nullptr;

	if (item_id == -1) {
		return TrackFX_GetFXGUID(track, fx_id);
	} else {
		MediaItem *item = GetTrackMediaItem(track, item_id);
		if (!item) return nullptr;
		MediaItem_Take *take = GetMediaItemTake(item, take_id);
		if (!take) return nullptr;
		return TakeFX_GetFXGUID(take, fx_id);
	}
}

LAST_FOCUSED_FX::LAST_FOCUSED_FX(int tr, int it, int tk, int fx, int pr): FX(tr, it, tk, fx, pr) {}

void LAST_FOCUSED_FX::last_focused_fx_observer() {
	int track_id, item_id, take_id, fx_id, param_id;
	bool ret = GetTouchedOrFocusedFX(1, &track_id, &item_id, &take_id, &fx_id, &param_id);
	if (ret) {
		if (*last_focused_fx != FX(track_id, item_id, take_id, fx_id, param_id)) {
			*last_focused_fx = LAST_FOCUSED_FX(track_id, item_id, take_id, fx_id, param_id);
		}
	} else {
		if (last_focused_fx->is_chain_visible()) {
			last_focused_fx->fx_id = last_focused_fx->is_rec_fx() ? rec_fx : 0;
			last_focused_fx->param = 2;
		}
	}
}

const LAST_FOCUSED_FX *LAST_FOCUSED_FX::get_last_focused_fx() {
	return last_focused_fx;
}

LAST_FOCUSED_FX LAST_FOCUSED_FX::_lff{};
LAST_FOCUSED_FX *LAST_FOCUSED_FX::last_focused_fx = &_lff;
