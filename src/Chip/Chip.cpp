#include "Chip.h"

namespace Chip8Emulator
{
void Chip::Startup(SDL_Renderer* renderer, Settings emuSettings)
{
	m_operationalSettings.instructionsPerSecond = emuSettings.instructionsPerSecond;
	m_operationalSettings.cosmacInstructionsSet = emuSettings.cosmacInstructionsSet;
	m_operationalSettings.addToIndexOverflowsVF = emuSettings.addToIndexOverflowsVF;
	m_operationalSettings.modernMemoryLoad		= emuSettings.modernMemoryLoad;

	m_memory.InitMemory();
	pc = 0x200;
	m_display.InitDisplay(renderer);
	m_keypad.Init();
	m_delayTimer = 0;
	m_soundTimer = 0;
}

void Chip::Cycle(double deltaTime)
{
	// Update Timers
	m_cyclesAccumulator += deltaTime;
	m_timerAccumulator += deltaTime;

	double cycleThreshold = (1.0 / m_operationalSettings.instructionsPerSecond);
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

			else if (n == 0x6)
				Op_LAShiftRight(x, y);

			else if (n == 0x7)
				Op_LASubtractX(x, y);

			else if (n == 0xE)
				Op_LAShiftLeft(x, y);

			else
				return false;

			return true;

		case 0xB:
			Op_JumpToWithOffset(x, nn, nnn);
			return true;

		case 0xC:
			Op_Random(x, nn);
			return true;

		case 0xE:
			if (nn == 0x9E)
				Op_SkipIfKeyPressed(x);
			else if (nn == 0xA1)
				Op_SkipIfKeyNotPressed(x);
			else 
				return false;

			return true;

		case 0xF:
			if (nn == 0x07)
				Op_SetVxToDelayTimerValue(x);
			else if (nn == 0x15)
				Op_SetDelayTimer(x);
			else if (nn == 0x18)
				Op_SetSoundTimer(x);
			else if (nn == 0x1E)
				Op_AddToIndex(x);
			else if (nn == 0x0A)
				Op_GetKey(x);
			else if (nn == 0x29)
				Op_FontCharacter(x);
			else if (nn == 0x33)
				Op_BinaryCodedDecimalConversion(x);
			else if (nn == 0x55)
				Op_StoreIntoMemory(x);
			else if (nn == 0x65)
				Op_LoadFromMemory(x);
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

void Chip::Op_JumpToWithOffset(uint8_t x, uint8_t nn, uint16_t nnn)
{
	if (m_operationalSettings.cosmacInstructionsSet)
	{
		pc = nnn;
		pc += m_vx[0];
	}
	else
	{
		pc = nnn;
		pc += m_vx[x];
	}
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

	// m_display.Update(); this is now done in the emulator
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

	m_vx[0xF] = (overflow) ? 1 : 0;
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

void Chip::Op_LAShiftRight(uint8_t x, uint8_t y)
{
	if (m_operationalSettings.cosmacInstructionsSet)
		m_vx[x] = m_vx[y];

	uint8_t bit = m_vx[x] & 0x1;
	m_vx[x] = m_vx[x] >> 1;

	m_vx[0xF] = bit;
}

void Chip::Op_LAShiftLeft(uint8_t x, uint8_t y)
{
	if (m_operationalSettings.cosmacInstructionsSet)
		m_vx[x] = m_vx[y];

	uint8_t bit = (m_vx[x] & 0x80) >> 7;
	m_vx[x] = m_vx[x] << 1;

	m_vx[0xF] = bit;
}

void Chip::Op_Random(uint8_t x, uint8_t nn)
{
	uint8_t num = rand() % 256;
	m_vx[x] = nn & num;
}

void Chip::Op_SkipIfKeyPressed(uint8_t x)
{
	if (m_inputRef.GetKeyHeld(SDL_GetScancodeFromKey(m_keypad.GetKeycodes()[m_vx[x]])))
		pc += 2;
}

void Chip::Op_SkipIfKeyNotPressed(uint8_t x)
{
	if (!m_inputRef.GetKeyHeld(SDL_GetScancodeFromKey(m_keypad.GetKeycodes()[m_vx[x]])))
		pc += 2;
}

void Chip::Op_SetVxToDelayTimerValue(uint8_t x)
{
	m_vx[x] = m_delayTimer;
}

void Chip::Op_SetDelayTimer(uint8_t x)
{
	m_delayTimer = m_vx[x];
}

void Chip::Op_SetSoundTimer(uint8_t x)
{
	m_soundTimer = m_vx[x];
}

void Chip::Op_AddToIndex(uint8_t x)
{
	uint16_t result = ir + m_vx[x];
	bool overflow = (result >= 0x1000);

	ir = result;

	if (m_operationalSettings.addToIndexOverflowsVF)
		m_vx[0xF] = (overflow) ? 1 : 0;
}

void Chip::Op_GetKey(uint8_t x)
{
	SDL_Keycode keyPressed = m_inputRef.AnyKeyUp();
	if (keyPressed == -1)
		pc -= 2;
	else
	{
		keyPressed = m_keypad.GetHexKey(keyPressed);

		if (keyPressed == 0xFF)
			pc -= 2;
		else
			m_vx[x] = keyPressed;
	}
}

void Chip::Op_FontCharacter(uint8_t x)
{
	ir = 0x50 + (5 * m_vx[x]);
}

void Chip::Op_BinaryCodedDecimalConversion(uint8_t x)
{
	m_memory.memory[ir]		= m_vx[x] / 100;
	m_memory.memory[ir+1]	= (m_vx[x] / 10) % 10;
	m_memory.memory[ir+2]	= m_vx[x] % 10;;
}

void Chip::Op_StoreIntoMemory(uint8_t x)
{
	uint16_t temp = ir;
	uint16_t& regRef = (m_operationalSettings.modernMemoryLoad) ? temp : ir;

	for (int i = 0; i <= x; i++)
	{
		m_memory.memory[regRef++] = m_vx[i];
	}
}

void Chip::Op_LoadFromMemory(uint8_t x)
{
	uint16_t temp = ir;
	uint16_t& regRef = (m_operationalSettings.modernMemoryLoad) ? temp : ir;

	for (int i = 0; i <= x; i++)
	{
		m_vx[i] = m_memory.memory[regRef++];
	}
}

void Chip::ApplySettings(Settings newSettings)
{
	m_operationalSettings = newSettings;
}

}
