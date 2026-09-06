#include  "api.h"
#include "fx_target.h"

#include "utils.h"

constexpr bool only_if_visible = true;

#define proj nullptr
using std::operator ""s;
using std::string;

//<------------------------------FX TARGET------------------------------<//

FX::FX(int tr_idx, int it_idx, int tk_idx, int fx_idx, int param_idx) {
	m_ctx = IFXContext::get_context(tr_idx, it_idx, tk_idx);
	m_fx_idx = MAKE_FX_NOT_REC(fx_idx);
}

inline bool FX::is_visible() {
	return m_ctx->get_fx_open(m_fx_idx);
}

inline void FX::toggle_show() {
	m_ctx->set_fx_open(m_fx_idx, !m_ctx->get_fx_open(m_fx_idx));
}

inline void FX::toggle_bypass() {
	if (not only_if_visible or m_ctx->get_fx_open(m_fx_idx)) {
		m_ctx->set_fx_enabled(m_fx_idx, !m_ctx->get_fx_enabled(m_fx_idx));
	}
}

inline void FX::toggle_offline() {
	if (not only_if_visible or m_ctx->get_fx_open(m_fx_idx)) {
		m_ctx->set_fx_offline(m_fx_idx, !m_ctx->get_fx_offline(m_fx_idx));
	}
}

inline void FX::remove() {
	if (not only_if_visible or m_ctx->get_fx_open(m_fx_idx)) {
		m_ctx->delete_fx(m_fx_idx);
	}
}

inline void FX::reset() {
	if (not only_if_visible or m_ctx->get_fx_open(m_fx_idx)) {
		m_ctx->reset_preset(m_fx_idx);
	}
}

inline string FX::get_guid() {
	return m_ctx->get_fx_guid(m_fx_idx);
}

inline string FX::get_chunk() {
	return m_ctx->get_fx_chunk(m_fx_idx);
}

//>------------------------------FX TARGET------------------------------>//


//<------------------------------FXCHAIN TARGET------------------------------<//

FXChain::FXChain(int tr_idx, int it_idx, int tk_idx, int fx_idx) {
	m_ctx = IFXContext::get_context(tr_idx, it_idx, tk_idx);
	m_fx_idx = fx_idx;
}

inline bool FXChain::is_visible() {
	return m_ctx->get_fx_chain_open();
}

inline void FXChain::toggle_show() {
	m_ctx->set_fx_chain_open(m_ctx->get_fx_chain_open());
}

void FXChain::toggle_bypass() {
	if (not only_if_visible or m_ctx->get_fx_chain_open()) {
		bool enable = true;

		m_ctx->_undo_begin_block(nullptr);

		for (int i = 0; i < m_ctx->get_count(); i++) {
			if (!m_ctx->get_fx_enabled(i)) {
				enable = false;
				break;
			}
		}

		enable = !enable;
		for (int i = 0; i < m_ctx->get_count(); i++) {
			m_ctx->set_fx_enabled(i, enable);
		}

		string undo_str = "FX chain bypass toggle: "s + m_ctx->get_undo_str();
		m_ctx->_undo_end_block(proj, undo_str.data());
	}
}

void FXChain::toggle_offline() {
	if (not only_if_visible or m_ctx->get_fx_chain_open()) {
		bool offline = true;

		m_ctx->_undo_begin_block(nullptr);

		for (int i = 0; i < m_ctx->get_count(); i++) {
			if (!m_ctx->get_fx_offline(i)) {
				offline = false;
				break;
			}
		}

		offline = !offline;
		for (int i = 0; i < m_ctx->get_count(); i++) {
			m_ctx->set_fx_offline(i, offline);
		}
		string undo_str = "FX chain offline toggle: "s + m_ctx->get_undo_str();
		m_ctx->_undo_end_block(proj, undo_str.data());
	}
}

void FXChain::remove() {
	if (not only_if_visible or m_ctx->get_fx_chain_open()) {
		m_ctx->_undo_begin_block(nullptr);

		for (int i = m_ctx->get_count() - 1; i >= 0; i--) {
			m_ctx->delete_fx(i);
		}

		string undo_str = "FX chain delete: "s + m_ctx->get_undo_str();
		m_ctx->_undo_end_block(proj, undo_str.data());
	}
}

void FXChain::reset() { // default fx chain preset is empty chain
	if (not only_if_visible or m_ctx->get_fx_chain_open()) {
		m_ctx->_undo_begin_block(nullptr);

		for (int i = m_ctx->get_count() - 1; i >= 0; i--) {
			m_ctx->delete_fx(i);
		}

		string undo_str = "Reset FX chain preset: "s + m_ctx->get_undo_str();
		m_ctx->_undo_end_block(proj, undo_str.data());
	}
}

inline string FXChain::get_guid() {
	return m_ctx->get_guid();
}

inline std::string FXChain::get_chunk() {
	return m_ctx->get_fx_chain_chunk();
}


//<------------------------------FXCHAIN TARGET------------------------------<//
