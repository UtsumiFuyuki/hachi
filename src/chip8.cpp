#include <print>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <fstream>
#include <random>
#include <chip8.hpp>
#include <raylib.h>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_SIZE = 80;

const unsigned int FONT_SET_ADDRESS = 0x50;

uint8_t fontSet[FONT_SIZE]
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

Chip8::Chip8()
{
    pc = START_ADDRESS;

    for (size_t i = 0; i < FONT_SIZE; ++i)
    {
        memory[FONT_SET_ADDRESS + i] = fontSet[i];
    }
}

Chip8::~Chip8()
{

}

void Chip8::loadRom(const char *filename)
{
    std::ifstream file (filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        std::streampos size = file.tellg();
        char *buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (size_t i = 0; i < size; ++i)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
}

void Chip8::runCycle()
{
    opcode = memory[pc];
    std::printf("opcode 0x%X\n", opcode);

    opcode = opcode << 8;

    opcode = opcode | memory[pc + 1];

    std::printf("opcode again :3 0x%X\n", opcode);

    // PC needs to be incremented before we preform the opcode because
    // some instructions modify the PC register
    pc += 2;

    if (pc > 4095)
    {
        pc = 4094;
        std::println("CLAMP!");
    }

    processOpcode();
}

void Chip8::processOpcode()
{
    if (opcode == 0x00E0)
    {
        std::println("Clearing screen!");
        OP_00E0();
    }
    else if (opcode == 0x00EE)
    {
        std::println("Returning from subroutine!");
        OP_00EE();
    }

    switch (opcode & 0xF000)
    {
        case (0x0000):
        {
            if (opcode == 0x00E0)
            {
                OP_00E0();
            }

            else if (opcode == 0x00EE)
            {
                OP_00EE();
            }
            break;
        }
        case (0x1000):
        {
            OP_1NNN();
            break;
        }

        case (0x2000):
        {
            OP_2NNN();
            break;
        }

        case (0x3000):
        {
            OP_3XNN();
            break;
        }

        case (0x4000):
        {
            OP_4XNN();
            break;
        }

        case (0x5000):
        {
            OP_5XY0();
            break;
        }

        case (0x6000):
        {
            std::println("Load a register with value!");

            OP_6XNN();
            break;
        }

        case (0x7000):
        {
            OP_7XNN();
        }

        case (0x8000):
        {
            switch(opcode & 0x000F)
            {
                case (0x0):
                {
                    OP_8XY0();
                    break;
                }

                case (0x1):
                {
                    OP_8XY1();
                    break;
                }

                case (0x2):
                {
                    OP_8XY2();
                    break;
                }

                case (0x3):
                {
                    OP_8XY3();
                    break;
                }

                case (0x4):
                {
                    OP_8XY4();
                    break;
                }

                case (0x5):
                {
                    OP_8XY5();
                    break;
                }

                case (0x6):
                {
                    OP_8XY6();
                    break;
                }

                case (0x7):
                {
                    OP_8XY7();
                    break;
                }

                case (0xE):
                {
                    OP_8XYE();
                    break;
                }
            }

            case (0x9000):
                OP_9XY0();
                break;
            
            case (0xA000):
                OP_ANNN();
                break;
            
            case (0xB000):
                OP_BNNN();
                break;

            case (0xC000):
                OP_CXNN();
                break;
            
            case (0xD000):
                OP_DXYN();
                break;
        }

        default:
            std::println("Unimplemented!");
    }
}

void Chip8::drawToScreen()
{
    int y = 0;
    int x = 0;
    int idx = 0;
    while (y < 32)
    {
        if (x % 64 == 0 && x != 0) {
            y++;
            x = 0;
        }
        DrawRectangle(x * 8, y * 8, 8, 8, video[idx] ? RAYWHITE : BLACK);
        x++;
        idx++;
    }
}

// Opcodes

// Clear Screen
void Chip8::OP_00E0()
{
    memset(video, 0, sizeof(video));
}

// Return from subroutine
void Chip8::OP_00EE()
{
    pc = stack[sp];
    sp--;
}

// Jump to address
void Chip8::OP_1NNN()
{
    uint16_t address = opcode & 0x0FFF;
    pc = address;
}

// Call subroutine
void Chip8::OP_2NNN()
{
    uint16_t address = opcode & 0x0FFF;
    stack[sp] = pc;
    sp++;

    pc = address;
}

void Chip8::OP_3XNN()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    if (registers[vx] == value)
    {
        pc += 2;
    }
}

