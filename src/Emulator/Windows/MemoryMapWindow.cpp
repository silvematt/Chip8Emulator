#include "MemoryMapWindow.h"

namespace Chip8Emulator
{
    void MemoryMapWindow::Draw()
    {
        ImGui::Begin("Memory Map", NULL);

        ImGui::Checkbox("Follow PC", &m_followPc);
        ImGui::SameLine();
        ImGui::TextDisabled("| PC: 0x%03X", m_chipRef.pc);
        ImGui::Separator();

        // Memory Grid
        ImGui::BeginChild("MemGrid");

        ImGuiListClipper clipper;
        clipper.Begin(m_totalRows);

        // While we can draw (returns false if there's no more space in the window) 
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                uint16_t rowAddress = row * m_bytesPerRow;

                // Display Address (0x200)
                ImGui::TextDisabled("0x%03X: ", rowAddress);
                ImGui::SameLine();

                // Display Hex Bytes
                for (int n = 0; n < m_bytesPerRow; n++)
                {
                    uint16_t curAddr = rowAddress + n;
                    uint8_t val = m_chipRef.m_memory.memory[curAddr];
                    bool isPc = (curAddr == m_chipRef.pc || curAddr == m_chipRef.pc + 1);

                    if (isPc)
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red

                    ImGui::Text("%02X", val);

                    if (isPc)
                        ImGui::PopStyleColor();

                    if (n < m_bytesPerRow - 1)
                        ImGui::SameLine();
                }
            }
        }
        clipper.End();

        // Auto-Scrolling logic
        if (m_followPc)
        {
            int pcRow = m_chipRef.pc / m_bytesPerRow;
            float scrollTarget = ImGui::GetCursorStartPos().y + pcRow * ImGui::GetTextLineHeightWithSpacing();
            ImGui::SetScrollFromPosY(scrollTarget, 0.5f);
        }

        ImGui::EndChild();
        ImGui::End();
    }
}