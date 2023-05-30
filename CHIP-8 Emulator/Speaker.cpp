#include <iostream>
#include <SDL_mixer.h>
#include "CHIP8.h"

Speaker::Speaker()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	sound = Mix_LoadWAV("awesomeness.wav");
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