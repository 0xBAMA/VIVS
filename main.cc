#include <iostream> //terminal I/O
using std::cout;
using std::endl;
using std::cin;

#include <vector>   //container

#include <algorithm> //std::copy

#include <stdlib.h> //qsort

#include <string>
using std::string;

#include <math.h>   //sqrt, pow


// Good, simple png library
#include "resources/lodepng.h"


// GLEW
#define GLEW_STATIC
#include <GL/glew.h>


// GLUT
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>


// Shader Compilation
#include "resources/shaders/Shader.h"


// glsl-style Vector and Matrix Library - separate includes for different functionality
#define GLM_FORCE_SWIZZLE			//supposedly ....
#include "resources/glm/glm.hpp" 									// general types
#include "resources/glm/gtc/matrix_transform.hpp" // orthographic view matrix (glm::ortho( left, right, bottom, top, zNear, zFar ))
#include "resources/glm/gtc/type_ptr.hpp" 				// allows the sending of a matrix (for glUniform)
#include "resources/glm/gtx/transform.hpp"				// rotate()

typedef glm::vec4 vec;







// // image dimensions, based on a quarter of my laptop screen resolution
// I'm going to have to look into glutEnterGameMode() to make full screen resolutions different
// const int image_height = 768/2;
// const int image_width = 1366/2;

const int image_height = 768;
const int image_width = 1366;

long int numFrames = 0;


//How many verticies to use, to represent all the voxels (default value)
int points_per_side = 100;
int NumVertices = points_per_side * points_per_side * points_per_side;


//and the array to hold them
const int MaxVerticies = 64000000;

const int num_directions = 48;

glm::vec3 *points[num_directions];
glm::vec3 *initial_points;









//used to hold the handles for all the separate arrays - see above for how to pick
GLuint array_buffers[48]; // there are 6 per quadrant, 8 quadrants for a total of 48





// UNIFORMS

//based upon the layout qualifiers in the vertex shader
// const int vColor_index = 0;
GLuint vPosition_index = 1;

GLuint view_location = 2;
GLuint rotation_location = 3;

// shape parameters




// not worrying about location, using glGetUniformLocation in the initialization to get the values



// used to hold the geometry values CPU-side


#define NUM_SPHERES   1
#define NUM_TRIANGLES 1
#define NUM_QUAD_HEXS 2
#define NUM_CYLINDERS 1
#define NUM_TUBES			1

//SPHERE
GLuint sphere_center_location;
glm::vec3 sphere_center_value[NUM_SPHERES];

GLuint sphere_radius_location;
float sphere_radius_value[NUM_SPHERES];

GLuint sphere_colors_location;
vec sphere_colors_values[NUM_SPHERES];





//TRIANGLE
GLuint triangle_point1_location;
glm::vec3 triangle_point1_values[NUM_TRIANGLES];

GLuint triangle_point2_location;
glm::vec3 triangle_point2_values[NUM_TRIANGLES];

GLuint triangle_point3_location;
glm::vec3 triangle_point3_values[NUM_TRIANGLES];

GLuint triangle_colors_location;
vec triangle_color_values[NUM_TRIANGLES];

GLuint triangle_thickness_location;
float thickness[NUM_TRIANGLES];

GLuint triangle_offsets_location;
glm::vec3 triangle_offsets[NUM_TRIANGLES];





//QUAD HEX/CUBOID VALUES
GLuint cuboid_a_location;
glm::vec3 cuboid_a_values[NUM_QUAD_HEXS];

GLuint cuboid_b_location;
glm::vec3 cuboid_b_values[NUM_QUAD_HEXS];

GLuint cuboid_c_location;
glm::vec3 cuboid_c_values[NUM_QUAD_HEXS];

GLuint cuboid_d_location;
glm::vec3 cuboid_d_values[NUM_QUAD_HEXS];

GLuint cuboid_e_location;
glm::vec3 cuboid_e_values[NUM_QUAD_HEXS];

GLuint cuboid_f_location;
glm::vec3 cuboid_f_values[NUM_QUAD_HEXS];

GLuint cuboid_g_location;
glm::vec3 cuboid_g_values[NUM_QUAD_HEXS];

GLuint cuboid_h_location;
glm::vec3 cuboid_h_values[NUM_QUAD_HEXS];


GLuint cuboid_colors_location;
vec cuboid_color_values[NUM_QUAD_HEXS];

GLuint cuboid_offsets_location;
glm::vec3 cuboid_offsets[NUM_QUAD_HEXS];






//CYLINDER VALUES
GLuint cylinder_tvec_location;
glm::vec3 cylinder_tvec_values[NUM_CYLINDERS];

GLuint cylinder_bvec_location;
glm::vec3 cylinder_bvec_values[NUM_CYLINDERS];


GLuint cylinder_radii_location;
float cylinder_radii_values[NUM_CYLINDERS];

