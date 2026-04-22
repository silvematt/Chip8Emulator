#pragma once

#include <cstring>
#include <stdint.h>
#include <stack>

#include "SDL.h"

#include "Components/Memory.h"
#include "Components/Display.h"

namespace Chip8Emulator
{
	constexpr double TIMERS_UPDATE_RATE = 0.0166666666; // 1/60, Timers are decreased by 1 60 times per seconds

class Chip
{
public:
	Chip()
	{
		m_timerAccumulator = 0.0;
	}

// Status
	bool m_isRunning = false;
	
	//-	Operational Settings
	//	Values existing in both places:
	//	Emulator::Settings::instructionsPerSeconds is the configuration, owned by the Emulator and where the user can edit
	//	Chip::m_instructionsPerSecond is the operational state, used at runtime inside Cycle
	//
	//	We may have cases where we don't want to change operational state from the Emulator settings unless at a sync point, so we'll have
	//	Chip::ApplySettings(const Emulator::Settings&) so the emulator can push updates at controlled moments
	uint16_t m_instructionsPerSecond = 700;

	// Accumulators
	double m_timerAccumulator = 0.0;
	double m_cyclesAcculumator = 0.0;
	
// Components:
	Memory	m_memory;
	Display m_display;

	// PC - Program Counter
	uint16_t pc = 0x200;

	// IR - Index Register
	uint16_t ir = 0x0;

	// Stack
	std::stack<uint16_t> m_stack;

	// Delay Timer
	uint8_t m_delayTimer = 0;

	// Sound Timer
	uint8_t m_soundTimer = 0;

	// Variable Registers
	uint8_t m_vx[16];

// Methods
public:
	void		Startup(uint8_t displayScaleFactor, uint16_t instructionsPerSeconds);


	void		Cycle(double deltaTime);
	uint16_t	Fetch();
	bool		DecodeExecute(uint16_t opcode);

	// Operations
	void		Op_ClearScreen();
	void		Op_JumpTo(uint16_t nnn);
	void		Op_SetRegister(uint8_t x, uint8_t nn);
	void		Op_AddToRegister(uint8_t x, uint8_t nn);
	void		Op_SetIndexReg(uint16_t nnn);
	void		Op_DrawToScreen(uint8_t x, uint8_t y, uint8_t n);
};
}
