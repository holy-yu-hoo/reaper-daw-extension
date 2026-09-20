#include  "api.h"
#include "color.h"
#include <random>
#include "utils.h"

#define add_bound(a,b,c) (a+=b); if (a>=c) (a%=c)
#define step 15
static constexpr bool normalize = true; // Делает получаемые цвета ярче и насыщеннее (и в общем случае красивее)

void set_selected_tracks_to_gradient(COMMAND_T* cmd) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, 359);
	std::uniform_real_distribution<double> dist2(0.7, 1);
	RGB color_rgb;
	HSV color_hsv{dist(gen), dist2(gen), dist2(gen)};
	int cnt_tracks = CountSelectedTracks2(nullptr, false);
	Undo_BeginBlock2(nullptr);
	if (cnt_tracks == 0) {
		cnt_tracks = CountTracks(nullptr);
		for (int i = 0; i < cnt_tracks; i++) {
			add_bound(color_hsv.h, step, 360);
			color_rgb = hsv_to_rgb(color_hsv);
			int c = ColorToNative(color_rgb.r, color_rgb.b, color_rgb.g);
			SetTrackColor(GetTrack(nullptr, i), c);
		}
	} else {
		for (int i = 0; i < cnt_tracks; i++) {
			add_bound(color_hsv.h, step, 360);
			color_rgb = hsv_to_rgb(color_hsv);
			int c = ColorToNative(color_rgb.r, color_rgb.b, color_rgb.g);
			SetTrackColor(GetSelectedTrack2(nullptr, i, false), c);
		}
	}
	if (cnt_tracks > 0) Undo_EndBlock2(nullptr, "Color tracks",UNDO_STATE_TRACKCFG);
	TrackList_AdjustWindows(false);
}
