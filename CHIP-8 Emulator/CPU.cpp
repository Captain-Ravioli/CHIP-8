#include <fstream>
#include <string>
#include <algorithm>
#include "CHIP8.h"
using namespace std;

/*

Specifications of CHIP-8:
- 4KB (4096 bytes) of memory										    => memory
- 16 8-bit (byte) registers												=> registers
- A 16-bit register to store memory addresses						    => memAddresses
- Two timers. One for the delay, and one for the sound.				    => delayTimer, soundTimer
- A program counter that stores the address currently being executed    => programCounter
- An array (std::vector) to represent the stack							=> stack

*/
CPU::CPU(Renderer* renderer, Keyboard* keyboard, Speaker* speaker)
{
	this->renderer = renderer;
	this->keyboard = keyboard;
	this->speaker = speaker;

	loadSpritesToMemory();
}

void CPU::cycle(SDL_Event event)
{
	for (int i = 0; i < speed; i++)
	{
		int recentKey = keyboard->cycle(event);
		if (paused)
		{
			if (recentKey != -1)
			{
				registers[heldX] = recentKey;
				paused = false;
			}
		}
		else
		{
			// opcodes are 16 bits (2 bytes) big, and one memory address contains 8 bits (1 byte) of data, so merge 2 of uint8_t
			uint16_t opcode = (memory[programCounter] << 8) | memory[programCounter + 1];
			executeInstruction(opcode);
		}
	}

	if (!paused)
		updateTimers();

	playSound();
	renderer->render();
}

