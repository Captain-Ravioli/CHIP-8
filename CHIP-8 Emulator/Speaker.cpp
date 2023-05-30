#include <iostream>
#include <SDL_mixer.h>
#include "CHIP8.h"

Speaker::Speaker()
{
	if (Mix_Init(MIX_INIT_MP3) == 0)
		printf("Failed to Mix_Init: %s", Mix_GetError());

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	sound = Mix_LoadWAV("awesomeness.mp3");
	if (!sound)
		printf("Failed to set sound: %s", Mix_GetError());
}

void Speaker::play()
{
	if (!isPlaying)
	{
		Mix_PlayChannel(-1, sound, 0);
		isPlaying = true;
	}
}

void Speaker::stop()
{
	if (isPlaying)
	{
		Mix_HaltChannel(-1);
		isPlaying = false;
	}
}