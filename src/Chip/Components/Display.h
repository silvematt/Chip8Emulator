#pragma once

#include <stdint.h>
#include "SDL.h"

namespace Chip8Emulator
{
class Display
	{
	public:
		uint32_t COLOR_WHITE;
		uint32_t COLOR_BLACK;

		uint8_t		m_scaleFactor;
		SDL_Window* m_window = nullptr;

		SDL_Surface* m_surface;
		uint32_t* m_pixels;

		int InitDisplay(uint8_t scaleFactor)
		{
			m_scaleFactor = scaleFactor;

			uint32_t winFlags = 0;
			//uint32_t winFlags = SDL_WINDOW_FULLSCREEN;
			m_window = SDL_CreateWindow("Chip8Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64 * m_scaleFactor, 32 * m_scaleFactor, winFlags);
			m_surface = SDL_GetWindowSurface(m_window);
			m_pixels = static_cast<uint32_t*>(m_surface->pixels);

			if (!m_window || !m_surface)
				return -1;

			// Init globals
			COLOR_BLACK = SDL_MapRGB(m_surface->format, 0, 0, 0);
			COLOR_WHITE = SDL_MapRGB(m_surface->format, 255, 255, 255);

			return 0;
		}

		void PutPixel(int x, int y, uint32_t color)
		{
			for (int i = 0; i < m_scaleFactor; i++)
			{
				for (int j = 0; j < m_scaleFactor; j++)
				{
					int px = x * m_scaleFactor + i;
					int py = y * m_scaleFactor + j;

					if (px >= 0 && px < m_surface->w &&
						py >= 0 && py < m_surface->h)
					m_pixels[px + py*m_surface->w] = color;
				}
			}
		}

		// Returns true if a pixel is turned off
		bool FlipPixel(int x, int y)
		{
			// Check collision first on the base pixel
			int px = x * m_scaleFactor;
			int py = y * m_scaleFactor;

			// Out of screen pixels are clipped
			if (px < 0 || px >= m_surface->w
				|| py < 0 || py >= m_surface->h)
				return false;

			bool collision = (m_pixels[px + py * m_surface->w] == COLOR_WHITE);

			// Fill with scale factor
			for (int i = 0; i < m_scaleFactor; i++)
			{
				for (int j = 0; j < m_scaleFactor; j++)
				{
					m_pixels[(px + i) + (py + j) * m_surface->w] = collision ? COLOR_BLACK : COLOR_WHITE;
				}
			}

			return collision;
		}

		void ClearScreen()
		{
			SDL_FillRect(m_surface, NULL, COLOR_BLACK);
		}

		void Update()
		{
			SDL_UpdateWindowSurface(m_window);
		}
	};
}