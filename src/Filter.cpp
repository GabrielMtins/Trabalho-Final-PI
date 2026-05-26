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

	float distanceMask(const Canvas& canvas, int i, int j) {
		float dx = float(i) / float(canvas.getWidth());
		float dy = float(j) / float(canvas.getHeight());

		dx *= 2.0f;
		dy *= 2.0f;

		dx -= 1.0f;
		dy -= 1.0f;

		float distance = dx * dx + dy * dy;

		distance *= 1.5f;

		if(distance > 1.0f) {
			return 0.0f;
		}

		return canvas.getPixel(i, j) * (1.0f - distance);
	}
}

