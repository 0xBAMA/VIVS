CC = g++ -time

MAKE_EXE = -o

GL_FLAGS = -lglut -lGLEW -lGL -lGLU

LODEPNG_FLAGS = resources/lodepng.cpp -ansi -O3

UNNECCESARY_DEBUG = -Wall -Wextra -pedantic



all: exe

exe: main.cc
	$(CC) main.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) exe


# example executables are kept in the associated folder, along with their shaders

# the shader files are referenced in such a way that you'll have to be in the
# folder for that example in order for it to know where to find them 


examples: david ico bars

david: examples/david/david.cc
	$(CC) examples/david/david.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/david/david

ico: examples/ico/ico.cc
	$(CC) examples/ico/ico.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/ico/ico

bars: examples/bars/bars.cc
	$(CC) examples/bars/bars.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/bars/bars
