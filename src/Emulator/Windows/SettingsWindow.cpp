#include "SettingsWindow.h"

#include "../Emulator.h"

namespace Chip8Emulator
{
void SettingsWindow::Draw()
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

	static uint32_t step = 100;
	static uint32_t stepFast = 1000;
	ImGui::Text("IPS");
	ImGui::SameLine();
	ImGui::InputScalar("##IPS", ImGuiDataType_U32, &settingsRef.instructionsPerSecond, &step, &stepFast, "%u");

	ImGui::PushItemWidth(50);
	ImGui::Text("Display Scale");
	ImGui::SameLine();

	ImGui::Dummy(ImVec2(0.0f, 20.0f)); // 20px vertical space


	if (ImGui::Button("Apply", ImVec2(-1, 0)))
	{
		Emulator& emu = Emulator::Instance();
		emu.GetChip().ApplySettings(emu.GetSettings());
	}

	ImGui::End();
}
}
