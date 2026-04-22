#include "Chip.h"

namespace Chip8Emulator
{
void Chip::Startup(uint8_t displayScaleFactor, uint16_t instructionsPerSeconds)
{
	m_instructionsPerSecond = instructionsPerSeconds;

	m_memory.InitMemory();
	m_display.InitDisplay(displayScaleFactor);
	m_delayTimer = 60;
	m_isRunning = true;
}

void Chip::Cycle(double deltaTime)
{
	// Update Timers
	m_cyclesAcculumator += deltaTime;
	m_timerAccumulator += deltaTime;

	double cycleThreshold = (1.0 / m_instructionsPerSecond);
	if (m_cyclesAcculumator >= cycleThreshold)
	{
		m_cyclesAcculumator -= cycleThreshold;

		// Perform a cycle
		uint16_t opcode = Fetch();
		if (!DecodeExecute(opcode))
		{
			SDL_Log("Critical Error! OpCode [%d] not implemented?", opcode);
		}
	}

	// Update timers if needed, these are updated regardless of the Cycle speed
	while (m_timerAccumulator >= TIMERS_UPDATE_RATE)
	{
		if (m_delayTimer > 0)
			m_delayTimer--;

		if (m_soundTimer > 0)
			m_soundTimer--;

		m_timerAccumulator -= TIMERS_UPDATE_RATE;
	}
}

uint16_t Chip::Fetch()
{
	uint16_t opcode = (m_memory.memory[pc] << 8) | m_memory.memory[pc + 1];
	pc += 2;
	return opcode;
}

bool Chip::DecodeExecute(uint16_t opcode)
{
	// Extract components using mask
	uint8_t  op		= (0xF000 & opcode) >> 12;
	uint8_t  x		= (0x0F00 & opcode) >> 8;
	uint8_t  y		= (0x00F0 & opcode) >> 4;
	uint8_t  n		= (0x000F & opcode);
	uint8_t  nn		= (0x00FF & opcode);
	uint16_t nnn	= (0x0FFF & opcode);

	switch (op)
	{
		case 0x0:
			if (nnn == 0x0E0)
			{
				Op_ClearScreen();
				return true;
			}
			break;

		case 0x1:
			Op_JumpTo(nnn);
			return true;

		case 0x6:
			Op_SetRegister(x, nn);
			return true;

		case 0x7:
			Op_AddToRegister(x, nn);
			return true;

		case 0xA:
			Op_SetIndexReg(nnn);
			return true;

		case 0xD:
			Op_DrawToScreen(x, y, n);
			return true;
	}

	return false;
}

void Chip::Op_ClearScreen()
{
	m_display.ClearScreen();
}

void Chip::Op_JumpTo(uint16_t nnn)
{
	pc = nnn;
}

void Chip::Op_SetRegister(uint8_t x, uint8_t nn)
{
	m_vx[x] = nn;
}

void Chip::Op_AddToRegister(uint8_t x, uint8_t nn)
{
	m_vx[x] += nn;
}

void Chip::Op_SetIndexReg(uint16_t nnn)
{
	ir = nnn;
}

void Chip::Op_DrawToScreen(uint8_t x, uint8_t y, uint8_t n)
{
	// Draw
	int px = m_vx[x] % 64;
	int py = m_vx[y] % 32;

	// Init collision flag
	m_vx[0xF] = 0;
	bool collOff = false;
	
	for (int j = 0; j < n; j++)
	{
		uint8_t sprite = m_memory.memory[ir + j];

		for (int i = 0; i < 8; i++)
		{
			//0x80 is 10000000
			if (sprite & (0x80 >> i))
			{
				bool coll = m_display.FlipPixel(px+i, py+j);

				if (!collOff && coll)
					collOff = true;
			}
		}
	}

	if (collOff)
		m_vx[0xF] = 1;

	m_display.Update();
}

}
