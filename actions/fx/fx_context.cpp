#include "api.h"
#include "fx_class.h"
#include "fx_utils.h"

#define proj nullptr


TrackFXContext::TrackFXContext(int tr_id, bool rec) {
	m_track = tr_id == -1 ? GetMasterTrack(proj) : GetTrack(proj, tr_id);
	m_rec = rec;
}

TrackFXContext::TrackFXContext(MediaTrack* track, bool rec): m_track(track), m_rec(rec) {}


int TrackFXContext::get_count() {
	return m_rec ? TrackFX_GetRecCount(m_track) : TrackFX_GetCount(m_track);
}

bool TrackFXContext::get_enabled(int fx_idx) {
	return TrackFX_GetEnabled(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}

void TrackFXContext::set_enabled(int fx_idx, bool enabled) {
	TrackFX_SetEnabled(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, enabled);
}


bool TrackFXContext::get_fx_open(int fx_idx) {
	return TrackFX_GetOpen(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx);
}


void TrackFXContext::set_fx_open(int fx_idx, bool open) {
	TrackFX_SetEnabled(m_track, m_rec ? MAKE_FX_REC(fx_idx) : fx_idx, open);
}

bool TrackFXContext::get_fx_chain_open() {
	return (m_rec ? TrackFX_GetRecChainVisible(m_track) : TrackFX_GetChainVisible(m_track)) != -1;
}

int TrackFXContext::get_fx_chain_open(bool more_specifically) {
	return m_rec ? TrackFX_GetRecChainVisible(m_track) : TrackFX_GetChainVisible(m_track);
}

void TrackFXContext::set_fx_chain_open(bool open) {
	TrackFX_Show(m_track, get_selected_fx(m_track, m_rec), open);
}

void TrackFXContext::set_fx_chain_open(bool open, int fx_idx) {
	TrackFX_Show(m_track, fx_idx, open);
}
