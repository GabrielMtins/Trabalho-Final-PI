#include "Canvas.hpp"
#include "SDL_surface.h"

#include <cstdint>
#include <cstdlib>

Canvas::Canvas(void) : w(0), h(0) {
}

Canvas::Canvas(int w, int h) : w(w), h(h) {
	data.resize(w * h);
}

float Canvas::getPixel(int i, int j) const {
	if(i < 0 || j < 0 || i >= w || j >= h) {
		return 0.0f;
	}

	return data.at(i + j * w);
}

void Canvas::setPixel(int i, int j, float value) {
	if(i < 0 || j < 0 || i >= w || j >= h) {
		return;
	}

	data.at(i + j * w) = value;
}

void Canvas::randomNoise(unsigned int seed) {
	srand(seed);

	for(int j = 0; j < h; j++) {
		for(int i = 0; i < w; i++) {
			setPixel(i, j, float(rand() % 256));
		}
	}
}

SDL_Surface * Canvas::toSurface(const std::array<SDL_Color, MAX_HEIGHT>& height_to_color) const {
	SDL_Surface *surface;
	uint32_t *pixels;

	if(data.empty()) {
		return NULL;
	}

	surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
	pixels = (uint32_t *) surface->pixels;

	for(int i = 0; i < w * h; i++) {
		int to_index = (int(data.at(i))) % MAX_HEIGHT;
		SDL_Color color = height_to_color.at(to_index);

		pixels[i] = SDL_MapRGB(surface->format, color.r, color.g, color.b);
	}

	return surface;
}
