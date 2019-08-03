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
# folder for that example in order for the executable to know where to find them

# example usage: 	make examples (or directly, make <example_name>)
#									cd examples/<example_name>
#									<example_name> <optional argument: num_points_per_side>



examples: david ico bars crankshaft ngine



# rotating triangles resembling a star of david at certain points during the rotation

david: examples/david/david.cc
	$(CC) examples/david/david.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/david/david



# a rotating icosahedron made of triangles

ico: examples/ico/ico.cc
	$(CC) examples/ico/ico.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/ico/ico



# a series of progress bars which fill, empty and recolor themselves based on a sine function

bars: examples/bars/bars.cc
	$(CC) examples/bars/bars.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/bars/bars



# a rotating v8 crankshaft

crankshaft: examples/crankshaft/crankshaft.cc
	$(CC) examples/crankshaft/crankshaft.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/crankshaft/crankshaft



# a rotating v8 engine

ngine: examples/ngine/ngine.cc
	$(CC) examples/ngine/ngine.cc $(GL_FLAGS) $(LODEPNG_FLAGS) $(MAKE_EXE) examples/ngine/ngine
