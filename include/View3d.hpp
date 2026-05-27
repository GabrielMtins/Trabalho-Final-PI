#ifndef VIEW3D_HPP
#define VIEW3D_HPP

#define VIEW_WIDTH 1280
#define VIEW_HEIGHT 720
#define VIEW_NUM_CHUNKS 5
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

		Vec3 camera;
		float rot_x = 0.0f, rot_y = 0.0f;

		SDL_Texture *texture = NULL;
	
	private:
		SDL_Color processPixel(const Canvas& canvas, const Heightmap& heightmap, int i, int j) const;
		Vec3 findIntersection(const Canvas& canvas, const Vec3& dir) const;

		SDL_Surface *surface = NULL;
		int current_chunk = 0;
};

#endif
