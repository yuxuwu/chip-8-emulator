#Compiler flags
CFLAGS = -std=c++14 -g -o
CFLAGSOBJECT = -c $(CFLAGS)
#Project Name
PNAME = Chip8-Emu
#Sources
SOURCES = main.cpp Chip8.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS)
	g++ $(OBJECTS) $(CFLAGS) $(PNAME) -lGL -lGLU -lglut

run: all
	./$(PNAME)

Chip8.o: Chip8.cpp Chip8.h
	g++ Chip8.cpp $(CFLAGSOBJECT) Chip8.o

main.o: main.cpp
	g++ main.cpp $(CFLAGSOBJECT) main.o
