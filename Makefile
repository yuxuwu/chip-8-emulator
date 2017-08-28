#Makefile variables
COMPILER =c++
#Project name
PNAME =Chip8-Emu
#Compiler flags
CFLAGS =-std=c++14
CFLAGSOBJECT =$(CFLAGS) -g -c
SFMLPROJECTFLAGS =-o $(PNAME) -lsfml-graphics -lsfml-window -lsfml-system
#Sources
MAIN =main.cpp
SOURCES =Chip8.cpp
OBJECTS =$(MAIN:.cpp=.o) $(SOURCES:.cpp=.o) 

all: $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(SFMLPROJECTFLAGS)

run: all
	./$(PNAME)

debug: all
	gdb ./$(PNAME)

checkmem: all
	valgrind ./$(PNAME)

Chip8.o: Chip8.cpp Chip8.h
	$(COMPILER) $(CFLAGSOBJECT) Chip8.cpp

main.o: main.cpp
	$(COMPILER) $(CFLAGSOBJECT) main.cpp

clean:
	rm $(OBJECTS) $(PNAME)
