#pragma once

#include "../../Chip/Chip.h"

namespace Chip8Emulator
{
	class MemoryMapWindow
	{
	private:
		Chip& m_chipRef;
		bool m_followPc;

		int m_bytesPerRow;
		int m_totalRows;

	public:
		MemoryMapWindow(Chip& c) : m_chipRef(c)
		{
			m_followPc = true;
			m_bytesPerRow = 16;
			m_totalRows = 4096 / m_bytesPerRow;
		}

		// Methdos
		void Draw();
	};
}

