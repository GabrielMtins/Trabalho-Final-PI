#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include "Canvas.hpp"
#include "Vec3.hpp"


class Heightmap {
	public:
		Heightmap(void);
		~Heightmap(void);

		/* Cria o colormap e salva na textura do objeto. */
		void makeColormap(SDL_Renderer *renderer, const Canvas& canvas);

		SDL_Color getColor(int i, int j) const;
		/* Interpolação de cores, principalmente utilizada na
		 * renderização 3d. */
		SDL_Color getColorInterpolated(float i, float j) const;

		/* Cria uma superfície de SDL2 para o heightmap. */
		SDL_Surface * makeSurface(void) const;

		SDL_Texture *texture = NULL;

		Vec3 position;
		float rot_x, rot_y;

	private:
		/* Computa a cor do pixel junto com shading. */
		SDL_Color computePixel(const Canvas& canvas, int i, int j) const;
		/* Computa o shading baseado na normal de cada parte do heightmap.
		 * A normal é calculada a partir da variação do terreno. */
		float computeShading(const Canvas& canvas, int i, int j) const;
		/* Faz a multiplicação da cor pelo índice de shading. */
		SDL_Color computeColorWithShading(const SDL_Color& color, float shade) const;
		/* Interpola linearmente entre duas cores. */
		SDL_Color mixColor(const SDL_Color& a, const SDL_Color& b, float inter) const;

		std::vector<SDL_Color> colormap;
		int w = 0, h = 0;
};


#endif
