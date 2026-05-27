#ifndef APP_HPP
#define APP_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Canvas.hpp"
#include "Generator.hpp"
#include "Heightmap.hpp"
#include "View3d.hpp"

class App {
	public:
		App(void);
		~App(void);

		void run(void);

	private:
		void loop(void);
		void renderWindows(void);
		void renderGeneratorWindow(void);
		void renderStepWindow(void);

		bool quit = false;

		SDL_Window *window;
		SDL_Renderer *renderer;

		Generator generator;
		Heightmap heightmap;
		View3d view3d;

		size_t current_step;
		int gen_seed;

		Generator::Mode mode = Generator::MODE_MOUNTAIN;
		std::string mode_name_str = "Montanhoso";
};

#endif
