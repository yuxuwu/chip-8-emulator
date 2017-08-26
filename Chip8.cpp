#include "Chip8.h"

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#define WINSIZE_X 64
#define WINSIZE_Y 32

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
    
    cout << opcode << endl;

    //Decode opcode
    switch(opcode & 0xF000){ //Get first letter
    }

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

sf::Uint8* Chip8::getGFX(){
    sf::Uint8* pixels = new sf::Uint8[WINSIZE_X * WINSIZE_Y * 4];
    for(int i = 0; i < WINSIZE_X*WINSIZE_Y*4; i+=4) {
        pixels[i] = gfx[i/4];
        pixels[i+1] = gfx[i/4];
        pixels[i+2] = gfx[i/4];
        pixels[i+3] = gfx[i/4];
    }
    return pixels;
}

bool Chip8::isDraw(){
    return drawFlag;
}