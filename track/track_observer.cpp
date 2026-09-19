#include "api.h"
#include "track_observer.h"
#include "utils.h"


void TrackObserver::SetSurfaceSelected(MediaTrack* track, bool selected) {
	m_auto_solo_mode->OnTrackEvent(TrackEvent{TrackEvent::SelectionChanged, track, selected});
}

int TrackObserver::Extended(int call, void* parm1, void* parm2, void* parm3) {
	switch (call) {
		case (CSURF_EXT_SETLASTTOUCHEDTRACK): {
			MediaTrack* track = static_cast<MediaTrack*>(parm1);
			return m_auto_solo_mode->OnTrackEvent(TrackEvent{TrackEvent::LastTouched, track});
			break;
		}
		default: {}
	}
	return 0;
}

void TrackObserver::set_auto_solo_mode(COMMAND_T* cmd) {
	int mode_val = cmd->user;
	if (m_cur_auto_solo_mode == mode_val) mode_val = 0;
	m_auto_solo_mode->OnDeactivate();
	switch (mode_val) {
		case 1: {
			m_auto_solo_mode = std::make_unique<AutoSoloModeLastTouch>();
			break;
		}
		case 2: {
			m_auto_solo_mode = std::make_unique<AutoSoloModeSelected>();
			break;
		}
		case 4: {
			m_auto_solo_mode = std::make_unique<AutoSoloModeLastTouchSel>();
			break;
		}
		case 0: {
			m_auto_solo_mode = std::make_unique<AutoSoloModeOff>();
			m_cur_auto_solo_mode = 0;
			return;
			break;
		}
		default: ;
	}
	m_cur_auto_solo_mode = mode_val;
	m_auto_solo_mode->OnActivate();

}

int TrackObserver::get_auto_solo_mode(COMMAND_T* cmd) {
	return m_cur_auto_solo_mode == cmd->user;
};

int TrackObserver::m_cur_auto_solo_mode = 0;
std::unique_ptr<ITrackSoloMode> TrackObserver::m_auto_solo_mode = std::make_unique<AutoSoloModeOff>();

TrackObserver g_track_observer{};
