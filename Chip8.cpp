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
        break;

        case 0x2000: // 2NNN: Calls subroutine at NNN

        case 0xA000: // ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;
            pc += 2;
        break;

        default:
            cout << "Unknown opcode: 0x" << opcode << endl;
    }
    
    updateTimers();
}

void Chip8::setKeys(){
}
