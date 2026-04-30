#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"


namespace Chip8Emulator
{
	class MenuBarWindow
	{
	public:
        inline static float YOffset = 0.0f;

        void Draw()
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
	};
}