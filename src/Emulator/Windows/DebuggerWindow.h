#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "../Chip/Chip.h"

namespace Chip8Emulator
{
class DebuggerWindow
{
public:
	bool m_oneInstructionAtTime = false;
	bool m_doOneInstruction = false;

	Chip& m_chipRef;
	bool m_pauseChipExecution = false;

public:
	DebuggerWindow(Chip& c) : m_chipRef(c)
	{
		m_oneInstructionAtTime = false;
		m_doOneInstruction = false;
	}

	void Draw();
};
}
