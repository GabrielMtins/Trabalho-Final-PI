#include "App.hpp"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

App::App(void) {
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);

	float main_scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);

	window = SDL_CreateWindow(
			"App",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			(int)(main_scale * WINDOW_WIDTH),
			(int)(main_scale * WINDOW_HEIGHT),
			SDL_WINDOW_RESIZABLE
			);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);
}

void App::run(void) {
	while(!quit) {
		loop();
	}
}

void App::loop(void) {
	SDL_Event event;
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	while(SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);

		if(event.type == SDL_QUIT) {
			quit = true;
		}
	}

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	renderWindows();

	ImGui::Render();

	SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
	SDL_RenderClear(renderer);

	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

	SDL_RenderPresent(renderer);
}

void App::renderWindows(void) {
	renderGeneratorWindow();
	renderStepWindow();

	//if(!view3d.isFinished()) {
		view3d.renderChunk(
				renderer,
				generator.getCanvas(),
				heightmap
				);
	//}
}

void App::renderGeneratorWindow(void) {
	ImGui::Begin("Gerador", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Escolha um modo de geração:");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImVec2 button_size(120.0f, 0.0f);

	if(ImGui::Button("Ilha", button_size)) {
		mode = Generator::MODE_ISLAND;
		mode_name_str = "Ilha";
	}

	ImGui::SameLine();

	if(ImGui::Button("Montanhoso", button_size)) {
		mode = Generator::MODE_MOUNTAIN;
		mode_name_str = "Montanhoso";
	}

	ImGui::SameLine();

	if(ImGui::Button("Planície", button_size)) {
		mode = Generator::MODE_PLAINS;
		mode_name_str = "Planície";
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Text("Tipo escolhido: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.3f, 1.0f, 1.0f, 1.0f), mode_name_str.c_str());

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Separator();

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Text("Insira uma seed de geração:");
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::InputInt("##genseed", &gen_seed, 1);

	if(ImGui::Button("Gerar seed aleatória")) {
		gen_seed = rand();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Separator();

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	if(ImGui::Button("Gerar heightmap")) {
		generator.setMode(mode);
		generator.setSeed(gen_seed);
		generator.resetGeneration();
		generator.render(renderer, heightmap);
		view3d.resetView();
	}

	ImGui::End();
}

void App::renderStepWindow(void) {
	ImGui::SetNextWindowSizeConstraints(
			ImVec2(0, 800),
			ImVec2(FLT_MAX, 800)
			);
	ImGui::Begin("Passos da Geração", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

	if(generator.steps.empty()) {
		ImGui::TextWrapped("Renderize alguma coisa para você ver as etapas!");
		ImGui::Dummy(ImVec2(400, 400));
		ImGui::End();
		return;
	}

	if(current_step >= generator.steps.size()) current_step = generator.steps.size() - 1;

	//auto& step = generator.steps.at(current_step);

	ImGui::Image(
			//(ImTextureID) step.texture,
			//(ImTextureID) heightmap.texture,
			(ImTextureID) view3d.texture,
			ImVec2(1280, 720)
			);

	//ImGui::TextWrapped("Descrição da etapa: %s", step.description.c_str());

	float buttonAreaHeight = ImGui::GetFrameHeightWithSpacing();
	ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

	// Posiciona no fundo
	ImGui::SetCursorPosY(contentMax.y - buttonAreaHeight);


	if(ImGui::Button("Anterior") && current_step != 0) {
		current_step--;
	}

	ImGui::SameLine();
	
	if(ImGui::Button("Próximo")) {
		current_step++;
	}

	ImGui::End();
}

App::~App(void) {
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit();
}
