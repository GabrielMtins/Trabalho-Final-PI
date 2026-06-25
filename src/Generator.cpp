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

		case MODE_PLAINS:
			generatePlains(renderer);
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

SDL_Surface * Generator::makeSurface(void) const {
	return canvas.toSurface(height_colors);
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
			"Adicionado 12 vezes um filtro de convolução gaussiana de tamanho 15 com sigma 1.5. "
			"Ele atua como um filtro passa-baixa, atenuando as frequências altas da imagem. "
			);

	auto my_histo = canvas.getNormalizedHistogram();

	auto histogramEq = [my_histo](const Canvas& canvas, int i, int j) {
		return Filter::applyTransform(canvas, i, j, my_histo);
	};

	canvas.applyToAllPixels(histogramEq);

	pushStep(
			renderer,
			"A convolução tende a jogar os valores para a média. "
			"Para distribuir os valores, realiza-se uma equalização de histograma."
			);

	for(int i = 0; i < 4; i++) {
		canvas.applyToAllPixels(Filter::gaussianBlur<3, 15>);
	}

	pushStep(
			renderer,
			"Adicionado 4 vezes um filtro de convolução gaussiana de tamanho 3 com sigma 1.5. "
			"A equalização do histograma cria algumas transições abruptas e "
			"o filtro tem o papel de suavizá-las."
			);

	auto increaseHeight = [](const Canvas& canvas, int i, int j) {
		return fminf(canvas.getPixel(i, j) * 0.5f + 128.0f, 255.0f);
	};

	canvas.applyToAllPixels(increaseHeight);

	pushStep(
			renderer,
			"Aqui, aumenta-se o terreno distribuindo as cores para um range entre 128-255."
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
			"Feito uma transformação baseada em uma máscara para aumentar a altura do terreno no centro. "
			"O objetivo é que quando o filtro passa-baixa atuar, a parte central sempre irá ficar maior, dando "
			"um aspecto montanhoso no centro."
			);

	for(int i = 0; i < 8; i++) {
		canvas.applyToAllPixels(Filter::averageBlur<9>);
	}

	pushStep(
			renderer,
			"Adicionado 8 vezes um filtro de caixa de tamanho 9. "
			"Ele atua como um filtro passa-baixa, atenuando as frequências altas da imagem. "
			);

	auto my_histo = canvas.getNormalizedHistogram();

	auto histogramEq = [my_histo](const Canvas& canvas, int i, int j) {
		return Filter::applyTransform(canvas, i, j, my_histo);
	};

	canvas.applyToAllPixels(histogramEq);

	pushStep(
			renderer,
			"A convolução tende a jogar os valores para a média. "
			"Para distribuir os valores, realiza-se uma equalização de histograma."
			);

	auto makeFloor = [](const Canvas& canvas, int i, int j) {
		float res = canvas.getPixel(i, j);

		if(res < 200.0f) return 0.0f;

		return res;
	};

	canvas.applyToAllPixels(makeFloor);

	pushStep(
			renderer,
			"Criado depressões na imagem ao zerar os valores abaixo de 200."
			);

	for(int i = 0; i < 8; i++) {
		canvas.applyToAllPixels(Filter::averageBlur<7>);
	}

	pushStep(
			renderer,
			"Adicionado 8 vezes um filtro de caixa de tamanho 7. "
			"Ele atua como um filtro passa-baixa, atenuando as frequências altas da imagem "
			"que foram criadas por conta do corte abrupto."
			);

	canvas.applyToAllPixels(Filter::distanceMask);
	
	pushStep(
			renderer,
			"Adicionado uma máscara de distância que atenua os valores de acordo "
			"com a distância do pixel ao centro da imagem."
			);
}

void Generator::generatePlains(SDL_Renderer *renderer) {
	canvas.randomNoise(seed);
	canvas.setOutsideValue(255.0f);

	pushStep(
			renderer,
			"Adicionado ruído aleatório."
			);

	for(int i = 0; i < 8; i++) {
		canvas.applyToAllPixels(Filter::gaussianBlur<15, 150>);
	}

	pushStep(
			renderer,
			"Adicionado 8 vezes um filtro de convolução gaussiana de tamanho 15 com sigma 15. "
			"Ele atua como um filtro passa-baixa, atenuando as frequências altas da imagem. "
			"O sigma alto deixa a imagem mais suave que é o objetivo."
			);

	auto my_histo = canvas.getNormalizedHistogram();

	auto histogramEq = [my_histo](const Canvas& canvas, int i, int j) {
		return Filter::applyTransform(canvas, i, j, my_histo);
	};

	canvas.applyToAllPixels(histogramEq);

	pushStep(
			renderer,
			"A convolução tende a jogar os valores para a média. "
			"Para distribuir os valores, realiza-se uma equalização de histograma."
			);

	auto makeHeight = [](const Canvas& canvas, int i, int j) {
		float res = canvas.getPixel(i, j);

		if(res < 100.0f) return 0.0f;

		return float(MAX_HEIGHT - 1);
	};

	canvas.applyToAllPixels(makeHeight);

	pushStep(
			renderer,
			"Criado depressões na imagem ao zerar os valores abaixo de 100."
			);

	for(int i = 0; i < 4; i++) {
		canvas.applyToAllPixels(Filter::gaussianBlur<5, 15>);
	}

	pushStep(
			renderer,
			"Adicionado 4 vezes um filtro de convolução gaussiana de tamanho 3 com sigma 5. "
			"Ele atua como um filtro passa-baixa, atenuando as frequências altas da imagem "
			"que foram criadas devido ao corte abrupto de valores."
			);

	auto adjustHeight = [](const Canvas& canvas, int i, int j) {
		return fminf(canvas.getPixel(i, j) * 0.15f + 0.0f * 255.0f, 255.0f);
	};

	canvas.applyToAllPixels(adjustHeight);

	pushStep(
			renderer,
			"Redistribuídoas cores no range 0-38 para se ajustar no heightmap."
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
