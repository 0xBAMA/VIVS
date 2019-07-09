CC = g++

MAKE_EXE = -o exe -time

GL_FLAGS = -lglut -lGLEW -lGL -lGLU

LODEPNG_FLAGS = resources/lodepng.cpp -ansi -O3

UNNECCESARY_DEBUG = -Wall -Wextra -pedantic



all: exe

exe: main.cc
	$(CC) main.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE)
