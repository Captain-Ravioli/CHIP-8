#include <SDL.h>
#include <cstdlib>
#include <algorithm>
#include <math.h>
#include "CHIP8.h"

Renderer::Renderer()
{
	this->scale = 10;
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		printf("Could not SDL_Init: %s\n", SDL_GetError());
	window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cols * scale, rows * scale, SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
	renderer = SDL_CreateRenderer(window, 0, 0);
}
Renderer::Renderer(float scale)
{
	this->scale = scale;
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		printf("Could not SDL_Init: %s\n", SDL_GetError());
	window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, cols * scale, rows * scale, SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI);
	renderer = SDL_CreateRenderer(window, 0, 0);
}

void Renderer::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	for (int i = 0; i < cols * rows; i++)
	{
		int x = i % cols;
		int y = (int)floorf((float)i / cols);

		if (display[i] > 0)
			drawPixel(x, y, true);
	}

	SDL_RenderPresent(renderer);
}

void Renderer::clearScreen()
{
	std::fill(display, display + 2048, 0);
}

void Renderer::quit()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool Renderer::setPixel(int x, int y)
{
	if (x < 0)
		x += cols;
	else if (x > cols)
		x -= cols;
	if (y < 0)
		y += rows;
	else if (y > rows)
		y -= rows;

	int pixelLocation = x + (y * cols);
	display[pixelLocation] ^= 1;
	return display[pixelLocation] != 1;
}
 
void Renderer::drawPixel(int xPos, int yPos, bool white)
{
	for (int y = 0; y < scale; y++)
	{
		for (int x = 0; x < scale; x++)
		{
			if (white)
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			else
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

			SDL_RenderDrawPoint(renderer, xPos * scale + x, yPos * scale+ y);
		}
	}
}