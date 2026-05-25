#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <array>
#include <vector>
#include <SDL2/SDL.h>

#define MAX_HEIGHT 256

class Canvas {
	public:
		Canvas(void);
		Canvas(int w, int h);

		float getPixel(int i, int j) const;
		void setPixel(int i, int j, float value);
		void randomNoise(unsigned int seed);

		int getWidth(void) const;
		int getHeight(void) const;

		SDL_Surface * toSurface(const std::array<SDL_Color, MAX_HEIGHT>& height_to_color) const;

		std::array<float, MAX_HEIGHT> getNormalizedHistogram(void) const;

		template<typename Functor>
		void applyToAllPixels(Functor func_color) {
			int i, j;
			Canvas other(w, h);

			#pragma omp parallel for collapse(2) private(i)
			for(j = 0; j < h; j++) {
				for(i = 0; i < w; i++) {
					other.setPixel(
							i, j,
							func_color(*this, i, j)
							);
				}
			}

			*this = other;
		}

	private:
		std::vector<float> data;
		int w, h;
};


#endif
