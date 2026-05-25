#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "Canvas.hpp"

namespace Filter {
	template<int size>
	float averageBlur(const Canvas& canvas, int i, int j) {
		static constexpr int radius = size / 2;
		static constexpr float inverse_average_sqr = 1.0f / float(size * size);
		float res = 0.0f;

		for(int y = -radius; y <= radius; y++) {
			for(int x = -radius; x <= radius; x++) {
				res += inverse_average_sqr * canvas.getPixel(i + x, j + y);
			}
		}

		return res;
	}

	template<int size, float sigma>
	float gaussianBlur(const Canvas& canvas, int i, int j) {
		static constexpr int radius = size / 2;

		static constexpr auto kernel1d = []() constexpr {
			std::array<float, size> k{};
			float sum = 0.0f;

			for(int x = -radius; x <= radius; x++) {
				k[x + radius] = std::exp(-(x * x) / (2.0f * sigma * sigma));
				sum += k[x + radius];
			}

			for(auto& weight : k) {
				weight /= sum;
			}

			return k;
		}();

		float temp = 0.0f, res = 0.0f;

		for(int y = -radius; y <= radius; y++) {
			temp = 0.0f;

			for(int x = -radius; x <= radius; x++) {
				temp += kernel1d[x + radius] * canvas.getPixel(i + x, j + y);
			}

			res += kernel1d[y + radius] * temp;
		}

		return res;
	}

	float applyLimit(const Canvas& canvas, int i, int j, float lower, float higher);

	float applyTransform(const Canvas& canvas, int i, int j, const std::array<float, MAX_HEIGHT>& palette);
};


#endif
