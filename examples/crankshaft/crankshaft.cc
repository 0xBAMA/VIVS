#include <iostream> //terminal I/O
using std::cout;
using std::endl;
using std::cin;

#include <vector>   //container

#include <string>
using std::string;

#include <math.h>   //sqrt, pow


// Good, simple png library
#include "../../resources/lodepng.h"


// GLEW
#define GLEW_STATIC
#include <GL/glew.h>


// GLUT
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>


// Shader Compilation
#include "../../resources/shaders/Shader.h"


// glsl-style Vector and Matrix Library - separate includes for different functionality
#include "../../resources/glm/glm.hpp" 									// general types
#include "../../resources/glm/gtc/matrix_transform.hpp" // orthographic view matrix (glm::ortho( left, right, bottom, top, zNear, zFar ))
#include "../../resources/glm/gtc/type_ptr.hpp" 				// allows the sending of a matrix (for glUniform)
#include "../../resources/glm/gtx/transform.hpp"				// rotate()

typedef glm::vec4 vec;







// // image dimensions, based on a quarter of my laptop screen resolution
// I'm going to have to look into glutEnterGameMode() to make full screen resolutions different
// const int image_height = 768/2;
// const int image_width = 1366/2;

const int image_height = 768;
const int image_width = 1366;


//How many verticies to use, to represent all the voxels
int points_per_side = 100;
int NumVertices = points_per_side * points_per_side * points_per_side;

long int numFrames = 0;

//and the array to hold them
const int MaxVerticies = 64000000;
vec points[MaxVerticies];





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
#define NUM_TRIANGLES 0
#define NUM_QUAD_HEXS 8
#define NUM_CYLINDERS 18


//SPHERE
GLuint sphere_center_location;
glm::vec3 sphere_center_value[NUM_SPHERES];

GLuint sphere_radius_location;
float sphere_radius_value[NUM_SPHERES];

GLuint sphere_colors_location;
vec sphere_colors_values[NUM_SPHERES];

GLuint sphere_offsets_location;
glm::vec3 sphere_offsets[NUM_SPHERES];





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






// ROTATION AND PROJECTION

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







// SHADER STUFF

GLuint shader_handle;

GLuint texture; //handle for the texture



void update_rotation();


void generate_points()
{
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

				points[index] = vec( x, y, z, 1.0f );

				// cout << index << endl;

				index++;
			}
		}
	}
}



// ----------------------



