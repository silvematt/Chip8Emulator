#include "Emulator.h"

#include <SDL.h>
#include <fstream>

#ifdef _WIN32
#include <windows.h>        // SetProcessDPIAware()
#endif

#include <nfd.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "Windows/SettingsWindow.h"

namespace Chip8Emulator
{
int Emulator::Init()
{
#ifdef _WIN32
	::SetProcessDPIAware();
#endif
	// Init SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init Error: %s\n", SDL_GetError());
		return -1;
	}

	if (m_input.Init() != 0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "InputInit Error!");
		return -2;
	}

	NFD_Init();

	srand(time(NULL));

	// Init Windows
	float main_scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);

	m_curScreenSize.x = (int)(SCREEN_WIDTH * main_scale);
	m_curScreenSize.y = (int)(SCREEN_HEIGHT * main_scale);

	uint32_t winFlags = SDL_WINDOW_RESIZABLE;
	//uint32_t winFlags = SDL_WINDOW_FULLSCREEN;
	m_window = SDL_CreateWindow("Chip8Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_curScreenSize.x, m_curScreenSize.y, winFlags);

	Uint32 renFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	m_renderer = SDL_CreateRenderer(m_window, -1, renFlags);

	if (!m_window || !m_renderer)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create Window/Renderer!");
		return -3;
	}

	// Initialize the Draw Color to black
	SDL_SetRenderDrawColor(m_renderer, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
	ImGui_ImplSDLRenderer2_Init(m_renderer);

	return 0;
}

int Emulator::Start()
{
	m_chip.Startup(m_renderer, m_emuSettings);
	m_isRunning = true;
	return 0;
}

void Emulator::Update()
{
	uint32_t lastUpdate = SDL_GetTicks();

	while (m_isRunning)
	{
		uint32_t current = SDL_GetTicks();
		double deltaTime = (current - lastUpdate) / 1000.0f;
		lastUpdate = current;

		//Handle events
		m_input.Handle();

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		SDL_RenderClear(m_renderer);

		// Run Chip
		if ((!m_debuggerWin.m_oneInstructionAtTime || m_debuggerWin.m_doOneInstruction) && !m_debuggerWin.m_pauseChipExecution && m_chip.m_isRunning)
		{
			m_chip.Cycle(deltaTime);
			m_debuggerWin.m_doOneInstruction = false;
		}

		m_chip.m_display.Update();

		// Draw Windows
		m_menuWin.Draw();
		m_settingsWin.Draw();
		m_viewportWin.Draw();
		m_debuggerWin.Draw();
		m_chipStatusWindow.Draw();
		m_memoryMapWin.Draw();

		// Render Imgui
		ImGui::Render();
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);

		SDL_RenderPresent(m_renderer);
	}

	Shutdown();
}

void Emulator::Stop()
{
	SDL_Log("Stopping the Emulator...");
	m_isRunning = false;
}

void Emulator::Shutdown()
{
	SDL_Log("Shutting down the Emulator...");
	m_chip.m_isRunning = false;

	// Cleanup
	m_chip.m_display.Destroy();

	// Cleanup
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);

	NFD_Quit();
	SDL_Quit();
}

void Emulator::DirectLoadRom(const std::string& path)
{
	// Load Rom and place seek at the end (ios::ate)
	std::ifstream rom(path, std::ios_base::binary | std::ios::ate);
	if (rom)
	{
		// Get size
		std::streamsize size = rom.tellg();

		// Move back to beginning and read directly in memory
		rom.seekg(0, std::ios::beg);
		rom.read(reinterpret_cast<char*>(&m_chip.m_memory.memory[m_chip.pc]), size);

		SDL_Log("ROM: '%s' loaded.", path.data());
		m_chip.m_isRunning = true;
	}
	else
		SDL_Log("Could not load ROM: '%s'", path.data());
}

void Emulator::ResetChip()
{
	m_chip.Startup(m_renderer, m_emuSettings);
}

}
