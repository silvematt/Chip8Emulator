#pragma once

#include <stdint.h>
#include "SDL.h"

namespace Chip8Emulator
{
	inline const int SCREEN_WIDTH = 64;
	inline const int SCREEN_HEIGHT = 32;

	inline constexpr SDL_Color colorBlack = { 0, 0, 0, SDL_ALPHA_OPAQUE };
	inline constexpr SDL_Color colorWhite = { 255, 255, 255, SDL_ALPHA_OPAQUE };

class Display
	{
	public:
		uint32_t COLOR_WHITE;
		uint32_t COLOR_BLACK;

		uint8_t			m_scaleFactor;

		SDL_Window*		m_window = nullptr;
		SDL_Renderer*	m_renderer = nullptr;
		SDL_Texture*	m_texture = nullptr;

		// Format
		uint32_t m_format;
		SDL_PixelFormat* m_pixelFormat = nullptr;

		// CPU frame buffer
		uint32_t m_internalBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

		int InitDisplay(uint8_t scaleFactor)
		{
			m_scaleFactor = scaleFactor;

			uint32_t winFlags = 0;
			//uint32_t winFlags = SDL_WINDOW_FULLSCREEN;
			m_window = SDL_CreateWindow("Chip8Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * m_scaleFactor, SCREEN_HEIGHT * m_scaleFactor, winFlags);

			Uint32 renFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
			m_renderer = SDL_CreateRenderer(m_window, -1, renFlags);

			if (!m_window || !m_renderer)
				return -1;

			// Determine Pixel format
			SDL_RendererInfo rInfo;
			SDL_GetRendererInfo(m_renderer, &rInfo);

			// Keep pixelated scaling
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

			m_texture = SDL_CreateTexture(m_renderer, rInfo.texture_formats[0], SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

			if (!m_texture)
				return -2;

			// Init globals
			SDL_QueryTexture(m_texture, &m_format, nullptr, nullptr, nullptr);

			// Create pixel format mapping
			m_pixelFormat = SDL_AllocFormat(m_format);

			// Map the colors
			COLOR_BLACK = SDL_MapRGB(m_pixelFormat, 0, 0, 0);
			COLOR_WHITE = SDL_MapRGB(m_pixelFormat, 255, 255, 255);

			// Initialize the Draw Color to black
			SDL_SetRenderDrawColor(m_renderer, colorBlack.r, colorBlack.g, colorBlack.b, colorBlack.a);

			return 0;
		}

		// Returns true if a pixel is turned off
		bool FlipPixel(int x, int y)
		{
			if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
				return false;

			uint32_t& pixel = m_internalBuffer[x + y * SCREEN_WIDTH];
			bool collision = (pixel == COLOR_WHITE);
			pixel = collision ? COLOR_BLACK : COLOR_WHITE;
			return collision;
		}

		void ClearScreen()
		{
			// Clear the internal buffer only, update the screen only before Present
			memset(m_internalBuffer, 0, sizeof(m_internalBuffer));
		}

		void Update()
		{
			SDL_RenderClear(m_renderer);

			SDL_UpdateTexture(m_texture, nullptr, m_internalBuffer, SCREEN_WIDTH * sizeof(uint32_t));
			SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
			SDL_RenderPresent(m_renderer);
		}

		void Destroy()
		{
			SDL_FreeFormat(m_pixelFormat);
			SDL_DestroyTexture(m_texture);
			SDL_DestroyRenderer(m_renderer);
			SDL_DestroyWindow(m_window);
		}
	};
}
