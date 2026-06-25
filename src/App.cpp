#include "App.hpp"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define RENDER_COOLDOWN 4

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

	current_tick = 0;
	next_tick_render = 0;

	srand(time(NULL));

	gen_seed = rand();
	gen_count = rand() % 100;
}

void App::run(void) {
	while(!quit) {
		loop();
	}
}

void App::loop(void) {
	SDL_Event event;
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	current_tick = SDL_GetTicks();

	while(SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);

		if(event.type == SDL_QUIT) {
			quit = true;
		}
	}

	if(current_tick > next_tick_render) {
		view3d.renderChunk(
				renderer,
				generator.getCanvas(),
				heightmap
				);

		next_tick_render = current_tick + RENDER_COOLDOWN;
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
	renderHeightmapWindow();
	renderView3d();
}

void App::renderGeneratorWindow(void) {
	ImGui::SetNextWindowPos(
			ImVec2(10, 10),
			ImGuiCond_FirstUseEver
			);

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

	if(ImGui::Button("Lagos", button_size)) {
		mode = Generator::MODE_PLAINS;
		mode_name_str = "Lagos";
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Text("Tipo escolhido: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.3f, 1.0f, 1.0f, 1.0f), "%s", mode_name_str.c_str());

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Separator();

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Text("Insira uma seed de geração:");
	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::InputInt("##genseed", &gen_seed, 1);

	if(ImGui::Button("Gerar seed aleatória")) {
		gen_seed = rand() + gen_count;
		gen_count++;
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Separator();

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	if(ImGui::Button("Gerar heightmap")) {
		generator.setMode(mode);
		generator.setSeed(gen_seed);
		generator.resetGeneration();
		current_step = 0;

		generator.render(renderer);
		heightmap.makeColormap(renderer, generator.getCanvas());
		setCameraPosition();

		view3d.resetView();
	}

	ImGui::SameLine();

	ImGui::SetNextItemWidth(-1);

	if(ImGui::Button("Salvar heightmap")) {
		if(save()) {
			show_modal_save_popup = true;
		}
	}

	if(show_modal_save_popup) {
		SDL_ShowSimpleMessageBox(0, "Salvo", "Os arquivos heightmap.png, colormap.png e view3d.png foram salvos!", window);
		show_modal_save_popup = false;
	}

	ImGui::End();
}

void App::renderStepWindow(void) {
	ImGui::SetNextWindowPos(
			ImVec2(ImGui::GetMainViewport()->Size.x - 500, 10),
			ImGuiCond_FirstUseEver
			);

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

	StepGen *step = generator.steps.at(current_step).get();

	ImGui::Image(
			(ImTextureID) step->texture,
			ImVec2(400, 400)
			);

	ImGui::TextWrapped("Descrição da etapa: %s", step->description.c_str());

	float buttonAreaHeight = ImGui::GetFrameHeightWithSpacing();
	ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

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

void App::renderHeightmapWindow(void) {
	ImGui::SetNextWindowPos(
			ImVec2(450, 10),
			ImGuiCond_FirstUseEver
			);

	ImGui::Begin("Heightmaps gerados", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

	if(generator.steps.empty()) {
		ImGui::Dummy(ImVec2(800, 400));
		ImGui::End();
		return;
	}

	StepGen *step = generator.steps.back().get();

	ImGui::Image(
			(ImTextureID) step->texture,
			ImVec2(400, 400)
			);

	ImGui::SameLine();

	ImGui::Image(
			(ImTextureID) heightmap.texture,
			ImVec2(400, 400)
			);

	ImGui::End();
}

void App::renderView3d(void) {
	ImGui::SetNextWindowPos(
			ImVec2(50, ImGui::GetMainViewport()->Size.y - 500),
			ImGuiCond_FirstUseEver
			);

	ImGui::Begin("Visualização 3d", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

	if(view3d.texture == NULL) {
		ImGui::Dummy(ImVec2(640, 360));
	} else {
		ImGui::Image(
				(ImTextureID) view3d.texture,
				ImVec2(640, 360)
				);
	}

	static const float my_tau = 3.1415f * 2.0f;

	ImGui::Text("Rotação: ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);

	if(ImGui::SliderFloat("##camera_rotation", &camera_rotation, 0.0f, my_tau)) {
		if(!generator.steps.empty()) {
			setCameraPosition();
			view3d.resetView();
		}
	}

	ImGui::End();
}

void App::setCameraPosition(void) {
	switch(mode) {
		case Generator::MODE_MOUNTAIN:
			view3d.setCameraPosRotatedFromCenter(
					Vec3(128.0f, 128.0f, 128.0f),
					camera_rotation,
					128.0f
					);

			view3d.rot_x = 0.7f;
			break;

		case Generator::MODE_ISLAND:
			view3d.setCameraPosRotatedFromCenter(
					Vec3(128.0f, 64.0f, 128.0f),
					camera_rotation,
					128.0f
					);

			view3d.rot_x = 0.5f;

			break;

		case Generator::MODE_PLAINS:
			view3d.setCameraPosRotatedFromCenter(
					Vec3(128.0f, 48.0f, 128.0f),
					camera_rotation,
					128.0f
					);

			view3d.rot_x = 0.7f;
			break;
	}
}

bool App::save(void) const {
	SDL_Surface *heightpng, *colorpng, *viewpng;

	if(generator.steps.empty()) {
		return false;
	}

	viewpng = view3d.getSurface();
	heightpng = generator.makeSurface();
	colorpng = heightmap.makeSurface();

	IMG_SavePNG(heightpng, "heightmap.png");
	IMG_SavePNG(colorpng, "colormap.png");
	IMG_SavePNG(viewpng, "view3d.png");

	SDL_FreeSurface(heightpng);
	SDL_FreeSurface(colorpng);

	return true;
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
