#pragma once
#include "utils.h"


class TrackObserver: public IReaperControlSurface {
	public:
	const char* GetTypeString() override { return "TrackObserver"; }
	const char* GetDescString() override { return "YH_TrackObserver"; }
	const char* GetConfigString() override { return ""; }


	void SetSurfaceSelected(MediaTrack* track, bool selected) override;

	int Extended(int call, void* parm1, void* parm2, void* parm3) override;

	static void set_auto_solo_mode(COMMAND_T* cmd);

	static int get_auto_solo_mode(COMMAND_T* cmd);

	protected:
	static int m_cur_auto_solo_mode;
	static std::unique_ptr<ITrackSoloMode> m_auto_solo_mode;
};


extern TrackObserver g_track_observer;
