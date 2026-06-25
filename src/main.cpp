/*
 * Trabalho disciplina SCC0251 - Processamento de Imagens
 * Realizado pelos alunos:
 * - Hélio Márcio Cabral Santos, N°USP: 14577862
 * - Gabriel Martins Monteiro, N°USP: 14572099
 *
 * O objetivo do trabalho é utilizar fitros típicos de processamento de imagens
 * para gerar heightmaps. O programa também renderiza uma imagem em 3d do heightmap.
 * Os conteúdos da disciplina são principalmente aplicados na seguintes partes:
 *
 * - Canvas: Abstração para uma imagem preto e branco com range de cores entre 0-255.
 *   Utilizada para a geração de heightmaps.
 * - Filters.hpp: Arquivos que contém os filtros utilizados e implementados, como de
 *   máscara por distância, filtro gaussiano e filtro de caixa/média.
 * - Generator: Classe responsável por aplicar os passos de cada filtro, gerando
 *   três possíveis tipos de imagem (ilha, planície/lagos e montanhas).
 * - Heightmap: Classe que pega o Canvas e transforma numa textura de heightmap colorida
 *   com os 
 *
 * As demais classes se resumem:
 * - App: classe que gerencia as janelas (SDL2 e dear imgui).
 * - Vec3: classe básica de vetor de três dimensões utilizada para cálculos matemáticos.
 * - colorange.h: o mapeamento de altura do heightmap (range 0-255) para cores RGB.
 * - View3d: responsável pela renderização 3d.
 *
 * O programa tem as seguintes dependências:
 * - libsdl2-dev
 * - libsdl2-image-dev
 * - dear imgui (já inclusa no repositório).
 * - Compilador de C++ compatível com o padrão C++17 (g++ ou clang++, por exemplo).
 * - cmake e make.
 *
 * Opcionalmente, o CMake detecta se há OpenMP e o utiliza para otimização
 * do programa e acelerar a geração de imagens.
 *
 * Para compilar e executar, basta executar o script:
 * ./run_build.sh
 *
 * Se preferir, pode compilar seguindo os passos manualmente:
 * $ cmake -DCMAKE_BUILD_TYPE=Release -B build
 * $ cd build && make && cd ..
 * Para executar:
 * $ ./build/projeto
 */

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
