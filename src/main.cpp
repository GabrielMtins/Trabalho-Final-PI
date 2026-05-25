#include <cstdio>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Canvas.hpp"
#include "Filters.hpp"

int main(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);

	std::array<SDL_Color, MAX_HEIGHT> colors;
	Canvas canvas(256, 256);

	for(int i = 0; i < MAX_HEIGHT; i++) {
		uint8_t actual = uint8_t(i);
		SDL_Color new_color = {actual, actual, actual, 255};
		colors.at(i) = new_color;
	}

	canvas.randomNoise(0);

	//canvas.applyToAllPixels(Filter::gaussianBlur<7, 1.5f>);
	//canvas.applyToAllPixels(Filter::averageBlur<7>);

	for(int i = 0; i < 16; i++) {
		//canvas.applyToAllPixels(Filter::averageBlur<7>);
		canvas.applyToAllPixels(Filter::gaussianBlur<15, 1.5f>);
	}

	auto limit = [](const Canvas& canvas, int i, int j) {
		return Filter::applyLimit(canvas, i, j, 128.0f, 134.0f);
	};

	//canvas.applyToAllPixels(limit);

	/*
	for(int i = 0; i < 4; i++) {
		//canvas.applyToAllPixels(Filter::averageBlur<7>);
		canvas.applyToAllPixels(Filter::gaussianBlur<7, 1.5f>);
	}
	*/

	auto my_histo = canvas.getNormalizedHistogram();

	auto histogramEq = [my_histo](const Canvas& canvas, int i, int j) {
		return Filter::applyTransform(canvas, i, j, my_histo);
	};

	auto afterFinal = [](const Canvas& canvas, int i, int j) {
		return canvas.getPixel(i, j) / 2 + 127.0f;
	};

	canvas.applyToAllPixels(histogramEq);
	canvas.applyToAllPixels(Filter::gaussianBlur<15, 1.5f>);
	//canvas.applyToAllPixels(afterFinal);

	SDL_Surface *surface = canvas.toSurface(colors);

	IMG_SavePNG(surface, "test.png");

	SDL_FreeSurface(surface);

	IMG_Quit();
	SDL_Quit();

	return 0;
}
