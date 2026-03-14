#include <cstdlib>
#include <print>
#include <raylib.h>
#include <chip8.hpp>

bool isRunning = true;

//Screen dimension constants
const int SCREEN_WIDTH = 64 * 8;
const int SCREEN_HEIGHT = 32 * 8;

int main(int argc, char **argv)
{
    Chip8 chip8;

    chip8.loadRom("test_opcode.ch8");

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hachi");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Update
        chip8.runCycle();

        // Drawing
        BeginDrawing();

        ClearBackground(WHITE);

        chip8.drawToScreen();

        EndDrawing();
    }

    return 0;
}