#pragma once
#include <cstdint>

namespace Chip8Emulator
{
    struct Settings
    {
		uint32_t instructionsPerSecond;

		bool cosmacInstructionsSet; // some chip8 instructions are ambigious between original cosmac vip and next versions
		bool addToIndexOverflowsVF;
		bool modernMemoryLoad;

		Settings()
		{
			instructionsPerSecond = 700;
			cosmacInstructionsSet = true;
			addToIndexOverflowsVF = true;
			modernMemoryLoad = false;
		}
    };
}