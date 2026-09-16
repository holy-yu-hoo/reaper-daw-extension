#include  "api.h"
#include "fx_target.h"
#include "utils.h"
#define proj nullptr
using std::operator ""s;
using std::string;

//<------------------------------FX TARGET------------------------------<//
FX::FX() : m_fx_idx(-1) {}

FX::FX(int tr_idx, int it_idx, int tk_idx, int fx_idx) {
	m_ctx = IFXContext::get_context(tr_idx, it_idx, tk_idx, IS_REC_FX(fx_idx));
	m_fx_idx = MAKE_FX_NOT_REC(fx_idx);
	if (m_ctx and m_fx_idx >= 0) {
		m_guid = m_ctx->get_fx_guid(m_fx_idx);
	}

}

FX::FX(std::shared_ptr<IFXContext> ctx, int fx_idx) : IFXTarget(ctx), m_fx_idx(fx_idx) {
	if (m_ctx and m_fx_idx >= 0) {
		m_guid = m_ctx->get_fx_guid(m_fx_idx);
	}
}

inline bool FX::is_visible() const {
	return m_ctx->get_fx_open(m_fx_idx);
}

inline void FX::toggle_show() const {
	m_ctx->set_fx_open(m_fx_idx, !m_ctx->get_fx_open(m_fx_idx));
}

inline void FX::toggle_bypass() const {
	if (m_ctx->get_fx_open(m_fx_idx)) {
		m_ctx->set_fx_enabled(m_fx_idx, !m_ctx->get_fx_enabled(m_fx_idx));
	}
}

inline void FX::toggle_offline() const {
	if (m_ctx->get_fx_open(m_fx_idx)) {
		m_ctx->set_fx_offline(m_fx_idx, !m_ctx->get_fx_offline(m_fx_idx));
	}
}

inline void FX::remove() const {
	if (m_ctx->get_fx_open(m_fx_idx)) {
		m_ctx->delete_fx(m_fx_idx);
	}
}

inline void FX::reset() const {
	if (m_ctx->get_fx_open(m_fx_idx)) {
		m_ctx->reset_preset(m_fx_idx);
	}
}

inline string FX::get_guid() const {
	return m_ctx->get_fx_guid(m_fx_idx);
}

inline string FX::get_chunk() const {
	return m_ctx->get_fx_chunk(m_fx_idx);
}

bool FX::set_chunk(std::string chunk) const {
	return m_ctx->set_fx_chunk(m_fx_idx, chunk);
}

bool FX::operator==(const IFXTarget& other) const {
	const FX* p_other = dynamic_cast<const FX*>(&other);
	if (!p_other) return false;
	return (m_ctx == p_other->m_ctx) and (m_guid == p_other->m_guid);
}

bool FX::operator!=(const IFXTarget& other) const {
	const FX* p_other = dynamic_cast<const FX*>(&other);
	if (!p_other) return true;
	return (m_ctx != p_other->m_ctx) or (m_guid != p_other->m_guid);
}

bool FX::operator!() const {
	return !is_valid();
}

FX::operator bool() const {
	return is_valid();
}

std::string FX::get_key() const {
	return m_guid;
}

bool FX::is_valid() const {
	return (m_ctx and m_fx_idx >= 0 and m_ctx->is_valid() and m_ctx->get_fx_guid(m_fx_idx) == m_guid);
}

std::string FX::get_config_param(std::string par_name) const {
	return m_ctx->get_config_param(m_fx_idx, par_name);
}

bool FX::set_config_param(std::string par_name, std::string par_value)const {
	return m_ctx->set_config_param(m_fx_idx, par_name, par_value);
}

void FX::copy(IFXContext* dest, int dest_fx_idx) const {
	m_ctx->copy_fx(m_fx_idx, dest, dest_fx_idx);
}

void FX::move(IFXContext* dest, int dest_fx_idx) const {
	m_ctx->move_fx(m_fx_idx, dest, dest_fx_idx);
}

std::string FX::Preset::get_fx_name()const {
	return m_fx_name;
}

FX::Preset FX::Preset::get(const FX* fx) {
	if (!fx) return FX::Preset();
	return FX::Preset(fx->get_chunk(), fx->get_config_param("fx_name"));
}

bool FX::Preset::set(const FX* fx, const FX::Preset& preset) {
	if (!fx || !fx->is_valid()) return false;
	if (preset.m_fx_name != fx->get_config_param("fx_name")) return false;
	fx->set_chunk(preset.m_preset);
	return true;
}


//>------------------------------FX TARGET------------------------------>//


//<------------------------------FXCHAIN TARGET------------------------------<//

