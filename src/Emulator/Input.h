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

	SDL_Keycode	m_anyKey;
	SDL_Keycode	m_anyKeyUp;
public:
	~Input();

	int				Init();
	void			Handle();

	int				GetKeyHeld(SDL_Scancode key) const;
	int				GetKeyDown(SDL_Scancode key) const;
	int				GetKeyUp(SDL_Scancode key) const;

	SDL_Keycode		AnyKeyDown() const;
	SDL_Keycode		AnyKeyUp() const;
};

}
