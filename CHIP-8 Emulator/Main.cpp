#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include "CHIP8.h"


int main(int argumentCount, char * arguments[])
{
	Renderer* renderer = new Renderer();
	Keyboard* keyboard = new Keyboard();
	Speaker* speaker = new Speaker();
	CPU* cpu = new CPU(renderer, keyboard, speaker);

	(*cpu).loadRom("CONNECT4");

	int fps = 100;
	int deltaTime = 1000 / fps;

	SDL_Event event;

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

		(*cpu).cycle(event);
	}
	return EXIT_SUCCESS;
}