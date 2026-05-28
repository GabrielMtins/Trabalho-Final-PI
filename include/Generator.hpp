#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "Canvas.hpp"
#include <string>
#include <SDL2/SDL.h>
#include <memory>

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
		const Canvas& getCanvas(void) const;

		std::vector<std::unique_ptr<StepGen>> steps;

	private:
		void generateMountain(SDL_Renderer *renderer);
		void generateIsland(SDL_Renderer *renderer);
		void pushStep(SDL_Renderer *renderer, const std::string& msg);

		Mode mode = MODE_MOUNTAIN;

		int seed = 0;

		Canvas canvas;

		std::array<SDL_Color, MAX_HEIGHT> height_colors;
};

#endif