void CPU::loadSpritesToMemory()
{
	// each byte is hex representation of one line of a sprite
	uint8_t sprites[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	// sprites are stored in memory starting at 0x000
	for (int i = 0; i < sizeof(sprites); i++)
		memory[i] = sprites[i];
}

void CPU::loadProgramToMemory(uint8_t program[])
{
	// programs are stored in memory starting at 0x200
	for (int i = 0; i < sizeof(program); i++)
		memory[i] = program[i + 0x200];
}
void CPU::loadProgramToMemory(vector<uint8_t> program)
{
	// programs are stored in memory starting at 0x200
	for (int i = 0; i < program.size(); i++)
		memory[i + 0x200] = program[i];
}

void CPU::loadRom(string romName)
{
	vector<uint8_t> rom = readFileU(romName.c_str());
	loadProgramToMemory(rom);
}

void CPU::executeInstruction(uint16_t opcode)
{
	// every opcode is 2 bytes
	programCounter += 2;

	// 2nd nibble
	int x = (opcode & 0x0F00) >> 8;
	// 3rd nibble
	int y = (opcode & 0x00F0) >> 4;
	int nibble = opcode & 0xF;

	int sum = registers[x] + registers[y];
	int sub = registers[x] - registers[y];
	int subn = registers[y] - registers[x];
	int r = (rand() / (double)RAND_MAX) * 0xFF;

	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode)
		{
		case 0x00E0:
			renderer->clearScreen();
			break;
		case 0x00EE:
			if (!stack.empty())
			{
				programCounter = stack.back();
				stack.pop_back();
			}
			break;
		}
		break;
	case 0x1000:
		programCounter = opcode & 0xFFF;
		break;
	case 0x2000:
		stack.push_back(programCounter);
		programCounter = opcode & 0xFFF;
		break;
	case 0x3000:
		if (registers[x] == (opcode & 0xFF))
			programCounter += 2;
		break;
	case 0x4000:
		if (registers[x] != (opcode & 0xFF))
			programCounter += 2;
		break;
	case 0x5000:
		if (registers[x] == registers[y])
			programCounter += 2;
		break;
	case 0x6000:
		registers[x] = opcode & 0xFF;
		break;
	case 0x7000:
		registers[x] += opcode & 0xFF;
		break;
	case 0x8000:
		switch (opcode & 0xF)
		{
		case 0x0:
			registers[x] = registers[y];
			break;
		case 0x1:
			registers[x] |= registers[y];
			break;
		case 0x2:
			registers[x] &= registers[y];
			break;
		case 0x3:
			registers[x] ^= registers[y];
			break;
		case 0x4:
			registers[0xF] = 0;
			if (sum > 0xFF)
				registers[0xF] = 1;

			registers[x] = sum;
			break;
		case 0x5:
			registers[0xF] = 0;
			if (sub > 0)
				registers[0xF] = 1;
			registers[x] = sub;
			break;
		case 0x6: // move least significant bit to register 16, then shift the x register to the right
			registers[0xF] = registers[x] & 0x1;
			registers[x] >>= 1;
			break;
		case 0x7:
			registers[0xF] = 0;
			if (subn > 0)
				registers[0xF] = 1;
			break;
		case 0xE: // move most significant bit to register 16, then shift the x register to the left
			registers[0xF] = registers[x] & 0x80;
			registers[x] <<= 1;
			break;
		}

		break;
	case 0x9000:
		if (registers[x] != registers[y])
			programCounter += 2;
		break;
	case 0xA000:
		memAddress = opcode & 0xFFF;
		break;
	case 0xB000:
		programCounter = opcode & 0xFFF + registers[0];
		break;
	case 0xC000:
		registers[x] = r & (opcode & 0xFF);
		break;
	case 0xD000:
		registers[0xF] = 0;

		for (int row = 0; row < nibble; row++)
		{
			uint8_t sprite = memory[memAddress + row];
			for (int col = 0; col < 8 /* each sprite is 8 pixels wide */; col++)
			{
				if ((sprite & 0x80) > 0 && renderer->setPixel(registers[x] + col, registers[y] + row))
					registers[0xF] = 1;
				sprite <<= 1;
			}
		}
		break;
	case 0xE000:
		switch (opcode & 0xFF)
		{
		case 0x9E:
			if (keyboard->isKeyPressed(registers[x]))
				programCounter += 2;
			break;
		case 0xA1:
			if (!keyboard->isKeyPressed(registers[x]))
				programCounter += 2;
			break;
		}

		break;
	case 0xF000:
		switch (opcode & 0xFF)
		{
		case 0x07:
			registers[x] = delayTimer;
			break;
		case 0x0A:
			heldX = x;
			paused = true;
			break;
		case 0x15:
			delayTimer = registers[x];
			break;
		case 0x18:
			soundTimer = registers[x];
			break;
		case 0x1E:
			memAddress += registers[x];
			break;
		case 0x29:
			memAddress = registers[x] * 5;
			break;
		case 0x33:
			memory[memAddress]     = (uint8_t)floor(registers[x] / 100);
			memory[memAddress + 1] = (uint8_t)floor(registers[x] / 10) % 10;
			memory[memAddress + 1] = (uint8_t)floor(registers[x] / 100) % 10;
			break;
		case 0x55:
			for (int i = 0; i <= x; i++)
				memory[memAddress + i] = registers[i];
			break;
		case 0x65:
			for (int i = 0; i <= x; i++)
				registers[i] = memory[memAddress + i];
			break;
		}

		break;

	default:
		break;
	}
}

void CPU::updateTimers()
{
	if (delayTimer > 0)
		delayTimer -= 1;
	if (soundTimer > 0)
		soundTimer -= 1;
}

void CPU::playSound()
{
	if (soundTimer > 0)
		speaker->play();
	else
		speaker->stop();
}

vector<unsigned char> CPU::readFile(const char* filename)
{
	std::streampos fileSize;
	std::ifstream file(filename, std::ios::binary);

	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> fileData(fileSize);
	file.read((char*)&fileData[0], fileSize);

	return fileData;
}
vector<uint8_t> CPU::readFileU(const char* filename)
{
	// open the file:
	std::streampos fileSize;
	std::ifstream file(filename, std::ios::binary);

	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> fileData(fileSize);
	file.read((char*)&fileData[0], fileSize);

	std::vector<uint8_t> fileBytes;
	for (int i = 0; i < fileData.size(); i++)
		fileBytes.push_back(fileData[i]);

	return fileBytes;
}