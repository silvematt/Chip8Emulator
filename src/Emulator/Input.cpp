#include "Input.h"
#include "Emulator.h"

namespace Chip8Emulator
{
Input::~Input()
{
	delete[] m_keys;
	delete[] m_prevKeys;
}

//--------------------------------------
// Initialize
//--------------------------------------
int Input::Init()
{
	const Uint8* keyboard = SDL_GetKeyboardState(&m_numKeys);

	m_keys = new Uint8[m_numKeys];
	m_prevKeys = new Uint8[m_numKeys];

	return 0;
}

//--------------------------------------
// Handles Input by polling events
//--------------------------------------
void Input::Handle()
{
	SDL_Event e;

	Emulator& emulator = Emulator::Instance();

	//Handle events
	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type)
		{
		case SDL_QUIT:
			emulator.Stop();
			break;

		case SDL_KEYDOWN:
			break;
		}
	}

	const Uint8* keyboard = SDL_GetKeyboardState(&m_numKeys);

	memcpy(m_prevKeys, m_keys, sizeof(m_prevKeys[0]) * m_numKeys);
	memcpy(m_keys, keyboard, sizeof(m_keys[0]) * m_numKeys);
}

int Input::GetKeyHeld(SDL_Scancode key) const
{
	if (key < 0 || key > m_numKeys)
		return -1;

	return m_keys[key];
}

int Input::GetKeyDown(SDL_Scancode key) const
{
	if (key < 0 || key > m_numKeys)
		return -1;

	return (m_keys[key] & ~m_prevKeys[key]);
}

int Input::GetKeyUp(SDL_Scancode key) const
{
	if (key < 0 || key >= m_numKeys)
		return -1;

	return (!m_keys[key] & m_prevKeys[key]);
}

}
