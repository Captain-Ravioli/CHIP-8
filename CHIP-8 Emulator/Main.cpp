#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include "CHIP8.h"
using namespace std;

int main(int argumentCount, char * arguments[])
{
	const char* romName = "BRIX";
	Renderer* renderer = new Renderer(romName);
	Keyboard* keyboard = new Keyboard();
	Speaker* speaker = new Speaker();
	CPU* cpu = new CPU(renderer, keyboard, speaker);

	cpu->loadRom(romName);

	int fps = 60;
	float deltaTime = 1000.0f / fps;

	SDL_Event event;

	int ticks = SDL_GetTicks();
	int currentDelta;
	int newTicks = SDL_GetTicks();
	while (true)
	{
		if (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				renderer->quit();
				break;
			}
		}

		keyboard->cycle(event);

		newTicks = SDL_GetTicks();
		currentDelta = newTicks - ticks;

		if (currentDelta + 1 > deltaTime)
		{
			cout << 1000 / currentDelta << " fps\n";
			ticks = newTicks;

			cpu->cycle(event);
		}
	}
	return EXIT_SUCCESS;
}