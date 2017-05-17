#ifndef CHIP8_H
#define CHIP8_H
class Chip8{
    private:
        unsigned short opcode; //opcodes are 2 bytes long; unsigned short == 2 bytes
        unsigned char memory[4096]; //4k memory in total
        unsigned char V[16]; //15 8-bit CPU registers, named V0,V1, up to VE. 16th register is carry flag
        unsigned short I; //index register
        unsigned short pc; //program counter

        //Graphics are black and white and is 2048px(64x32)
        unsigned char gfx[64 * 32];

        //If greater than 0, both counters count down at 60Hz(60 opcodes per second)
        unsigned char delay_timer;
        unsigned char sound_timer;

        //Certain opcodes requires a jump, so a stack needs to be implemented
        unsigned short stack[16]; //Chip-8 has 16 stack spaces
        unsigned short sp; //stack pointer
        unsigned short pc; //program counter

        unsigned char key[16]; //hex based (0x0 - 0xF) keypad for input

        void updateTimers();
    public:
        Chip8();
        void loadGame(string game);
        void emulateCycle();
        void setKeys();
};
#endif
