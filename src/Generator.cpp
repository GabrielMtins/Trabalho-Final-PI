#include "Generator.hpp"
#include "Filters.hpp"

Generator::Generator(void) {
	canvas = Canvas(256, 256, 256.0f);
	steps.reserve(16);

	for(int i = 0; i < MAX_HEIGHT; i++) {
		uint8_t actual = uint8_t(i);
		SDL_Color new_color = {actual, actual, actual, 255};
		height_colors.at(i) = new_color;
	}
}

void Generator::setMode(Mode mode) {
	this->mode = mode;
}

void Generator::render(SDL_Renderer *renderer) {
	switch(mode) {
		case MODE_MOUNTAIN:
			generateMountain(renderer);
			break;

		default:
			break;
	}
}

void Generator::resetGeneration(void) {
	steps.clear();
}

void Generator::setSeed(int seed) {
	this->seed = seed;
}

void Generator::generateMountain(SDL_Renderer *renderer) {
	canvas.randomNoise(seed);

	steps.emplace_back(
			canvas.toTexture(renderer, height_colors),
			"Adicionado ruído aleatório."
			);

	for(int i = 0; i < 12; i++) {
		canvas.applyToAllPixels(Filter::gaussianBlur<7, 1.5f>);
	}

	steps.emplace_back(
			canvas.toTexture(renderer, height_colors),
			"Adicionado filtro passa-baixa (convolução gaussiana com tamanho 7 e sigma 1.5f) 8 vezes."
			);

	auto my_histo = canvas.getNormalizedHistogram();

	auto histogramEq = [my_histo](const Canvas& canvas, int i, int j) {
		return Filter::applyTransform(canvas, i, j, my_histo);
	};

	canvas.applyToAllPixels(histogramEq);

	steps.emplace_back(
			canvas.toTexture(renderer, height_colors),
			"Feito equalização de histograma para distribuir as cores."
			);

	for(int i = 0; i < 4; i++) {
		canvas.applyToAllPixels(Filter::gaussianBlur<7, 1.5f>);
	}

	steps.emplace_back(
			canvas.toTexture(renderer, height_colors),
			"Adicionado filtro passa-baixa gaussiano para suavizar as diferenças de altura criada pela equalização do histograma."
			);
}
