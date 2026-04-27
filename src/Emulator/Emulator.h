#pragma once

#include <string>
#include <random>

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

private:
	bool		m_isRunning = false;
	Settings	m_emuSettings;

	Input		m_input;
	Chip		m_chip;

public:
	Emulator() : m_isRunning(false), m_chip(m_input)
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
		return m_emuSettings;
	}
};
}
