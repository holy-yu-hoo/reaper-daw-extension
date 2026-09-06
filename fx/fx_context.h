#pragma once

class IFXContext {
	public:
	virtual bool get_fx_open(int fx_idx) =0;

	virtual void set_fx_open(int fx_idx, bool open) =0;

	virtual bool get_fx_chain_open() =0;

	virtual int get_fx_chain_open(bool more_specifically); // returns the number of the open effect, -1 or -2 (see TrackFX_GetChainVisible)

	virtual void set_fx_chain_open(bool open) =0;

	virtual void set_fx_chain_open(bool open, int fx_idx) =0;

	virtual bool get_fx_enabled(int fx_idx) = 0;

	virtual void set_fx_enabled(int fx_idx, bool enabled) = 0;

	virtual bool get_fx_offline(int fx_idx) = 0;

	virtual void set_fx_offline(int fx_idx, bool offline) = 0;

	virtual void delete_fx(int fx_idx) =0;

	virtual bool reset_preset(int fx_idx) =0;

	virtual int get_count() =0;

	virtual std::string get_fx_guid(int fx_idx) =0;

	virtual std::string get_guid() =0;

	virtual std::string get_fx_chunk(int fx_idx) =0;

	virtual std::string get_fx_chain_chunk() =0;

	virtual std::string get_undo_str() =0;

	virtual std::string get_config_param(int fx_idx, const std::string &param_name) =0;

	virtual std::string get_full_name(int fx_idx) =0; // name kind: "type: name (vendor)"

	virtual std::string get_name(int fx_idx) =0; // name kind: "name" (without "type: " and " (vendor)")

	virtual ~IFXContext() = default;

	static std::shared_ptr<IFXContext> get_context(int tr_idx, int it_idx, int tk_idx, int param = 0); // return track or take context

	virtual void _undo_begin_block(ReaProject* project) =0;

	virtual void _undo_end_block(ReaProject* project, std::string undo_str) =0;
};


class TrackFXContext: public IFXContext {
	MediaTrack* m_track = nullptr;
	bool m_rec;

	public:
	TrackFXContext(int tr_idx, bool rec = false);

	TrackFXContext(MediaTrack* track, bool rec = false);

	virtual bool get_fx_open(int fx_idx) override;

	virtual void set_fx_open(int fx_idx, bool open) override;

	virtual bool get_fx_chain_open() override;

	virtual int get_fx_chain_open(bool more_specifically) override;

	virtual void set_fx_chain_open(bool open) override;

	virtual void set_fx_chain_open(bool open, int fx_idx) override;

	virtual bool get_fx_enabled(int fx_idx) override;

	virtual void set_fx_enabled(int fx_idx, bool enabled) override;

	virtual bool get_fx_offline(int fx_idx) override;

	virtual void set_fx_offline(int fx_idx, bool offline) override;

	virtual void delete_fx(int fx_idx) override;

	virtual bool reset_preset(int fx_idx) override;

	virtual int get_count() override;

	virtual std::string get_fx_guid(int fx_idx) override;

	virtual std::string get_guid() override;

	virtual std::string get_fx_chunk(int fx_idx) override;

	virtual std::string get_fx_chain_chunk() override;

	virtual std::string get_undo_str() override;

	virtual std::string get_config_param(int fx_idx, const std::string &param_name) override;

	virtual std::string get_full_name(int fx_idx) override;

	virtual std::string get_name(int fx_idx) override;

	virtual ~TrackFXContext() override = default;

	virtual void _undo_begin_block(ReaProject* project) override;

	virtual void _undo_end_block(ReaProject* project, std::string undo_str) override;

};


class TakeFXContext: public IFXContext {
	MediaItem_Take* m_take = nullptr;

	public:
	TakeFXContext(int tr_idx, int it_idx, int tk_idx);

	TakeFXContext(MediaItem_Take* take);

	virtual bool get_fx_open(int fx_idx) override;

	virtual void set_fx_open(int fx_idx, bool open) override;

	virtual bool get_fx_chain_open() override;

	virtual int get_fx_chain_open(bool more_specifically) override;

	virtual void set_fx_chain_open(bool open) override;

	virtual void set_fx_chain_open(bool open, int fx_idx) override;

	virtual bool get_fx_enabled(int fx_idx) override;

	virtual void set_fx_enabled(int fx_idx, bool enabled) override;

	virtual bool get_fx_offline(int fx_idx) override;

	virtual void set_fx_offline(int fx_idx, bool offline) override;

	virtual void delete_fx(int fx_idx) override;

	virtual bool reset_preset(int fx_idx) override;

	virtual int get_count() override;

	virtual std::string get_fx_guid(int fx_idx) override;

	virtual std::string get_guid() override;

	virtual std::string get_fx_chunk(int fx_idx) override;

	virtual std::string get_fx_chain_chunk() override;

	virtual std::string get_undo_str() override;

	virtual std::string get_config_param(int fx_idx, const std::string &param_name) override;

	virtual std::string get_full_name(int fx_idx) override;

	virtual std::string get_name(int fx_idx) override;

	virtual ~TakeFXContext() override = default;

	virtual void _undo_begin_block(ReaProject* project) override;

	virtual void _undo_end_block(ReaProject* project, std::string undo_str) override;
};
