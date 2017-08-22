#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Chip8.h"

using namespace std;

Chip8 chip8;

#define WINSIZE_X 64
#define WINSIZE_Y 32

int main(int argc, char ** argv) {
    if(argc<2){
        cout << "Usage: Chip8-Emu chip8-application" << endl;
        return 1;
    }

    if(!chip8.loadApplication(argv[1])){
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(WINSIZE_X, WINSIZE_Y), "My Window");
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Uint8 *pixels = new sf::Uint8[WINSIZE_X * WINSIZE_Y * 4];

    if (!texture.create(WINSIZE_X, WINSIZE_Y)) { 
        std::cout << "Error: texture could not be created" << std::endl;
    }

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
        for(int y = 0; y < WINSIZE_Y; y++) {
          for(int x = 0; x < WINSIZE_X; x++) { pixels[(y*WINSIZE_X*4)+(4*x)]       = 255; // R
            pixels[(y*WINSIZE_X*4)+(4*x)+1]     = 255; // G
            pixels[(y*WINSIZE_X*4)+(4*x)+2]     = 0; // B
            pixels[(y*WINSIZE_X*4)+(4*x)+3]     = 200; // A
          }
        } 
        texture.update(pixels);

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