#include "ViewportWindow.h"

#include "../Emulator.h"

namespace Chip8Emulator
{
	void ViewportWindow::Draw()
	{
		Emulator& emu = Emulator::Instance();

		ImGui::Begin("Viewport", NULL, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse);
		
		// Contains the image inside the child "rect", so that it fits the window
		ImGui::BeginChild("Container");

		ImVec2 size = ImGui::GetWindowSize();
		ImGui::Image((ImTextureID)emu.GetChip().m_display.GetDisplayTexture(), size);

		ImGui::EndChild();

		ImGui::End();
	}
}