GLuint cylinder_colors_location;
vec cylinder_color_values[NUM_CYLINDERS];

GLuint cylinder_offsets_location;
glm::vec3 cylinder_offsets[NUM_CYLINDERS];




//TUBE VALUES
GLuint tube_tvec_location;
glm::vec3 tube_tvec_values[NUM_TUBES];

GLuint tube_bvec_location;
glm::vec3 tube_bvec_values[NUM_TUBES];

GLuint tube_inner_radii_location;
float tube_inner_radii_values[NUM_TUBES];

GLuint tube_outer_radii_location;
float tube_outer_radii_values[NUM_TUBES];

GLuint tube_colors_location;
vec tube_color_values[NUM_TUBES];

GLuint tube_offsets_location;
glm::vec3 tube_offsets[NUM_TUBES];








// ROTATION AND PROJECTION (of the points)

float x_rot = 0.0f;
float y_rot = 1.0f;
float z_rot = 2.0f;


glm::mat4 rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
// glm::mat4 rotation = glm::mat4( 1.0 );



float base_scale = 0.95;

GLfloat left = -1.366f  * base_scale;
GLfloat right = 1.366f  * base_scale;
GLfloat top = -0.768f   * base_scale;
GLfloat bottom = 0.768f * base_scale;
GLfloat zNear = -1.0f   * base_scale;
GLfloat zFar = 1.0f     * base_scale;

glm::mat4 projection = glm::ortho(left, right, top, bottom, zNear, zFar);


float point_size = 2.0;
bool rotate_triangle = true;
bool rotate_hexahedrons = true;
int current_buffer_index = 0;	//the currently bound buffer - used to check if you need to bind a new one







// SHADER STUFF

GLuint shader_handle;

GLuint texture; //handle for the texture






//utilities
void update_rotation();
int calcOrder( const glm::vec3 & dir ); // thanks Inigo - source: http://www.iquilezles.org/www/articles/volumesort/volumesort.htm
void timer(int); //need to forward declare this for the initialization


void generate_points()
{
	initial_points = new glm::vec3[MaxVerticies];	//keeping this separate from anything that goes to the GPU now

	float total_edge_length = 1.0f;

	// float total_edge_length = 0.8f;
	float start_dimension = -1 * (total_edge_length / 2);

	float increment = total_edge_length / points_per_side;
	float x,y,z;

	// cout << increment << endl << endl;

	int index = 0;

	for(float x_step = 0; x_step < points_per_side; x_step++ )
	{

		x = start_dimension + x_step * increment;

		for(float y_step = 0; y_step < points_per_side; y_step++ )
		{

			y = start_dimension + y_step * increment;

			for(float z_step = 0; z_step < points_per_side; z_step++ )
			{

				z = start_dimension + z_step * increment;

				initial_points[index] = vec( x, y, z, 1.0f );


				// cout << index << endl;

				index++;
			}
		}
	}
}





//this is all the stuff related to sorting the 48 arrays
//trying this to get around the alpha limitations

//	http://www.iquilezles.org/www/articles/volumesort/volumesort.htm




glm::vec3 sorting_test_point;

int compare_by_distance(const void* p1, const void* p2)
{
	//the value of the test point is assigned outside this function

	glm::vec3 vecp1 = *(glm::vec3 *) p1;
	glm::vec3 vecp2 = *(glm::vec3 *) p2;

	float p1_dist = glm::distance( vecp1, sorting_test_point);
	float p2_dist = glm::distance( vecp2, sorting_test_point);

	//Return values
	//
	// 	<0	The element pointed to by p1 goes before the element pointed to by p2
	// 	 0	The element pointed to by p1 is equivalent to the element pointed to by p2
	//	>0	The element pointed to by p1 goes after the element pointed to by p2

	if(p1_dist > p2_dist)
	{// p1 is farther away, it comes first in the list
		return(-1);
	}

	if(p2_dist > p1_dist)
	{// p2 is farther away, it comes first in the list
		return(1);
	}

	return 0;

}





