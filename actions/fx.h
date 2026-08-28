#pragma once

void toggle_show_last_focused_fx(COMMAND_T *cmd);

void toggle_show_last_focused_fx_chain(COMMAND_T *cmd);

void toggle_bypass_last_focused_fx(COMMAND_T *cmd);

void toggle_bypass_last_focused_fx_chain(COMMAND_T *cmd);

void toggle_offline_last_focused_fx(COMMAND_T *cmd);

void toggle_offline_last_focused_fx_chain(COMMAND_T *cmd);

void delete_last_focused_fx(COMMAND_T *cmd);

void delete_last_focused_fx_chain(COMMAND_T *cmd);

void fx_ab_comparer(COMMAND_T *cmd);

void fx_chain_ab_comparer(COMMAND_T *cmd);


class FX {
	public:
	int track_id, item_id, take_id, fx_id, param;
	GUID *guid = nullptr;

	FX() = default;

	FX(int, int, int, int, int);

	bool is_valid() const; // checks if fx is in the right place

	bool is_visible() const; // float window or fx chain window
	bool is_chain_visible() const;

	bool is_exist() const; // does it exist in the project

	bool is_exist_2(); // does it exist in the project (and update the information)

	inline bool is_take_fx() const;

	inline bool is_rec_fx() const;

	inline bool is_cont_fx() const;

	inline bool is_chain() const;

	bool operator==(const FX &) const;

	bool operator!=(const FX &) const;

	GUID *_get_fx_guid() const;
};


class LAST_FOCUSED_FX: public FX {
	public:
	LAST_FOCUSED_FX(const LAST_FOCUSED_FX &) = delete;

	LAST_FOCUSED_FX(LAST_FOCUSED_FX &&) = default;

	LAST_FOCUSED_FX &operator=(const LAST_FOCUSED_FX &) = delete;

	LAST_FOCUSED_FX &operator=(LAST_FOCUSED_FX &&) = default;

	static const LAST_FOCUSED_FX *get_last_focused_fx();

	static void last_focused_fx_observer();

	protected:
	LAST_FOCUSED_FX() = default;

	LAST_FOCUSED_FX(int, int, int, int, int);


	static LAST_FOCUSED_FX _lff;
	static LAST_FOCUSED_FX *last_focused_fx;
};
