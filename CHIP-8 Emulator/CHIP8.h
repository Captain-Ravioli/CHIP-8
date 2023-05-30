#pragma once
#include <vector>
#include <unordered_set>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>
using namespace std;

#ifndef RENDERER_H
#define RENDERER_H

class Renderer
{
public:
	SDL_Window* window;
	SDL_Renderer* renderer;

	Renderer();
	bool setPixel(int x, int y);
	void render();
	void clearScreen();
	void quit();

private:
	const int cols = 64, rows = 32;
	const int scale = 30;

	int display[2048] = { };

	void drawPixel(int xPos, int yPos, bool white);
};

#endif // !RENDERER_H

#ifndef KEYBOARD_H
#define KEYBOARD_H

class Keyboard
{
public:
	Keyboard();
	int cycle(SDL_Event event);
	bool isKeyPressed(uint8_t key);
	int getCode(uint8_t key);
private:
	unordered_set<uint8_t> heldKeys = { };
};

#endif // !KEYBOARD_H

#ifndef SPEAKER_H
#define SPEAKER_H

class Speaker
{
public:
	Speaker();
	void play();
	void stop();
private:
	Mix_Chunk* sound;
	bool isPlaying = false;;
};

#endif // !SPEAKER_H

#ifndef CPU_H
#define CPU_H

class CPU
{
public:
	CPU(Renderer* renderer, Keyboard* keyboard, Speaker* speaker);
	void cycle(SDL_Event event);
	void loadRom(string romName);
private:
	Renderer* renderer;
	Keyboard* keyboard;
	Speaker* speaker;

	uint8_t memory[4096] = { };
	uint8_t registers[16] = { };
	uint16_t memAddress = 0;

	int delayTimer = 0;
	int soundTimer = 0;

	uint16_t programCounter = 0x200;

	vector<uint16_t> stack;

	int heldX = -1;

	bool paused = false;
	const int speed = 10;

	void loadSpritesToMemory();
	void loadProgramToMemory(uint8_t program[]);
	void loadProgramToMemory(vector<uint8_t> program);
	void executeInstruction(uint16_t opcode);
	void updateTimers();
	void playSound();

	vector<unsigned char> readFile(const char* filename);
	vector<uint8_t> readFileU(const char* filename);
};

#endif // !CPU_H