void sort_48x()
{

	glm::vec3 view_vectors[] = {
	//these are ordered by the index that they map to
		glm::vec3(   1.0f,   0.75f,   0.1f),
		glm::vec3(   1.0f,   0.75f,  -0.1f),
		glm::vec3(   1.0f,  -0.75f,   0.1f),
		glm::vec3(   1.0f,  -0.75f,  -0.1f),
		glm::vec3(  -1.0f,   0.75f,   0.1f),
		glm::vec3(  -1.0f,   0.75f,  -0.1f),
		glm::vec3(  -1.0f,  -0.75f,   0.1f),
		glm::vec3(  -1.0f,  -0.75f,  -0.1f),
		glm::vec3(   1.0f,    0.1f,  0.75f),
		glm::vec3(   1.0f,   -0.1f,  0.75f),
		glm::vec3(   1.0f,    0.1f, -0.75f),
		glm::vec3(   1.0f,   -0.1f, -0.75f),
		glm::vec3(  -1.0f,    0.1f,  0.75f),
		glm::vec3(  -1.0f,   -0.1f,  0.75f),
		glm::vec3(  -1.0f,    0.1f, -0.75f),
		glm::vec3(  -1.0f,   -0.1f, -0.75f),
		glm::vec3(  0.75f,    1.0f,   0.1f),
		glm::vec3(  0.75f,    1.0f,  -0.1f),
		glm::vec3( -0.75f,    1.0f,   0.1f),
		glm::vec3( -0.75f,    1.0f,  -0.1f),
		glm::vec3(  0.75f,   -1.0f,   0.1f),
		glm::vec3(  0.75f,   -1.0f,  -0.1f),
		glm::vec3( -0.75f,   -1.0f,   0.1f),
		glm::vec3( -0.75f,   -1.0f,  -0.1f),
		glm::vec3(   0.1f,    1.0f,  0.75f),
		glm::vec3(  -0.1f,    1.0f,  0.75f),
		glm::vec3(   0.1f,    1.0f, -0.75f),
		glm::vec3(  -0.1f,    1.0f, -0.75f),
		glm::vec3(   0.1f,   -1.0f,  0.75f),
		glm::vec3(  -0.1f,   -1.0f,  0.75f),
		glm::vec3(   0.1f,   -1.0f, -0.75f),
		glm::vec3(  -0.1f,   -1.0f, -0.75f),
		glm::vec3(  0.75f,    0.1f,   1.0f),
		glm::vec3(  0.75f,   -0.1f,   1.0f),
		glm::vec3( -0.75f,    0.1f,   1.0f),
		glm::vec3( -0.75f,   -0.1f,   1.0f),
		glm::vec3(  0.75f,    0.1f,  -1.0f),
		glm::vec3(  0.75f,   -0.1f,  -1.0f),
		glm::vec3( -0.75f,    0.1f,  -1.0f),
		glm::vec3( -0.75f,   -0.1f,  -1.0f),
		glm::vec3(   0.1f,   0.75f,   1.0f),
		glm::vec3(  -0.1f,   0.75f,   1.0f),
		glm::vec3(   0.1f,  -0.75f,   1.0f),
		glm::vec3(  -0.1f,  -0.75f,   1.0f),
		glm::vec3(   0.1f,   0.75f,  -1.0f),
		glm::vec3(  -0.1f,   0.75f,  -1.0f),
		glm::vec3(   0.1f,  -0.75f,  -1.0f),
		glm::vec3(  -0.1f,  -0.75f,  -1.0f)

	};

	cout << endl;

	for(int i = 0; i < num_directions; i++)
	{
		cout << "\rsorting array " << i << std::flush;

		sorting_test_point = glm::vec3(0.0f, 0.f, 0.0f) + 2.0f * view_vectors[i];

		qsort(initial_points, NumVertices, sizeof(glm::vec3), compare_by_distance);

		std::copy(initial_points, initial_points + NumVertices, points[i]);
	}

	cout << "\rall arrays sorted     " << endl;
}



// ----------------------



void init()
{


	// The rest of the initialization
	glClearColor( 0.618, 0.618, 0.618, 1.0 );
	// glClearColor( 1.0, 1.0, 1.0, 1.0 );


	glPointSize(point_size);


	// enable z buffer for occlusion
	glEnable( GL_DEPTH_TEST );


	// alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// TEXTURE (HEIGHTMAP)
	glGenTextures(1, &texture); // Generate an ID
	glBindTexture(GL_TEXTURE_2D, texture); // use the specified ID

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	unsigned width, height;
	std::vector<unsigned char> image_data;

	unsigned error = lodepng::decode( image_data, width, height, "AustraliaHeightmap.png", LCT_GREY, 8 );

	if( error == 0 )
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0,  GL_RED, GL_UNSIGNED_BYTE, &image_data[0]);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;

		std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
	}












 	//allocation of the arrays

	for(int i = 0; i < num_directions; i++)
	{
		points[i] = new glm::vec3[MaxVerticies];

		//generate the ordering for that entry in that array -

		//first number is any one of the following 6 - (+x, -x, +y, -y, +z, -z)
		//second number is one of the four remaining once the first and it's complement are removed
		//third number has two choices once the second and it's complement are removed
	}




	generate_points();

	//after this, we have the points


	sort_48x();

	//after this, we have 48 arrays
















	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );



	// Create and initialize the buffer objects - 48x
	glGenBuffers( 48, &array_buffers[0] );




	for(int i = 0; i < 48; i++)
	{

		//loop this 0 - 47

		glBindBuffer( GL_ARRAY_BUFFER, array_buffers[i] ); 																			//this is what sets the active buffer
		glBufferData( GL_ARRAY_BUFFER, NumVertices * sizeof(glm::vec3), NULL, GL_STATIC_DRAW );	//initialize with NULL
		glBufferSubData( GL_ARRAY_BUFFER, 0, NumVertices * sizeof(glm::vec3), points[i] );			//send the data

	}









	// we're starting with zero bound - this will change upon any rotation
	glBindBuffer( GL_ARRAY_BUFFER, array_buffers[0] );


	// cout << NumVertices * sizeof(vec); //variable size, but defaults put it at 6 000 000 bytes





	// Use the shader program ( compiled in the initialization )
	glUseProgram( shader_handle );



