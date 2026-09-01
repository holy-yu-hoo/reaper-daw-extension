#pragma once
#include <memory>


class IFXContext {
	public:
	virtual bool get_fx_open(int fx_idx) =0;

	virtual void set_fx_open(int fx_idx, bool open) =0;

	virtual bool get_fx_chain_open() =0;

	virtual int get_fx_chain_open(bool more_specifically); // returns the number of the open effect, -1 or -2 (see TrackFX_GetChainVisible)

	virtual void set_fx_chain_open(bool open) =0;

	virtual void set_fx_chain_open(bool open, int fx_idx) =0;

	virtual bool get_enabled(int fx_idx) = 0;

	virtual void set_enabled(int fx_idx, bool enabled) = 0;

	virtual bool get_offline(int fx_idx) = 0;

	virtual void set_offline(int fx_idx, bool offline) = 0;

	virtual void remove(int fx_idx) =0;

	virtual void reset_preset(int fx_idx) =0;

	virtual int get_count() =0;

	virtual GUID* get_fx_guid(int fx_idx) =0;

	virtual GUID* get_guid() =0;

	virtual std::string get_fx_chunk(int fx_idx) =0;

	virtual std::string get_fx_chain_chunk() =0;

	virtual std::string get_undo_str() =0;

	virtual ~IFXContext() = default;

	static std::shared_ptr<IFXContext> get_context(int tr_idx, int it_idx, int tk_idx); // return track or take context
};


class TrackFXContext: public IFXContext {
	MediaTrack* m_track = nullptr;
	bool m_rec;

	public:
	TrackFXContext(int tr_id, bool rec = false);

	TrackFXContext(MediaTrack* track, bool rec = false);

	virtual bool get_fx_open(int fx_idx) override;

	virtual void set_fx_open(int fx_idx, bool open) override;

	virtual bool get_fx_chain_open() override;

	virtual int get_fx_chain_open(bool more_specifically) override;

	virtual void set_fx_chain_open(bool open) override;

	virtual void set_fx_chain_open(bool open, int fx_idx) override;

	virtual bool get_enabled(int fx_idx) override;

	virtual void set_enabled(int fx_idx, bool enabled) override;

	virtual bool get_offline(int fx_idx) override;

	virtual void set_offline(int fx_idx, bool offline) override;

	virtual void remove(int fx_idx) override;

	virtual void reset_preset(int fx_idx) override;

	virtual int get_count() override;

	virtual GUID* get_fx_guid(int fx_idx) override;

	virtual GUID* get_guid() override;

	virtual std::string get_fx_chunk(int fx_idx) override;

	virtual std::string get_fx_chain_chunk() override;

	virtual std::string get_undo_str() override;

	virtual ~TrackFXContext() override = default;
};


class TakeFXContext: public IFXContext {

	MediaItem_Take* m_take = nullptr;

	public:
	TakeFXContext(int tk_id);

	TakeFXContext(MediaItem_Take* take);

	virtual bool get_fx_open(int fx_idx) override;

	virtual void set_fx_open(int fx_idx, bool open) override;

	virtual bool get_fx_chain_open() override;

	virtual void set_fx_chain_open(bool open) override;

	virtual void set_fx_chain_open(bool open, int fx_idx) override;

	virtual bool get_enabled(int fx_idx) override;

	virtual void set_enabled(int fx_idx, bool enabled) override;

	virtual bool get_offline(int fx_idx) override;

	virtual void set_offline(int fx_idx, bool offline) override;

	virtual void remove(int fx_idx) override;

	virtual void reset_preset(int fx_idx) override;

	virtual int get_count() override;

	virtual GUID* get_fx_guid(int fx_idx) override;

	virtual GUID* get_guid() override;

	virtual std::string get_fx_chunk(int fx_idx) override;

	virtual std::string get_fx_chain_chunk() override;

	virtual std::string get_undo_str() override;

	virtual ~TakeFXContext() override = default;
};


class IFXTarget {
	public:
	virtual bool is_visible() =0;

	virtual void toggle_show() =0;

	virtual void toggle_bypass() =0;

	virtual void toggle_offline() =0;

	virtual void remove() =0;

	virtual void reset() =0;

	virtual GUID* get_guid() =0;

	virtual std::string get_chunk() =0;

	virtual ~IFXTarget() = default;

	static std::unique_ptr<IFXTarget> get_target(int tr_idx, int it_idx, int tk_idx, int fx_idx, int param_idx); // param not used yet
};


class FX: public IFXTarget {
	std::unique_ptr<IFXContext> ctx;
	int fx_idx;

	public:
	FX(int tr_idx, int it_idx, int tk_idx, int fx_idx, int param_idx);

	virtual bool is_visible() override;

	virtual void toggle_show() override;

	virtual void toggle_bypass() override;

	virtual void toggle_offline() override;

	virtual void remove() override;

	virtual void reset() override;

	virtual GUID* get_guid() override;

	virtual std::string get_chunk() override;

	virtual ~FX() override = default;
};


class FXChain: public IFXTarget {
	std::unique_ptr<IFXContext> ctx;
	int fx_idx;

	public:
	virtual bool is_visible() override;

	virtual void toggle_show() override;

	virtual void toggle_bypass() override;

	virtual void toggle_offline() override;

	virtual void remove() override;

	virtual void reset() override;

	virtual GUID* get_guid() override;

	virtual std::string get_chunk() override;

	virtual ~FXChain() override = default;

};
