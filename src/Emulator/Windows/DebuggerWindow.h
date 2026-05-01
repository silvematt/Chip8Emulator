#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

namespace Chip8Emulator
{
class DebuggerWindow
{
public:
	bool m_debuggerEnabled = false;
	bool m_oneInstructionAtTime = false;
	bool m_doOneInstruction = false;

public:
	void Draw();
};
}
