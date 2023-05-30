#include <SDL.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include "CHIP8.h"

Keyboard::Keyboard()
{

}

int Keyboard::cycle(SDL_Event event)
{
	if (event.type == SDL_KEYDOWN)
	{
		int key = getCode(event.key.keysym.sym);
		uint8_t result = key;
		if (key != -1 && !isKeyPressed(result))
			heldKeys.insert(result);
		return result;
	}
	else if (event.type == SDL_KEYUP)
	{
		int key = getCode(event.key.keysym.sym);
		uint8_t result = key;
		if (key != -1 && isKeyPressed(result))
			heldKeys.erase(result);
	}
	return -1;
}

bool Keyboard::isKeyPressed(uint8_t key)
{
	return heldKeys.count(key);
}

int Keyboard::getCode(uint8_t key)
{
	switch (key)
	{
	case SDLK_1:
		return 0x1;
	case SDLK_2:
		return 0x2;
	case SDLK_3:
		return 0x3;
	case SDLK_4:
		return 0xc;
	case SDLK_q:
		return 0x4;
	case SDLK_w:
		return 0x5;
	case SDLK_e:
		return 0x6;
	case SDLK_r:
		return 0xD;
	case SDLK_a:
		return 0x7;
	case SDLK_s:
		return 0x8;
	case SDLK_d:
		return 0x9;
	case SDLK_f:
		return 0xE;
	case SDLK_z:
		return 0xA;
	case SDLK_x:
		return 0x0;
	case SDLK_c:
		return 0xB;
	case SDLK_v:
		return 0xF;
	default:
		return -1;
	}
}