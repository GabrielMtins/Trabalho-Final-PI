#ifndef HEIGHTMAP_HPP
#define HEIGHTMAP_HPP

#include "Canvas.hpp"
#include "Vec3.hpp"


class Heightmap {
	public:
		Heightmap(void);
		~Heightmap(void);

		void makeColormap(SDL_Renderer *renderer, const Canvas& canvas);
		SDL_Color getColor(int i, int j) const;
		SDL_Color getColorInterpolated(float i, float j) const;

		SDL_Texture *texture = NULL;

		Vec3 position;
		float rot_x, rot_y;

	private:
		SDL_Surface * makeSurface(const Canvas& canvas) const;
		SDL_Color computePixel(const Canvas& canvas, int i, int j) const;
		float computeShading(const Canvas& canvas, int i, int j) const;
		SDL_Color computeColorWithShading(const SDL_Color& color, float shade) const;
		SDL_Color mixColor(const SDL_Color& a, const SDL_Color& b, float inter) const;

		std::vector<SDL_Color> colormap;
		int w = 0, h = 0;
};


#endif
