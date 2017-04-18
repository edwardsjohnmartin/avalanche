#ifndef XMATH_H
#define XMATH_H

#include <cstdlib>

namespace xlib {
	float fclamp(float x, float min, float max) {
		if (x < min) x = min;
		else if (x > max) x = max;
		return x;
	}

	float frand() {
		return float(rand() % 2048) / 2047.0f;
	}

	template <class T> T clamp(const T &val, const T &min, const T &max) {
		if (val < min) return min;
		if (val > max) return max;
		return val;
	}
}

#endif