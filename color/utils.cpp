#include "api.h"
#include "utils.h"
#include <algorithm>
#include <cmath>


HSV rgb_to_hsv(short R, short G, short B) {
	short mx = (std::max)({R, G, B});
	short mn = (std::min)({R, G, B});
	short d = static_cast<short>(mx - mn);

	HSV out;
	out.v = static_cast<float>(mx) * (1.0f / 255.0f);
	out.s = (mx != 0) ? static_cast<float>(d) / static_cast<float>(mx) : 0.0f;

	if (d == 0) {
		out.h = 0.0f;
		return out;
	}

	short hh;
	if (mx == R) hh = static_cast<short>(60 * (G - B) / d);
	else if (mx == G) hh = static_cast<short>(120 + 60 * (B - R) / d);
	else hh = static_cast<short>(240 + 60 * (R - G) / d);

	if (hh < 0) hh = static_cast<short>(hh + 360);
	out.h = static_cast<float>(hh);
	return out;
}

HSV rgb_to_hsv(const RGB &in) {
	return rgb_to_hsv(in.r, in.g, in.b);
}

// ---------- HSV -> RGB ----------
RGB hsv_to_rgb(float h, float s, float v) {
	h = std::fmod(h, 360.0f);
	if (h < 0.0f) h += 360.0f;

	float c = v * s;
	float hPrime = h * (1.0f / 60.0f);
	float x = c * (1.0f - std::fabs(std::fmod(hPrime, 2.0f) - 1.0f));

	float r1, g1, b1;
	switch (static_cast<int>(hPrime)) {
		case 0: r1 = c;
			g1 = x;
			b1 = 0;
			break;
		case 1: r1 = x;
			g1 = c;
			b1 = 0;
			break;
		case 2: r1 = 0;
			g1 = c;
			b1 = x;
			break;
		case 3: r1 = 0;
			g1 = x;
			b1 = c;
			break;
		case 4: r1 = x;
			g1 = 0;
			b1 = c;
			break;
		default: r1 = c;
			g1 = 0;
			b1 = x;
			break;
	}

	float m = v - c;
	auto to8 = [](float f) -> short {
		int i = static_cast<int>(f * 255.0f + 0.5f);
		return static_cast<short>(i < 0 ? 0 : (i > 255 ? 255 : i));
	};

	return RGB{to8(r1 + m), to8(g1 + m), to8(b1 + m)};
}

RGB hsv_to_rgb(const HSV &in) {
	RGB out = hsv_to_rgb(in.h, in.s, in.v);
	return out;
}
