

void select_track_cycle(COMMAND_T* cmd);

void select_track_cycle_keep_selection(COMMAND_T* cmd);

void select_tracks_cycle(COMMAND_T* cmd);

class TrackOSC :public IReaperControlSurface {
public:
	virtual const char* GetTypeString() { return "TrackOSC"; }
	virtual const char* GetDescString() { return "YH_TrackOSC"; }
	virtual const char* GetConfigString() { return ""; }



	virtual void SetSurfaceSelected(MediaTrack* trackid, bool selected);

	virtual int Extended(int call, void* parm1, void* parm2, void* parm3);

	static void set_mode(COMMAND_T* cmd) { mode == cmd->user ? mode = 0 : mode = cmd->user; }

	static int get_mode(COMMAND_T* cmd) { return mode == cmd->user; }

protected:
	static int mode; // 0 - off; 2 - last touched; 4 - seleceted; 6 - last touched while selected
	int last_touched_handler(MediaTrack* track) {
		if (mode & 2) {
			for (int i = 0;i < CountTracks(nullptr);i++) {
				SetMediaTrackInfo_Value(GetTrack(nullptr, i), "I_SOLO", false);
			}
			SetMediaTrackInfo_Value(track, "I_SOLO", true);
		}
		return 0;
	}

	int selected_handler(MediaTrack* track, bool selected) {
		if (mode & 4) {
			SetMediaTrackInfo_Value(track, "I_SOLO", selected);
		}
		return 0;
	}
};

extern TrackOSC g_track_osc;