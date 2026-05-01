#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#include "../Settings.h"


namespace Chip8Emulator
{
class SettingsWindow
{
public:
	SettingsWindow(Settings& s) :settingsRef(s)
	{

	}

private:
	Settings& settingsRef;

	uint32_t step = 100;
	uint32_t step_fast = 1000;

public:
	void Draw()
	{
		ImGui::Begin("Settings:");
		ImGui::Text("Emulator Settings:");

		ImGui::Separator();
		ImGui::PushItemWidth(50);

		// Settings
		ImGui::Checkbox("Cosmac Instruction Set", &settingsRef.cosmacInstructionsSet);
		ImGui::Checkbox("Add To Index Overflows VF", &settingsRef.addToIndexOverflowsVF);
		ImGui::Checkbox("Modern Memory Load", &settingsRef.modernMemoryLoad);

			
		ImGui::PushItemWidth(100);
		ImGui::Text("IPS");
		ImGui::SameLine();
		if (ImGui::InputScalar("##IPS", ImGuiDataType_U32, &settingsRef.instructionsPerSecond, &step, &step_fast, "%u"))
		{
			SDL_Log("changed!! %d", settingsRef.instructionsPerSecond);
		}

		ImGui::PushItemWidth(50);
		ImGui::Text("Display Scale");
		ImGui::SameLine();
		if (ImGui::InputScalar("##Display Scale", ImGuiDataType_U32, &settingsRef.displayScale))
		{
			SDL_Log("changed!! %d", settingsRef.displayScale);
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f)); // 20px vertical space


		if (ImGui::Button("Apply", ImVec2(-1, 0)))
		{

		}

		ImGui::End();
	}
};
}
