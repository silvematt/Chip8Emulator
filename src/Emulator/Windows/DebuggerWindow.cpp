#include "DebuggerWindow.h"

#include "../Emulator.h"

#include <nfd.h> 

namespace Chip8Emulator
{
	void DebuggerWindow::Draw()
	{
		Emulator& emu = Emulator::Instance();

		ImGui::Begin("Debugger", NULL);

		if (ImGui::Button("Load ROM", ImVec2(-1, 0)))
		{
			nfdchar_t* outPath = nullptr;
			nfdfilteritem_t filterItem[1] = { { "CHIP-8 ROM", "ch8,rom,bin" } };
			nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, nullptr);

			if (result == NFD_OKAY)
			{
				// outPath contains the selected file path
				emu.ResetChip();
				emu.DirectLoadRom(outPath);
				NFD_FreePath(outPath);
			}
			else if (result == NFD_CANCEL)
			{
				// Operation cancelled
			}
			else
			{
				// Error
				printf("NFD Error: %s\n", NFD_GetError());
			}
		}

		if (ImGui::Button("Reset Chip", ImVec2(-1, 0)))
		{
			emu.ResetChip();
		}

		if (m_pauseChipExecution)
		{
			if (ImGui::Button("Resume Execution", ImVec2(-1, 0)))
				m_pauseChipExecution = false;
		}
		else
		{
			if (ImGui::Button("Pause Execution", ImVec2(-1, 0)))
				m_pauseChipExecution = true;
		}

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
