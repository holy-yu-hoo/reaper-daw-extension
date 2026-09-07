#include "api.h"
#include "fx_observer.h"
#include "fx_context.h"

#define proj nullptr
using std::operator ""s;
using std::string;

static FX g_last_focused_fx{};
static FXChain g_last_focused_fx_chain{};

void FXObserver::capture_state(const FXChain* chain, std::vector<FXInfo> &state_info) {
	if (!chain->is_valid()) return;
	state_info.clear();
	std::shared_ptr<IFXContext> ctx = m_last_focused_fx_chain->m_ctx;
	int count = ctx->get_count();

	for (int i = 0; i < count; ++i) {
		FXInfo info;
		info.guid = ctx->get_fx_guid(i);
		info.name = ctx->get_name(i);
		info.index = i;
		state_info.push_back(info);
	}
}

void FXObserver::detect_changes(std::vector<FXInfo> &state_a, std::vector<FXInfo> &state_b) {
	capture_state(m_last_focused_fx_chain, state_a);
	int sz = min(state_a.size(), m_last_state.size());
	int f = 0;
	while (f < sz) {
		if (state_a[f].guid == state_b[f].guid and state_a[f].name != state_b[f].name) { // rename

			g_fx_observer.Extended(
				static_cast<int>(ChangeType::Renamed),
				static_cast<void*>(m_last_focused_fx_chain->m_ctx.get()),
				reinterpret_cast<void*>(state_a[f].index),
				nullptr
			);

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

	if (_ctx) return; // for add, delete, move for track - csurf
	sz = state_b.size();
	int g;
	f = 0;
	while (f < sz) {
		if (m_last_focused_fx_chain->get_fx_by_guid(state_b[f].guid) == -1) { // delete

			g_fx_observer.Extended(
				static_cast<int>(ChangeType::Removed),
				static_cast<void*>(m_last_focused_fx_chain->m_ctx.get()),
				reinterpret_cast<void*>(state_b[f].index),
				nullptr
			);

			state_b.erase(state_b.begin() + f);
			sz -= 1;


		} else { // move
			g = std::find_if(state_a.begin(), state_a.end(), [&state_b,f](const FXInfo &info) { return info.index == state_b[f].index; }) - state_a.begin();

			g_fx_observer.Extended(
				static_cast<int>(ChangeType::Moved),
				static_cast<void*>(m_last_focused_fx_chain->m_ctx.get()),
				reinterpret_cast<void*>(state_a[f].index),
				nullptr
			);

			state_b.erase(state_b.begin() + f);
			state_a.erase(state_a.begin() + g);
			sz -= 1;
		}
	}

	for (f = 0; f < state_a.size(); f++) { // add
		g_fx_observer.Extended(
			static_cast<int>(ChangeType::Added),
			static_cast<void*>(m_last_focused_fx_chain->m_ctx.get()),
			reinterpret_cast<void*>(state_a[f].index),
			nullptr
		);
	}
}

int FXObserver::Extended(int call, void* parm1, void* parm2, void* parm3) {
	switch (call) {

		case CSURF_EXT_SETFOCUSEDFX: {
			ShowConsoleMsg("set focused fx\n");
			set_focused_fx_handler();
			break;
		}
		case static_cast<int>(ChangeType::Moved): {
			ShowConsoleMsg("Move fx\n");
			break;
		}
		case static_cast<int>(ChangeType::Renamed): {
			ShowConsoleMsg("Rename fx\n");
			break;
		}
		case static_cast<int>(ChangeType::Removed): {
			ShowConsoleMsg("Remove fx\n");
			break;
		}
		case static_cast<int>(ChangeType::Added): {
			ShowConsoleMsg("Add fx\n");
			break;
		}
		case CSURF_EXT_SETFXCHANGE: {
			ShowConsoleMsg("Change fx\n");
		}
		default: {};
	}

	return 0;
}

// The user can change the names and order of fx only from the chain window or the track effects interface
// the first requires an observer, the second is solved through csurf
void FXObserver::fx_change_observer() {
	static int _changes = GetProjectStateChangeCount(proj);
	int changes = GetProjectStateChangeCount(proj);
	if (changes == _changes) return;
	std::vector<FXInfo> cur_state;
	capture_state(m_last_focused_fx_chain, cur_state);
	if (cur_state != m_last_state) {
		detect_changes(cur_state, m_last_state);
		capture_state(m_last_focused_fx_chain, m_last_state);
	}
}

void FXObserver::set_focused_fx_handler() {
	int tr_idx, it_idx, tk_idx, fx_idx, pr_idx;
	bool ret = GetTouchedOrFocusedFX(1, &tr_idx, &it_idx, &tk_idx, &fx_idx, &pr_idx);
	if (ret) {
		FX fx(tr_idx, it_idx, tk_idx, fx_idx);
		if (*m_last_focused_fx != fx) *m_last_focused_fx = fx;
		FXChain fx_chain(tr_idx, it_idx, tk_idx, fx_idx);
		if (*m_last_focused_fx_chain != fx_chain) {
			*m_last_focused_fx_chain = fx_chain;
			capture_state(m_last_focused_fx_chain, m_last_state);
		}

	}
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
