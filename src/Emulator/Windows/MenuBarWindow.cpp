#include "MenuBarWindow.h"

#include "../Emulator.h"

namespace Chip8Emulator
{
	void MenuBarWindow::Draw()
	{
        // Main menu bar
        if (ImGui::BeginMainMenuBar())
        {
            YOffset = ImGui::GetWindowSize().y;

            if (ImGui::BeginMenu("File"))
            {
                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                {
                    Emulator::Instance().Stop();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
	}
}