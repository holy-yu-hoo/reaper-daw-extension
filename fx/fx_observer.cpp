#include "api.h"
#include "fx_observer.h"
#include "fx_context.h"

#define proj nullptr
using std::operator ""s;
using std::string;

// namespace {
// bool is_rec_context(IFXContext* ctx) {
// 	if (!ctx) return false;
// 	const string key = ctx->get_key();
// 	return key.size() >= 4 && key.compare(key.size() - 4, 4, "_rec") == 0;
// }
// }

void FXObserver::propagate_extend(ChangeType call, IFXContext* ctx, int fx_idx, int param3) {
	g_fx_observer.Extended(
		static_cast<int>(call),
		static_cast<void*>(ctx),
		reinterpret_cast<void*>(static_cast<INT_PTR>(fx_idx)),
		nullptr
	);
}


static FX g_last_focused_fx{};
static FXChain g_last_focused_fx_chain{};

void FXObserver::capture_state(const FXChain* chain, std::vector<FXInfo> &state_info) {
	if (!chain || !chain->is_valid()) {
		state_info.clear();
		return;
	}
	state_info.clear();
	std::shared_ptr<IFXContext> ctx = chain->m_ctx;
	if (!ctx) return;
	int count = ctx->get_count();

	for (int i = 0; i < count; ++i) {
		FXInfo info;
		info.guid = ctx->get_fx_guid(i);
		info.name = ctx->get_full_name(i);
		info.index = i;
		state_info.push_back(info);
	}
}

void FXObserver::detect_changes(std::vector<FXInfo> &state_a, std::vector<FXInfo> &state_b) {
	capture_state(m_last_focused_fx_chain, state_a);
	int sz = min(state_a.size(), state_b.size());
	int f = 0;
	while (f < sz) {
		if (state_a[f].guid == state_b[f].guid and state_a[f].name != state_b[f].name) { // rename
			propagate_extend(ChangeType::Renamed, m_last_focused_fx_chain->m_ctx.get(), state_a[f].index);

			state_a.erase(state_a.begin() + f);
			state_b.erase(state_b.begin() + f);
			sz -= 1;
		} else if (state_a[f].guid == state_b[f].guid and state_a[f].name == state_b[f].name) { // not change
			state_a.erase(state_a.begin() + f);
			state_b.erase(state_b.begin() + f);
			sz -= 1;
		} else {
			f++;
		}
	}

	TrackFXContext* _ctx = dynamic_cast<TrackFXContext*>(m_last_focused_fx_chain->m_ctx.get());

	if (_ctx) return; // non-rec track FX rely on csurf notifications
	sz = state_b.size();
	int g;
	f = 0;
	while (f < sz) {
		if (m_last_focused_fx_chain->get_fx_by_guid(state_b[f].guid) == -1) { // delete
			propagate_extend(ChangeType::Removed, m_last_focused_fx_chain->m_ctx.get(), state_b[f].index);
			state_b.erase(state_b.begin() + f);
			sz -= 1;

		} else { // move
			g = std::find_if(state_a.begin(), state_a.end(), [&state_b,f](const FXInfo &info) { return info.guid == state_b[f].guid; }) - state_a.begin();
			if (g >= static_cast<int>(state_a.size())) {
				f++;
				continue;
			}
			state_b.erase(state_b.begin() + f);
			state_a.erase(state_a.begin() + g);
			sz -= 1;
			propagate_extend(ChangeType::Moved, m_last_focused_fx_chain->m_ctx.get());
		}
	}

	for (f = 0; f < state_a.size(); f++) { // add
		propagate_extend(ChangeType::Added, m_last_focused_fx_chain->m_ctx.get(), state_a[f].index);;
	}
}

void FXObserver::refresh_last_focused_fx_index() {
	if (!m_last_focused_fx_chain || !m_last_focused_fx_chain->is_valid()) return;
	if (!m_last_focused_fx || m_last_focused_fx->m_guid.empty()) return;

	const int fx_idx = m_last_focused_fx_chain->get_fx_by_guid(m_last_focused_fx->m_guid);
	m_last_focused_fx->m_fx_idx = fx_idx;
}