// SET UP VERTEX ARRAY

	// vertex locations
	vPosition_index = glGetAttribLocation( shader_handle, "vPosition" );
	glEnableVertexAttribArray( vPosition_index );
	glVertexAttribPointer( vPosition_index, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (0) );


	// // vertex colors
	// glEnableVertexAttribArray( vColor_index );
	// glVertexAttribPointer( vColor_index, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (sizeof(points)) );


// NOW ALL THE UNIFORMS


// TRANSFORMS

	view_location = glGetUniformLocation( shader_handle, "view" );
	rotation_location = glGetUniformLocation( shader_handle, "rotation" );


	glUniformMatrix4fv( view_location, 1, GL_FALSE,  glm::value_ptr( projection ) );
	glUniformMatrix4fv( rotation_location, 1, GL_FALSE,  glm::value_ptr( rotation ) );


// GEOMETRY



//SPHERES
	if(NUM_SPHERES)
	{
	// SPHERE VALUES

		sphere_center_location = glGetUniformLocation( shader_handle, "sphere_center" );
		sphere_radius_location = glGetUniformLocation( shader_handle, "sphere_radius" );
		sphere_colors_location = glGetUniformLocation( shader_handle, "sphere_colors" );



	// PUT INITIAL GEOMETRY HERE


		sphere_center_value[0] = glm::vec3( 0.0f, 0.0f, 0.0f );
		sphere_radius_value[0] = 0.55f;
		sphere_colors_values[0] = vec(0.2f, 0.0f, 0.1f, 0.06f);



		glUniform3fv(sphere_center_location, NUM_SPHERES, glm::value_ptr( sphere_center_value[0] ) );
		glUniform1fv(sphere_radius_location, NUM_SPHERES, &sphere_radius_value[0] );
		glUniform4fv(sphere_colors_location, NUM_SPHERES, glm::value_ptr( sphere_colors_values[0] ) );


	}











//TRIANGLES
	if(NUM_TRIANGLES)
	{
	// TRIANGLE VALUES

		triangle_point1_location = glGetUniformLocation( shader_handle, "triangle_point1" );
		triangle_point2_location = glGetUniformLocation( shader_handle, "triangle_point2" );
		triangle_point3_location = glGetUniformLocation( shader_handle, "triangle_point3" );

		triangle_colors_location = glGetUniformLocation( shader_handle, "triangle_colors" );

		triangle_thickness_location = glGetUniformLocation( shader_handle, "triangle_thickness" );

		triangle_offsets_location = glGetUniformLocation( shader_handle, "triangle_offsets");




	// INITIAL TRIANGLE DATA
	// PUT INITIAL GEOMETRY HERE


		triangle_point1_values[0] = glm::vec3( 0.30f, -0.15f, -0.15f);
		triangle_point2_values[0] = glm::vec3(-0.15f,  0.30f, -0.15f);
		triangle_point3_values[0] = glm::vec3(-0.15f, -0.15f,  0.30f);

		triangle_color_values[0] = glm::vec4( 0.99f, 0.25f, 0.29f, 1.0f );
		thickness[0] = 0.08f;
		triangle_offsets[0] = glm::vec3(0.0f, 0.0f, 0.0f);















	 	glUniform3fv( triangle_point1_location, NUM_TRIANGLES, glm::value_ptr( triangle_point1_values[0] ) );
		glUniform3fv( triangle_point2_location, NUM_TRIANGLES, glm::value_ptr( triangle_point2_values[0] ) );
		glUniform3fv( triangle_point3_location, NUM_TRIANGLES, glm::value_ptr( triangle_point3_values[0] ) );

		glUniform4fv( triangle_colors_location, NUM_TRIANGLES, glm::value_ptr( triangle_color_values[0] ) );


		glUniform1fv( triangle_thickness_location, NUM_TRIANGLES, &thickness[0]);

		glUniform3fv( triangle_offsets_location, NUM_TRIANGLES, glm::value_ptr( triangle_offsets[0] ) );

	}













