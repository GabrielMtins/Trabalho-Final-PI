#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "Canvas.hpp"
#include <string>
#include <SDL2/SDL.h>
#include <memory>

/* Estrutura que armazena cada passo de geração, utilizada para demonstrar
 * cada passo com sua textura correspondente junto com sua descrição. */
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
		/* Modos de geração */
		enum Mode {
			MODE_ISLAND,
			MODE_MOUNTAIN,
			MODE_PLAINS
		};

		Generator(void);
		void setMode(Mode mode);

		/* Renderiza para as SDL_Texture e adiciona-as na lista de passos de geração. */
		void render(SDL_Renderer *renderer);

		/* Reseta a geração (limpa as texturas e passos anteriores). */
		void resetGeneration(void);

		/* Define a seed de geração. */
		void setSeed(int seed);

		/* Retorna o canvas como const, utilizado para gerar heightmap etc. */
		const Canvas& getCanvas(void) const;

		/* Retorna o estado da geração final como SDL_Surface * para pixels */
		SDL_Surface * makeSurface(void) const;

		std::vector<std::unique_ptr<StepGen>> steps;

	private:
		/* Geração dos três tipos de terreno. */
		void generateMountain(SDL_Renderer *renderer);
		void generateIsland(SDL_Renderer *renderer);
		void generatePlains(SDL_Renderer *renderer);

		/* Adiciona um passo de renderização, transformando o Canvas atual em uma SDL_Texture *
		 * e adicionando no vetor de steps junto com a sua respectiva descrição. */
		void pushStep(SDL_Renderer *renderer, const std::string& msg);

		Mode mode = MODE_MOUNTAIN;

		int seed = 0;

		Canvas canvas;

		std::array<SDL_Color, MAX_HEIGHT> height_colors;
};

#endif
