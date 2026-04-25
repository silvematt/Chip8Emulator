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
	double m_cyclesAccumulator = 0.0;
	
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
	void		Startup(uint8_t displayScaleFactor, uint16_t instructionsPerSecond);


	void		Cycle(double deltaTime);
	uint16_t	Fetch();
	bool		DecodeExecute(uint16_t opcode);

	// Operations
				
	void		Op_ClearScreen();										// 00E0
	void		Op_JumpTo(uint16_t nnn);								// 1NNN 
	void		Op_SetRegister(uint8_t x, uint8_t nn);					// 6XNN 
	void		Op_AddToRegister(uint8_t x, uint8_t nn);				// 7XNN 
	void		Op_SetIndexReg(uint16_t nnn);							// ANNN 
	void		Op_DrawToScreen(uint8_t x, uint8_t y, uint8_t n);		// DXYN 

	void		Op_CallSubrtn(uint16_t nnn);							// 2NNN
	void		Op_ReturnFromSubrtn();									// 00EE

	void		Op_SkipNnCondIfEqual(uint8_t x, uint8_t nn);			// 3XNN 
	void		Op_SkipNnCondIfNotEqual(uint8_t x, uint8_t nn);			// 4XNN

	void		Op_SkipXyCondIfEqual(uint8_t x, uint8_t y);				// 5XY0
	void		Op_SkipXyCondIfNotEqual(uint8_t x, uint8_t y);			// 9XY0

	// LA = LogicArithmetic
	void		Op_LASet(uint8_t x, uint8_t y);							// 8XY0
	void		Op_LABinaryOr(uint8_t x, uint8_t y);					// 8XY1
	void		Op_LABinaryAnd(uint8_t x, uint8_t y);					// 8XY2
	void		Op_LALogicalXor(uint8_t x, uint8_t y);					// 8XY3
	void		Op_LAAdd(uint8_t x, uint8_t y);							// 8XY4
	void		Op_LASubtractY(uint8_t x, uint8_t y);					// 8XY5 
	void		Op_LASubtractX(uint8_t x, uint8_t y);					// 8XY7 
};
}
