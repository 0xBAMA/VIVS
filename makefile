CC = g++ -time

MAKE_EXE = -o

GL_FLAGS = -lglut -lGLEW -lGL -lGLU

LODEPNG_FLAGS = resources/lodepng.cpp -ansi -O3

UNNECCESARY_DEBUG = -Wall -Wextra -pedantic



all: exe

exe: main.cc
	$(CC) main.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) exe




examples: david ico

david: examples/david/david.cc
	$(CC) examples/david/david.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/david/david

ico: examples/ico/ico.cc
	$(CC) examples/ico/ico.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/ico/ico
