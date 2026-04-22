#pragma once

#include "SDL.h"

namespace Chip8Emulator
{
class Input
{
private:
	Uint8*	m_keys;
	Uint8*	m_prevKeys;
	int		m_numKeys;

public:
	~Input();

	int				Init();
	void			Handle();

	int				GetKeyHeld(SDL_Scancode key) const;
	int				GetKeyDown(SDL_Scancode key) const;
	int				GetKeyUp(SDL_Scancode key) const;

};

}
