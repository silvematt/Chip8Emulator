#include "DebuggerWindow.h"

#include "../Emulator.h"

namespace Chip8Emulator
{
	void DebuggerWindow::Draw()
	{
		Emulator& emu = Emulator::Instance();

		ImGui::Begin("Debugger", NULL);

		ImGui::Checkbox("One Instruction Per Time", &m_oneInstructionAtTime);

		if (m_oneInstructionAtTime)
		{
			if (ImGui::Button("Do Instruction", ImVec2(-1,0)))
			{
				m_doOneInstruction = true;
			}
		}

		ImGui::End();
	}
}
