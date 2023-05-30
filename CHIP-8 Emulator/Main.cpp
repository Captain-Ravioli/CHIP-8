#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include "CHIP8.h"


int main(int argumentCount, char * arguments[])
{
	Renderer* renderer = new Renderer(10);
	Keyboard* keyboard = new Keyboard();
	Speaker* speaker = new Speaker();
	CPU* cpu = new CPU(renderer, keyboard, speaker);

	(*cpu).loadRom("BLITZ");

	int fps = 100;
	int deltaTime = 1000 / fps;

	SDL_Event event;

	Uint64 ticks = SDL_GetTicks64();
	while (true)
	{
		if (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				(*renderer).quit();
				break;
			}
		}

		if (SDL_GetTicks64() - ticks >= deltaTime)
			continue;

		(*cpu).cycle(event);
		std::cout << (float)1000 / (SDL_GetTicks64() - ticks) << " fps\n";
		ticks = SDL_GetTicks64();
	}
	return EXIT_SUCCESS;
}