//QUADRILATERAL HEXAHEDRON (CUBOID)
	if(NUM_QUAD_HEXS)
	{
		cuboid_a_location = glGetUniformLocation( shader_handle, "cuboid_a");
		cuboid_b_location = glGetUniformLocation( shader_handle, "cuboid_b");
		cuboid_c_location = glGetUniformLocation( shader_handle, "cuboid_c");
		cuboid_d_location = glGetUniformLocation( shader_handle, "cuboid_d");
		cuboid_e_location = glGetUniformLocation( shader_handle, "cuboid_e");
		cuboid_f_location = glGetUniformLocation( shader_handle, "cuboid_f");
		cuboid_g_location = glGetUniformLocation( shader_handle, "cuboid_g");
		cuboid_h_location = glGetUniformLocation( shader_handle, "cuboid_h");

		cuboid_colors_location = glGetUniformLocation( shader_handle, "cuboid_colors");

		cuboid_offsets_location = glGetUniformLocation( shader_handle, "cuboid_offsets");



		// glm::vec3 a = glm::vec3(-,+,+);
		// glm::vec3 b = glm::vec3(-,-,+);
		// glm::vec3 c = glm::vec3(+,+,+);
		// glm::vec3 d = glm::vec3(+,-,+);
		// glm::vec3 e = glm::vec3(-,+,-);
		// glm::vec3 f = glm::vec3(-,-,-);
		// glm::vec3 g = glm::vec3(+,+,-);
		// glm::vec3 h = glm::vec3(+,-,-);

		// 	   e-------g    +y
		// 	  /|      /|		 |
		// 	 / |     / |     |___+x
		// 	a-------c  |    /
		// 	|  f----|--h   +z
		// 	| /     | /
		//  |/      |/
		// 	b-------d

		// float offset = 0.3f;

		glm::mat4 rot = glm::rotate(3.14159265359f / 2.0f, glm::vec3(1.0f, 1.0f, 0.0f));

		cuboid_a_values[0] = rot * vec(-0.1f,  0.1f,  0.1f, 1.0f);
		cuboid_b_values[0] = rot * vec(-0.1f, -0.1f,  0.1f, 1.0f);
		cuboid_c_values[0] = rot * vec( 0.1f,  0.1f,  0.1f, 1.0f);
		cuboid_d_values[0] = rot * vec( 0.1f, -0.1f,  0.1f, 1.0f);
		cuboid_e_values[0] = rot * vec(-0.1f,  0.1f, -0.1f, 1.0f);
		cuboid_f_values[0] = rot * vec(-0.1f, -0.1f, -0.1f, 1.0f);
		cuboid_g_values[0] = rot * vec( 0.1f,  0.1f, -0.1f, 1.0f);
		cuboid_h_values[0] = rot * vec( 0.1f, -0.1f, -0.1f, 1.0f);

		cuboid_color_values[0] = vec(0.0f, 0.2f, 0.4f, 1.0f);
		cuboid_offsets[0] = glm::vec3(0.0f, 0.0f, 0.4f);



		cuboid_a_values[1] = rot * vec(-0.07f,  0.07f,  0.07f, 1.0f);
		cuboid_b_values[1] = rot * vec(-0.07f, -0.07f,  0.07f, 1.0f);
		cuboid_c_values[1] = rot * vec( 0.07f,  0.07f,  0.07f, 1.0f);
		cuboid_d_values[1] = rot * vec( 0.07f, -0.07f,  0.07f, 1.0f);
		cuboid_e_values[1] = rot * vec(-0.07f,  0.07f, -0.07f, 1.0f);
		cuboid_f_values[1] = rot * vec(-0.07f, -0.07f, -0.07f, 1.0f);
		cuboid_g_values[1] = rot * vec( 0.07f,  0.07f, -0.07f, 1.0f);
		cuboid_h_values[1] = rot * vec( 0.07f, -0.07f, -0.07f, 1.0f);

		cuboid_color_values[1] = vec(0.5f, 0.2f, 0.1f, 1.0f);
		cuboid_offsets[1] = glm::vec3(0.0f, 0.0f, 0.4f);





		glUniform3fv(cuboid_a_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_a_values[0] ) );
		glUniform3fv(cuboid_b_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_b_values[0] ) );
		glUniform3fv(cuboid_c_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_c_values[0] ) );
		glUniform3fv(cuboid_d_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_d_values[0] ) );
		glUniform3fv(cuboid_e_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_e_values[0] ) );
		glUniform3fv(cuboid_f_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_f_values[0] ) );
		glUniform3fv(cuboid_g_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_g_values[0] ) );
		glUniform3fv(cuboid_h_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_h_values[0] ) );

		glUniform4fv(cuboid_colors_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_color_values[0] ) );

		glUniform3fv(cuboid_offsets_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_offsets[0] ) );

	}












