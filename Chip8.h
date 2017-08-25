#ifndef CHIP8_H
#define CHIP8_H

#include <SFML/Graphics.hpp>

class Chip8{
    private:
        unsigned short opcode; //opcodes are 2 bytes long; unsigned short == 2 bytes
        unsigned char memory[4096]; //4k memory in total
        unsigned char V[16]; //15 8-bit CPU registers, named V0,V1, up to VE. 16th register is carry flag
        unsigned short I; //index register

        //Graphics are black and white and is 2048px(64x32)
        bool draw_flag;

        //If greater than 0, both counters count down at 60Hz(60 opcodes per second)
        unsigned char delay_timer;
        unsigned char sound_timer;

        //Certain opcodes requires a jump, so a stack needs to be implemented
        unsigned short stack[16]; //Chip-8 has 16 stack spaces
        unsigned short sp; //stack pointer
        unsigned short pc; //program counter
        
        void updateTimers();
        void init();

        bool drawFlag;
        unsigned char gfx[64*32];


    public:
        unsigned char key[16]; //hex based (0x0 - 0xF) keypad for input
        Chip8();
        ~Chip8();
        bool loadApplication(char const* filename);
        void emulateCycle();
        void setKeys();
        sf::Uint8* getGFX();
        bool isDraw();
};
#endif
