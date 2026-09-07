#pragma once
#include "fx_target.h"

void toggle_show_last_focused_(COMMAND_T* cmd);


class FXObserver: public IReaperControlSurface {
	public:
	const char* GetTypeString() override { return "YH_FX_OBSERVER"; }
	const char* GetDescString() override { return "FX changes"; }
	const char* GetConfigString() override { return ""; }


	struct FXInfo {
		std::string guid;
		std::string name;
		int index;

		bool operator ==(const FXInfo &o) const { return guid == o.guid and name == o.name and index == o.index; }
		bool operator !=(const FXInfo &o) const { return guid != o.guid or name != o.name or index != o.index; }
	};


	enum class ChangeType {
		None,
		Added,
		Removed,
		Moved,
		Renamed,
	};


	int Extended(int call, void* parm1, void* parm2, void* parm3) override;

	static void set_focused_fx_handler();

	static const FX* get_last_focused_fx();

	static const FXChain* get_last_focused_fx_chain();

	static const IFXTarget* get_last_focused(bool chain);

	static void fx_change_observer();

	protected:
	static std::vector<FXInfo> m_last_state;

	static void capture_state(const FXChain* chain, std::vector<FXInfo> &state);

	static void detect_changes(std::vector<FXInfo> &state_a, std::vector<FXInfo> &state_b);

	static FX* m_last_focused_fx;
	static FXChain* m_last_focused_fx_chain;
};


extern FXObserver g_fx_observer;