void init()
{

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

	unsigned error = lodepng::decode( image_data, width, height, "../../AustraliaHeightmap.png", LCT_GREY, 8 );

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



	// The rest of the initialization
	glClearColor( 0.618, 0.618, 0.618, 1.0 );
	// glClearColor( 1.0, 1.0, 1.0, 1.0 );


	glPointSize(point_size);



	generate_points();


	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays( 1, &vao );
	glBindVertexArray( vao );

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers( 1, &buffer );
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STATIC_DRAW );

	// glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW );	//replace the above line with this to add buffer space for per-vertex colors
	// glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors );					  	//then use this to buffer this data

	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );


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

		sphere_offsets_location = glGetUniformLocation( shader_handle, "sphere_offsets");


	// PUT INITIAL GEOMETRY HERE


		sphere_center_value[0] = glm::vec3( 0.0f, 0.0f, 0.0f );
		sphere_radius_value[0] = 1.0f;
		sphere_colors_values[0] = vec(0.0f, 0.0f, 0.2f, 0.005f);
		sphere_offsets[0] = glm::vec3(0.0f, 0.0f, 0.0f);

		glUniform3fv(sphere_center_location, NUM_SPHERES, glm::value_ptr( sphere_center_value[0] ) );
		glUniform1fv(sphere_radius_location, NUM_SPHERES, &sphere_radius_value[0] );
		glUniform4fv(sphere_colors_location, NUM_SPHERES, glm::value_ptr( sphere_colors_values[0] ) );

		glUniform3fv(sphere_offsets_location, NUM_SPHERES, glm::value_ptr( sphere_offsets[0] ) );

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


	//this is just to maintain a placeholder

		// triangle_point1_values[0] = glm::vec3(  0.0f, -0.2f, -0.2f );
		// triangle_point2_values[0] = glm::vec3( -0.2f,  0.0f, -0.2f );
		// triangle_point3_values[0] = glm::vec3( -0.2f, -0.2f,  0.0f );
		//
		// triangle_color_values[0] = glm::vec4( 0.99f, 0.25f, 0.29f, 1.0f );
		//
		// thickness[0] = 0.04f;
		// triangle_offsets[0] = glm::vec3(0.0f, 0.0f, 0.0f);






		// PUT INITIAL GEOMETRY HERE







	 	glUniform3fv( triangle_point1_location, NUM_TRIANGLES, glm::value_ptr( triangle_point1_values[0] ) );
		glUniform3fv( triangle_point2_location, NUM_TRIANGLES, glm::value_ptr( triangle_point2_values[0] ) );
		glUniform3fv( triangle_point3_location, NUM_TRIANGLES, glm::value_ptr( triangle_point3_values[0] ) );

		glUniform3fv( triangle_colors_location, NUM_TRIANGLES, glm::value_ptr( triangle_color_values[0] ) );


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





		// PUT INITIAL GEOMETRY HERE


		//WHITEBOARD TIME

		vec a1 = vec( 0.0f,  0.04f, -0.35f, 1.0f);
		vec b1 = vec( 0.0f, -0.04f, -0.35f, 1.0f);
		vec c1 = vec( 0.13f,  0.025f, -0.32f, 1.0f);
		vec d1 = vec( 0.13f, -0.025f, -0.32f, 1.0f);
		vec e1 = vec( 0.0f,  0.04f, -0.38f, 1.0f);
		vec f1 = vec( 0.0f, -0.04f, -0.38f, 1.0f);
		vec g1 = vec( 0.13f,  0.025f, -0.35f, 1.0f);
		vec h1 = vec( 0.13f, -0.025f, -0.35f, 1.0f);


		vec a2 = vec( 0.0f,  0.04f, -0.25f, 1.0f);
		vec b2 = vec( 0.0f, -0.04f, -0.25f, 1.0f);
		vec c2 = vec( 0.13f,  0.025f, -0.28f, 1.0f);
		vec d2 = vec( 0.13f, -0.025f, -0.28f, 1.0f);
		vec e2 = vec( 0.0f,  0.04f, -0.22f, 1.0f);
		vec f2 = vec( 0.0f, -0.04f, -0.22f, 1.0f);
		vec g2 = vec( 0.13f,  0.025f, -0.25f, 1.0f);
		vec h2 = vec( 0.13f, -0.025f, -0.25f, 1.0f);



		cuboid_a_values[0] = a1;
		cuboid_b_values[0] = b1;
		cuboid_c_values[0] = c1;
		cuboid_d_values[0] = d1;
		cuboid_e_values[0] = e1;
		cuboid_f_values[0] = f1;
		cuboid_g_values[0] = g1;
		cuboid_h_values[0] = h1;
		cuboid_color_values[0] = vec(0.25f, 0.25f, 0.25f, 1.0f);
		cuboid_offsets[0] = glm::vec3(-0.3f, 0.0f, 0.0f);

		cuboid_a_values[1] = a2;
		cuboid_b_values[1] = b2;
		cuboid_c_values[1] = c2;
		cuboid_d_values[1] = d2;
		cuboid_e_values[1] = e2;
		cuboid_f_values[1] = f2;
		cuboid_g_values[1] = g2;
		cuboid_h_values[1] = h2;
		cuboid_color_values[1] = vec(0.25f, 0.25f, 0.25f, 1.0f);
		cuboid_offsets[1] = glm::vec3(-0.3f, 0.0f, 0.0f);



		//rotate a1-h2 90 degrees

		glm::mat4 rot = glm::rotate(3.14159265359f / 2.0f, glm::vec3(0.0f, 0.0f, 1.0f));

		vec off = vec(0.0f, 0.0f, 0.2f, 0.0f);

		a1 = rot * vec( 0.0f,  0.04f, -0.15f, 1.0f);
		b1 = rot * vec( 0.0f, -0.04f, -0.15f, 1.0f);
		c1 = rot * vec( 0.13f,  0.025f, -0.12f, 1.0f);
		d1 = rot * vec( 0.13f, -0.025f, -0.12f, 1.0f);
		e1 = rot * vec( 0.0f,  0.04f, -0.18f, 1.0f);
		f1 = rot * vec( 0.0f, -0.04f, -0.18f, 1.0f);
		g1 = rot * vec( 0.13f,  0.025f, -0.15f, 1.0f);
		h1 = rot * vec( 0.13f, -0.025f, -0.15f, 1.0f);


		a2 = rot * vec( 0.0f,  0.04f, -0.05f, 1.0f);
		b2 = rot * vec( 0.0f, -0.04f, -0.05f, 1.0f);
		c2 = rot * vec( 0.13f,  0.025f, -0.08f, 1.0f);
		d2 = rot * vec( 0.13f, -0.025f, -0.08f, 1.0f);
		e2 = rot * vec( 0.0f,  0.04f, -0.02f, 1.0f);
		f2 = rot * vec( 0.0f, -0.04f, -0.02f, 1.0f);
		g2 = rot * vec( 0.13f,  0.025f, -0.05f, 1.0f);
		h2 = rot * vec( 0.13f, -0.025f, -0.05f, 1.0f);



		cuboid_a_values[2] = a1;
		cuboid_b_values[2] = b1;
		cuboid_c_values[2] = c1;
		cuboid_d_values[2] = d1;
		cuboid_e_values[2] = e1;
		cuboid_f_values[2] = f1;
		cuboid_g_values[2] = g1;
		cuboid_h_values[2] = h1;
		cuboid_color_values[2] = vec(0.25f, 0.25f, 0.25f, 1.0f);
		cuboid_offsets[2] = glm::vec3(-0.3f, 0.0f, 0.0f);


		cuboid_a_values[3] = a2;
		cuboid_b_values[3] = b2;
		cuboid_c_values[3] = c2;
		cuboid_d_values[3] = d2;
		cuboid_e_values[3] = e2;
		cuboid_f_values[3] = f2;
		cuboid_g_values[3] = g2;
		cuboid_h_values[3] = h2;
		cuboid_color_values[3] = vec(0.25f, 0.25f, 0.25f, 1.0f);
		cuboid_offsets[3] = glm::vec3(-0.3f, 0.0f, 0.0f);





		//rotate a1-h2 180 degrees

		rot = glm::rotate(3.14159265359f * 1.5f, glm::vec3(0.0f, 0.0f, 1.0f));


		a1 = rot * vec( 0.0f,  0.04f, 0.15f, 1.0f);
		b1 = rot * vec( 0.0f, -0.04f, 0.15f, 1.0f);
		c1 = rot * vec( 0.13f,  0.025f, 0.12f, 1.0f);
		d1 = rot * vec( 0.13f, -0.025f, 0.12f, 1.0f);
		e1 = rot * vec( 0.0f,  0.04f, 0.18f, 1.0f);
		f1 = rot * vec( 0.0f, -0.04f, 0.18f, 1.0f);
		g1 = rot * vec( 0.13f,  0.025f, 0.15f, 1.0f);
		h1 = rot * vec( 0.13f, -0.025f, 0.15f, 1.0f);


		a2 = rot * vec( 0.0f,  0.04f, 0.05f, 1.0f);
		b2 = rot * vec( 0.0f, -0.04f, 0.05f, 1.0f);
		c2 = rot * vec( 0.13f,  0.025f, 0.08f, 1.0f);
		d2 = rot * vec( 0.13f, -0.025f, 0.08f, 1.0f);
		e2 = rot * vec( 0.0f,  0.04f, 0.02f, 1.0f);
		f2 = rot * vec( 0.0f, -0.04f, 0.02f, 1.0f);
		g2 = rot * vec( 0.13f,  0.025f, 0.05f, 1.0f);
		h2 = rot * vec( 0.13f, -0.025f, 0.05f, 1.0f);


		cuboid_a_values[4] = a1;
		cuboid_b_values[4] = b1;
		cuboid_c_values[4] = c1;
		cuboid_d_values[4] = d1;
		cuboid_e_values[4] = e1;
		cuboid_f_values[4] = f1;
		cuboid_g_values[4] = g1;
		cuboid_h_values[4] = h1;
		cuboid_color_values[4] = vec(0.25f, 0.25f, 0.25f, 1.0f);
		cuboid_offsets[4] = glm::vec3(-0.3f, 0.0f, 0.0f);

		cuboid_a_values[5] = a2;
		cuboid_b_values[5] = b2;
		cuboid_c_values[5] = c2;
		cuboid_d_values[5] = d2;
		cuboid_e_values[5] = e2;
		cuboid_f_values[5] = f2;
		cuboid_g_values[5] = g2;
		cuboid_h_values[5] = h2;
		cuboid_color_values[5] = vec(0.25f, 0.25f, 0.25f, 1.0f);
		cuboid_offsets[5] = glm::vec3(-0.3f, 0.0f, 0.0f);





		//rotate a1-h2 90 degrees

		rot = glm::rotate(3.14159265359f, glm::vec3(0.0f, 0.0f, 1.0f));


		a1 = rot * vec( 0.0f,  0.04f, 0.35f, 1.0f);
		b1 = rot * vec( 0.0f, -0.04f, 0.35f, 1.0f);
		c1 = rot * vec( 0.13f,  0.025f, 0.32f, 1.0f);
		d1 = rot * vec( 0.13f, -0.025f, 0.32f, 1.0f);
		e1 = rot * vec( 0.0f,  0.04f, 0.38f, 1.0f);
		f1 = rot * vec( 0.0f, -0.04f, 0.38f, 1.0f);
		g1 = rot * vec( 0.13f,  0.025f, 0.35f, 1.0f);
		h1 = rot * vec( 0.13f, -0.025f, 0.35f, 1.0f);


		a2 = rot * vec( 0.0f,  0.04f, 0.25f, 1.0f);
		b2 = rot * vec( 0.0f, -0.04f, 0.25f, 1.0f);
		c2 = rot * vec( 0.13f,  0.025f, 0.28f, 1.0f);
		d2 = rot * vec( 0.13f, -0.025f, 0.28f, 1.0f);
		e2 = rot * vec( 0.0f,  0.04f, 0.22f, 1.0f);
		f2 = rot * vec( 0.0f, -0.04f, 0.22f, 1.0f);
		g2 = rot * vec( 0.13f,  0.025f, 0.25f, 1.0f);
		h2 = rot * vec( 0.13f, -0.025f, 0.25f, 1.0f);


		cuboid_a_values[6] = a1;
		cuboid_b_values[6] = b1;
		cuboid_c_values[6] = c1;
		cuboid_d_values[6] = d1;
		cuboid_e_values[6] = e1;
		cuboid_f_values[6] = f1;
		cuboid_g_values[6] = g1;
		cuboid_h_values[6] = h1;
		cuboid_color_values[6] = vec(0.25f, 0.25f, 0.25f, 1.0f);
		cuboid_offsets[6] = glm::vec3(-0.3f, 0.0f, 0.0f);

		cuboid_a_values[7] = a2;
		cuboid_b_values[7] = b2;
		cuboid_c_values[7] = c2;
		cuboid_d_values[7] = d2;
		cuboid_e_values[7] = e2;
		cuboid_f_values[7] = f2;
		cuboid_g_values[7] = g2;
		cuboid_h_values[7] = h2;
		cuboid_color_values[7] = vec(0.25f, 0.25f, 0.25f, 1.0f);
		cuboid_offsets[7] = glm::vec3(-0.3f, 0.0f, 0.0f);





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



		// PUT INITIAL GEOMETRY HERE


