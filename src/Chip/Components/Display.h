#pragma once

#include <stdint.h>
#include <algorithm>
#include "SDL.h"

#include "../../Emulator/Windows/MenuBarWindow.h"
#include "../../Utility/Vector.h"


namespace Chip8Emulator
{
class Display
	{
	public:
		static constexpr int CHIP_DISPLAY_WIDTH = 64;
		static constexpr int CHIP_DISPLAY_HEIGHT = 32;

		uint32_t COLOR_WHITE;
		uint32_t COLOR_BLACK;

		SDL_Renderer*	rendererPtr;
		SDL_Texture*	m_texture = nullptr;

		// Format
		uint32_t			m_format;
		SDL_PixelFormat*	m_pixelFormat = nullptr;

		// CPU frame buffer
		uint32_t m_internalBuffer[CHIP_DISPLAY_WIDTH * CHIP_DISPLAY_HEIGHT];

		int InitDisplay(SDL_Renderer* r);

		// Returns true if a pixel is turned off
		bool FlipPixel(int x, int y);

		// Updates the screen
		void Update();
		void ClearScreen();

		void Destroy();

		const SDL_Texture* GetDisplayTexture() const
		{
			return m_texture;
		}
	};
}
