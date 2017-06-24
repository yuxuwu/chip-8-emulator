#ifndef CHIP8_H
#define CHIP8_H

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

        unsigned char fontset[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, //0 
            0x20, 0x60, 0x20, 0x20, 0x70, //1 
            0xF0, 0x10, 0xF0, 0x80, 0xF0, //2 
            0xF0, 0x10, 0xF0, 0x10, 0xF0, //3 
            0x90, 0x90, 0xF0, 0x10, 0x10, //4 
            0xF0, 0x80, 0xF0, 0x90, 0xF0, //5 
            0xF0, 0x80, 0xF0, 0x90, 0xF0, //6 
            0xF0, 0x10, 0x20, 0x40, 0x40, //7 
            0xF0, 0x90, 0xF0, 0x90, 0xF0, //8 
            0xF0, 0x90, 0xF0, 0x10, 0xF0, //9 
            0xF0, 0x90, 0xF0, 0x90, 0x90, //A 
            0xE0, 0x90, 0xE0, 0x90, 0xE0, //B 
            0XF0, 0x80, 0x80, 0x80, 0xF0, //C 
            0xF0, 0x90, 0x90, 0x90, 0xE0, //D 
            0xF0, 0x80, 0xF0, 0x80, 0xF0, //E 
            0xF0, 0x80, 0xF0, 0x80, 0x80  //F 
        };

        void updateTimers();

    public:

        bool drawFlag;
        unsigned char gfx[64*32];
        unsigned char key[16]; //hex based (0x0 - 0xF) keypad for input

        Chip8();
        bool loadApplication(const char * filename);
        void emulateCycle();
        void setKeys();
};
#endif
