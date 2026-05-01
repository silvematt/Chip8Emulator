#pragma once

#include "../../Chip/Chip.h"

namespace Chip8Emulator
{
class ChipStatusWindow
{
private:
	Chip& m_chipRef;

public:
	ChipStatusWindow(Chip& c) : m_chipRef(c)
	{
	}
	
// Methdos
	void Draw();
};
}

