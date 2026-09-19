#pragma once

struct TrackEvent {
	enum Type { SelectionChanged = 1, LastTouched = 2 } type;


	MediaTrack* track;
	int state = 0;
};


void unsolo_all(ReaProject* proj = nullptr);

int get_cycled_index(int idx, int offset, int total);

int get_solo_ip_settings();


class ITrackSoloMode {
	public:
	virtual ~ITrackSoloMode() = default;

	virtual void OnActivate() = 0;

	virtual void OnDeactivate() = 0;

	virtual bool OnTrackEvent(const TrackEvent &event) = 0;
};


class AutoSoloModeOff: virtual public ITrackSoloMode {
	public:
	void OnActivate() override {}

	void OnDeactivate() override {}

	bool OnTrackEvent(const TrackEvent &event) override { return false; }
};


class AutoSoloModeLastTouch: virtual public ITrackSoloMode {
	public:
	void OnActivate() override;

	void OnDeactivate() override;

	bool OnTrackEvent(const TrackEvent &event) override;
};


class AutoSoloModeSelected: virtual public ITrackSoloMode {
	public:
	void OnActivate() override;

	void OnDeactivate() override;

	bool OnTrackEvent(const TrackEvent &event) override;
};


class AutoSoloModeLastTouchSel: virtual public ITrackSoloMode {
	public:
	void OnActivate() override;

	void OnDeactivate() override;

	bool OnTrackEvent(const TrackEvent &event) override;
};
