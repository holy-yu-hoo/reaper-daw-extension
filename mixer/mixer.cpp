#include "api.h"
#include "mixer.h"
#include <thread>
#include <atomic>
#include <chrono>

#define AUTO_HIDE_SECONDS 3.0
#define SCROLL_HOLD_SECONDS 0.3

static bool mixer_is_visible() {
	return GetToggleCommandState(40078) == 1;
}

// mixer|track variables
static std::thread* g_watcher_thread = nullptr;
static std::atomic<bool> g_stop_requested(false);
static std::atomic<MediaTrack*> g_current_track(nullptr);

// scroll variables
static std::atomic<MediaTrack*> g_scroll_target(nullptr);
static std::chrono::steady_clock::time_point g_scroll_start;

static void scroll_to_track() { // force scroll, since single SetMixerScroll not work
	MediaTrack* target = g_scroll_target.load();
	if (!target) {
		plugin_register("-timer", static_cast<void*>(scroll_to_track));
		return;
	}
	SetMixerScroll(target);
	TrackList_AdjustWindows(false);

	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration<double>(now - g_scroll_start).count() >= SCROLL_HOLD_SECONDS) {
		plugin_register("-timer", static_cast<void*>(scroll_to_track));
		g_scroll_target.store(nullptr);
	}
}

static bool is_mixer_focused(HWND mixer_hwnd) {
	if (!mixer_hwnd || !IsWindow(mixer_hwnd)) return false;

	MediaTrack* track = g_current_track.load();
	if (!track) return false;

	int tr = -1, it = -1, tk = -1, fx = -1, pr = -1;
	GetTouchedOrFocusedFX(1, &tr, &it, &tk, &fx, &pr);
	int target_idx = static_cast<int>(GetMediaTrackInfo_Value(track, "IP_TRACKNUMBER")) - 1;

	if (tr == target_idx) return true;

	HWND fg = GetForegroundWindow(); // check focus on window
	if (!fg) return false;

	if (fg == mixer_hwnd || IsChild(mixer_hwnd, fg)) return true;

	if (IsChild(fg, mixer_hwnd)) return true;

	HWND parent = GetParent(fg);
	while (parent) {
		if (parent == mixer_hwnd) return true;
		parent = GetParent(parent);
	}

	return false;
}

static void stop_mixer_watcher() {
	if (g_watcher_thread) {
		g_stop_requested.store(true);
		g_watcher_thread->join();
		delete g_watcher_thread;
		g_watcher_thread = nullptr;
	}
}

static void watcher_loop(HWND mixer_hwnd) {
	auto last_focus_loss = std::chrono::steady_clock::now();
	bool focus_was_lost = false;

	while (!g_stop_requested.load()) {
		MediaTrack* track = g_current_track.load();
		if (!track || !IsWindow(mixer_hwnd)) break;

		if (!is_mixer_focused(mixer_hwnd)) {
			if (!focus_was_lost) {
				last_focus_loss = std::chrono::steady_clock::now();
				focus_was_lost = true;
			} else {
				auto now = std::chrono::steady_clock::now();
				if (std::chrono::duration<double>(now - last_focus_loss).count() >= AUTO_HIDE_SECONDS) {
					Main_OnCommand(40078, 0);
					break;
				}
			}
		} else {
			focus_was_lost = false; // focus returns
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

static void start_mixer_watcher(HWND mixer_hwnd) {
	stop_mixer_watcher();
	g_stop_requested.store(false);
	g_watcher_thread = new std::thread(watcher_loop, mixer_hwnd);
}


void update_mixer_track(MediaTrack* track) { // observe change last touch track and switch state
	if (!track || track == GetMasterTrack(nullptr)) return;
	if (track == g_current_track.load()) return;
	if (!mixer_is_visible()) return;

	g_current_track.store(track);

	g_scroll_target.store(track);
	g_scroll_start = std::chrono::steady_clock::now();
	plugin_register("timer", static_cast<void*>(scroll_to_track));
}

void smart_show_mixer(COMMAND_T* cmd) {
	int context;
	double position;
	MediaTrack* track = BR_TrackAtMouseCursor(&context, &position);
	if (!track or context == 1 or track == GetMasterTrack(nullptr)) return;

	if (track == g_current_track.load() and mixer_is_visible()) {
		Main_OnCommand(40078, 0);
		stop_mixer_watcher();
		g_current_track.store(nullptr);
		return;
	}

	if (!mixer_is_visible()) {
		Main_OnCommand(40078, 0);
		Sleep(50);
	}

	if (cmd->user) SetOnlyTrackSelected(track);
	g_current_track.store(track);
	g_scroll_target.store(track);
	g_scroll_start = std::chrono::steady_clock::now();
	plugin_register("timer", static_cast<void*>(scroll_to_track));

	bool is_dock;
	HWND mixer = static_cast<HWND>(BR_Win32_GetMixerHwnd(&is_dock));
	if (!mixer) return;

	SetForegroundWindow(mixer);
	start_mixer_watcher(mixer);
}

void smart_show_mixer_without_hide(COMMAND_T* cmd) {
	int context;
	double position;
	MediaTrack* track = BR_TrackAtMouseCursor(&context, &position);
	if (!track || context == 1 || track == GetMasterTrack(nullptr)) return;


	if (track == g_current_track.load() && mixer_is_visible()) {
		Main_OnCommand(40078, 0);
		stop_mixer_watcher();
		g_current_track.store(nullptr);
		return;
	}

	// Сбрасываем слежку за фокусом, если она была запущена ранее
	stop_mixer_watcher();

	if (!mixer_is_visible()) {
		Main_OnCommand(40078, 0);
		Sleep(50);
	}

	if (cmd->user) SetOnlyTrackSelected(track);

	g_current_track.store(track);
	g_scroll_target.store(track);
	g_scroll_start = std::chrono::steady_clock::now();
	plugin_register("timer", static_cast<void*>(scroll_to_track));

	bool is_dock;
	HWND mixer = static_cast<HWND>(BR_Win32_GetMixerHwnd(&is_dock));
	if (!mixer) return;

	SetForegroundWindow(mixer);
}
