#include "Emulator.h"

#include <SDL.h>
#include <fstream>


namespace Chip8Emulator
{
int Emulator::Init()
{
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

	return 0;
}

int Emulator::Start()
{
	m_chip.Startup(m_settings.displayScale, m_settings.instructionsPerSeconds);
	DirectLoadRom("IBMLogo.ch8");
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

		// Run Chip
		m_chip.Cycle(deltaTime);

		SDL_Delay(1);
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
	}
	else
		SDL_Log("Could not load ROM: '%s'", path.data());
}

}
