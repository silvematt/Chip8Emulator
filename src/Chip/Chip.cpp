#include "Chip.h"

namespace Chip8Emulator
{
void Chip::Startup(uint8_t displayScaleFactor, uint16_t instructionsPerSecond)
{
	m_instructionsPerSecond = instructionsPerSecond;

	m_memory.InitMemory();
	pc = 0x200;
	m_display.InitDisplay(displayScaleFactor);
	m_delayTimer = 0;
	m_soundTimer = 0;
	m_isRunning = true;
}

void Chip::Cycle(double deltaTime)
{
	// Update Timers
	m_cyclesAccumulator += deltaTime;
	m_timerAccumulator += deltaTime;

	double cycleThreshold = (1.0 / m_instructionsPerSecond);
	while (m_cyclesAccumulator >= cycleThreshold)
	{
		m_cyclesAccumulator -= cycleThreshold;

		// Perform a cycle
		uint16_t opcode = Fetch();
		if (!DecodeExecute(opcode))
		{
			SDL_Log("Critical Error! OpCode [0x%04X] not implemented?", opcode);
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

			if (nnn == 0x0EE)
			{
				Op_ReturnFromSubrtn();
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

		case 0x2:
			Op_CallSubrtn(nnn);
			return true;

		case 0x3:
			Op_SkipNnCondIfEqual(x, nn);
			return true;

		case 0x4:
			Op_SkipNnCondIfNotEqual(x, nn);
			return true;

		case 0x5:
			Op_SkipXyCondIfEqual(x, y);
			return true;

		case 0x9:
			Op_SkipXyCondIfNotEqual(x, y);
			return true;

		case 0x8:
			if (n == 0x0)
				Op_LASet(x, y);

			else if (n == 0x1)
				Op_LABinaryOr(x, y);

			else if (n == 0x2)
				Op_LABinaryAnd(x, y);

			else if (n == 0x3)
				Op_LALogicalXor(x, y);

			else if (n == 0x4)
				Op_LAAdd(x, y);

			else if (n == 0x5)
				Op_LASubtractY(x, y);

			else if (n == 0x7)
				Op_LASubtractX(x, y);
			else
				return false;

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

void Chip::Op_CallSubrtn(uint16_t nnn)
{
	m_stack.push(pc);
	pc = nnn;
}

void Chip::Op_ReturnFromSubrtn()
{
	if (!m_stack.empty())
	{
		pc = m_stack.top();
		m_stack.pop();
	}
	else
		SDL_Log("Stack underflow during [Op_ReturnFromSubrtn]");
}

void Chip::Op_SkipNnCondIfEqual(uint8_t x, uint8_t nn)
{
	if (m_vx[x] == nn)
		pc += 2;
}

void Chip::Op_SkipNnCondIfNotEqual(uint8_t x, uint8_t nn)
{
	if (m_vx[x] != nn)
		pc += 2;
}

void Chip::Op_SkipXyCondIfEqual(uint8_t x, uint8_t y)
{
	if (m_vx[x] == m_vx[y])
		pc += 2;

}

void Chip::Op_SkipXyCondIfNotEqual(uint8_t x, uint8_t y)
{
	if (m_vx[x] != m_vx[y])
		pc += 2;
}

void Chip::Op_LASet(uint8_t x, uint8_t y)
{
	m_vx[x] = m_vx[y];
}

void Chip::Op_LABinaryOr(uint8_t x, uint8_t y)
{
	m_vx[x] = m_vx[x] | m_vx[y];
}

void Chip::Op_LABinaryAnd(uint8_t x, uint8_t y)
{
	m_vx[x] = m_vx[x] & m_vx[y];
}

void Chip::Op_LALogicalXor(uint8_t x, uint8_t y)
{
	m_vx[x] = m_vx[x] ^ m_vx[y];
}

void Chip::Op_LAAdd(uint8_t x, uint8_t y)
{
	uint16_t result = m_vx[x] + m_vx[y];
	bool overflow = (result > 255);

	// Set
	m_vx[x] = result;

	if (overflow)
		m_vx[0xF] = 1;
	else
		m_vx[0xF] = 0;
}

void Chip::Op_LASubtractY(uint8_t x, uint8_t y)
{
	uint8_t flag = (m_vx[x] >= m_vx[y]) ? 1 : 0;
	m_vx[x] = m_vx[x] - m_vx[y];

	m_vx[0xF] = flag;
}

void Chip::Op_LASubtractX(uint8_t x, uint8_t y)
{
	uint8_t flag = (m_vx[y] >= m_vx[x]) ? 1 : 0;
	m_vx[x] = m_vx[y] - m_vx[x];

	m_vx[0xF] = flag;

}

}
