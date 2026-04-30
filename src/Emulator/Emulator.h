#pragma once

#include <string>
#include <random>

#include "Input.h"
#include "../Chip/Chip.h"

#include "Windows/MenuBarWindow.h"
#include "Windows/SettingsWindow.h"


namespace Chip8Emulator
{
// ===============================================
// Emulator Environment
// ===============================================
class Emulator
{
	static constexpr int SCREEN_WIDTH = 1920;
	static constexpr int SCREEN_HEIGHT = 1080;

	static inline const SDL_Color colorBlack = { 0, 0, 0, SDL_ALPHA_OPAQUE };
	static inline const SDL_Color colorWhite = { 255, 255, 255, SDL_ALPHA_OPAQUE };

public:
	static Emulator& Instance()
	{
		static Emulator instance;
		return instance;
	}

private:
	bool		m_isRunning = false;
	Settings	m_emuSettings;

	Input		m_input;
	Chip		m_chip;

	SDL_Window*		m_window = nullptr;
	SDL_Renderer*	m_renderer = nullptr;

	// Imgui Windows
	MenuBarWindow	m_menuWin;
	SettingsWindow	m_settingsWin;

public:
	Emulator() : m_isRunning(false), m_chip(m_input), m_settingsWin(m_emuSettings)
	{
	}

	// ================================================
	// Initializes Emulator environment and libraries
	// ================================================
	int Init();

	// ================================================
	// Starts the Emulator and chip
	// ================================================
	int Start();

	// ================================================
	// Emulator's Update
	// ================================================
	void Update();

	// ================================================
	// Stops the Emulator, forcing it to Shutdown()
	// ================================================
	void Stop();

	// ================================================
	// Shuts the Emulator down, stopping its components
	// ================================================
	void Shutdown();

	// ================================================
	// Loads a ROM directly into memory
	// ================================================
	void DirectLoadRom(const std::string& path);

	const Settings& GetSettings()
	{
		return m_emuSettings;
	}
};
}
