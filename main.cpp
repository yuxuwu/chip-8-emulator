#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <map>

#include "Chip8.h"

using namespace std;

//Chip8 chip8;

#define WIDTH 64
#define HEIGHT 32

Chip8 chip8;

vector<sf::Uint8> transformToPix(vector<sf::Uint8> pixels, vector<unsigned char> gfx);
void detectKeyPress(sf::Event event);

int main(int argc, char ** argv) {
    if(argc < 2){
        cout << "Usage: ./Chip8-Emu \"Chip8_Rom_File\"" << endl;
        return 1;
    }

    // //Setup keybinding maps
    // keyBindings[sf::Keyboard::X] = 0x0;
    // keyBindings[sf::Keyboard::Num1] = 0x1;
    // keyBindings[sf::Keyboard::Num2] = 0x2;
    // keyBindings[sf::Keyboard::Num3] = 0x3;
    // keyBindings[sf::Keyboard::Q] = 0x4;
    // keyBindings[sf::Keyboard::W] = 0x5;
    // keyBindings[sf::Keyboard::E] = 0x6;
    // keyBindings[sf::Keyboard::A] = 0x7;
    // keyBindings[sf::Keyboard::S] = 0x8;
    // keyBindings[sf::Keyboard::D] = 0x9;
    // keyBindings[sf::Keyboard::Z] = 0xA;
    // keyBindings[sf::Keyboard::C] = 0xB;
    // keyBindings[sf::Keyboard::Num4] = 0xC;
    // keyBindings[sf::Keyboard::R] = 0xD;
    // keyBindings[sf::Keyboard::F] = 0xE;
    // keyBindings[sf::Keyboard::V] = 0xF;

    //Load game
    if(!chip8.loadApplication(argv[1])){
        cout << "Rom file " << argv[1] << " not found" << endl;
        return 1;
    }

    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Chip8-Emu");
    sf::Sprite sprite;
    sf::Texture texture;
    vector<sf::Uint8> pixels(WIDTH*HEIGHT*4);

    if (!texture.create(WIDTH, HEIGHT))
        std::cout << "Error: texture could not be created" << std::endl;

    while(window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event)) {
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
            }
        }

        //Detect key presses and update chip8.keys[]
        detectKeyPress(event);

        //Update texture
        chip8.emulateCycle();
        if(chip8.drawFlag){
            pixels = transformToPix(chip8.getGFX(), pixels);
            texture.update(&pixels[0]);

            //Update sprite with texture
            sprite.setTexture(texture);

            //Clear window
            window.clear(sf::Color::Black);

            //Draw everything
            window.draw(sprite);
            window.display();
            chip8.drawFlag = false;
        }
        
    }
    return 0;
}

vector<sf::Uint8> transformToPix(vector<sf::Uint8> gfx, vector<sf::Uint8> pixels){
    vector<sf::Uint8> new_pixels(WIDTH*HEIGHT*4);
    for(int i = 0; i < WIDTH*HEIGHT; i++) {
        if(gfx[i]){
            new_pixels[i*4] = 255;   //R
            new_pixels[i*4+1] = 255; //G
            new_pixels[i*4+2] = 255; //B
            new_pixels[i*4+3] = 255; //A
        }else{
            new_pixels[i*4] = 0;
            new_pixels[i*4+1] = 0;
            new_pixels[i*4+2] = 0; 
            new_pixels[i*4+3] = 255;
        }
    }
    return new_pixels;
}

void detectKeyPress(sf::Event event){  
    if(event.type == sf::Event::KeyPressed){
        chip8.keys[0x0] = event.key.code == sf::Keyboard::X;
        chip8.keys[0x1] = event.key.code == sf::Keyboard::Num1;
        chip8.keys[0x2] = event.key.code == sf::Keyboard::Num2;
        chip8.keys[0x3] = event.key.code == sf::Keyboard::Num3;
        chip8.keys[0x4] = event.key.code == sf::Keyboard::Q;
        chip8.keys[0x5] = event.key.code == sf::Keyboard::W;
        chip8.keys[0x6] = event.key.code == sf::Keyboard::E;
        chip8.keys[0x7] = event.key.code == sf::Keyboard::A;
        chip8.keys[0x8] = event.key.code == sf::Keyboard::S;
        chip8.keys[0x9] = event.key.code == sf::Keyboard::D;
        chip8.keys[0xA] = event.key.code == sf::Keyboard::Z;
        chip8.keys[0xB] = event.key.code == sf::Keyboard::C;
        chip8.keys[0xC] = event.key.code == sf::Keyboard::Num4;
        chip8.keys[0xD] = event.key.code == sf::Keyboard::R;
        chip8.keys[0xE] = event.key.code == sf::Keyboard::F;
        chip8.keys[0xF] = event.key.code == sf::Keyboard::V;
    }
    if(event.type == sf::Event::KeyReleased){
        chip8.keys[0x0] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::X;
        chip8.keys[0x1] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::Num1;
        chip8.keys[0x2] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::Num2;
        chip8.keys[0x3] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::Num3;
        chip8.keys[0x4] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::Q;
        chip8.keys[0x5] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::W;
        chip8.keys[0x6] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::E ;
        chip8.keys[0x7] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::A;
        chip8.keys[0x8] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::S;
        chip8.keys[0x9] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::D;
        chip8.keys[0xA] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::Z;
        chip8.keys[0xB] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::C;
        chip8.keys[0xC] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::Num4;
        chip8.keys[0xD] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::R;
        chip8.keys[0xE] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::F;
        chip8.keys[0xF] = chip8.keys[0x0] && !event.key.code == sf::Keyboard::V;
    }

}