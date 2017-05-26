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

chip8.o: Chip8.cpp Chip8.h
	g++ chip8.cpp $(CFLAGSOBJECT) chip8.o

main.o: main.cpp
	g++ main.cpp $(CFLAGSOBJECT) main.o
