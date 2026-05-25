#include "Filters.hpp"

namespace Filter {
	float applyLimit(const Canvas& canvas, int i, int j, float lower, float higher) {
		float res = 0.0f;

		res = canvas.getPixel(i, j);

		if(res < lower) res = 0.0f;
		if(res > higher) res = MAX_HEIGHT - 1;

		return res;
	}

	float applyTransform(const Canvas& canvas, int i, int j, const std::array<float, MAX_HEIGHT>& palette) {
		float res = canvas.getPixel(i, j);
		int to_index = int(res) % MAX_HEIGHT;

		return palette.at(to_index);
	}
}

