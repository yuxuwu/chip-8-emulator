#include "Chip8.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>

using namespace std;

unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0 
    0x20, 0x60, 0x20, 0x20, 0x70, //1 
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2 
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3 
    0x90, 0x90, 0xF0, 0x10, 0x10, //4 
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //5 
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6 
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8 
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9 
    0xF0, 0x10, 0x20, 0x40, 0x40, //7 
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A 
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B 
    0XF0, 0x80, 0x80, 0x80, 0xF0, //C 
    0xF0, 0x90, 0x90, 0x90, 0xE0, //D 
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E 
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F 
};

void Chip8::updateTimers(){
    if(delay_timer > 0){
        delay_timer--;
    }

    if(sound_timer > 0){
        if(sound_timer == 1)
            cout << "BEEP!" << endl;
        sound_timer--;
    }
}

Chip8::Chip8(){
}

Chip8::~Chip8(){
}

void Chip8::init(){

    cerr << "Initializing emulator" << endl;
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    //Clear display
    for(int i = 0; i < 2048; i++){
        gfx[i] = 0;
    }

    //Clear memory
    for(int i = 0; i < 4096; i++){
        memory[i] = 0;
    }

    //Clear key input
    for(int i = 0; i < 16; i++){
        key[i] = 0;
    }

    //Clear stack
    for(int i = 0; i < 16; i++){
        stack[i] = 0;
    }

    //Clear registers
    for(int i = 0; i < 16; i++){
        V[i] = 0;
    }

    //Set fontset
    for(int i = 0; i < 80; i++){
        memory[i] = fontset[i];
    }

    //Reset timers
    delay_timer = 0;
    sound_timer = 0;

    //Clear screen once
    drawFlag = true;

    srand(time(NULL));
}

void Chip8::emulateCycle(){
    //Fetch opcode
    opcode = memory[pc] << 8 | memory[pc+1]; //merge 2bytes in memory to 4byte opcode

    //Decode opcode
    switch(opcode & 0xF000){ //Get first letter

        case 0x0000:
            switch(opcode & 0x000F){
                case 0x0000: //0x00E0: Clears screen
                    for(int i = 0; i < 64 * 32; i++){
                        gfx[i] = 0x0; //turns bit off
                    } 
                    draw_flag = true;
                    pc += 2;
                break;

                case 0x000E: //0x00EE: Returns from subroutine
                    sp--;
                    pc = stack[sp]; //put stored address in stack back into pc
                    pc += 2;
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
            else
                pc += 2;
        break;

        case 0x4000: // 4XNN: Skips the next instruction if Vx != NN
            if(V[(opcode & 0x0F00) >> 8] != opcode & 0x00FF)
                pc += 4;
            else
                pc += 2;
        break;

        case 0x5000: // 5XY0: Skips the next instruction if Vx == Vy
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
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
            else
                pc += 2;
        break;

        case 0xA000: // ANNN: Sets I to the address of NNN
            I = opcode & 0x0FFF;
            pc += 2;
        break;

        case 0xB000: // BNNN: Jumps to the address NNN plus V0
            pc = V[0x0] + (opcode & 0x0FFF);
        break;

        case 0xC000: // CXNN: Sets Vx to the result of rand() & NN
            V[(opcode & 0x0F00) >> 8] = (unsigned short)rand() & (opcode & 0x00FF);
            pc += 2;
        break;

        case 0xD000:{ // DXYN: Draws a sprite at (Vx, Vy) with a width of 8 and height of N. 
                     //       Each row of 8 pixels is read as bit-coded starting from memory location I. 
                     //       I does not change after this instruction. 
                     //       VF is set to 1 if any pixels are flipped from set to unset and 0 if that doesn't happen
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for(int yLine = 0; yLine < height; yLine++){
                pixel = memory[I + yLine];
                for(int xLine = 0; xLine < 8; xLine++){
                    if((pixel & (0x80 >> xLine)) != 0){
                        if(gfx[(x + xLine + ((y + yLine) * 64))] == 1){
                            V[0xF] = 1;
                        }
                        gfx[x + xLine + ((y + yLine) * 64)] ^= 1;
                    }
                }
            }

            draw_flag = true;
            pc += 2;
        }
        break;

        case 0xE000:
            switch(opcode & 0x000F){
                case 0x000E: // EX9E: Skips next instruction if the key stored in Vx is pressed
                    if(key[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 4;
                    else
                        pc += 2;
                break;

                case 0x0001: // EXA1: Skips next instruction if the key stored in Vx isn't pressed
                    if(key[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 4;
                    else
                        pc += 2;
                break;
            }
        break;

        case 0xF000:
            switch(opcode & 0x00FF){
                case 0x0007: //FX07: Sets Vx to the value of the delay timer
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                break;

                case 0x000A:{ //FX0A: Halts until a key is pressed, then key is stored in Vx
                    bool key_press = false;

                    for(int i = 0; i < 16; i++){
                        if(key[i] != 0){
                            V[(opcode & 0x0F00) >> 8] = key_press;
                            key_press = true;
                        }
                    }

                    if(!key_press) return;
                    pc += 2;
                }
                break;

                case 0x0015: //FX15: Sets delay timer to Vx
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                break;

                case 0x0018: //FX18: Sets sound timer to Vx
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                break;

                case 0x001E: //FX1E: Adds Vx to I
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                break;

                case 0x0029: //FX29: Sets I to the locaton of the sprite for the character in Vx (Characters 0-F are represented by a 4x5 font
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                break;

                case 0x0033: //FX33: Stores the binary decimal representation of Vx
                             //      Most significant digits stored at address in I
                             //      Middle digit at I + 1
                             //      Least significant digit at I + 2
                             //      (Basically, hundreds at I, tens at I+1, ones at I+2)
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I+1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I+2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                break;

                case 0x0055:{ //FX55: Stores V0 to Vx (including V0) in memory starting at address I
                    int index = (opcode & 0x0F00) >> 8;
                    for(int i = 0; i < index; i++){
                        memory[I] = V[i];
                        I++;
                    }
                    pc += 2;
                }
                break;

                case 0x0065: //FX65: Fills V0 to Vx with values starting at address I
                    int index = (opcode & 0x0F00) >> 8;
                    for(int i = 0; i < index; i++){
                        V[i] = memory[I];
                        I++;
                    }
                    pc += 2;
                break;
            }
        break;

        default:
            cout << "Unknown opcode: 0x" << opcode << endl;
    }
    
    updateTimers();
}

bool Chip8::loadApplication(char const* filename){
    init();
    cerr << "Loading " << filename << endl;

    //Open file
    FILE *pFile = fopen(filename, "rb");
    if(pFile == NULL){
        cerr << "Error: file not found" << endl;
        return false;
    }

    //Check filesize
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);
    cerr << "Filesize: " << (int)lSize << endl;

    //Allocate memory to contain the whole file
    char * buffer = (char*)malloc(sizeof(char*) * lSize);
    if(buffer == NULL){
        cerr << "Error: memory error, could not allocate memory" << endl;
        return false;
    }

    //Copy the file into the buffer
    size_t result = fread(buffer, 1, lSize, pFile);
    if(result != lSize){
        cerr << "Error: Reading error" << endl;
        return false;
    }

    //Copy buffer into Chip8 Memory
    if((4096-512) > lSize){
        for(int i = 0; i < lSize; i++){
           memory[i + 512] = buffer[i];
        }
    }else{
        cerr << "Error: ROM too big for memory" << endl;
    }

    //Close file, free buffer
    fclose(pFile);
    free(buffer);

    cerr << "ROM loaded" << endl;
    return true;
}
