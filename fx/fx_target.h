#pragma once
#include "fx_context.h"


class IFXTarget {
	public:
	virtual bool is_visible() =0;

	virtual void toggle_show() =0;

	virtual void toggle_bypass() =0;

	virtual void toggle_offline() =0;

	virtual void remove() =0;

	virtual void reset() =0;

	virtual std::string get_guid() =0;

	virtual std::string get_chunk() =0;

	virtual ~IFXTarget() = default;

	static std::unique_ptr<IFXTarget> get_target(int tr_idx, int it_idx, int tk_idx, int fx_idx, int param_idx); // param not used yet
};


class FX: public IFXTarget {
	std::shared_ptr<IFXContext> m_ctx;
	int m_fx_idx;

	public:
	FX(int tr_idx, int it_idx, int tk_idx, int fx_idx, int param_idx);

	virtual bool is_visible() override;

	virtual void toggle_show() override;

	virtual void toggle_bypass() override;

	virtual void toggle_offline() override;

	virtual void remove() override;

	virtual void reset() override;

	virtual std::string get_guid() override;

	virtual std::string get_chunk() override;

	virtual ~FX() override = default;
};


class FXChain: public IFXTarget {
	std::shared_ptr<IFXContext> m_ctx;
	int m_fx_idx;

	public:
	FXChain(int tr_idx, int it_idx, int tk_idx, int fx_idx);

	virtual bool is_visible() override;

	virtual void toggle_show() override;

	virtual void toggle_bypass() override;

	virtual void toggle_offline() override;

	virtual void remove() override;

	virtual void reset() override;

	virtual std::string get_guid() override;

	virtual std::string get_chunk() override;

	virtual ~FXChain() override = default;

};