void Chip8::OP_4XNN()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    if (registers[vx] != value)
    {
        pc += 2;
    }
}

void Chip8::OP_5XY0()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    if (registers[vx] == registers[vy])
    {
        pc += 2;
    }
}

void Chip8::OP_6XNN()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;

    assert(vx < 16);

    registers[vx] = value;

    std::println("Register V{} set to {}", vx, value);
}

void Chip8::OP_7XNN()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t value = opcode & 0x00FF;
    
    registers[vx] += value;
}

void Chip8::OP_8XY0()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    registers[vx] = registers[vy];
    std::println("Stored value in V{} in V{}", vy, vx);
}

void Chip8::OP_8XY1()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    registers[vx] |= registers[vy];
}

void Chip8::OP_8XY2()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    registers[vx] &= registers[vy];
}

void Chip8::OP_8XY3()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    registers[vx] ^= registers[vy];
}

void Chip8::OP_8XY4()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    uint8_t sum = registers[vx] + registers[vy];
    if (sum > vx)
    {
        registers[0xF] = 1;
    }

    else
    {
        registers[0xF] = 0;
    }

    registers[vx] = sum;
}

void Chip8::OP_8XY5()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    if (registers[vy] > registers[vx])
    {
        registers[0xF] = 0;
    }

    else
    {
        registers[0xF] = 1;
    }
    registers[vx] -= registers[vy];
}

void Chip8::OP_8XY6()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    registers[0xF] = registers[vx] & 0x1;
    registers[vx] = registers[vy] >> 1;
}

void Chip8::OP_8XY7()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    if (registers[vx] > registers[vy])
    {
        registers[0xF] = 0;
    }

    else
    {
        registers[0xF] = 1;
    }
    registers[vx] = registers[vy] - registers[vx];
}

void Chip8::OP_8XYE()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    registers[0xF] = registers[vy] >> 7;

    registers[vx] = registers[vy] << 1;
}

void Chip8::OP_9XY0()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;

    if (registers[vx] != registers[vy])
    {
        pc += 2;
    }
}

void Chip8::OP_ANNN()
{
    uint16_t address = opcode & 0x0FFF;
    i = address;
}

void Chip8::OP_BNNN()
{
    pc = (opcode & 0x0FFF) + registers[0];
}

void Chip8::OP_CXNN()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t mask = opcode & 0x00FF;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 15);

    registers[vx] = distrib(gen) & mask;
}

void Chip8::OP_DXYN()
{
    uint8_t vx = (opcode & 0x0F00) >> 8;
    uint8_t vy = (opcode & 0x00F0) >> 4;
    uint8_t spriteData = opcode & 0x000F;

    if (vx > 0x3F)
    {
        registers[vx] %= 64;
    }

    if (vy > 0x1F)
    {
        registers[vy] %= 32;
    }

    uint16_t idx = i;

    uint8_t x = registers[vx];
    uint8_t y = registers[vy];

    registers[0xF] = 0;

    while (idx < (i + spriteData))
    {
        for (int k = 7; k >= 0; k--)
        {
            std::println("Bit {} of sprite data is {}", k, (memory[idx] >> k) & 0x1);
            video[x + (y * 64)] = ((memory[idx] >> k) & 0x1);

            if (!video[x + (y * 64)])
            {
                // Pixel was changed to unset, set VF to 1
                registers[0xF] = 1;
            }

            x++;
        }
        x = registers[vx];
        y++;

        idx++;
    }

    std::println("DRAW!!!!!");
}