//CYLINDERS
	if(NUM_CYLINDERS)
	{
		cylinder_tvec_location = glGetUniformLocation( shader_handle, "cylinder_tvec");
		cylinder_bvec_location = glGetUniformLocation( shader_handle, "cylinder_bvec");
		cylinder_radii_location = glGetUniformLocation( shader_handle, "cylinder_radii");
		cylinder_colors_location = glGetUniformLocation( shader_handle, "cylinder_colors");

		cylinder_offsets_location = glGetUniformLocation( shader_handle, "cylinder_offsets");




		tube_tvec_location = glGetUniformLocation( shader_handle, "tube_tvec");
		tube_bvec_location = glGetUniformLocation( shader_handle, "tube_bvec");
		tube_inner_radii_location = glGetUniformLocation( shader_handle, "tube_inner_radii");
		tube_outer_radii_location = glGetUniformLocation( shader_handle, "tube_outer_radii");
		tube_colors_location = glGetUniformLocation( shader_handle, "tube_colors");

		tube_offsets_location = glGetUniformLocation( shader_handle, "tube_offsets");







		cylinder_tvec_values[0] = glm::vec3(0.2f, 0.2f, 0.2f);
		cylinder_bvec_values[0] = glm::vec3(-0.2f, -0.2f, -0.2f);

		cylinder_radii_values[0] = 0.08f;

		cylinder_color_values[0] = vec(0.5f, 0.25f, 0.05f, 0.8f);
		cylinder_offsets[0] = glm::vec3(0.0f, 0.0f, 0.0f);







		tube_tvec_values[0] = glm::vec3(0.12f, 0.12f, 0.12f);
		tube_bvec_values[0] = glm::vec3(-0.12f, -0.12f, -0.12f);

		tube_inner_radii_values[0] = 0.13f;
		tube_outer_radii_values[0] = 0.2f;

		tube_color_values[0] = vec(0.5f, 0.25f, 0.05f, 0.5f);
		tube_offsets[0] = glm::vec3(0.0f, 0.0f, 0.0f);









		//THEN SEND ALL THE CYLINDER VALUES TO THE GPU
		glUniform3fv(cylinder_tvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_tvec_values[0] ) );
		glUniform3fv(cylinder_bvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_bvec_values[0] ) );
		glUniform1fv(cylinder_radii_location, NUM_CYLINDERS, &cylinder_radii_values[0] );
		glUniform4fv(cylinder_colors_location, NUM_CYLINDERS, glm::value_ptr( cylinder_color_values[0] ) );

		glUniform3fv(cylinder_offsets_location, NUM_CYLINDERS, glm::value_ptr( cylinder_offsets[0] ) );



		//THEN SEND ALL THE TUBE VALUES TO THE GPU
		glUniform3fv(tube_tvec_location, NUM_TUBES, glm::value_ptr( tube_tvec_values[0] ) );
		glUniform3fv(tube_bvec_location, NUM_TUBES, glm::value_ptr( tube_bvec_values[0] ) );
		glUniform1fv(tube_inner_radii_location, NUM_TUBES, &tube_inner_radii_values[0] );
		glUniform1fv(tube_outer_radii_location, NUM_TUBES, &tube_outer_radii_values[0] );
		glUniform4fv(tube_colors_location, NUM_TUBES, glm::value_ptr( tube_color_values[0] ) );

		glUniform3fv(tube_offsets_location, NUM_TUBES, glm::value_ptr( tube_offsets[0] ) );


	}


}



// ----------------------



void display( void )
{



	//get the vector to the camera
	glm::vec3 dir = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f)) * vec(0.0f, 0.0f, -1.0f, 0.0f); 	//the direction from the camera to the center

	//find the index referenced by this vector
	int temp = calcOrder( dir );

	//check against what buffer is currently bound - update if needed
	if(temp != current_buffer_index)
	{
		current_buffer_index = temp;
		glBindBuffer( GL_ARRAY_BUFFER, array_buffers[current_buffer_index] );
	}





	//clear the screen
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//draw geometry into back buffer
	glDrawArrays( GL_POINTS, 0, NumVertices );

	//swap to display
	glutSwapBuffers();

}



// ----------------------



