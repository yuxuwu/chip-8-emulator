#include "Chip8.h"
#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

Chip8 myChip8;

//Window sizing
int modifier = 10;
int display_width = SCREEN_WIDTH * modifier;
int display_height = SCREEN_HEIGHT * modifier;

void display();
void reshape_window(GLsizei w, GLsizei h);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);

#define DRAWWITHTEXTURE
typedef char u8;
u8 screenData[SCREEN_HEIGHT][SCREEN_WIDTH][3];
void setupTexture();

int main(int argc, char **argv){

    srand(time(NULL));

    //Get name of rom file
    if(argc < 2){
        printf("Usage: myChip8.exe chip8application\n\n");
        return 1;
    }

    //Load game
    if(!myChip8.loadApplication(argv[1]))
        return 1;

    //Setup OpenGL
    //setup graphics and input callbacks
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 
    
    glutInitWindowSize(display_width, display_height);
    glutInitWindowPosition(320, 320);
    glutCreateWindow("Chip-8 Emulator");

    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape_window);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

#ifdef DRAWWITHTEXTURE
    setupTexture();
#endif

    glutMainLoop();

    return 0;
}

void setupTexture(){
    //Clear screen
    for(int y = 0; y < SCREEN_HEIGHT; ++y){
        for(int x = 0; x < SCREEN_WIDTH; ++x){
            screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0;
        }
    }

    //Create a texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData);

    //Setup the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    //Enable textures
    glEnable(GL_TEXTURE_2D);
}

void updateTexture(const Chip8& c8){
    //Update pixels
    for(int y = 0; y < 32; ++y)
        for(int x = 0; x < 64; ++x)
            if(c8.gfx[(y*64) + x] == 0)
                screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 0; //Disabled
            else
                screenData[y][x][0] = screenData[y][x][1] = screenData[y][x][2] = 255; //Enabled

    //Update Texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)screenData); 

    glBegin( GL_QUADS );
        glTexCoord2d(0.0, 0.0);     glVertex2d(0.0,           0.0);
        glTexCoord2d(1.0, 0.0);     glVertex2d(display_width, 0.0);
        glTexCoord2d(1.0, 1.0);     glVertex2d(display_width, display_height); 
        glTexCoord2d(0.0, 1.0);     glVertex2d(0.0,           display_height);
    glEnd();
        
}

void drawPixel(int x, int y){
    glBegin(GL_QUADS);
        glVertex3f((x * modifier) + 0.0f,       (y * modifier) + 0.0f,      0.0f);
        glVertex3f((x * modifier) + 0.0f,       (y * modifier) + modifier,  0.0f);
        glVertex3f((x * modifier) + modifier,   (y * modifier) + modifier,  0.0f);
        glVertex3f((x * modifier) + modifier,   (y * modifier) + 0.0f,      0.0f);
    glEnd();
}

void updateQuads(const Chip8& c8){
    //Draw
    for(int y = 0; y < 32; ++y)
       for(int x = 0; x < 64; ++x){
           if(c8.gfx[(y*64) + x] == 0)
               glColor3f(0.0f, 0.0f, 0.0f);
           else
               glColor3f(1.0f, 1.0f, 1.0f);

           drawPixel(x,y);
       } 
}

void display(){
    myChip8.emulateCycle();

    if(myChip8.drawFlag){
        //Clear framebuffer
        glClear(GL_COLOR_BUFFER_BIT);

#ifdef DRAWWITHTEXTURE
        updateTexture(myChip8);
#else
        updateQuads(myChip8);
#endif

        //Swap buffers
        glutSwapBuffers();

        //Processed frame
        myChip8.drawFlag = false;
    }
}

void reshape_window(GLsizei w, GLsizei h){
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, w, h);

    //Resize quad
    display_width = w;
    display_height = h;
}

void keyboardDown(unsigned char key, int x, int y){
    if(key == 27)
        exit(0);

    switch(key){
        case '1':   myChip8.key[0x1] = 1;   break;
        case '2':   myChip8.key[0x2] = 1;   break;
        case '3':   myChip8.key[0x3] = 1;   break;
        case '4':   myChip8.key[0xC] = 1;   break;

        case 'q':   myChip8.key[0x4] = 1;   break;
        case 'w':   myChip8.key[0x5] = 1;   break;
        case 'e':   myChip8.key[0x6] = 1;   break;
        case 'r':   myChip8.key[0xD] = 1;   break;

        case 'a':   myChip8.key[0x7] = 1;   break;
        case 's':   myChip8.key[0x8] = 1;   break;
        case 'd':   myChip8.key[0x9] = 1;   break;
        case 'f':   myChip8.key[0xE] = 1;   break;

        case 'z':   myChip8.key[0xA] = 1;   break;
        case 'x':   myChip8.key[0x0] = 1;   break;
        case 'c':   myChip8.key[0xB] = 1;   break;
        case 'v':   myChip8.key[0xF] = 1;   break;
    }
}

void keyboardUp(unsigned char key, int x, int y){
    switch(key){
        case '1':   myChip8.key[0x1] = 0;   break;
        case '2':   myChip8.key[0x2] = 0;   break;
        case '3':   myChip8.key[0x3] = 0;   break;
        case '4':   myChip8.key[0xC] = 0;   break;

        case 'q':   myChip8.key[0x4] = 0;   break;
        case 'w':   myChip8.key[0x5] = 0;   break;
        case 'e':   myChip8.key[0x6] = 0;   break;
        case 'r':   myChip8.key[0xD] = 0;   break;

        case 'a':   myChip8.key[0x7] = 0;   break;
        case 's':   myChip8.key[0x8] = 0;   break;
        case 'd':   myChip8.key[0x9] = 0;   break;
        case 'f':   myChip8.key[0xE] = 0;   break;

        case 'z':   myChip8.key[0xA] = 0;   break;
        case 'x':   myChip8.key[0x0] = 0;   break;
        case 'c':   myChip8.key[0xB] = 0;   break;
        case 'v':   myChip8.key[0xF] = 0;   break;
    }
}
