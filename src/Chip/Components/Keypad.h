#pragma once
#include <cstdint>
#include <unordered_map>
#include <array>

class Keypad
{
public:
	std::array<uint8_t, 16> keycodes;
	std::unordered_map<uint8_t, uint8_t> inputmap;

public:
	void Init()
	{
		inputmap[49] = 0x0;
		inputmap[50] = 0x1;
		inputmap[51] = 0x2;
		inputmap[52] = 0x3;

		inputmap[113] = 0x4;
		inputmap[119] = 0x5;
		inputmap[101] = 0x6;
		inputmap[114] = 0x7;

		inputmap[97] = 0x8;
		inputmap[115] = 0x9;
		inputmap[100] = 0xA;
		inputmap[102] = 0xB;

		inputmap[122] = 0xC;
		inputmap[120] = 0xD;
		inputmap[99] = 0xE;
		inputmap[118] = 0xF;

		keycodes = 
		{
			49, 50, 51, 52,     // 1, 2, 3, 4
			113, 119, 101, 114, // q, w, e, r
			97, 115, 100, 102,  // a, s, d, f
			122, 120, 99, 118   // z, x, c, v
		};
	}

	const uint8_t* GetKeycodes() const
	{
		return keycodes.data();
	}

	SDL_Keycode GetHexKey(SDL_Keycode code) const
	{
		auto it = inputmap.find(code);
		if (it == inputmap.end())
		{
			return 0xFF; // Means not found
		}

		return it->second;
	}
};