void timer(int)
{











	// glUniform3fv( triangle_point1_location, NUM_TRIANGLES, glm::value_ptr( triangle_point1_values[0] ) );
	// glUniform3fv( triangle_point2_location, NUM_TRIANGLES, glm::value_ptr( triangle_point2_values[0] ) );
	// glUniform3fv( triangle_point3_location, NUM_TRIANGLES, glm::value_ptr( triangle_point3_values[0] ) );
	//
	//
	// //UPDATE THE GPU-SIDE VALUES OF ALL CYLINDERS
	//
	// glUniform3fv(cylinder_tvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_tvec_values[0] ) );
	// glUniform3fv(cylinder_bvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_bvec_values[0] ) );
	// glUniform4fv(cylinder_colors_location, NUM_CYLINDERS, glm::value_ptr( cylinder_color_values[0] ) );
	//
	// //THEN SEND ALL THE TUBE VALUES TO THE GPU
	// glUniform3fv(tube_tvec_location, NUM_TUBES, glm::value_ptr( tube_tvec_values[0] ) );
	// glUniform3fv(tube_bvec_location, NUM_TUBES, glm::value_ptr( tube_bvec_values[0] ) );




	glm::mat4 cuboid_rot = glm::rotate(0.01f, glm::vec3(1.0f, 0.0f, 1.0f));



	cuboid_a_values[0] = cuboid_rot * vec(cuboid_a_values[0], 1.0f);
	cuboid_b_values[0] = cuboid_rot * vec(cuboid_b_values[0], 1.0f);
	cuboid_c_values[0] = cuboid_rot * vec(cuboid_c_values[0], 1.0f);
	cuboid_d_values[0] = cuboid_rot * vec(cuboid_d_values[0], 1.0f);
	cuboid_e_values[0] = cuboid_rot * vec(cuboid_e_values[0], 1.0f);
	cuboid_f_values[0] = cuboid_rot * vec(cuboid_f_values[0], 1.0f);
	cuboid_g_values[0] = cuboid_rot * vec(cuboid_g_values[0], 1.0f);
	cuboid_h_values[0] = cuboid_rot * vec(cuboid_h_values[0], 1.0f);

	cuboid_a_values[1] = cuboid_rot * vec(cuboid_a_values[1], 1.0f);
	cuboid_b_values[1] = cuboid_rot * vec(cuboid_b_values[1], 1.0f);
	cuboid_c_values[1] = cuboid_rot * vec(cuboid_c_values[1], 1.0f);
	cuboid_d_values[1] = cuboid_rot * vec(cuboid_d_values[1], 1.0f);
	cuboid_e_values[1] = cuboid_rot * vec(cuboid_e_values[1], 1.0f);
	cuboid_f_values[1] = cuboid_rot * vec(cuboid_f_values[1], 1.0f);
	cuboid_g_values[1] = cuboid_rot * vec(cuboid_g_values[1], 1.0f);
	cuboid_h_values[1] = cuboid_rot * vec(cuboid_h_values[1], 1.0f);



	//UPDATE THE GPU-SIDE VALUES OF ALL CUBOIDS

	glUniform3fv(cuboid_a_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_a_values[0] ) );
	glUniform3fv(cuboid_b_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_b_values[0] ) );
	glUniform3fv(cuboid_c_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_c_values[0] ) );
	glUniform3fv(cuboid_d_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_d_values[0] ) );
	glUniform3fv(cuboid_e_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_e_values[0] ) );
	glUniform3fv(cuboid_f_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_f_values[0] ) );
	glUniform3fv(cuboid_g_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_g_values[0] ) );
	glUniform3fv(cuboid_h_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_h_values[0] ) );


	glutPostRedisplay();

	numFrames++;

	glutTimerFunc(1000.0/30.0, timer, 0);

}



// ----------------------



void idle( void )
{
	// glutPostRedisplay();
}



// ----------------------



