#pragma once

#include <cstring>
#include <stdint.h>
#include <stack>

#include "SDL.h"

#include "../Emulator/Settings.h"
#include "Components/Memory.h"
#include "Components/Display.h"
#include "Components/Keypad.h"

#include "../Emulator/Input.h"

namespace Chip8Emulator
{
	constexpr double TIMERS_UPDATE_RATE = 0.0166666666; // 1/60, Timers are decreased by 1 60 times per seconds

class Chip
{
public:
	Chip(Input& i) : m_inputRef(i)
	{
		m_timerAccumulator = 0.0;
	}

// Status
	bool m_isRunning = false;

	//-	Operational Settings
	//	Values existing in both places:
	//	Emulator::m_emusSettings.instructionsPerSecond is the configuration, owned by the Emulator and where the user can edit
	//	Chip::m_operationalSettings.instructionsPerSecond is the operational state, used at runtime inside Cycle
	//
	//	We may have cases where we don't want to change operational state from the Emulator settings unless at a sync point, so we'll have
	//	Chip::ApplySettings(const Emulator::Settings&) so the emulator can push updates at controlled moments
	Settings m_operationalSettings;

	// Accumulators
	double m_timerAccumulator = 0.0;
	double m_cyclesAccumulator = 0.0;
	
// Components:
	Memory	m_memory;
	Display m_display;
	Keypad	m_keypad;

	Input&	m_inputRef;

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
	uint8_t m_vx[16] = { 0 };

// Methods
public:
	void		Startup(SDL_Renderer* renderer, Settings emuSettings);

	void		ApplySettings(Settings newSettings);

	void		Cycle(double deltaTime);
	uint16_t	Fetch();
	bool		DecodeExecute(uint16_t opcode);

	// Operations
				
	void		Op_ClearScreen();											// 00E0
	void		Op_JumpTo(uint16_t nnn);									// 1NNN 
	void		Op_JumpToWithOffset(uint8_t x, uint8_t nn, uint16_t nnn);	// BNNN 
	void		Op_SetRegister(uint8_t x, uint8_t nn);						// 6XNN 
	void		Op_AddToRegister(uint8_t x, uint8_t nn);					// 7XNN 
	void		Op_SetIndexReg(uint16_t nnn);								// ANNN 
	void		Op_DrawToScreen(uint8_t x, uint8_t y, uint8_t n);			// DXYN 

	void		Op_CallSubrtn(uint16_t nnn);								// 2NNN
	void		Op_ReturnFromSubrtn();										// 00EE

	void		Op_SkipNnCondIfEqual(uint8_t x, uint8_t nn);				// 3XNN 
	void		Op_SkipNnCondIfNotEqual(uint8_t x, uint8_t nn);				// 4XNN

	void		Op_SkipXyCondIfEqual(uint8_t x, uint8_t y);					// 5XY0
	void		Op_SkipXyCondIfNotEqual(uint8_t x, uint8_t y);				// 9XY0

	// LA = LogicArithmetic
	void		Op_LASet(uint8_t x, uint8_t y);								// 8XY0
	void		Op_LABinaryOr(uint8_t x, uint8_t y);						// 8XY1
	void		Op_LABinaryAnd(uint8_t x, uint8_t y);						// 8XY2
	void		Op_LALogicalXor(uint8_t x, uint8_t y);						// 8XY3
	void		Op_LAAdd(uint8_t x, uint8_t y);								// 8XY4
	void		Op_LASubtractY(uint8_t x, uint8_t y);						// 8XY5 
	void		Op_LASubtractX(uint8_t x, uint8_t y);						// 8XY7 
	void		Op_LAShiftRight(uint8_t x, uint8_t y);						// 8XY6
	void		Op_LAShiftLeft(uint8_t x, uint8_t y);						// 8XYE

	void		Op_Random(uint8_t x, uint8_t nn);							// CXNN

	void		Op_SkipIfKeyPressed(uint8_t x);								// EX9E
	void		Op_SkipIfKeyNotPressed(uint8_t x);							// EXA1 

	void		Op_SetVxToDelayTimerValue(uint8_t x);						// FX07 
	void		Op_SetDelayTimer(uint8_t x);								// FX15 
	void		Op_SetSoundTimer(uint8_t x);								// FX18 

	void		Op_AddToIndex(uint8_t x);									// FX1E

	void		Op_GetKey(uint8_t x);										// FX0A
	void		Op_FontCharacter(uint8_t x);								// FX29
	void		Op_BinaryCodedDecimalConversion(uint8_t x);					// FX33
	void		Op_StoreIntoMemory(uint8_t x);								// FX55 
	void		Op_LoadFromMemory(uint8_t x);								// FX65 
};
}
