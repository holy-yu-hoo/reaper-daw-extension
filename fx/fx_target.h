#pragma once
#include "fx_context.h"


class IFXTarget {
protected:
	std::shared_ptr<IFXContext> m_ctx;
public:
	IFXTarget(std::shared_ptr<IFXContext> ctx = nullptr) :m_ctx(ctx) {};

	virtual bool is_visible() const = 0;

	virtual void toggle_show() const = 0;

	virtual void toggle_bypass() const = 0;

	virtual void toggle_offline() const = 0;

	virtual void remove() const = 0;

	virtual void reset() const = 0;

	virtual std::string get_guid() const = 0;

	virtual std::string get_chunk() const = 0;

	virtual bool set_chunk(std::string chunk) const = 0;

	virtual bool operator==(const IFXTarget& other) const = 0;

	virtual bool operator!=(const IFXTarget& other) const = 0;

	virtual operator bool() const = 0;

	virtual bool operator!() const = 0;

	virtual std::string get_key() const = 0;

	virtual bool is_valid() const = 0;

	virtual void copy(IFXContext* dest, int dest_fx_idx = 0)const = 0;

	virtual void move(IFXContext* dest, int dest_fx_idx = 0) const = 0;

	virtual ~IFXTarget() = default;

	virtual IFXContext* get_context() const { return m_ctx.get(); };

	// static std::unique_ptr<IFXTarget> get_target(int tr_idx, int it_idx, int tk_idx, int fx_idx, int param_idx); // param not used yet
};


class FX : public IFXTarget {
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

	virtual bool set_chunk(std::string chunk) const override;

	virtual bool operator==(const IFXTarget& other) const override;

	virtual bool operator!=(const IFXTarget& other) const override;

	virtual bool operator!() const override;

	virtual operator bool() const override;

	virtual std::string get_key() const override;

	virtual bool is_valid() const override;

	virtual void copy(IFXContext* dest, int dest_fx_idx = 0) const override;

	virtual void move(IFXContext* dest, int dest_fx_idx = 0)const override;

	virtual std::string get_config_param(std::string par_name)const;

	virtual bool set_config_param(std::string par_name, std::string par_value)const;
	virtual int get_fx_idx()const { return m_fx_idx; };

	class Preset {
	public:
		Preset(std::string preset = "", std::string fx_name = "") : m_preset(std::move(preset)), m_fx_name(std::move(fx_name)) {}
		std::string get_fx_name() const;

		static FX::Preset get(const FX* fx);

		static bool set(const FX* fx, const FX::Preset& preset);

	protected:
		std::string m_fx_name;
		std::string m_preset;
	};

	virtual FX::Preset get_preset()const { return FX::Preset::get(this); }
	virtual bool set_preset(FX::Preset& preset) const { return FX::Preset::set(this, preset); }



	virtual ~FX() override = default;
};


class FXChain : public IFXTarget {
	friend class FXObserver;
	friend class FX;

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

	virtual bool set_chunk(std::string chunk) const override;

	virtual bool operator==(const IFXTarget&) const override;

	virtual bool operator!=(const IFXTarget&) const override;

	virtual bool operator!() const override;

	virtual operator bool() const override;

	virtual std::string get_key() const override;

	virtual bool is_valid() const override;

	virtual FX get_fx(int fx_idx) const;

	virtual int get_count()const;

	virtual int add_fx_by_name(std::string name)const;

	virtual int get_fx_by_guid(std::string p_guid)const;

	virtual void copy(IFXContext* dest, int dest_fx_idx = 0)const override;

	virtual void move(IFXContext* dest, int dest_fx_idx = 0)const override;

	class Preset {
	public:
		friend class FX::Preset;

		Preset(std::vector<FX::Preset> preset_list = {}) : m_preset_list(std::move(preset_list)) {}

		static FXChain::Preset get(const FXChain* fx_chain);

		static bool set(const FXChain* fx_chain, const FXChain::Preset& preset);

	protected:
		std::vector<FX::Preset> m_preset_list;
	};

	virtual FXChain::Preset get_preset() const { return FXChain::Preset::get(this); }
	virtual bool set_preset(FXChain::Preset& preset) const { return FXChain::Preset::set(this, preset); }

	virtual ~FXChain() override = default;
};
