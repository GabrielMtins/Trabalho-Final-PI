#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "Canvas.hpp"
#include <string>
#include <SDL2/SDL.h>

struct StepGen {
	SDL_Texture *texture;
	std::string description;

	StepGen(void) : texture(NULL), description("") {}

	StepGen(SDL_Texture *texture, const std::string& description) : 
		texture(texture),
		description(description)
	{
	}

	~StepGen(void) {
		if(texture != NULL) {
			SDL_DestroyTexture(texture);
		}
	}
};

class Generator {
	public:
		enum Mode {
			MODE_ISLAND,
			MODE_MOUNTAIN,
			MODE_PLAINS
		};

		Generator(void);
		void setMode(Mode mode);
		void render(SDL_Renderer *renderer);
		void resetGeneration(void);
		void setSeed(int seed);

		std::vector<StepGen> steps;

	private:
		void generateMountain(SDL_Renderer *renderer);

		Mode mode = MODE_MOUNTAIN;

		SDL_Texture *heightmap = NULL;
		SDL_Texture *colormap = NULL;
		int seed = 0;

		Canvas canvas;

		std::array<SDL_Color, MAX_HEIGHT> height_colors;
};

#endif
