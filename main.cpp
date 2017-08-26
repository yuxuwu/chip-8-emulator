#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <unistd.h>
#include "Chip8.h"

using namespace std;

//Chip8 chip8;

#define WINSIZE_X 64
#define WINSIZE_Y 32

vector<unsigned char> emulateCycle(vector<unsigned char>& gfx, int position, int offset);
vector<sf::Uint8> getGFX(vector<sf::Uint8>& pixels, vector<unsigned char> gfx);
void printPixels(vector<unsigned char> gfx);

int main(int argc, char ** argv) {
    sf::RenderWindow window(sf::VideoMode(WINSIZE_X, WINSIZE_Y), "Chip8-Emu");
    sf::Sprite sprite;
    sf::Texture texture;
    vector<sf::Uint8> pixels(WINSIZE_X*WINSIZE_Y*4);
    vector<unsigned char> gfx = vector<unsigned char>(WINSIZE_X*WINSIZE_Y, 0);

    int position = 0;
    int offset = 20;

    if (!texture.create(WINSIZE_X, WINSIZE_Y))
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

        //Update texture
        emulateCycle(gfx, position, offset);
        pixels = getGFX(pixels, gfx);
        texture.update(&pixels[0]);

        //Update sprite with texture
        sprite.setTexture(texture);

        //Clear window
        window.clear(sf::Color::White);

        //Draw everything
        window.draw(sprite);
        window.display();
        position++;
        if(position > 64*32) position = 0;
    }
    return 0;
}

//GFX is a 64*32 2 bit array
vector<sf::Uint8> getGFX(vector<sf::Uint8>& pixels, vector<unsigned char> gfx){
    for(int i = 0; i < WINSIZE_X*WINSIZE_Y; i++) {
        if(gfx[i]){
            pixels[i*4] = 255;   //R
            pixels[i*4+1] = 255; //G
            pixels[i*4+2] = 255; //B
            pixels[i*4+3] = 255; //A
        }else{
            pixels[i*4] = 0;
            pixels[i*4+1] = 0;
            pixels[i*4+2] = 0; 
            pixels[i*4+3] = 255;
        }
    }
    return pixels;
}

vector<unsigned char> emulateCycle(vector<unsigned char>& gfx, int position, int offset){
    if(position != 0){
        gfx[position-1] = 0;
    }
    else{
        gfx[WINSIZE_X*WINSIZE_Y] = 0;
    }

    for (int i = 0; i < offset; i++){
        if(position >= 64*32)
            position = 0;
        gfx[position+i] = 1; 
    }
    return gfx;
}

void printPixels(vector<unsigned char> gfx){
    for(int i = 0; i < WINSIZE_X; i++){
        for(int j = 0; j < WINSIZE_Y; j++){
            if(gfx[i*j])
                cout << "1";
            else
                cout << "0";
        }
        cout << endl;
    }
}