//MAIN JOURNALS (aligned with the centerline of the crankshaft)

		cylinder_tvec_values[0] = glm::vec3(0.0f, 0.0f, -0.45f);
		cylinder_bvec_values[0] = glm::vec3(0.0f, 0.0f, -0.35f);

		cylinder_radii_values[0] = 0.03f;

		cylinder_color_values[0] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[0] = glm::vec3(-0.3f, 0.0f, 0.0f);




		cylinder_tvec_values[1] = glm::vec3(0.0f, 0.0f, -0.37f);
		cylinder_bvec_values[1] = glm::vec3(0.0f, 0.0f, -0.35f);

		cylinder_radii_values[1] = 0.04f;

		cylinder_color_values[1] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[1] = glm::vec3(-0.3f, 0.0f, 0.0f);



		//-------




		cylinder_tvec_values[2] = glm::vec3(0.0f, 0.0f, -0.25f);
		cylinder_bvec_values[2] = glm::vec3(0.0f, 0.0f, -0.23f);

		cylinder_radii_values[2] = 0.04f;

		cylinder_color_values[2] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[2] = glm::vec3(-0.3f, 0.0f, 0.0f);




		cylinder_tvec_values[3] = glm::vec3(0.0f, 0.0f, -0.25f);
		cylinder_bvec_values[3] = glm::vec3(0.0f, 0.0f, -0.15f);

		cylinder_radii_values[3] = 0.03f;

		cylinder_color_values[3] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[3] = glm::vec3(-0.3f, 0.0f, 0.0f);




		cylinder_tvec_values[4] = glm::vec3(0.0f, 0.0f, -0.17f);
		cylinder_bvec_values[4] = glm::vec3(0.0f, 0.0f, -0.15f);

		cylinder_radii_values[4] = 0.04f;

		cylinder_color_values[4] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[4] = glm::vec3(-0.3f, 0.0f, 0.0f);



		//-------



		cylinder_tvec_values[6] = glm::vec3(0.0f, 0.0f, -0.05f);
		cylinder_bvec_values[6] = glm::vec3(0.0f, 0.0f, -0.03f);

		cylinder_radii_values[6] = 0.04f;

		cylinder_color_values[6] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[6] = glm::vec3(-0.3f, 0.0f, 0.0f);




		cylinder_tvec_values[7] = glm::vec3(0.0f, 0.0f, -0.05f);
		cylinder_bvec_values[7] = glm::vec3(0.0f, 0.0f, 0.05f);

		cylinder_radii_values[7] = 0.03f;

		cylinder_color_values[7] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[7] = glm::vec3(-0.3f, 0.0f, 0.0f);




		cylinder_tvec_values[8] = glm::vec3(0.0f, 0.0f, 0.03f);
		cylinder_bvec_values[8] = glm::vec3(0.0f, 0.0f, 0.05f);

		cylinder_radii_values[8] = 0.04f;

		cylinder_color_values[8] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[8] = glm::vec3(-0.3f, 0.0f, 0.0f);



		//-------




		cylinder_tvec_values[9] = glm::vec3(0.0f, 0.0f, 0.15f);
		cylinder_bvec_values[9] = glm::vec3(0.0f, 0.0f, 0.17f);

		cylinder_radii_values[9] = 0.04f;

		cylinder_color_values[9] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[9] = glm::vec3(-0.3f, 0.0f, 0.0f);




		cylinder_tvec_values[10] = glm::vec3(0.0f, 0.0f, 0.15f);
		cylinder_bvec_values[10] = glm::vec3(0.0f, 0.0f, 0.25f);

		cylinder_radii_values[10] = 0.03f;

		cylinder_color_values[10] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[10] = glm::vec3(-0.3f, 0.0f, 0.0f);




		cylinder_tvec_values[11] = glm::vec3(0.0f, 0.0f, 0.23f);
		cylinder_bvec_values[11] = glm::vec3(0.0f, 0.0f, 0.25f);

		cylinder_radii_values[11] = 0.04f;

		cylinder_color_values[11] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[11] = glm::vec3(-0.3f, 0.0f, 0.0f);



		//-------



		cylinder_tvec_values[12] = glm::vec3(0.0f, 0.0f, 0.35f);
		cylinder_bvec_values[12] = glm::vec3(0.0f, 0.0f, 0.37f);

		cylinder_radii_values[12] = 0.04f;

		cylinder_color_values[12] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[12] = glm::vec3(-0.3f, 0.0f, 0.0f);




		cylinder_tvec_values[13] = glm::vec3(0.0f, 0.0f, 0.35f);
		cylinder_bvec_values[13] = glm::vec3(0.0f, 0.0f, 0.45f);

		cylinder_radii_values[13] = 0.03f;

		cylinder_color_values[13] = vec(0.25f, 0.25f, 0.25f, 1.0f);

		cylinder_offsets[13] = glm::vec3(-0.3f, 0.0f, 0.0f);




