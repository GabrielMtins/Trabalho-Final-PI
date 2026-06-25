#ifndef VIEW3D_HPP
#define VIEW3D_HPP

/*
 * A visualização divide a imagem em chunks, isso serve para não dar um "engasgo" e nem
 * consumir 100% da CPU de uma vez quando a imagem 3d é renderizada. */

#define VIEW_WIDTH 640
#define VIEW_HEIGHT 360
#define VIEW_NUM_CHUNKS 10
#define VIEW_CHUNK_SIZE (VIEW_HEIGHT / VIEW_NUM_CHUNKS)

#include "Vec3.hpp"

#include <SDL2/SDL.h>

class Canvas;
class Heightmap;

class View3d {
	public:
		View3d(void);
		~View3d(void);

		void renderChunk(SDL_Renderer *renderer, const Canvas& canvas, const Heightmap& heightmap);
		void resetView(void);
		void setCameraPosRotatedFromCenter(const Vec3& center, float rotation, float distance);
		SDL_Surface * getSurface(void) const;

		Vec3 camera;
		float rot_x = 0.0f, rot_y = 0.0f;

		SDL_Texture *texture = NULL;
	
	private:
		/* Processa cada cor de pixel individualmente */
		SDL_Color processPixel(const Canvas& canvas, const Heightmap& heightmap, int i, int j) const;
		/* Encontra e retorna a intersecção da semirreta com o canvas */
		Vec3 findIntersection(const Canvas& canvas, const Vec3& dir) const;

		SDL_Surface *surface = NULL;
		int current_chunk = 0;

		float cos_rot_x, sin_rot_x;
		float cos_rot_y, sin_rot_y;
};

#endif
