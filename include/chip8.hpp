#pragma once

#include <cstdint>
#include <fstream>

class Chip8
{
    public:
        Chip8();
        ~Chip8();

        void loadRom(const char *filename);
        void runCycle();
        void processOpcode();
        void drawToScreen();

        // Opcodes

        // Clear the screen
        void OP_00E0();

        // Return from subroutine
        void OP_00EE();

        // Jump to address NNN
        void OP_1NNN();

        // Execute subroutine starting at NNN
        void OP_2NNN();

        // Skip the following instruction if the value of VX is equal to NN
        void OP_3XNN();

        void OP_4XNN();

        void OP_5XY0();

        // Store value NN in VX
        void OP_6XNN();

        void OP_7XNN();
        
        void OP_8XY0();
        void OP_8XY1();
        void OP_8XY2();
        void OP_8XY3();
        void OP_8XY4();
        void OP_8XY5();
        void OP_8XY6();
        void OP_8XY7();
        void OP_8XYE();

        // Skip the following instruction if the value of VX is not equal to the value of VY
        void OP_9XY0();

        void OP_ANNN();
        void OP_BNNN();
        void OP_CXNN();
        void OP_DXYN();
    private:
        uint8_t registers[16] = {};
        uint8_t memory[4096] = {};
        uint16_t stack[16] = {};
        uint16_t i{};
        uint16_t pc{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        
        // TODO: Use a uint16_t instead
        uint8_t keypad[16]{};
        bool video[64 * 32] {};
        uint16_t opcode{};
};