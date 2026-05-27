#include "Heightmap.hpp"

#include "colorange.h"

#include "Vec3.hpp"

Heightmap::Heightmap(void) {
}

Heightmap::~Heightmap(void) {
	if(texture != NULL) {
		SDL_DestroyTexture(texture);
	}
}

void Heightmap::makeColormap(SDL_Renderer *renderer, const Canvas& canvas) {
	int i, j;
	colormap = std::vector<SDL_Color>(canvas.getWidth() * canvas.getHeight());
	w = canvas.getWidth();
	h = canvas.getHeight();

	#pragma omp parallel for collapse(2) private(i)
	for(j = 0; j < canvas.getHeight(); j++) {
		for(i = 0; i < canvas.getWidth(); i++) {
			colormap[i + j * canvas.getWidth()] = computePixel(canvas, i, j);
		}
	}

	SDL_Surface *surface = makeSurface(canvas);

	if(texture != NULL) {
		SDL_DestroyTexture(texture);
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FreeSurface(surface);
}

SDL_Color Heightmap::getColor(int i, int j) const {
	if(i < 0) i = 0;
	if(j < 0) j = 0;
	if(i >= w) i = w - 1;
	if(j >= h) j = h - 1;

	return colormap.at(i + j * w);
}

SDL_Color Heightmap::getColorInterpolated(float i, float j) const {
	float part_i = i - floorf(i);
	float part_j = j - floorf(j);

	SDL_Color a, b, c, d;

	a = getColor(i, j);
	b = getColor(i + 1, j);

	c = getColor(i, j + 1);
	d = getColor(i + 1, j + 1);

	return mixColor(
			mixColor(a, b, part_i),
			mixColor(c, d, part_i),
			part_j
			);
}

SDL_Surface * Heightmap::makeSurface(const Canvas& canvas) const {
	SDL_Surface *surface = SDL_CreateRGBSurface(
			0,
			canvas.getWidth(),
			canvas.getHeight(),
			32,
			0, 0, 0, 0
			);

	uint32_t *pixels = (uint32_t *) surface->pixels;

	for(size_t i = 0; i < colormap.size(); i++) {
		SDL_Color c = colormap.at(i);

		pixels[i] = SDL_MapRGBA(
				surface->format,
				c.r, c.g, c.b, c.a
				);
	}

	return surface;
}

SDL_Color Heightmap::computePixel(const Canvas& canvas, int i, int j) const {
	int to_index = int(canvas.getPixel(i, j)) % MAX_HEIGHT;
	float shading = computeShading(canvas, i, j);
	SDL_Color color = height_to_colormap[to_index];

	return computeColorWithShading(
			color,
			shading
			);
}

float Heightmap::computeShading(const Canvas& canvas, int i, int j) const {
	static constexpr float delta = 1.0f;
	static const Vec3 light_dir = Vec3(-1.0f, -1.0f, 0.3f).normalized();

	float current_height = canvas.getPixel(i, j);

	Vec3 dx = Vec3(
			delta,
			canvas.getPixel(i + 1, j) - current_height,
			0.0f
		   );


	Vec3 dy = Vec3(
			0.0f,
			canvas.getPixel(i, j + 1) - current_height,
			delta
		   );

	Vec3 normal = dx.cross(dy).normalized();

	float shading = fmaxf(normal.dot(light_dir), 0.0f) * 0.5f + 0.5f;

	if(shading < 0.0f) return 0.0f;
	if(shading > 1.0f) return 1.0f;

	return shading;
}

SDL_Color Heightmap::computeColorWithShading(const SDL_Color& color, float shade) const {
	int r, g, b, a;

	r = int(shade * float(color.r));
	g = int(shade * float(color.g));
	b = int(shade * float(color.b));
	a = int(shade * float(color.a));

	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
	if(a > 255) a = 255;

	return SDL_Color{uint8_t(r), uint8_t(g), uint8_t(b), uint8_t(a)};
}

SDL_Color Heightmap::mixColor(const SDL_Color& c1, const SDL_Color& c2, float inter) const {
	float p1 = 1.0f - inter;
	float p2 = inter;

	int r, g, b, a;

	r = int(p1 * float(c1.r) + p2 * float(c2.r));
	g = int(p1 * float(c1.g) + p2 * float(c2.g));
	b = int(p1 * float(c1.b) + p2 * float(c2.b));
	a = int(p1 * float(c1.a) + p2 * float(c2.a));

	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
	if(a > 255) a = 255;

	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;
	if(a < 0) a = 0;

	return SDL_Color{uint8_t(r), uint8_t(g), uint8_t(b), uint8_t(a)};
}
