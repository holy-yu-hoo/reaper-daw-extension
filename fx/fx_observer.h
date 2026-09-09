#pragma once
#include "fx_target.h"
#include "fx.h"

void toggle_show_last_focused_(COMMAND_T* cmd);


class FXObserver: public IReaperControlSurface {
	public:
	const char* GetTypeString() override { return "YH_FX_OBSERVER"; }
	const char* GetDescString() override { return "FX changes"; }
	const char* GetConfigString() override { return ""; }


	//<---------- OBSERVE CHANGES ----------<//

	int Extended(int call, void* parm1, void* parm2, void* parm3) override;


	struct FXInfo {
		std::string guid;
		std::string name;
		int index;

		bool operator ==(const FXInfo &o) const { return guid == o.guid and name == o.name and index == o.index; }
		bool operator !=(const FXInfo &o) const { return guid != o.guid or name != o.name or index != o.index; }
	};


	enum class ChangeType {
		None = 0,
		Added = 2,
		Removed = 4,
		Moved = 8,
		Renamed = 16,
		Any = Added | Removed | Moved | Renamed,
	};

	protected:
	static void propagate_extend(ChangeType call, IFXContext* ctx, int fx_idx = -1, int param3 = -1);

	static std::vector<FXInfo> m_last_state;

	static void capture_state(const FXChain* chain, std::vector<FXInfo> &state);

	static void detect_changes(std::vector<FXInfo> &state_a, std::vector<FXInfo> &state_b);

	static void refresh_last_focused_fx_index();

	static void set_focused_fx_handler();

	static void fx_add_handler(IFXContext* ctx, int fx_idx = -1);

	static void fx_remove_handler(IFXContext* ctx, int fx_idx = -1);

	static void fx_move_handler(IFXContext* ctx, int fx_idx = -1);

	static void fx_rename_handler(IFXContext* ctx, int fx_idx = -1);

	static void fx_any_handler(IFXContext* ctx, int fx_idx = -1);

	//>---------- OBSERVE CHANGES ---------->//


	//<---------- LAST FOCUSED ----------<//
	public:
	static const FX* get_last_focused_fx();

	static const FXChain* get_last_focused_fx_chain();

	static const IFXTarget* get_last_focused(bool chain);

	static void fx_change_observer();

	static FX* m_last_focused_fx;
	static FXChain* m_last_focused_fx_chain;
	//>---------- LAST FOCUSED ---------->//
};


extern FXObserver g_fx_observer;