//ROD JOURNALS

// https://i.kinja-img.com/gawker-media/image/upload/s--TjZVv1cf--/c_scale,f_auto,fl_progressive,q_80,w_800/lijsom4kg4vspuddmp4g.jpg



	//1,5 UP  (+x)
		cylinder_tvec_values[14] = glm::vec3(0.1f, 0.0f, -0.34f);
		cylinder_bvec_values[14] = glm::vec3(0.1f, 0.0f, -0.26f);

		cylinder_radii_values[14] = 0.025f;

		cylinder_color_values[14] = vec(0.7f, 0.7f, 0.7f, 1.0f);

		cylinder_offsets[14] = glm::vec3(-0.3f, 0.0f, 0.0f);


	//2,6 RIGHT  (+y)
		cylinder_tvec_values[15] = glm::vec3(0.0f, 0.1f, -0.14f);
		cylinder_bvec_values[15] = glm::vec3(0.0f, 0.1f, -0.06f);

		cylinder_radii_values[15] = 0.025f;

		cylinder_color_values[15] = vec(0.7f, 0.7f, 0.7f, 1.0f);

		cylinder_offsets[15] = glm::vec3(-0.3f, 0.0f, 0.0f);


	//3,7 LEFT   (-y)
		cylinder_tvec_values[16] = glm::vec3(0.0f, -0.1f, 0.06f);
		cylinder_bvec_values[16] = glm::vec3(0.0f, -0.1f, 0.14f);

		cylinder_radii_values[16] = 0.025f;

		cylinder_color_values[16] = vec(0.7f, 0.7f, 0.7f, 1.0f);

		cylinder_offsets[16] = glm::vec3(-0.3f, 0.0f, 0.0f);

	//4,8 DOWN  (-x)
		cylinder_tvec_values[17] = glm::vec3(-0.1f, 0.0f, 0.26f);
		cylinder_bvec_values[17] = glm::vec3(-0.1f, 0.0f, 0.34f);

		cylinder_radii_values[17] = 0.025f;

		cylinder_color_values[17] = vec(0.7f, 0.7f, 0.7f, 1.0f);

		cylinder_offsets[17] = glm::vec3(-0.3f, 0.0f, 0.0f);


	//ENDCAPS





		glUniform3fv(cylinder_tvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_tvec_values[0] ) );
		glUniform3fv(cylinder_bvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_bvec_values[0] ) );
		glUniform1fv(cylinder_radii_location, NUM_CYLINDERS, &cylinder_radii_values[0] );
		glUniform4fv(cylinder_colors_location, NUM_CYLINDERS, glm::value_ptr( cylinder_color_values[0] ) );

		glUniform3fv(cylinder_offsets_location, NUM_CYLINDERS, glm::value_ptr( cylinder_offsets[0] ) );

	}

}



