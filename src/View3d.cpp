#include "View3d.hpp"
#include "Heightmap.hpp"
#include "Canvas.hpp"

#define MAX_STEP_SIZE 0.5f
#define MAX_NUM_STEPS 1000
#define HEIGHT_SCALE 0.3f

static const SDL_Color sky_color = {0, 200, 255, 255};

View3d::View3d(void) {
	surface = SDL_CreateRGBSurface(
			0,
			VIEW_WIDTH,
			VIEW_HEIGHT,
			32,
			0, 0, 0, 0
			);

	camera = Vec3(128.0f, 128.0f, 0.0f);
	current_chunk = VIEW_NUM_CHUNKS;

	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xff, 0xff, 0x00));
}

void View3d::resetView(void) {
	current_chunk = 0;
}

void View3d::renderChunk(SDL_Renderer *renderer, const Canvas& canvas, const Heightmap& heightmap) {
	int i, j;

	if(current_chunk == VIEW_NUM_CHUNKS) {
		if(texture != NULL) SDL_DestroyTexture(texture);

		texture = SDL_CreateTextureFromSurface(renderer, surface);

		return;
	}

	#pragma omp parallel for collapse(2) private(i) schedule(dynamic)
	for(j = 0; j < VIEW_CHUNK_SIZE; j++) {
		for(i = 0; i < VIEW_WIDTH; i++) {
			uint32_t *pixels = (uint32_t *) surface->pixels;
			int to_index = i + (j + current_chunk * VIEW_CHUNK_SIZE) * VIEW_WIDTH;
			SDL_Color c = processPixel(canvas, heightmap, i, j + current_chunk * VIEW_CHUNK_SIZE);

			pixels[to_index] = SDL_MapRGBA(
					surface->format,
					c.r, c.g, c.b, c.a
					);
		}
	}

	current_chunk++;
}

SDL_Color View3d::processPixel(const Canvas& canvas, const Heightmap& heightmap, int i, int j) const {
	Vec3 dir = Vec3(
		float(i - VIEW_WIDTH / 2) / float(VIEW_HEIGHT),
		-float(j - VIEW_HEIGHT / 2) / float(VIEW_HEIGHT),
		0.3f
	).normalized();

	/*
	float ang = -0.1f;

	dir.rotateX(cosf(ang), sinf(ang));
	*/

	dir *= MAX_STEP_SIZE;

	Vec3 intersection = findIntersection(canvas, dir);

	if(intersection.y < -1.0f) {
		return sky_color;
	}

	return heightmap.getColorInterpolated(
			(intersection.x),
			(intersection.z)
			);
}

Vec3 View3d::findIntersection(const Canvas& canvas, const Vec3& dir) const {
	Vec3 pos = camera;

	for(int i = 0; i < MAX_NUM_STEPS; i++) {
		int x, z;

		pos += dir;

		x = int(pos.x);
		z = int(pos.z);

		float h = canvas.getPixel(x, z) * HEIGHT_SCALE;

		if(x < 0 || z < 0 || x >= canvas.getWidth() || z >= canvas.getHeight()) {
			break;
		}

		if(pos.y < h) {
			return Vec3(pos.x, h, pos.z);
		}
	}

	pos.y = -10.0f;

	return pos;
}

View3d::~View3d(void) {
	if(surface != NULL) {
		SDL_FreeSurface(surface);
	}

	if(texture != NULL) {
		SDL_DestroyTexture(texture);
	}
}
