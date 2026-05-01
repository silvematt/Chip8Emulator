#include "ChipStatusWindow.h"

#include "../Emulator.h"

namespace Chip8Emulator
{
    void ChipStatusWindow::Draw()
    {
        Emulator& emu = Emulator::Instance();

        ImGui::Begin("Chip Status", NULL);

        ImGui::TextDisabled("Execution");
        ImGui::Separator();
        ImGui::Text("PC: 0x%03X", m_chipRef.pc);
        ImGui::Text("IR: 0x%03X", m_chipRef.ir);

        ImGui::Spacing();

        ImGui::TextDisabled("Timers");
        ImGui::Separator();
        ImGui::Text("Delay: %02d", m_chipRef.m_delayTimer);
        ImGui::Text("Sound: %02d", m_chipRef.m_soundTimer);

        ImGui::Spacing();

        ImGui::TextDisabled("Variable Registers");
        ImGui::Separator();

        // Display registers in columns of 4
        if (ImGui::BeginTable("VxTable", 4, ImGuiTableFlags_BordersInner | ImGuiTableFlags_SizingFixedFit))
        {
            for (int i = 0; i < 16; i++)
            {
                ImGui::TableNextColumn();
                ImGui::Text("V%X: 0x%02X", i, m_chipRef.m_vx[i]);
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();

        ImGui::TextDisabled("Stack");
        ImGui::Separator();
        ImGui::Text("Depth: %d", m_chipRef.m_stack.size());

        // Display the top element of the stack if not empty
        if (!m_chipRef.m_stack.empty())
            ImGui::Text("Top:   0x%03X", m_chipRef.m_stack.top());
        else
            ImGui::TextDisabled("Stack is empty");

        ImGui::End();
    }

}
