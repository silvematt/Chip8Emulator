#include <iostream>
#include "SDL.h"

#include "Emulator/Emulator.h"

// Undefine SDL_main
#undef main

int main(int argc, char* argv[])
{
	auto& emu = Chip8Emulator::Emulator::Instance();

	if (emu.Init() == 0)
	{
		emu.Start();
		emu.Update();
	}

	return 0;
}
