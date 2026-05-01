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
                if (ImGui::MenuItem("Open"))
                {

                }
                if (ImGui::MenuItem("Save"))
                {

                }

                ImGui::Separator();

                if (ImGui::MenuItem("Exit"))
                {
                    Emulator::Instance().Stop();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Copy"))
                {

                }
                if (ImGui::MenuItem("Paste"))
                {

                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
	}
}