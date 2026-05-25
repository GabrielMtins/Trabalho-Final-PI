#include <cstdio>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <memory>

#include "Canvas.hpp"
#include "Filters.hpp"
#include "App.hpp"

int main(void) {
	auto app = std::make_unique<App>();

	app->run();

	return 0;
}
