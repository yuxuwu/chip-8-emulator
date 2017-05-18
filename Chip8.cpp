#include <iostream>
#include "Chip8.h"

using namespace std;

void Chip8::updateTimers(){
    if(delay_timer > 0){
        delay_timer--;
    }

    if(sound_timer > 0){
        if(sound_timer == 1)
            cout << "BEEP! << endl;
        sound_timer--;
    }
}

Chip8::Chip8(){
    /* set all registers to zero, clear all memory */

    opcode = 0; //Reset current opcode
    I = 0; //Reset index register
    sp = 0; //Reset stack pointer
    pc = 0x200; //Program counter starts at 0x200

    //Clear display
    //Clear stack
    //Clear registers V0-VF
    //Clear memory
    
    //Load fontset
    for(int i = 0; i < 80; ++i){
        //memory[i] = chip8_fontset[i]; //TODO: implement chip8_fontset[]
    }

    //Reset timers
    
}

/*
void Chip8::loadGame(){
    for(int i = 0; i < bufferSize; ++i){
        memory[i+512] = buffer[i]; //TODO: load game into memory
    }
}
*/

void Chip8::emulateCycle(){
    //Fetch opcode
    opcode = memory[pc] << 8 | memory[pc+1]; //merge 2bytes in memory to 4byte opcode
    //Decode opcode TODO: implement opcodes
    switch(opcode & 0xF000){ //Get first letter

        case 0x0000:
            switch(opcode & 0x000F){
                case 0x0000: //0x00E0: Clears screen
                break;

                case 0x000E: //0x00EE: Returns from subroutine
                break;

                default:
                    cout << "Unknown opcode [0x000]: 0x" << opcode << endl;
            }
        break;

        case 0x1000: // 1NNN: Jumps to address NNN
            pc = opcode & 0x0FFF;
        break;

        case 0x2000: // 2NNN: Calls subroutine at NNN
            stack[sp] = pc;
            sp++;
            pc = opcode & 0x0FFF;
        break;

        case 0x3000: // 3XNN: Skips next instruction if Vx == NN
            if(V[(opcode & 0x0F00) >> 8] == opcode & 0x00FF)
                pc += 4;
        break;

        case 0x4000: // 4XNN: Skips the next instruction if Vx != NN
            if(V[(opcode & 0x0F00) >> 8] != opcode & 0x00FF)
                pc += 4;
        break;

        case 0x5000: // 5XY0: Skips the next instruction if Vx == Vy
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                pc += 4;
        break;

        case 0x6000: // 6XNN: Sets Vx to NN
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc+=2;
        break;

        case 0x7000: // 7XNN: Adds NN to Vx
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc+=2;
        break;

        case 0x8000:
            switch(opcode & 0x000F){
                case 0x0000: // 8XY0: Sets Vx to the value of Vy
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                break;

                case 0x0001: // 8XY1: Sets Vx to Vx | Vy
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                break;

                case 0x0002: // 8XY2: Sets Vx to Vx & Vy
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                break;

                case 0x0003: // 8XY3: Sets Vx to Vx ^(xor) Vy
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                break;

                case 0x0004: // 8XY4: Adds Vy to Vx. If there is a carry, set VF (carry bit) to 1
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1;
                    else 
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0005: // 8XY5: Subtracts Vy from Vx. If there is a borrow, set VF to 0, and 1 if there isn't
                    if(V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])
                        V[0xF] = 0;
                    else 
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                break;

                case 0x0006: // 8XY6: Shifts Vx right by 1. VF is set to the least significant bit of Vx before the shift
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x000F;
                    V[(opcode & 0x0F00) >> 8] >> 1;
                    pc += 2;
                break;

                case 0x0007: // 8XY7: Sets Vx to Vy - Vx. If there is a borrow, set VF to 0, and 1 if there isn't
                    if(V[(opcode & 0x00F0) >> 4] < V[(opcode & 0x0F00) >> 8])
                        V[0xF] = 0;
                    else 
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                break;

                case 0x000E: // 8XYE: Shifits Vx left by 1. VF is set to the most significant bit of Vx before the shift
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0xF000;
                    V[(opcode & 0x0F00) >> 8] << 1;
                    pc += 2;
                break;

                default:
                    cout << "Unknown opcode [0x000]: 0x" << opcode << endl;
            }
        break;

        case 0x9000: // 9XY0: Skips the next instruction if Vx != Vy
            if(V[(opcode & 0x0F00) >> 8] !=  V[(opcode & 0x00F0) >> 4])
                pc += 4;
        break;

        case 0xA000: // ANNN: Sets I to the address of NNN
            I = opcode & 0x0FFF;
            pc += 2;
        break;

        case 0xB000: // BNNN: Jumps to the address NNN plus V0
        break;

        case 0xC000: // CXNN: Sets Vx to the result of rand() & NN
        break;

        case 0xD000: // DXYN: Draws a sprite at (Vx, Vy) with a width of 8 and height of N. 
                     //       Each row of 8 pixels is read as bit-coded starting from memory location I. 
                     //       I does not change after this instruction. 
                     //       VF is set to 1 if any pixels are flipped from set to unset and 0 if that doesn't happen
        break;

        case 0xE000:
            switch(opcode & 0x000F){
                case 0x000E: // EX9E: Skips next instruction if the key stored in Vx is pressed
                break;

                case 0x0001: // EXA1: Skips enxt instruction if the key sotred in Vx isn't pressed
                break;
            }
        break;

        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007: //FX07: Sets Vx to the value of the delay timer
                break;

                case 0x000A: //FX0A: Halts until a key is pressed, then key is stored in Vx
                break;

                case 0x0015: //FX15: Sets delay timer to Vx
                break;

                case 0x0018: //FX18: Sets sound timer to Vx
                break;

                case 0x001E: //FX1E: Adds Vx to I
                break;

                case 0x0029 //FX29: Sets I to the locaton of the sprite for the character in Vx (Characters 0-F are represented by a 4x5 font
                break;

                case 0x0033 //FX33: Stores the binary decimal representation of Vx
                            //      Most significant digits stored at address in I
                            //      Middle digit at I + 1
                            //      Least significant digit at I + 2
                            //      (Basically, hundreds at I, tens at I+1, ones at I+2)
                break;

                case 0x0055 //FX55: Stores V0 to Vx (including V0) in memory starting at address I
                break;

                case 0x0065 //FX65: Fills V0 to Vx with values starting at address I
                break;
            }
        break;

        case default:
            cout << "Unknown opcode: 0x" << opcode << endl;
    }
    
    updateTimers();
}

void Chip8::setKeys(){
}
