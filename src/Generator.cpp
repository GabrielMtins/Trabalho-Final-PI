#include "Generator.hpp"
#include "Filters.hpp"

#include "colorange.h"

Generator::Generator(void) {
	canvas = Canvas(256, 256, 128.0f);
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
	resetGeneration();

	switch(mode) {
		case MODE_MOUNTAIN:
			generateMountain(renderer);
			break;

		case MODE_ISLAND:
			generateIsland(renderer);
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

const Canvas& Generator::getCanvas(void) const {
	return canvas;
}

void Generator::generateMountain(SDL_Renderer *renderer) {
	canvas.randomNoise(seed);
	canvas.setOutsideValue(128.0f);

	pushStep(
			renderer,
			"Adicionado ruído aleatório."
			);

	for(int i = 0; i < 12; i++) {
		canvas.applyToAllPixels(Filter::gaussianBlur<15, 15>);
	}

	pushStep(
			renderer,
			"Adicionado filtro passa-baixa (convolução gaussiana com tamanho 7 e sigma 1.5f) 8 vezes."
			);

	auto my_histo = canvas.getNormalizedHistogram();

	auto histogramEq = [my_histo](const Canvas& canvas, int i, int j) {
		return Filter::applyTransform(canvas, i, j, my_histo);
	};

	canvas.applyToAllPixels(histogramEq);

	pushStep(
			renderer,
			"Feito equalização de histograma para distribuir as cores."
			);

	for(int i = 0; i < 4; i++) {
		canvas.applyToAllPixels(Filter::gaussianBlur<3, 15>);
	}

	pushStep(
			renderer,
			"Adicionado filtro passa-baixa gaussiano para suavizar as diferenças de altura criada pela equalização do histograma."
			);

	auto increaseHeight = [](const Canvas& canvas, int i, int j) {
		return fminf(canvas.getPixel(i, j) * 0.5f + 128.0f, 255.0f);
	};

	canvas.applyToAllPixels(increaseHeight);

	pushStep(
			renderer,
			"Aumentado a altura do terreno artificialmente"
			);
}

void Generator::generateIsland(SDL_Renderer *renderer) {
	canvas.randomNoise(seed);
	canvas.setOutsideValue(0.0f);

	pushStep(
			renderer,
			"Adicionado ruído aleatório."
			);

	auto makeCenterHeight = [](const Canvas& canvas, int i, int j) {
		float res = canvas.getPixel(i, j);

		int center_i = i - canvas.getWidth() / 2;
		int center_j = j - canvas.getHeight() / 2;

		int distance = center_i * center_i + center_j * center_j;
		float part = (1.0f - float(distance) / 1000.0f);

		if(part < 0.0f) part = 0.0f;
		if(part > 1.0f) part = 1.0f;

		res += part * 0.5f * res;

		if(res > MAX_HEIGHT - 1) res = MAX_HEIGHT - 1;

		return res;
	};

	canvas.applyToAllPixels(makeCenterHeight);

	pushStep(
			renderer,
			"Aumentado o valor dos pixels do centro para criar região de ilha."
			);

	for(int i = 0; i < 8; i++) {
		canvas.applyToAllPixels(Filter::averageBlur<9>);
	}

	pushStep(
			renderer,
			"Aplicado filtro passa-baixa (convolução de média com kernel de tamanho 7)."
			);

	auto my_histo = canvas.getNormalizedHistogram();

	auto histogramEq = [my_histo](const Canvas& canvas, int i, int j) {
		return Filter::applyTransform(canvas, i, j, my_histo);
	};

	canvas.applyToAllPixels(histogramEq);

	pushStep(
			renderer,
			"Feito equalização de histograma para distribuir as cores."
			);

	auto makeFloor = [](const Canvas& canvas, int i, int j) {
		float res = canvas.getPixel(i, j);

		if(res < 200.0f) return 0.0f;

		return res;
	};

	canvas.applyToAllPixels(makeFloor);

	pushStep(
			renderer,
			"Criado depressões na imagem ao zerar os valores abaixo de 128."
			);

	for(int i = 0; i < 8; i++) {
		canvas.applyToAllPixels(Filter::averageBlur<7>);
	}

	pushStep(
			renderer,
			"Adicionado filtro passa-baixa para suavizar as montanhas."
			);

	canvas.applyToAllPixels(Filter::distanceMask);
	
	pushStep(
			renderer,
			"Adicionado máscara de distância."
			);
}

void Generator::pushStep(SDL_Renderer *renderer, const std::string& msg) {
	steps.push_back(
			std::make_unique<StepGen>(
				canvas.toTexture(renderer, height_colors),
				msg
				)
			);
}