int FXObserver::Extended(int call, void* parm1, void* parm2, void* parm3) {
	switch (call) {

		case CSURF_EXT_SETFOCUSEDFX: {
			#ifdef _DEBUG
			ShowConsoleMsg("Change focused fx\n");
			#endif

			set_focused_fx_handler();
			break;
		}
		case static_cast<int>(ChangeType::Added): {
			fx_renamer(static_cast<IFXContext*>(parm1));
			fx_add_handler(static_cast<IFXContext*>(parm1), reinterpret_cast<INT_PTR>(parm2));
			break;
		}
		case static_cast<int>(ChangeType::Removed): {
			fx_renamer(static_cast<IFXContext*>(parm1));
			fx_remove_handler(static_cast<IFXContext*>(parm1), reinterpret_cast<INT_PTR>(parm2));
			break;
		}
		case static_cast<int>(ChangeType::Moved): {
			refresh_last_focused_fx_index();
			fx_renamer(static_cast<IFXContext*>(parm1));
			fx_move_handler(static_cast<IFXContext*>(parm1), reinterpret_cast<INT_PTR>(parm2));

			break;
		}
		case static_cast<int>(ChangeType::Renamed): {
			fx_renamer(static_cast<IFXContext*>(parm1));
			fx_rename_handler(static_cast<IFXContext*>(parm1), reinterpret_cast<INT_PTR>(parm2));
			break;
		}

		case static_cast<int>(ChangeType::Any): {
			refresh_last_focused_fx_index();
			fx_renamer(static_cast<IFXContext*>(parm1));
			break;
		}
		case CSURF_EXT_SETFXCHANGE: {
			refresh_last_focused_fx_index();
			MediaTrack* track = static_cast<MediaTrack*>(parm1);
			if (!track) break;
			const INT_PTR flags = reinterpret_cast<INT_PTR>(parm2);
			if (track == GetMasterTrack(proj)) {
				std::shared_ptr<IFXContext> norm_ctx = IFXContext::get_context(track, 0);
				if (norm_ctx) fx_renamer(norm_ctx.get());
				std::shared_ptr<IFXContext> rec_ctx = IFXContext::get_context(track, 1);
				if (rec_ctx) fx_renamer(rec_ctx.get());
			} else {
				std::shared_ptr<IFXContext> ctx = IFXContext::get_context(track, flags & 1);
				if (ctx) fx_renamer(ctx.get());
			}
			break;
		}
		default: {};
	}

	return 0;
}

// The user can change the names and order of fx only from the chain window or the track effects interface
// the first requires an observer, the second is solved through csurf
void FXObserver::fx_change_observer() {
	if (!m_last_focused_fx_chain || !m_last_focused_fx_chain->is_valid()) {
		return;
	}
	static int _changes = GetProjectStateChangeCount(proj);
	int changes = GetProjectStateChangeCount(proj);
	if (changes == _changes) return;
	std::vector<FXInfo> cur_state;
	capture_state(m_last_focused_fx_chain, cur_state);
	if (cur_state != m_last_state) {
		detect_changes(cur_state, m_last_state);
		capture_state(m_last_focused_fx_chain, m_last_state);
		refresh_last_focused_fx_index();
	}
	_changes = changes;
}

void FXObserver::set_focused_fx_handler() {
	int tr_idx, it_idx, tk_idx, fx_idx, pr_idx;
	bool ret = GetTouchedOrFocusedFX(1, &tr_idx, &it_idx, &tk_idx, &fx_idx, &pr_idx);
	if (ret) {
		FX fx(tr_idx, it_idx, tk_idx, fx_idx);
		*m_last_focused_fx = fx;
		FXChain fx_chain(tr_idx, it_idx, tk_idx, fx_idx);
		if (*m_last_focused_fx_chain != fx_chain) {
			*m_last_focused_fx_chain = fx_chain;
			capture_state(m_last_focused_fx_chain, m_last_state);
		}
		refresh_last_focused_fx_index();

	}
}

void FXObserver::fx_add_handler(IFXContext* ctx, int fx_idx) {
	#ifdef _DEBUG
	ShowConsoleMsg("add fx\n");
	#endif
}

void FXObserver::fx_remove_handler(IFXContext* ctx, int fx_idx) {
	#ifdef _DEBUG
	ShowConsoleMsg("remove fx\n");
	#endif
}

void FXObserver::fx_move_handler(IFXContext* ctx, int fx_idx) {
	#ifdef _DEBUG
	ShowConsoleMsg("move fx\n");
	#endif
}

void FXObserver::fx_rename_handler(IFXContext* ctx, int fx_idx) {
	#ifdef _DEBUG
	ShowConsoleMsg("rename fx\n");
	#endif
}

void FXObserver::fx_any_handler(IFXContext* ctx, int fx_idx) {
	#ifdef _DEBUG
	ShowConsoleMsg("change fx\n");
	#endif
	fx_renamer(ctx);
}

const FX* FXObserver::get_last_focused_fx() {
	return m_last_focused_fx;
}

const FXChain* FXObserver::get_last_focused_fx_chain() {
	return m_last_focused_fx_chain;
}

const IFXTarget* FXObserver::get_last_focused(bool chain) {
	if (chain) return m_last_focused_fx_chain;
	else return m_last_focused_fx;
}

std::vector<FXObserver::FXInfo> FXObserver::m_last_state;


FX* FXObserver::m_last_focused_fx = &g_last_focused_fx;
FXChain* FXObserver::m_last_focused_fx_chain = &g_last_focused_fx_chain;

FXObserver g_fx_observer{};
