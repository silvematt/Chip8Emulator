#pragma once

#include <string>

#include "Input.h"
#include "../Chip/Chip.h"

namespace Chip8Emulator
{
// ===============================================
// Emulator Environment
// ===============================================
class Emulator
{

public:
	static Emulator& Instance()
	{
		static Emulator instance;
		return instance;
	}

	struct Settings
	{
		uint16_t displayScale;
		uint32_t instructionsPerSeconds;

		Settings()
		{
			displayScale = 20;
			instructionsPerSeconds = 700;
		}
	};

private:
	bool		m_isRunning = false;
	Settings	m_settings;

	Input		m_input;
	Chip		m_chip;

public:
	Emulator() : m_isRunning(false)
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

	Settings& GetSettings()
	{
		return m_settings;
	}
};
}
