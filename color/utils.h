#pragma once


struct RGB {
	short r, g, b;
};


struct HSV {
	int h;
	double s;
	double v;
}; // h: [0..360), c,v: [0..1]

HSV rgb_to_hsv(const RGB &in);

HSV rgb_to_hsv(short R, short G, short B);

RGB hsv_to_rgb(float h, float s, float v);

RGB hsv_to_rgb(const HSV &in);