FXChain::FXChain() {}

FXChain::FXChain(int tr_idx, int it_idx, int tk_idx, int fx_idx) {
	m_ctx = IFXContext::get_context(tr_idx, it_idx, tk_idx, IS_REC_FX(fx_idx));
}

FXChain::FXChain(std::shared_ptr<IFXContext> ctx) : IFXTarget(ctx) {}

inline bool FXChain::is_visible() const {
	return m_ctx->get_fx_chain_open();
}

inline void FXChain::toggle_show() const {
	m_ctx->set_fx_chain_open(!m_ctx->get_fx_chain_open());
}

void FXChain::toggle_bypass() const {
	if (!m_ctx->get_fx_chain_open()) return;
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

void FXChain::toggle_offline() const {
	if (!m_ctx->get_fx_chain_open()) return;
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

void FXChain::remove() const {
	if (!m_ctx || m_ctx->get_count() <= 0) return;
	m_ctx->_undo_begin_block(nullptr);

	for (int i = m_ctx->get_count() - 1; i >= 0; i--) {
		m_ctx->delete_fx(i);
	}

	string undo_str = "FX chain delete: "s + m_ctx->get_undo_str();
	m_ctx->_undo_end_block(proj, undo_str.data());
}

void FXChain::reset() const { // default fx chain preset is empty chain
	if (!m_ctx || m_ctx->get_count() <= 0) return;
	m_ctx->_undo_begin_block(nullptr);

	for (int i = m_ctx->get_count() - 1; i >= 0; i--) {
		m_ctx->delete_fx(i);
	}

	string undo_str = "Reset FX chain preset: "s + m_ctx->get_undo_str();
	m_ctx->_undo_end_block(proj, undo_str.data());

}

inline string FXChain::get_guid() const {
	return m_ctx->get_guid();
}

inline std::string FXChain::get_chunk() const {
	return "";
}

bool FXChain::set_chunk(std::string chunk) const {
	return false;
}

bool FXChain::operator==(const IFXTarget& other) const {
	const FXChain* p_other = dynamic_cast<const FXChain*>(&other);
	if (!p_other) return false;
	return (m_ctx == p_other->m_ctx);
}

bool FXChain::operator!=(const IFXTarget& other) const {
	const FXChain* p_other = dynamic_cast<const FXChain*>(&other);
	if (!p_other) return true;
	return (m_ctx != p_other->m_ctx);
}

bool FXChain::operator!() const {
	return !is_valid();
}

FXChain::operator bool() const {
	return is_valid();
}

string FXChain::get_key() const {
	return m_ctx->get_key();
}

bool FXChain::is_valid() const {
	return m_ctx and m_ctx->is_valid();
}

FX FXChain::get_fx(int fx_idx) const {
	return FX(m_ctx, fx_idx);
}


inline int FXChain::get_count() const {
	return m_ctx->get_count();
}

int FXChain::add_fx_by_name(std::string name) const {
	return m_ctx->add_fx_by_name(name);
}

int FXChain::get_fx_by_guid(string guid)const {
	for (int f = 0; f < m_ctx->get_count(); f++) {
		if (guid == m_ctx->get_fx_guid(f)) {
			return f;
		}
	}
	return -1;
}

void FXChain::copy(IFXContext* dest, int dest_fx_idx)const {
	for (int i = 0;i < get_count();i++) {
		m_ctx->copy_fx(i, dest, dest_fx_idx);
	}

}

void FXChain::move(IFXContext* dest, int dest_fx_idx)const {
	for (int i = 0;i < get_count();i++) {
		m_ctx->move_fx(i, dest, dest_fx_idx);
	}
}

FXChain::Preset FXChain::Preset::get(const FXChain* fx_chain) {
	FXChain::Preset chain_preset{};
	if (!fx_chain) return chain_preset;
	for (int f = 0; f < fx_chain->get_count(); f++) {
		FX fx = fx_chain->get_fx(f);
		chain_preset.m_preset_list.push_back(FX::Preset::get(&fx));
	}
	return chain_preset;
}

bool FXChain::Preset::set(const FXChain* fx_chain, const FXChain::Preset& preset) {
	if (!fx_chain) return false;

	fx_chain->remove();
	for (int i = 0; i < preset.m_preset_list.size(); ++i) {
		const FX::Preset& item = preset.m_preset_list[i];
		int pos = fx_chain->add_fx_by_name(item.get_fx_name());
		if (pos < 0) continue;
		FX fx = fx_chain->get_fx(pos);
		FX::Preset::set(&fx, item);
	}
	return true;
}


//<------------------------------FXCHAIN TARGET------------------------------<//