void keyboard( unsigned char key, int x, int y )
{

  switch( key )
	{
		// quit
		case 033: // Escape Key
		case 'q':
		case 'Q':
		  exit( EXIT_SUCCESS );
		  break;

		case 'a':
			//zoom in
			left 		*= 0.9;
			right 	*= 0.9;
			top 		*= 0.9;
			bottom	*= 0.9;
			zNear 	*= 0.9;
			zFar 		*= 0.9;

			projection = glm::ortho(left, right, top, bottom, zNear, zFar);

			glUniformMatrix4fv( view_location, 1, GL_FALSE,  glm::value_ptr( projection ) );
			break;

		case 'z':
			//zoom out
			left 		*= ( 1 / 0.9 );
			right 	*= ( 1 / 0.9 );
			top 		*= ( 1 / 0.9 );
			bottom 	*= ( 1 / 0.9 );
			zNear 	*= ( 1 / 0.9 );
			zFar 		*= ( 1 / 0.9 );

			projection = glm::ortho(left, right, top, bottom, zNear, zFar);

			glUniformMatrix4fv( view_location, 1, GL_FALSE,  glm::value_ptr( projection ) );
			break;

		case 's':
			//increase point size
			point_size *= 1 / 0.98f;

			glPointSize(point_size);

			break;

		case 'x':
			//decrease point size
			point_size *= 0.98f;

			glPointSize(point_size);

			break;

		case 'w':
			//toggle rotation of the triangle
			rotate_triangle = rotate_triangle ? false : true;
			rotate_hexahedrons = rotate_hexahedrons ? false : true;

			break;

		// CONTROLING THE ROTATION OF THE BLOCK

		case 'f': //reset the block's rotation

			// original values
			// x_rot = 0.0f;
			// y_rot = 45.0f;
			// z_rot = 90.0f;


			if(x_rot == 0.34f && y_rot == 0.99f && z_rot == 2.0f )
			{
				x_rot = 0.77;	//more vertical angle
				y_rot = 0.51;
				z_rot = 2.02;
			}
			else
			{
				x_rot = 0.34f; //this angle makes things feel isometric, I'm into it
				y_rot = 0.99f;
				z_rot = 2.0f;
			}





			update_rotation();

			break;

		case 'e': // output the values of the current rotation

			cout << "xrot " << x_rot << " yrot " << y_rot << " zrot " << z_rot << endl;

			break;

		case 't':	// add to the block's x rotation

			x_rot += 0.01f;

			update_rotation();


			break;

		case 'c': // subtract from the block's x rotation

			x_rot -= 0.01f;

			update_rotation();

			break;

		case 'g': // add to the block's y rotation

			y_rot += 0.01f;

			update_rotation();

			break;

		case 'd': // subtract from the block's y rotation

			y_rot -= 0.01f;

			update_rotation();

			break;

		case 'v': // add to the block's z rotation

			z_rot += 0.01f;

			update_rotation();

			break;

		case 'r': // subtract from the block's z rotation

			z_rot -= 0.01f;

			update_rotation();

			break;

		case 'k':
			current_buffer_index++;
			glBindBuffer( GL_ARRAY_BUFFER, array_buffers[current_buffer_index] );
			break;
		case 'l':
			current_buffer_index--;
			glBindBuffer( GL_ARRAY_BUFFER, array_buffers[current_buffer_index] );
			break;
	}

}



// ----------------------



void mouse( int button, int state, int x, int y )
{

  if ( state == GLUT_DOWN )
	{
		switch( button )
		{
		  case GLUT_LEFT_BUTTON:
			   	//do something for the left mouse button
				break;
		  case GLUT_MIDDLE_BUTTON:
					//do something for
					// the middle mouse button

		  		break;
		  case GLUT_RIGHT_BUTTON:
					//do something for the right mouse button
				break;
		}
  }
}



// ----------------------



int main( int argc, char **argv )
{

	cout << "GLUT Initializing...";

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( image_width / 2, image_height / 2 );

	glutInitContextVersion( 4, 5 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "GLUT Window" );
	glutFullScreen();

	// glutGameModeString("640x480");
	//
	// if(glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
	// {
	// 	glutEnterGameMode();
	// }

	cout << "\rGLUT Initialization Complete." << endl;


	glewExperimental = GL_TRUE;
	glewInit();

	cout << "OpenGL Context established, version is: " << glGetString( GL_VERSION ) << endl;










	cout << "Shader Compilation Starting...";

	Shader theShader( "resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl" );

	shader_handle = theShader.Program;

	cout << "\rShader Compilation Complete.  " << endl;


	if(argc == 2) //input argument defines edge length
	{
		points_per_side = atoi(argv[1]);

		NumVertices = points_per_side * points_per_side * points_per_side;

		if(NumVertices >= MaxVerticies)
		{
			cout << "too big a number, exiting" << endl;
			exit(-1);
		}
	}

	//otherwise use the default



	cout << "Generating Geometry";

  init();

	cout << "\rInitialization done." << endl;





  glutDisplayFunc( display );
  glutKeyboardFunc( keyboard );
  glutMouseFunc( mouse );
  glutIdleFunc( idle );
	glutTimerFunc( 1000.0/60.0, timer, 0 );

  glutMainLoop( );



	cout << "Exiting" << endl;

	return 0;
}













//UTILITIES

void update_rotation()
{ // uses global x rotation, y rotation, z rotation

	rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv( rotation_location, 1, GL_FALSE,  glm::value_ptr( rotation ) );

}





int calcOrder( const glm::vec3 & dir )
{
		int signs;

		const int   sx = dir.x<0.0f;
		const int   sy = dir.y<0.0f;
		const int   sz = dir.z<0.0f;
		const float ax = fabsf( dir.x );
		const float ay = fabsf( dir.y );
		const float az = fabsf( dir.z );

		if( ax>ay && ax>az )
		{
				if( ay>az ) signs = 0 + ((sx<<2)|(sy<<1)|sz);
				else        signs = 8 + ((sx<<2)|(sz<<1)|sy);
		}
		else if( ay>az )
		{
				if( ax>az ) signs = 16 + ((sy<<2)|(sx<<1)|sz);
				else        signs = 24 + ((sy<<2)|(sz<<1)|sx);
		}
		else
		{
				if( ax>ay ) signs = 32 + ((sz<<2)|(sx<<1)|sy);
				else        signs = 40 + ((sz<<2)|(sy<<1)|sx);
		}

		return signs;
}