// ----------------------



void display( void )
{

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	glDrawArrays( GL_POINTS, 0, NumVertices );

	glutSwapBuffers();

}



// ----------------------



void timer(int)
{

//ROTATE CRANKSHAFT - those located along the center line will not change, so no rotation is neccesary

	//this is the same for all the following rotations (all rotating components of the crankshaft)

	int frames_per_rotation = 360;

	float frame_rotation_increment = ( ( 2.0f * 3.14159265359f ) / frames_per_rotation );

	glm::mat4 rot = glm::rotate( frame_rotation_increment, glm::vec3( 0.0f, 0.0f, 1.0f ) );


	//ROTATE CRANKSHAFT JOURNALS (CYLINDERS index 14 to 17)
	cylinder_tvec_values[14] = rot * vec(cylinder_tvec_values[14], 1.0f);
	cylinder_bvec_values[14] = rot * vec(cylinder_bvec_values[14], 1.0f);

	cylinder_tvec_values[15] = rot * vec(cylinder_tvec_values[15], 1.0f);
	cylinder_bvec_values[15] = rot * vec(cylinder_bvec_values[15], 1.0f);

	cylinder_tvec_values[16] = rot * vec(cylinder_tvec_values[16], 1.0f);
	cylinder_bvec_values[16] = rot * vec(cylinder_bvec_values[16], 1.0f);

	cylinder_tvec_values[17] = rot * vec(cylinder_tvec_values[17], 1.0f);
	cylinder_bvec_values[17] = rot * vec(cylinder_bvec_values[17], 1.0f);


	//UPDATE THE GPU-SIDE VALUES OF ALL CYLINDERS

	glUniform3fv(cylinder_tvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_tvec_values[0] ) );
	glUniform3fv(cylinder_bvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_bvec_values[0] ) );



	//ROTATE WEBS (CUBOIDS index 1 to 7)

	cuboid_a_values[0] = rot * vec(cuboid_a_values[0], 1.0f);
	cuboid_b_values[0] = rot * vec(cuboid_b_values[0], 1.0f);
	cuboid_c_values[0] = rot * vec(cuboid_c_values[0], 1.0f);
	cuboid_d_values[0] = rot * vec(cuboid_d_values[0], 1.0f);
	cuboid_e_values[0] = rot * vec(cuboid_e_values[0], 1.0f);
	cuboid_f_values[0] = rot * vec(cuboid_f_values[0], 1.0f);
	cuboid_g_values[0] = rot * vec(cuboid_g_values[0], 1.0f);
	cuboid_h_values[0] = rot * vec(cuboid_h_values[0], 1.0f);

	cuboid_a_values[1] = rot * vec(cuboid_a_values[1], 1.0f);
	cuboid_b_values[1] = rot * vec(cuboid_b_values[1], 1.0f);
	cuboid_c_values[1] = rot * vec(cuboid_c_values[1], 1.0f);
	cuboid_d_values[1] = rot * vec(cuboid_d_values[1], 1.0f);
	cuboid_e_values[1] = rot * vec(cuboid_e_values[1], 1.0f);
	cuboid_f_values[1] = rot * vec(cuboid_f_values[1], 1.0f);
	cuboid_g_values[1] = rot * vec(cuboid_g_values[1], 1.0f);
	cuboid_h_values[1] = rot * vec(cuboid_h_values[1], 1.0f);

	cuboid_a_values[2] = rot * vec(cuboid_a_values[2], 1.0f);
	cuboid_b_values[2] = rot * vec(cuboid_b_values[2], 1.0f);
	cuboid_c_values[2] = rot * vec(cuboid_c_values[2], 1.0f);
	cuboid_d_values[2] = rot * vec(cuboid_d_values[2], 1.0f);
	cuboid_e_values[2] = rot * vec(cuboid_e_values[2], 1.0f);
	cuboid_f_values[2] = rot * vec(cuboid_f_values[2], 1.0f);
	cuboid_g_values[2] = rot * vec(cuboid_g_values[2], 1.0f);
	cuboid_h_values[2] = rot * vec(cuboid_h_values[2], 1.0f);

	cuboid_a_values[3] = rot * vec(cuboid_a_values[3], 1.0f);
	cuboid_b_values[3] = rot * vec(cuboid_b_values[3], 1.0f);
	cuboid_c_values[3] = rot * vec(cuboid_c_values[3], 1.0f);
	cuboid_d_values[3] = rot * vec(cuboid_d_values[3], 1.0f);
	cuboid_e_values[3] = rot * vec(cuboid_e_values[3], 1.0f);
	cuboid_f_values[3] = rot * vec(cuboid_f_values[3], 1.0f);
	cuboid_g_values[3] = rot * vec(cuboid_g_values[3], 1.0f);
	cuboid_h_values[3] = rot * vec(cuboid_h_values[3], 1.0f);

	cuboid_a_values[4] = rot * vec(cuboid_a_values[4], 1.0f);
	cuboid_b_values[4] = rot * vec(cuboid_b_values[4], 1.0f);
	cuboid_c_values[4] = rot * vec(cuboid_c_values[4], 1.0f);
	cuboid_d_values[4] = rot * vec(cuboid_d_values[4], 1.0f);
	cuboid_e_values[4] = rot * vec(cuboid_e_values[4], 1.0f);
	cuboid_f_values[4] = rot * vec(cuboid_f_values[4], 1.0f);
	cuboid_g_values[4] = rot * vec(cuboid_g_values[4], 1.0f);
	cuboid_h_values[4] = rot * vec(cuboid_h_values[4], 1.0f);

	cuboid_a_values[5] = rot * vec(cuboid_a_values[5], 1.0f);
	cuboid_b_values[5] = rot * vec(cuboid_b_values[5], 1.0f);
	cuboid_c_values[5] = rot * vec(cuboid_c_values[5], 1.0f);
	cuboid_d_values[5] = rot * vec(cuboid_d_values[5], 1.0f);
	cuboid_e_values[5] = rot * vec(cuboid_e_values[5], 1.0f);
	cuboid_f_values[5] = rot * vec(cuboid_f_values[5], 1.0f);
	cuboid_g_values[5] = rot * vec(cuboid_g_values[5], 1.0f);
	cuboid_h_values[5] = rot * vec(cuboid_h_values[5], 1.0f);

	cuboid_a_values[6] = rot * vec(cuboid_a_values[6], 1.0f);
	cuboid_b_values[6] = rot * vec(cuboid_b_values[6], 1.0f);
	cuboid_c_values[6] = rot * vec(cuboid_c_values[6], 1.0f);
	cuboid_d_values[6] = rot * vec(cuboid_d_values[6], 1.0f);
	cuboid_e_values[6] = rot * vec(cuboid_e_values[6], 1.0f);
	cuboid_f_values[6] = rot * vec(cuboid_f_values[6], 1.0f);
	cuboid_g_values[6] = rot * vec(cuboid_g_values[6], 1.0f);
	cuboid_h_values[6] = rot * vec(cuboid_h_values[6], 1.0f);

	cuboid_a_values[7] = rot * vec(cuboid_a_values[7], 1.0f);
	cuboid_b_values[7] = rot * vec(cuboid_b_values[7], 1.0f);
	cuboid_c_values[7] = rot * vec(cuboid_c_values[7], 1.0f);
	cuboid_d_values[7] = rot * vec(cuboid_d_values[7], 1.0f);
	cuboid_e_values[7] = rot * vec(cuboid_e_values[7], 1.0f);
	cuboid_f_values[7] = rot * vec(cuboid_f_values[7], 1.0f);
	cuboid_g_values[7] = rot * vec(cuboid_g_values[7], 1.0f);
	cuboid_h_values[7] = rot * vec(cuboid_h_values[7], 1.0f);

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

			x_rot = 0.0f;
			y_rot = 45.0f;
			z_rot = 90.0f;

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
					//do something for the middle mouse button
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
	glutInitWindowSize( image_width, image_height );
	// glutInitWindowSize( 500, 500 );
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




	// for(int i = 0; i < NUM_TRIANGLES; i++)
	// {
	// 	cout << vec(0.75f + 0.25 * sin(i + 1.5), 0.25f + 0.25 * sin(i), 0.5f + 0.5 * cos(i + 2.0), 1.0f)[0] << " "
	// 				<< vec(0.75f + 0.25 * sin(i + 1.5), 0.25f + 0.25 * sin(i), 0.5f + 0.5 * cos(i + 2.0), 1.0f)[1] << " "
	// 				 << vec(0.75f + 0.25 * sin(i + 1.5), 0.25f + 0.25 * sin(i), 0.5f + 0.5 * cos(i + 2.0), 1.0f)[2] << " "
	// 				  << vec(0.75f + 0.25 * sin(i + 1.5), 0.25f + 0.25 * sin(i), 0.5f + 0.5 * cos(i + 2.0), 1.0f)[3] << endl;
	// }


// colors 1-12

// 0.999374 	0.25 				0.291927 		1
// 0.899618 	0.460368 		0.00500375 	1
// 0.662304 	0.477324 		0.173178 		1
// 0.505617 	0.28528 		0.641831 		1
// 0.573615 	0.0607994 	0.980085 		1
// 0.80378 		0.0102689 	0.876951 		1
// 0.9845 		0.180146 		0.42725 		1
// 0.949622 	0.414247 		0.0444349 	1
// 0.731212 	0.49734 		0.0804642 	1
// 0.530076 	0.35303 		0.502213 		1
// 0.531137 	0.113995 		0.921927 		1
// 0.73342 		2.44836e-06 0.953723 		1






	cout << "Shader Compilation Starting...";

	Shader theShader( "vertex_crankshaft.glsl", "fragment_crankshaft.glsl" );

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
