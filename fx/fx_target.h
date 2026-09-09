#pragma once
#include "fx_context.h"


class IFXTarget {
	public:
	virtual bool is_visible() const =0;

	virtual void toggle_show() const =0;

	virtual void toggle_bypass() const =0;

	virtual void toggle_offline() const =0;

	virtual void remove() const =0;

	virtual void reset() const =0;

	virtual std::string get_guid() const =0;

	virtual std::string get_chunk() const =0;

	virtual void set_chunk(std::string chunk) const =0;

	virtual bool operator==(const IFXTarget &other) const =0;

	virtual bool operator!=(const IFXTarget &other) const =0;

	virtual operator bool() const =0;

	virtual bool operator!() const =0;

	virtual std::string get_key() const =0;

	virtual bool is_valid() const =0;

	virtual ~IFXTarget() = default;

	// static std::unique_ptr<IFXTarget> get_target(int tr_idx, int it_idx, int tk_idx, int fx_idx, int param_idx); // param not used yet
};


class FX: public IFXTarget {
	std::shared_ptr<IFXContext> m_ctx;
	int m_fx_idx;
	std::string m_guid;
	friend class FXObserver;

	public:
	FX();

	FX(int tr_idx, int it_idx, int tk_idx, int fx_idx);

	FX(std::shared_ptr<IFXContext> ctx, int fx_idx);

	virtual bool is_visible() const override;

	virtual void toggle_show() const override;

	virtual void toggle_bypass() const override;

	virtual void toggle_offline() const override;

	virtual void remove() const override;

	virtual void reset() const override;

	virtual std::string get_guid() const override;

	virtual std::string get_chunk() const override;

	virtual void set_chunk(std::string chunk) const override;

	virtual bool operator==(const IFXTarget &other) const override;

	virtual bool operator!=(const IFXTarget &other) const override;

	virtual bool operator!() const override;

	virtual operator bool() const override;

	virtual std::string get_key() const override;

	virtual bool is_valid() const override;

	virtual std::string get_config_param(std::string par_name);

	virtual bool set_config_param(std::string par_name, std::string par_value);

	virtual ~FX() override = default;
};


class FXChain: public IFXTarget {
	std::shared_ptr<IFXContext> m_ctx;

	friend class FXObserver;

	public:
	FXChain();

	FXChain(int tr_idx, int it_idx, int tk_idx, int fx_idx);

	FXChain(std::shared_ptr<IFXContext> ctx);

	virtual bool is_visible() const override;

	virtual void toggle_show() const override;

	virtual void toggle_bypass() const override;

	virtual void toggle_offline() const override;

	virtual void remove() const override;

	virtual void reset() const override;

	virtual std::string get_guid() const override;

	virtual std::string get_chunk() const override;

	virtual void set_chunk(std::string chunk) const override;

	virtual bool operator==(const IFXTarget &) const override;

	virtual bool operator!=(const IFXTarget &) const override;

	virtual bool operator!() const override;

	virtual operator bool() const override;

	virtual std::string get_key() const override;

	virtual bool is_valid() const override;

	virtual FX get_fx(int fx_idx) const;

	virtual ~FXChain() override = default;

	virtual int get_count();

	virtual int add_fx_by_name(std::string name);

	virtual int get_fx_by_guid(std::string p_guid);
};
