#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <typeinfo>

#include "Chip8.h"

#define WIDTH 64
#define HEIGHT 32

using namespace std;

const unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
    0xF0, 0x80, 0xF0, 0x80, 0x80	// F
};

void Chip8::updateTimers(){
    if(delay_timer > 0)
        delay_timer--;

    if(sound_timer > 0){
        if(sound_timer == 1)
            cerr << "BEEP!" << endl;
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
    for(int i = 0; i < (WIDTH*HEIGHT); i++)
        gfx.push_back(0);

    //Clear memory
    for(int i = 0; i < 4096; i++)
        memory[i] = 0;

    //Load fontset
    for(int i = 0; i < 80; i++)
        memory[i] = fontset[i];

    //Clear stack
    for(int i = 0; i < 16; i++)
        stack[i] = 0;

    //Clear registers
    for(int i = 0; i < 16; i++)
        V[i] = 0;

    //Clear key
    for(int i = 0; i < 16; i++)
        keys[i] = 0;

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
    
    cerr << "0x" << hex << opcode << endl;
    cerr << "PC: " << pc << endl;
    cerr << "Stack position: " << sp << endl;

    //Decode opcode
    switch(opcode & 0xF000){
        
        case 0x0000: {
            switch (opcode & 0x000F){
                //Opcode: 0x00E0 Clear the screen
                case 0x0000:
                
                    for(int i = 0; i < (WIDTH*HEIGHT); i++){
                        gfx[i] = 0;
                    }
                    drawFlag = true;
                    
                    pc+=2;
                break;

                //Opcode: 0x00EE Return from subroutine
                case 0x000E:
                    cerr << "Returning from subroutine" << endl;
                    sp--;
                    pc = stack[sp];
                    pc+=2;
                    cerr << "New pc: " << pc << endl;
                break;
            }
            break;
        }

        //Opcode 0x1NNN Jumps to address NNN
        case 0x1000:
            pc = opcode & 0x0FFF;
        break;
        
        //Opcode 0x2NNN Calls subroutine at NNN
        case 0x2000:
            cerr << "Calling subroutine at 0x" << hex << (opcode & 0x0FFF) << endl;
            //Save the current address
            cerr <<"Saving current PC: " << pc << endl;
            stack[sp] = pc;
            //Push down stack
            sp++;
            //Set program counter to new address
            pc = (opcode & 0x0FFF);
        break;

        //Opcde 0x3XNN Skips the next instruction if 
        //Vx == NN (increase stack pointer by 2)
        case 0x3000:
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc+=2;
            pc+=2;
        break;

        //Opcode 0x4XNN Skips the next instruction if 
        //VX doesn't equal NN.
        case 0x4000:
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc+=2;
            pc+=2;
        break;

        //Opcode 0x5XYO Skips the next instruction if 
        //VX equals VY
        case 0x5000:
            if(V[(opcode & 0x0F00) >> 8] == V[opcode & 0x00F0])
                pc+=2;
            pc+=2;
        break;

        //Opcode 0x6XNN Sets VX to NN
        case 0x6000:
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc+=2;
        break;

        //Opcode 0x7XNN Adds NN to Vx
        case 0x7000:
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc+=2;
        break;

        //Opcodes 0x8000
        case 0x8000: {
            switch (opcode & 0x000F){
                //Opcode 0x8XY0 Sets VX to the value of VY
                case 0x0000:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                break;

                //Opcode 0x8XY1 Sets VX to VX OR with VY
                case 0x0001:
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                break;

                //Opcode 0x8XY2 Sets VX to VX AND with VY
                case 0x0002:
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                break;

                //Opcode 0x8XY3 Sets VX to VX XOR with VY
                case 0x0003:
                    V[(opcode & 0x0F00) >> 8] ^= V[opcode & 0x00F0] >> 4;
                    pc+=2;
                break;

                //Opcode 0x8XY4 Adds VY to VX. VF is set 
                //to 1 when there's a carry, and to 0 when there isn't
                case 0x0004: {
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1; //carry
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                }
                break;

                //Opcode 0x8XY5 VY is subtracted from VX. VF is set to 0 
                //when there's a borrow, and 1 when there isn't
                case 0x0005: {
                    if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) 
                        V[0xF] = 0; //there is a borrow
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc+=2;
                }
                break;

                //Opcode 0x8XY6  Shifts VX right by one. VF is set to the 
                //value of the least significant bit of VX before the shift
                case 0x0006:
                    V[15] = V[(opcode & 0x0F00) >> 8] & 0x0001;
                    V[(opcode & 0x0F00) >> 8] >>= 1; 
                    pc+=2;
                break;

                //Opcode 0x8XY7 Sets VX to VY minus VX. VF is set to 
                //0 when there's a borrow, and 1 when there isn't
                case 0x0007: { 
                    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])   // VY-VX
                        V[0xF] = 0; // there is a borrow
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];                 
                    pc+=2;
                }
                break;

                //Opcode 0x8XYE Shifts VX left by one. VF is set to the 
                //value of the most significant bit of VX before the shift
                case 0x000E:
                    V[15] = V[(opcode & 0x0F00) >> 8] & 0x8000;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc+=2;
                break;
            }
        }
        break;

        //Opcode 0x9XY0 Skips the next instruction if VX doesn't 
        //equal VY
        case 0x9000:
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc+=2;
            pc+=2;
        break;

        //Opcode 0xANNN Sets I to the address NNN
        case 0xA000:
            I = opcode & 0x0FFF;
            pc+=2;
        break;

        //Opcode 0xBNNN Jumps to the address NNN plus V0
        case 0xB000:
            pc = (opcode & 0x0FFF) + V[0];
        break;

        //Opcode 0xCXNN Sets VX to the result of a 
        //bitwise and operation on a random number
        case 0xC000:
            V[(opcode & 0x0F00) >> 8]  = (opcode & 0x00FF) & (unsigned char)(rand()%256);
            pc+=2;
        break; 

        //Opcode 0xDXYN Draws a sprite at coordinate (VX, VY) 
        //that has a width of 8 pixels and a height of N pixels.
        //Each row of 8 pixels is read as bit-coded starting
        //from memory location I; I value doesn’t change after 
        //the execution of this instruction. As described above, 
        //VF is set to 1 if any screen pixels are flipped from set
        //to unset when the sprite is drawn, and to 0 if that doesn’t
        //happen
        case 0xD000: {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;
            cerr << "Location of I: " << I << endl;
            cerr << "X: " << x << " Y: " << y << " Height: " << height << endl;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++){
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++){
                    if((pixel & (0x80 >> xline)) != 0){
                        if(gfx[(x + xline + ((y + yline) * 64))] == 1){
                            V[0xF] = 1;                                    
                        }
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc+=2;
        }
        break;

        case 0xE000: {
            switch (opcode & 0x000F){
                //Opcode 0xEX9E Skips the next instruction if 
                //the key stored in VX is pressed
                case 0x000E:
                    if(keys[V[(opcode & 0x0F00) >> 8]]){
                        pc+=2; 
                        cout << "Key was pressed" << endl;
                    }
                    pc+=2;
                break;

                //Opcode 0xEXA1 Skips the next instruction if
                //the key stored in VX isn't pressed
                case 0x0001:
                    if(!keys[V[(opcode & 0x0F00) >> 8]]){
                        // cout << "Key " << V[(opcode & 0x0F00) >> 8] << "was not presssed" << endl;
                        pc+=2;
                    }
                    pc+=2;
                break;
            }
        break;
        }

        case 0xF000: {
            switch(opcode & 0x00FF){
                //Opcode 0xFX07 Sets VX to the value of the delay timer
                case 0x0007: 	
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc+=2; 
                break;

                //Opcode 0xFX0A A key press is awaited, and then stored in VX
                case 0x000A: {
                    bool key_pressed = false;
                    // cout << "Waiting for a key press..." << endl;
                    for(unsigned char i = 0; i < 0xF; i++){
                        if(keys[i]){
                            cout << "Key was pressed: Chip8 Emu" << endl;
                            V[(opcode & 0x0F00) >> 8] = i;
                            key_pressed=true;
                        }
                    }

                    //if a key was not pressed, return and try the cycle again
                    if(!key_pressed)
                        return;

                    pc+=2;
                break;
                }

                //Opcode 0xFX15 Sets the delay timer to Vx
                case 0x0015:
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc+=2;
                break;

                //Opcode 0xFX18 Sets the sound timer to Vx
                case 0x0018:
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc+=2;
                break;

                //Opcode 0xFX1E Adds Vx to I
                //VF is set to 1 when there is a range overflow (4095), 0 if there isn't
                case 0x001E:
                    if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;

                    I += V[(opcode & 0x0F00) >> 8];
                    pc+=2;
                break;

                //Opcode 0xFX29 Sets I to the location of the 
                //sprite for the character in Vx. Characters 0-F 
                //(in hexadecimal) are represented by a 4x5 font
                //Characters are stored in memory locations 0x0-0x80
                case 0x0029:
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc+=2;
                break;

                //Opcode 0xFX33 Stores the binary representation
                //of Vx in three chunks at I, I+1, and I+2
                case 0x0033:
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I+1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I+2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc+=2;
                break;

                //Opcode 0xFX55 Stores V0 to VX (including VX) in 
                //memory starting at address I
                case 0x0055:
                    for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                        memory[I+i] = V[i];

                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc+=2;
                break;

                //Opcode 0xFX65 Fills V0 to VX (including VX) with 
                //values from memory starting at address I
                case 0x0065:
                    for(int i = 0; i < (opcode & 0x0F00 >> 8)+1; i++)
                        V[i] = memory[I+i];

                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc+=2;
                break;
            }
        break;
        }

        default:
            cerr << "Unknown opcode 0x" << hex << opcode << endl;
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

vector<sf::Uint8> Chip8::getGFX(){
    vector<sf::Uint8> pixels(WIDTH * HEIGHT * 4);
    for(int i = 0; i < WIDTH*HEIGHT*4; i++) {
        pixels[i] = gfx[i];
        pixels[i+1] = gfx[i];
        pixels[i+2] = gfx[i];
        pixels[i+3] = gfx[i];
    }
    return pixels;
}