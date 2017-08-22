#Makefile variables
 
#Project name
PNAME = Chip8-Emu
#Compiler flags
CFLAGS = -std=c++14
CFLAGSOBJECT = -g -c
SFMLPROJECTFLAGS = -o $(PNAME) -lsfml-graphics -lsfml-window -lsfml-system
#Sources
MAIN = main.cpp
SOURCES = Chip8.cpp
OBJECTS = $(MAIN:.cpp=.o) $(SOURCES:.cpp=.o) 

all: $(OBJECTS)
	g++ $(OBJECTS) $(SFMLPROJECTFLAGS)

run: all
	./$(PNAME)

debug: all
	gdb ./$(PNAME)

checkmem: all
	valgrind ./$(PNAME)

Chip8.o: Chip8.cpp Chip8.h
	g++ $(CFLAGSOBJECT) Chip8.cpp

main.o: main.cpp
	g++ $(CFLAGSOBJECT) main.cpp

clean:
	rm $(OBJECTS) $(PNAME)