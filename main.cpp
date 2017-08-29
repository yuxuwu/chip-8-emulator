#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "Chip8.h"

using namespace std;

//Chip8 chip8;

#define WIDTH 64
#define HEIGHT 32

Chip8 chip8;

vector<sf::Uint8> transformToPix(vector<sf::Uint8> pixels, vector<unsigned char> gfx);
void detectKeyPress();

int main(int argc, char ** argv) {
    chip8.init();
    if(argc < 2){
        cout << "Usage: ./Chip8-Emu \"Chip8_Rom_File\"" << endl;
        return 1;
    }

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
    vector<unsigned char> gfx = vector<unsigned char>(WIDTH*HEIGHT, 0);

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
        detectKeyPress();

        //Update texture
        //emulateCycle(gfx, position, offset);
        chip8.emulateCycle();
        //pixels = getGFX(pixels, gfx);
        pixels = transformToPix(chip8.getGFX(), pixels);
        texture.update(&pixels[0]);

        //Update sprite with texture
        sprite.setTexture(texture);

        //Clear window
        window.clear(sf::Color::Black);

        //Draw everything
        window.draw(sprite);
        window.display();
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

void detectKeyPress(){
    chip8.keys[0x0] = sf::Keyboard::isKeyPressed(sf::Keyboard::X);
    chip8.keys[0x1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
    chip8.keys[0x2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
    chip8.keys[0x3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);
    chip8.keys[0x4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
    chip8.keys[0x5] = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    chip8.keys[0x6] = sf::Keyboard::isKeyPressed(sf::Keyboard::E);
    chip8.keys[0x7] = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    chip8.keys[0x8] = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    chip8.keys[0x9] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    chip8.keys[0xA] = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);
    chip8.keys[0xB] = sf::Keyboard::isKeyPressed(sf::Keyboard::C);
    chip8.keys[0xC] = sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);
    chip8.keys[0xD] = sf::Keyboard::isKeyPressed(sf::Keyboard::R);
    chip8.keys[0xE] = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
    chip8.keys[0xF] = sf::Keyboard::isKeyPressed(sf::Keyboard::V);
}