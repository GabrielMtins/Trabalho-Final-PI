#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <array>
#include <vector>
#include <SDL2/SDL.h>

#define MAX_HEIGHT 256

class Canvas {
	public:
		Canvas(void);
		Canvas(int w, int h, float outside_value = 0.0f);

		/* Retorna o pixel na posição (i, j).
		 * Caso (i, j) não esteja dentro das dimensões, a função
		 * retorna outside_value (valor de padding). */
		float getPixel(int i, int j) const;

		/* Função com interpolação bilinear. */
		float getPixelInterpolated(float i, float j) const;
		void setPixel(int i, int j, float value);

		/* Popula a imagem com ruído aleatório a partir de uma seed. */
		void randomNoise(unsigned int seed);

		/* Retornam as dimensões da imagem. */
		int getWidth(void) const;
		int getHeight(void) const;

		/* Retorna os valores máximos e mínimos, usado para normalização. */
		float getMaxValue(void) const;
		float getMinValue(void) const;

		/* Define o valor de "fora", ou seja, o valor que a função getPixel() retorna
		 * caso (i, j) não esteja dentro do range correto. Muito utilizado na convolução
		 * por ser um padding. */
		void setOutsideValue(float outside_value);

		SDL_Surface * toSurface(const std::array<SDL_Color, MAX_HEIGHT>& height_to_color) const;
		SDL_Texture * toTexture(SDL_Renderer *renderer, const std::array<SDL_Color, MAX_HEIGHT>& height_to_color) const;

		/* Computa e retorna o histograma normalizado. */
		std::array<float, MAX_HEIGHT> getNormalizedHistogram(void) const;

		/* Função para aplicar uma transformação de cores para
		 * cada pixel da imagem. Foi paralelizada com OpenMP por questões de
		 * velocidade, mas caso não haja OpenMP, o programa ignora e roda
		 * o código sequencialmente. */
		template<typename Functor>
		void applyToAllPixels(Functor func_color) {
			int i, j;
			Canvas other(w, h, outside_value);

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
		float outside_value;
};


#endif
