#include "Display.h"

#include "../Emulator/Emulator.h"

namespace Chip8Emulator
{
	int Display::InitDisplay(SDL_Renderer* r)
	{
		rendererPtr = r;

		// Determine Pixel format
		SDL_RendererInfo rInfo;
		SDL_GetRendererInfo(rendererPtr, &rInfo);

		// Keep pixelated scaling
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

		m_texture = SDL_CreateTexture(rendererPtr, rInfo.texture_formats[0], SDL_TEXTUREACCESS_STREAMING, CHIP_DISPLAY_WIDTH, CHIP_DISPLAY_HEIGHT);

		if (!m_texture)
			return -2;

		// Init globals
		SDL_QueryTexture(m_texture, &m_format, nullptr, nullptr, nullptr);

		// Create pixel format mapping
		m_pixelFormat = SDL_AllocFormat(m_format);

		// Map the colors
		COLOR_BLACK = SDL_MapRGB(m_pixelFormat, 0, 255, 0);
		COLOR_WHITE = SDL_MapRGB(m_pixelFormat, 255, 255, 255);

		std::fill(std::begin(m_internalBuffer), std::end(m_internalBuffer), COLOR_BLACK);

		return 0;
	}

	// Returns true if a pixel is turned off
	bool Display::FlipPixel(int x, int y)
	{
		if (x < 0 || x >= CHIP_DISPLAY_WIDTH || y < 0 || y >= CHIP_DISPLAY_HEIGHT)
			return false;

		uint32_t& pixel = m_internalBuffer[x + y * CHIP_DISPLAY_WIDTH];
		bool collision = (pixel == COLOR_WHITE);
		pixel = collision ? COLOR_BLACK : COLOR_WHITE;
		return collision;
	}

	void Display::Update()
	{
		// Update the texture only, we don't render in the SDL_Window but in the Viewport (imgui)
		SDL_UpdateTexture(m_texture, nullptr, m_internalBuffer, CHIP_DISPLAY_WIDTH * sizeof(uint32_t));
		
		//Vector2Int screenSize = Emulator::Instance().GetScreenSize();
		//SDL_Rect dstRect = { (screenSize.x - 64 * (screenSize.x / 120)) / 2, 1 + MenuBarWindow::YOffset, 64 * (screenSize.x / 120), 32 * (screenSize.x / 120) };
		//SDL_RenderCopy(rendererPtr, m_texture, nullptr, &dstRect);
	}

	void Display::ClearScreen()
	{
		// Clear the internal buffer only, update the screen only before Present
		std::fill(std::begin(m_internalBuffer), std::end(m_internalBuffer), COLOR_BLACK);
	}

	void Display::Destroy()
	{
		SDL_FreeFormat(m_pixelFormat);
		SDL_DestroyTexture(m_texture);
	}
};
