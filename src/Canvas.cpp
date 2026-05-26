#include "Canvas.hpp"
#include "SDL_surface.h"

#include <cstdint>
#include <cstdlib>

Canvas::Canvas(void) : w(0), h(0), outside_value(0.0f) {
}

Canvas::Canvas(int w, int h, float outside_value) : w(w), h(h), outside_value(outside_value) {
	data.resize(w * h);
}

float Canvas::getPixel(int i, int j) const {
	if(i < 0 || j < 0 || i >= w || j >= h) {
		return outside_value;
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

int Canvas::getWidth(void) const {
	return w;
}

int Canvas::getHeight(void) const {
	return h;
}

float Canvas::getMaxValue(void) const {
	float value = 0.0f;

	for(float i : data) {
		if(i > value) value = i;
	}

	return value;
}

float Canvas::getMinValue(void) const {
	float value = MAX_HEIGHT;

	for(float i : data) {
		if(i < value) value = i;
	}

	return value;
}

void Canvas::setOutsideValue(float outside_value) {
	this->outside_value = outside_value;
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

SDL_Texture * Canvas::toTexture(SDL_Renderer *renderer, const std::array<SDL_Color, MAX_HEIGHT>& height_to_color) const {
	SDL_Surface *surface;
	SDL_Texture *texture;

	surface = toSurface(height_to_color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);

	return texture;
}

std::array<float, MAX_HEIGHT> Canvas::getNormalizedHistogram(void) const {
	std::array<float, MAX_HEIGHT> histogram{};
	float sum = 0.0f;

	for(float i : data) {
		histogram.at(i)++;
		sum += 1.0f;
	}

	for(int i = 1; i < MAX_HEIGHT; i++) {
		histogram[i] += histogram[i - 1];
	}

	for(float& i : histogram) {
		i = 255.0f * i / sum;
	}

	return histogram;
}
