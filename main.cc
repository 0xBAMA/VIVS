#include <iostream> //terminal I/O
using std::cout;
using std::endl;
using std::cin;

#include <vector>   //container

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
#define NUM_TRIANGLES 8
#define NUM_QUAD_HEXS 16
#define NUM_CYLINDERS 42
#define NUM_TUBES			48

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








//STUFF SPECIFIC TO THIS DEMO

glm::vec3 standard_offset = glm::vec3(-0.25f, 0.0f, 0.0f);

float pressure_chamber_alpha = 0.1f;

float crank_to_bottom_of_cylinder = 0.175f;
float bottom_of_cylinder_to_top = 0.22f;

float engine_cylinder_radius = 0.092f;

float pressure_chamber_scale = 0.9f;


// offsets for the cylinders along the crank
float cylinder_1_z_offset;
float cylinder_2_z_offset;
float cylinder_3_z_offset;
float cylinder_4_z_offset;
float cylinder_5_z_offset;
float cylinder_6_z_offset;
float cylinder_7_z_offset;
float cylinder_8_z_offset;

// center of the bearing connection - used to locate the bottom side of the connecting rod
glm::vec3 cylinder_1_rod_connection;
glm::vec3 cylinder_2_rod_connection;
glm::vec3 cylinder_3_rod_connection;
glm::vec3 cylinder_4_rod_connection;
glm::vec3 cylinder_5_rod_connection;
glm::vec3 cylinder_6_rod_connection;
glm::vec3 cylinder_7_rod_connection;
glm::vec3 cylinder_8_rod_connection;

//location of the top of the piston
glm::vec3 cylinder_1_piston_location;
glm::vec3 cylinder_2_piston_location;
glm::vec3 cylinder_3_piston_location;
glm::vec3 cylinder_4_piston_location;
glm::vec3 cylinder_5_piston_location;
glm::vec3 cylinder_6_piston_location;
glm::vec3 cylinder_7_piston_location;
glm::vec3 cylinder_8_piston_location;



// where the bottom of the piston is
glm::vec3 bank1_start_vector = crank_to_bottom_of_cylinder * glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
glm::vec3 bank2_start_vector = crank_to_bottom_of_cylinder * glm::normalize(glm::vec3(1.0f,  1.0f, 0.0f));

// where the top of the cylinder is
glm::vec3 bank1_end_vector = bank1_start_vector + bottom_of_cylinder_to_top * glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));
glm::vec3 bank2_end_vector = bank2_start_vector + bottom_of_cylinder_to_top * glm::normalize(glm::vec3(1.0f,  1.0f, 0.0f));

glm::vec3 bank1_middle_vector = ( bank1_start_vector + bank1_end_vector ) / 2.0f;
glm::vec3 bank2_middle_vector = ( bank2_start_vector + bank2_end_vector ) / 2.0f;

// distance to move down the cylinder to locate the bottom of the piston
glm::vec3 bank1_piston_offset = -0.1f * ((bank1_start_vector + bank1_end_vector) / 2.0f);
glm::vec3 bank2_piston_offset = -0.1f * ((bank2_start_vector + bank2_end_vector) / 2.0f);

// use the cylinder_n_piston_location to locate the top of piston n, then use
// cylinder_n_piston_location + bankn_piston_offset for the appropriate bank to locate the bottom




float con_rod_crank_connection_radius = 0.05f;
float bearing_cutout_diameter = 0.035;
float bearing_diameter = 0.03;


float cylinder_1_start = -0.33f;
float cylinder_1_end = -0.305f;

float cylinder_2_start = -0.13f;
float cylinder_2_end = -0.105f;


float cylinder_3_start = 0.07f;
float cylinder_3_end = 0.095f;


float cylinder_4_start = 0.27f;
float cylinder_4_end = 0.295f;


float cylinder_5_start = -0.29f;
float cylinder_5_end = -0.265f;


float cylinder_6_start = -0.095f;
float cylinder_6_end = -0.07f;


float cylinder_7_start = 0.105f;
float cylinder_7_end = 0.13f;


float cylinder_8_start = 0.305f;
float cylinder_8_end = 0.33f;














//colors

// vec ambient_color = vec(0.2f, 0.0f, 0.08f, 0.06f);

vec ambient_color = vec(0.2f, 0.0f, 0.1f, 0.006f);
vec crank_color = vec(0.3f, 0.3f, 0.3f, 0.55f);
vec liner_color = vec(0.2f, 0.2f, 0.2f, 0.3f);
vec rod_journal_color = vec(0.7f, 0.7f, 0.7f, 0.5f);
// vec rod_bearing_color = vec(1.0f, 0.5f, 0.0f, 0.65f);
vec rod_bearing_color = vec(1.0f, 0.5f, 0.0f, 1.0f);


//yet to be established
vec piston_color = vec(0.25f, 0.35f, 0.3f, 1.0f);
vec piston_ring_color = vec(0.6f, 0.5f, 0.0f, 0.5f);
vec con_rod_color = vec(0.5f, 0.25f, 0.05f, 1.0f);






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







// SHADER STUFF

GLuint shader_handle;

GLuint texture; //handle for the texture



void update_rotation();
void timer(int); //need to forward declare this for the initialization


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
		sphere_colors_values[0] = ambient_color;
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





		// triangle_color_values[0] = con_rod_color;



		thickness[0] = 0.02f;
		triangle_offsets[0] = standard_offset;
		triangle_color_values[0] = con_rod_color;


		thickness[1] = 0.02f;
		triangle_offsets[1] = standard_offset;
		triangle_color_values[1] = con_rod_color;


		thickness[2] = 0.02f;
		triangle_offsets[2] = standard_offset;
		triangle_color_values[2] = con_rod_color;


		thickness[3] = 0.02f;
		triangle_offsets[3] = standard_offset;
		triangle_color_values[3] = con_rod_color;


		thickness[4] = 0.02f;
		triangle_offsets[4] = standard_offset;
		triangle_color_values[4] = con_rod_color;


		thickness[5] = 0.02f;
		triangle_offsets[5] = standard_offset;
		triangle_color_values[5] = con_rod_color;


		thickness[6] = 0.02f;
		triangle_offsets[6] = standard_offset;
		triangle_color_values[6] = con_rod_color;


		thickness[7] = 0.02f;
		triangle_offsets[7] = standard_offset;
		triangle_color_values[7] = con_rod_color;


		// triangle_point1_values[0] = glm::vec3(0.0f, 1.0f, 0.0f);
		// triangle_point2_values[0] = glm::vec3(1.0f, 0.0f, 0.0f);
		// triangle_point3_values[0] = glm::vec3(0.0f, 0.0f, 1.0f);









		// PUT INITIAL GEOMETRY HERE







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
		cuboid_color_values[0] = crank_color;
		cuboid_offsets[0] = standard_offset;

		cuboid_a_values[1] = a2;
		cuboid_b_values[1] = b2;
		cuboid_c_values[1] = c2;
		cuboid_d_values[1] = d2;
		cuboid_e_values[1] = e2;
		cuboid_f_values[1] = f2;
		cuboid_g_values[1] = g2;
		cuboid_h_values[1] = h2;
		cuboid_color_values[1] = crank_color;
		cuboid_offsets[1] = standard_offset;



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
		cuboid_color_values[2] = crank_color;
		cuboid_offsets[2] = standard_offset;


		cuboid_a_values[3] = a2;
		cuboid_b_values[3] = b2;
		cuboid_c_values[3] = c2;
		cuboid_d_values[3] = d2;
		cuboid_e_values[3] = e2;
		cuboid_f_values[3] = f2;
		cuboid_g_values[3] = g2;
		cuboid_h_values[3] = h2;
		cuboid_color_values[3] = crank_color;
		cuboid_offsets[3] = standard_offset;





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
		cuboid_color_values[4] = crank_color;
		cuboid_offsets[4] = standard_offset;

		cuboid_a_values[5] = a2;
		cuboid_b_values[5] = b2;
		cuboid_c_values[5] = c2;
		cuboid_d_values[5] = d2;
		cuboid_e_values[5] = e2;
		cuboid_f_values[5] = f2;
		cuboid_g_values[5] = g2;
		cuboid_h_values[5] = h2;
		cuboid_color_values[5] = crank_color;
		cuboid_offsets[5] = standard_offset;





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
		cuboid_color_values[6] = crank_color;
		cuboid_offsets[6] = standard_offset;

		cuboid_a_values[7] = a2;
		cuboid_b_values[7] = b2;
		cuboid_c_values[7] = c2;
		cuboid_d_values[7] = d2;
		cuboid_e_values[7] = e2;
		cuboid_f_values[7] = f2;
		cuboid_g_values[7] = g2;
		cuboid_h_values[7] = h2;
		cuboid_color_values[7] = crank_color;
		cuboid_offsets[7] = standard_offset;


//con rods
		cuboid_color_values[8] = con_rod_color;
		cuboid_offsets[8] = standard_offset;

		cuboid_color_values[9] = con_rod_color;
		cuboid_offsets[9] = standard_offset;

		cuboid_color_values[10] = con_rod_color;
		cuboid_offsets[10] = standard_offset;

		cuboid_color_values[11] = con_rod_color;
		cuboid_offsets[11] = standard_offset;

		cuboid_color_values[12] = con_rod_color;
		cuboid_offsets[12] = standard_offset;

		cuboid_color_values[13] = con_rod_color;
		cuboid_offsets[13] = standard_offset;

		cuboid_color_values[14] = con_rod_color;
		cuboid_offsets[14] = standard_offset;

		cuboid_color_values[15] = con_rod_color;
		cuboid_offsets[15] = standard_offset;





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






















//cylinder values


// - I forgot this is going to have to take place before the cylinder liner or else that's going to be an issue I think



//MAIN JOURNALS (aligned with the centerline of the crankshaft)

		cylinder_tvec_values[0] = glm::vec3(0.0f, 0.0f, -0.45f);
		cylinder_bvec_values[0] = glm::vec3(0.0f, 0.0f, -0.35f);

		cylinder_radii_values[0] = 0.03f;

		cylinder_color_values[0] = crank_color;

		cylinder_offsets[0] = standard_offset;




		cylinder_tvec_values[1] = glm::vec3(0.0f, 0.0f, -0.37f);
		cylinder_bvec_values[1] = glm::vec3(0.0f, 0.0f, -0.35f);

		cylinder_radii_values[1] = 0.04f;

		cylinder_color_values[1] = crank_color;

		cylinder_offsets[1] = standard_offset;



		//-------




		cylinder_tvec_values[2] = glm::vec3(0.0f, 0.0f, -0.25f);
		cylinder_bvec_values[2] = glm::vec3(0.0f, 0.0f, -0.23f);

		cylinder_radii_values[2] = 0.04f;

		cylinder_color_values[2] = crank_color;

		cylinder_offsets[2] = standard_offset;




		cylinder_tvec_values[3] = glm::vec3(0.0f, 0.0f, -0.25f);
		cylinder_bvec_values[3] = glm::vec3(0.0f, 0.0f, -0.15f);

		cylinder_radii_values[3] = 0.03f;

		cylinder_color_values[3] = crank_color;

		cylinder_offsets[3] = standard_offset;




		cylinder_tvec_values[4] = glm::vec3(0.0f, 0.0f, -0.17f);
		cylinder_bvec_values[4] = glm::vec3(0.0f, 0.0f, -0.15f);

		cylinder_radii_values[4] = 0.04f;

		cylinder_color_values[4] = crank_color;

		cylinder_offsets[4] = standard_offset;



		//-------



		cylinder_tvec_values[6] = glm::vec3(0.0f, 0.0f, -0.05f);
		cylinder_bvec_values[6] = glm::vec3(0.0f, 0.0f, -0.03f);

		cylinder_radii_values[6] = 0.04f;

		cylinder_color_values[6] = crank_color;

		cylinder_offsets[6] = standard_offset;




		cylinder_tvec_values[7] = glm::vec3(0.0f, 0.0f, -0.05f);
		cylinder_bvec_values[7] = glm::vec3(0.0f, 0.0f, 0.05f);

		cylinder_radii_values[7] = 0.03f;

		cylinder_color_values[7] = crank_color;

		cylinder_offsets[7] = standard_offset;




		cylinder_tvec_values[8] = glm::vec3(0.0f, 0.0f, 0.03f);
		cylinder_bvec_values[8] = glm::vec3(0.0f, 0.0f, 0.05f);

		cylinder_radii_values[8] = 0.04f;

		cylinder_color_values[8] = crank_color;

		cylinder_offsets[8] = standard_offset;



		//-------




		cylinder_tvec_values[9] = glm::vec3(0.0f, 0.0f, 0.15f);
		cylinder_bvec_values[9] = glm::vec3(0.0f, 0.0f, 0.17f);

		cylinder_radii_values[9] = 0.04f;

		cylinder_color_values[9] = crank_color;

		cylinder_offsets[9] = standard_offset;




		cylinder_tvec_values[10] = glm::vec3(0.0f, 0.0f, 0.15f);
		cylinder_bvec_values[10] = glm::vec3(0.0f, 0.0f, 0.25f);

		cylinder_radii_values[10] = 0.03f;

		cylinder_color_values[10] = crank_color;

		cylinder_offsets[10] = standard_offset;




		cylinder_tvec_values[11] = glm::vec3(0.0f, 0.0f, 0.23f);
		cylinder_bvec_values[11] = glm::vec3(0.0f, 0.0f, 0.25f);

		cylinder_radii_values[11] = 0.04f;

		cylinder_color_values[11] = crank_color;

		cylinder_offsets[11] = standard_offset;



		//-------



		cylinder_tvec_values[12] = glm::vec3(0.0f, 0.0f, 0.35f);
		cylinder_bvec_values[12] = glm::vec3(0.0f, 0.0f, 0.37f);

		cylinder_radii_values[12] = 0.04f;

		cylinder_color_values[12] = crank_color;

		cylinder_offsets[12] = standard_offset;




		cylinder_tvec_values[13] = glm::vec3(0.0f, 0.0f, 0.35f);
		cylinder_bvec_values[13] = glm::vec3(0.0f, 0.0f, 0.45f);

		cylinder_radii_values[13] = 0.03f;

		cylinder_color_values[13] = crank_color;

		cylinder_offsets[13] = standard_offset;






//CONNECTING ROD CONNECTIONS - there are 8 connecting rods
// reference: https://image.slidesharecdn.com/pistonmanufacturing-150201085312-conversion-gate01/95/piston-manufacturing-process-5-638.jpg?cb=1422802465



		cylinder_1_z_offset = (cylinder_1_start + cylinder_1_end) / 2.0f;
		cylinder_2_z_offset = (cylinder_2_start + cylinder_2_end) / 2.0f;
		cylinder_3_z_offset = (cylinder_3_start + cylinder_3_end) / 2.0f;
		cylinder_4_z_offset = (cylinder_4_start + cylinder_4_end) / 2.0f;
		cylinder_5_z_offset = (cylinder_5_start + cylinder_5_end) / 2.0f;
		cylinder_6_z_offset = (cylinder_6_start + cylinder_6_end) / 2.0f;
		cylinder_7_z_offset = (cylinder_7_start + cylinder_7_end) / 2.0f;
		cylinder_8_z_offset = (cylinder_8_start + cylinder_8_end) / 2.0f;










		//cylinder pressure chamber (variable size)

		cylinder_tvec_values[14] = bank1_end_vector + glm::vec3(0.0f, 0.0f, cylinder_1_z_offset);
		cylinder_bvec_values[14] = bank1_start_vector + glm::vec3(0.0f, 0.0f, cylinder_1_z_offset);

		cylinder_radii_values[14] = engine_cylinder_radius * pressure_chamber_scale;

		cylinder_color_values[14] = vec(1.0f, 0.05f, 0.0f, pressure_chamber_alpha);

		cylinder_offsets[14] = standard_offset;

		//cylinder pressure chamber (variable size)

		cylinder_tvec_values[15] = bank1_end_vector + glm::vec3(0.0f, 0.0f, cylinder_2_z_offset);
		cylinder_bvec_values[15] = bank1_start_vector + glm::vec3(0.0f, 0.0f, cylinder_2_z_offset);

		cylinder_radii_values[15] = engine_cylinder_radius * pressure_chamber_scale;

		cylinder_color_values[15] = vec(1.0f, 0.05f, 0.0f, pressure_chamber_alpha);

		cylinder_offsets[15] = standard_offset;

		//cylinder pressure chamber (variable size)

		cylinder_tvec_values[16] = bank1_end_vector + glm::vec3(0.0f, 0.0f, cylinder_3_z_offset);
		cylinder_bvec_values[16] = bank1_start_vector + glm::vec3(0.0f, 0.0f, cylinder_3_z_offset);

		cylinder_radii_values[16] = engine_cylinder_radius * pressure_chamber_scale;

		cylinder_color_values[16] = vec(1.0f, 0.05f, 0.0f, pressure_chamber_alpha);

		cylinder_offsets[16] = standard_offset;


		//cylinder pressure chamber (variable size)

		cylinder_tvec_values[17] = bank1_end_vector + glm::vec3(0.0f, 0.0f, cylinder_4_z_offset);
		cylinder_bvec_values[17] = bank1_start_vector + glm::vec3(0.0f, 0.0f, cylinder_4_z_offset);

		cylinder_radii_values[17] = engine_cylinder_radius * pressure_chamber_scale;

		cylinder_color_values[17] = vec(1.0f, 0.05f, 0.0f, pressure_chamber_alpha);

		cylinder_offsets[17] = standard_offset;

		//cylinder pressure chamber (variable size)

		cylinder_tvec_values[18] = bank2_end_vector + glm::vec3(0.0f, 0.0f, cylinder_5_z_offset);
		cylinder_bvec_values[18] = bank2_start_vector + glm::vec3(0.0f, 0.0f, cylinder_5_z_offset);

		cylinder_radii_values[18] = engine_cylinder_radius * pressure_chamber_scale;

		cylinder_color_values[18] = vec(1.0f, 0.05f, 0.0f, pressure_chamber_alpha);

		cylinder_offsets[18] = standard_offset;

		//cylinder pressure chamber (variable size)

		cylinder_tvec_values[19] = bank2_end_vector + glm::vec3(0.0f, 0.0f, cylinder_6_z_offset);
		cylinder_bvec_values[19] = bank2_start_vector + glm::vec3(0.0f, 0.0f, cylinder_6_z_offset);

		cylinder_radii_values[19] = engine_cylinder_radius * pressure_chamber_scale;

		cylinder_color_values[19] = vec(1.0f, 0.05f, 0.0f, pressure_chamber_alpha);

		cylinder_offsets[19] = standard_offset;

		//cylinder pressure chamber (variable size)

		cylinder_tvec_values[20] = bank2_end_vector + glm::vec3(0.0f, 0.0f, cylinder_7_z_offset);
		cylinder_bvec_values[20] = bank2_start_vector + glm::vec3(0.0f, 0.0f, cylinder_7_z_offset);

		cylinder_radii_values[20] = engine_cylinder_radius * pressure_chamber_scale;

		cylinder_color_values[20] = vec(1.0f, 0.05f, 0.0f, pressure_chamber_alpha);

		cylinder_offsets[20] = standard_offset;

		//cylinder pressure chamber (variable size)

		cylinder_tvec_values[21] = bank2_end_vector + glm::vec3(0.0f, 0.0f, cylinder_8_z_offset);
		cylinder_bvec_values[21] = bank2_start_vector + glm::vec3(0.0f, 0.0f, cylinder_8_z_offset);

		cylinder_radii_values[21] = engine_cylinder_radius * pressure_chamber_scale;

		cylinder_color_values[21] = vec(1.0f, 0.05f, 0.0f, pressure_chamber_alpha);

		cylinder_offsets[21] = standard_offset;



	//ROD JOURNALS - must be added after the bearings, cutouts, and bottoms of con rods

	// https://i.kinja-img.com/gawker-media/image/upload/s--TjZVv1cf--/c_scale,f_auto,fl_progressive,q_80,w_800/lijsom4kg4vspuddmp4g.jpg



		//1,5 UP  (+x)
			cylinder_tvec_values[22] = glm::vec3(0.1f, 0.0f, -0.34f);
			cylinder_bvec_values[22] = glm::vec3(0.1f, 0.0f, -0.26f);

			cylinder_radii_values[22] = 0.025f;

			cylinder_color_values[22] = rod_journal_color;

			cylinder_offsets[22] = standard_offset;


		//2,6 RIGHT  (+y)
			cylinder_tvec_values[23] = glm::vec3(0.0f, 0.1f, -0.14f);
			cylinder_bvec_values[23] = glm::vec3(0.0f, 0.1f, -0.06f);

			cylinder_radii_values[23] = 0.025f;

			cylinder_color_values[23] = rod_journal_color;

			cylinder_offsets[23] = standard_offset;


		//3,7 LEFT   (-y)
			cylinder_tvec_values[24] = glm::vec3(0.0f, -0.1f, 0.06f);
			cylinder_bvec_values[24] = glm::vec3(0.0f, -0.1f, 0.14f);

			cylinder_radii_values[24] = 0.025f;

			cylinder_color_values[24] = rod_journal_color;

			cylinder_offsets[24] = standard_offset;

		//4,8 DOWN  (-x)
			cylinder_tvec_values[25] = glm::vec3(-0.1f, 0.0f, 0.26f);
			cylinder_bvec_values[25] = glm::vec3(-0.1f, 0.0f, 0.34f);

			cylinder_radii_values[25] = 0.025f;

			cylinder_color_values[25] = rod_journal_color;

			cylinder_offsets[25] = standard_offset;





		// 26 to 33 are piston tops - bvec and tvec are set by timer()
		float piston_radius = engine_cylinder_radius * 0.95f;

		cylinder_radii_values[26] = piston_radius;
		cylinder_color_values[26] = piston_color;
		cylinder_offsets[26] = standard_offset;

		cylinder_radii_values[27] = piston_radius;
		cylinder_color_values[27] = piston_color;
		cylinder_offsets[27] = standard_offset;

		cylinder_radii_values[28] = piston_radius;
		cylinder_color_values[28] = piston_color;
		cylinder_offsets[28] = standard_offset;

		cylinder_radii_values[29] = piston_radius;
		cylinder_color_values[29] = piston_color;
		cylinder_offsets[29] = standard_offset;

		cylinder_radii_values[30] = piston_radius;
		cylinder_color_values[30] = piston_color;
		cylinder_offsets[30] = standard_offset;

		cylinder_radii_values[31] = piston_radius;
		cylinder_color_values[31] = piston_color;
		cylinder_offsets[31] = standard_offset;

		cylinder_radii_values[32] = piston_radius;
		cylinder_color_values[32] = piston_color;
		cylinder_offsets[32] = standard_offset;

		cylinder_radii_values[33] = piston_radius;
		cylinder_color_values[33] = piston_color;
		cylinder_offsets[33] = standard_offset;




		//wrist pins
		float wrist_pin_radius = 0.02f;

		cylinder_radii_values[34] = wrist_pin_radius;
		cylinder_color_values[34] = crank_color;
		cylinder_offsets[34] = standard_offset;

		cylinder_radii_values[35] = wrist_pin_radius;
		cylinder_color_values[35] = crank_color;
		cylinder_offsets[35] = standard_offset;

		cylinder_radii_values[36] = wrist_pin_radius;
		cylinder_color_values[36] = crank_color;
		cylinder_offsets[36] = standard_offset;

		cylinder_radii_values[37] = wrist_pin_radius;
		cylinder_color_values[37] = crank_color;
		cylinder_offsets[37] = standard_offset;

		cylinder_radii_values[38] = wrist_pin_radius;
		cylinder_color_values[38] = crank_color;
		cylinder_offsets[38] = standard_offset;

		cylinder_radii_values[39] = wrist_pin_radius;
		cylinder_color_values[39] = crank_color;
		cylinder_offsets[39] = standard_offset;

		cylinder_radii_values[40] = wrist_pin_radius;
		cylinder_color_values[40] = crank_color;
		cylinder_offsets[40] = standard_offset;

		cylinder_radii_values[41] = wrist_pin_radius;
		cylinder_color_values[41] = crank_color;
		cylinder_offsets[41] = standard_offset;



		// cylinder_tvec_values[66] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[66] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[66] = 0.0f;
		// cylinder_color_values[66] = ambient_color;
		// cylinder_offsets[66] = standard_offset;
		//
		// cylinder_tvec_values[67] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[67] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[67] = 0.0f;
		// cylinder_color_values[67] = ambient_color;
		// cylinder_offsets[67] = standard_offset;
		//
		// cylinder_tvec_values[68] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[68] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[68] = 0.0f;
		// cylinder_color_values[68] = ambient_color;
		// cylinder_offsets[68] = standard_offset;
		//
		// cylinder_tvec_values[69] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[69] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[69] = 0.0f;
		// cylinder_color_values[69] = ambient_color;
		// cylinder_offsets[69] = standard_offset;
		//
		//
		// cylinder_tvec_values[70] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[70] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[70] = 0.0f;
		// cylinder_color_values[70] = ambient_color;
		// cylinder_offsets[70] = standard_offset;
		//
		// cylinder_tvec_values[71] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[71] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[71] = 0.0f;
		// cylinder_color_values[71] = ambient_color;
		// cylinder_offsets[71] = standard_offset;
		//
		// cylinder_tvec_values[72] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[72] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[72] = 0.0f;
		// cylinder_color_values[72] = ambient_color;
		// cylinder_offsets[72] = standard_offset;
		//
		// cylinder_tvec_values[73] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[73] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[73] = 0.0f;
		// cylinder_color_values[73] = ambient_color;
		// cylinder_offsets[73] = standard_offset;
		//
		// cylinder_tvec_values[74] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[74] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[74] = 0.0f;
		// cylinder_color_values[74] = ambient_color;
		// cylinder_offsets[74] = standard_offset;
		//
		// cylinder_tvec_values[75] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[75] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[75] = 0.0f;
		// cylinder_color_values[75] = ambient_color;
		// cylinder_offsets[75] = standard_offset;
		//
		// cylinder_tvec_values[76] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_bvec_values[76] = glm::vec3(0.0f, 0.0f, 0.0f);
		// cylinder_radii_values[76] = 0.0f;
		// cylinder_color_values[76] = ambient_color;
		// cylinder_offsets[76] = standard_offset;






// TUBES





	//cylinder

	tube_tvec_values[0] = bank1_start_vector + glm::vec3(0.0f, 0.0f, (cylinder_1_start + cylinder_1_end) / 2.0f);
	tube_bvec_values[0] = bank1_end_vector + glm::vec3(0.0f, 0.0f, (cylinder_1_start + cylinder_1_end) / 2.0f);

	tube_outer_radii_values[0] = engine_cylinder_radius;
	tube_inner_radii_values[0] = engine_cylinder_radius * pressure_chamber_scale;

	tube_color_values[0] = liner_color;

	tube_offsets[0] = standard_offset;

	//cylinder

	tube_tvec_values[1] = bank1_start_vector + glm::vec3(0.0f, 0.0f, (cylinder_2_start + cylinder_2_end) / 2.0f);
	tube_bvec_values[1] = bank1_end_vector + glm::vec3(0.0f, 0.0f, (cylinder_2_start + cylinder_2_end) / 2.0f);

	tube_outer_radii_values[1] = engine_cylinder_radius;
	tube_inner_radii_values[1] = engine_cylinder_radius * pressure_chamber_scale;

	tube_color_values[1] = liner_color;

	tube_offsets[1] = standard_offset;

	//cylinder

	tube_tvec_values[2] = bank1_start_vector + glm::vec3(0.0f, 0.0f, (cylinder_3_start + cylinder_3_end) / 2.0f);
	tube_bvec_values[2] = bank1_end_vector + glm::vec3(0.0f, 0.0f, (cylinder_3_start + cylinder_3_end) / 2.0f);

	tube_outer_radii_values[2] = engine_cylinder_radius;
	tube_inner_radii_values[2] = engine_cylinder_radius * pressure_chamber_scale;

	tube_color_values[2] = liner_color;

	tube_offsets[2] = standard_offset;

	//cylinder

	tube_tvec_values[3] = bank1_start_vector + glm::vec3(0.0f, 0.0f, (cylinder_4_start + cylinder_4_end) / 2.0f);
	tube_bvec_values[3] = bank1_end_vector + glm::vec3(0.0f, 0.0f, (cylinder_4_start + cylinder_4_end) / 2.0f);

	tube_outer_radii_values[3] = engine_cylinder_radius;
	tube_inner_radii_values[3] = engine_cylinder_radius * pressure_chamber_scale;

	tube_color_values[3] = liner_color;

	tube_offsets[3] = standard_offset;

	//cylinder

	tube_tvec_values[4] = bank2_start_vector + glm::vec3(0.0f, 0.0f, (cylinder_5_start + cylinder_5_end) / 2.0f);
	tube_bvec_values[4] = bank2_end_vector + glm::vec3(0.0f, 0.0f, (cylinder_5_start + cylinder_5_end) / 2.0f);

	tube_outer_radii_values[4] = engine_cylinder_radius;
	tube_inner_radii_values[4] = engine_cylinder_radius * pressure_chamber_scale;

	tube_color_values[4] = liner_color;

	tube_offsets[4] = standard_offset;

	//cylinder

	tube_tvec_values[5] = bank2_start_vector + glm::vec3(0.0f, 0.0f, (cylinder_6_start + cylinder_6_end) / 2.0f);
	tube_bvec_values[5] = bank2_end_vector + glm::vec3(0.0f, 0.0f, (cylinder_6_start + cylinder_6_end) / 2.0f);

	tube_outer_radii_values[5] = engine_cylinder_radius;
	tube_inner_radii_values[5] = engine_cylinder_radius * pressure_chamber_scale;

	tube_color_values[5] = liner_color;

	tube_offsets[5] = standard_offset;

	//cylinder

	tube_tvec_values[6] = bank2_start_vector + glm::vec3(0.0f, 0.0f, (cylinder_7_start + cylinder_7_end) / 2.0f);
	tube_bvec_values[6] = bank2_end_vector + glm::vec3(0.0f, 0.0f, (cylinder_7_start + cylinder_7_end) / 2.0f);

	tube_outer_radii_values[6] = engine_cylinder_radius;
	tube_inner_radii_values[6] = engine_cylinder_radius * pressure_chamber_scale;

	tube_color_values[6] = liner_color;

	tube_offsets[6] = standard_offset;

	//cylinder

	tube_tvec_values[7] = bank2_start_vector + glm::vec3(0.0f, 0.0f, (cylinder_8_start + cylinder_8_end) / 2.0f);
	tube_bvec_values[7] = bank2_end_vector + glm::vec3(0.0f, 0.0f, (cylinder_8_start + cylinder_8_end) / 2.0f);

	tube_outer_radii_values[7] = engine_cylinder_radius;
	tube_inner_radii_values[7] = engine_cylinder_radius * pressure_chamber_scale;

	tube_color_values[7] = liner_color;

	tube_offsets[7] = standard_offset;








	//bearing

	tube_tvec_values[8] = glm::vec3(0.1f, 0.0f, cylinder_1_start);
	tube_bvec_values[8] = glm::vec3(0.1f, 0.0f, cylinder_1_end);

	tube_inner_radii_values[8] = bearing_diameter;
	tube_outer_radii_values[8] = bearing_cutout_diameter;

	tube_color_values[8] = rod_bearing_color;

	tube_offsets[8] = standard_offset;

	//bearing

	tube_tvec_values[9] = glm::vec3(0.0f, 0.1f, cylinder_2_start);
	tube_bvec_values[9] = glm::vec3(0.0f, 0.1f, cylinder_2_end);

	tube_inner_radii_values[9] = bearing_diameter;
	tube_outer_radii_values[9] = bearing_cutout_diameter;

	tube_color_values[9] = rod_bearing_color;

	tube_offsets[9] = standard_offset;

	//bearing

	tube_tvec_values[10] = glm::vec3(0.0f, -0.1f, cylinder_3_start);
	tube_bvec_values[10] = glm::vec3(0.0f, -0.1f, cylinder_3_end);

	tube_inner_radii_values[10] = bearing_diameter;
	tube_outer_radii_values[10] = bearing_cutout_diameter;

	tube_color_values[10] = rod_bearing_color;

	tube_offsets[10] = standard_offset;

	//bearing

	tube_tvec_values[11] = glm::vec3(-0.1f, 0.0f, cylinder_4_start);
	tube_bvec_values[11] = glm::vec3(-0.1f, 0.0f, cylinder_4_end);

	tube_inner_radii_values[11] = bearing_diameter;
	tube_outer_radii_values[11] = bearing_cutout_diameter;

	tube_color_values[11] = rod_bearing_color;

	tube_offsets[11] = standard_offset;


	//bearing

	tube_tvec_values[12] = glm::vec3(0.1f, 0.0f, cylinder_5_start);
	tube_bvec_values[12] = glm::vec3(0.1f, 0.0f, cylinder_5_end);

	tube_inner_radii_values[12] = bearing_diameter;
	tube_outer_radii_values[12] = bearing_cutout_diameter;

	tube_color_values[12] = rod_bearing_color;

	tube_offsets[12] = standard_offset;


	//bearing

	tube_tvec_values[13] = glm::vec3(0.0f, 0.1f, cylinder_6_start);
	tube_bvec_values[13] = glm::vec3(0.0f, 0.1f, cylinder_6_end);

	tube_inner_radii_values[13] = bearing_diameter;
	tube_outer_radii_values[13] = bearing_cutout_diameter;

	tube_color_values[13] = rod_bearing_color;

	tube_offsets[13] = standard_offset;

	//bearing

	tube_tvec_values[14] = glm::vec3(0.0f, -0.1f, cylinder_7_start);
	tube_bvec_values[14] = glm::vec3(0.0f, -0.1f, cylinder_7_end);

	tube_inner_radii_values[14] = bearing_diameter;
	tube_outer_radii_values[14] = bearing_cutout_diameter;

	tube_color_values[14] = rod_bearing_color;

	tube_offsets[14] = standard_offset;

	//bearing

	tube_tvec_values[15] = glm::vec3(-0.1f, 0.0f, cylinder_8_start);
	tube_bvec_values[15] = glm::vec3(-0.1f, 0.0f, cylinder_8_end);

	tube_inner_radii_values[15] = bearing_diameter;
	tube_outer_radii_values[15] = bearing_cutout_diameter;

	tube_color_values[15] = rod_bearing_color;

	tube_offsets[15] = standard_offset;






	//bottom of con rod

	tube_tvec_values[16] = glm::vec3(0.1f, 0.0f, cylinder_1_start);
	tube_bvec_values[16] = glm::vec3(0.1f, 0.0f, cylinder_1_end);

	tube_outer_radii_values[16] = con_rod_crank_connection_radius;
	tube_inner_radii_values[16] = bearing_cutout_diameter;

	tube_color_values[16] = con_rod_color;

	tube_offsets[16] = standard_offset;

	//bottom of con rod

	tube_tvec_values[17] = glm::vec3(0.0f, 0.1f, cylinder_2_start);
	tube_bvec_values[17] = glm::vec3(0.0f, 0.1f, cylinder_2_end);

	tube_outer_radii_values[17] = con_rod_crank_connection_radius;
	tube_inner_radii_values[17] = bearing_cutout_diameter;


	tube_color_values[17] = con_rod_color;


	tube_offsets[17] = standard_offset;
	//bottom of con rod

	tube_tvec_values[18] = glm::vec3(0.0f, -0.1f, cylinder_3_start);
	tube_bvec_values[18] = glm::vec3(0.0f, -0.1f, cylinder_3_end);

	tube_outer_radii_values[18] = con_rod_crank_connection_radius;
	tube_inner_radii_values[18] = bearing_cutout_diameter;


	tube_color_values[18] = con_rod_color;

	tube_offsets[18] = standard_offset;

	//bottom of con rod

	tube_tvec_values[19] = glm::vec3(-0.1f, 0.0f, cylinder_4_start);
	tube_bvec_values[19] = glm::vec3(-0.1f, 0.0f, cylinder_4_end);

	tube_outer_radii_values[19] = con_rod_crank_connection_radius;
	tube_inner_radii_values[19] = bearing_cutout_diameter;


	tube_color_values[19] = con_rod_color;

	tube_offsets[19] = standard_offset;

	//bottom of con rod

	tube_tvec_values[20] = glm::vec3(0.1f, 0.0f, cylinder_5_start);
	tube_bvec_values[20] = glm::vec3(0.1f, 0.0f, cylinder_5_end);

	tube_outer_radii_values[20] = con_rod_crank_connection_radius;
	tube_inner_radii_values[20] = bearing_cutout_diameter;


	tube_color_values[20] = con_rod_color;

	tube_offsets[20] = standard_offset;

	//bottom of con rod

	tube_tvec_values[21] = glm::vec3(0.0f, 0.1f, cylinder_6_start);
	tube_bvec_values[21] = glm::vec3(0.0f, 0.1f, cylinder_6_end);

	tube_outer_radii_values[21] = con_rod_crank_connection_radius;
	tube_inner_radii_values[21] = bearing_cutout_diameter;


	tube_color_values[21] = con_rod_color;

	tube_offsets[21] = standard_offset;

	//bottom of con rod

	tube_tvec_values[22] = glm::vec3(0.0f, -0.1f, cylinder_7_start);
	tube_bvec_values[22] = glm::vec3(0.0f, -0.1f, cylinder_7_end);

	tube_outer_radii_values[22] = con_rod_crank_connection_radius;
	tube_inner_radii_values[22] = bearing_cutout_diameter;


	tube_color_values[22] = con_rod_color;

	tube_offsets[22] = standard_offset;

	//bottom of con rod

	tube_tvec_values[23] = glm::vec3(-0.1f, 0.0f, cylinder_8_start);
	tube_bvec_values[23] = glm::vec3(-0.1f, 0.0f, cylinder_8_end);

	tube_outer_radii_values[23] = con_rod_crank_connection_radius;
	tube_inner_radii_values[23] = bearing_cutout_diameter;


	tube_color_values[23] = con_rod_color;

	tube_offsets[23] = standard_offset;







	//body of piston



	tube_outer_radii_values[24] = engine_cylinder_radius * pressure_chamber_scale;
	tube_inner_radii_values[24] = engine_cylinder_radius * pressure_chamber_scale * 0.8f;

	tube_color_values[24] = piston_color;
	tube_offsets[24] = standard_offset;


	tube_outer_radii_values[25] = engine_cylinder_radius * pressure_chamber_scale;
	tube_inner_radii_values[25] = engine_cylinder_radius * pressure_chamber_scale * 0.8f;

	tube_color_values[25] = piston_color;
	tube_offsets[25] = standard_offset;


	tube_outer_radii_values[26] = engine_cylinder_radius * pressure_chamber_scale;
	tube_inner_radii_values[26] = engine_cylinder_radius * pressure_chamber_scale * 0.8f;

	tube_color_values[26] = piston_color;
	tube_offsets[26] = standard_offset;


	tube_outer_radii_values[27] = engine_cylinder_radius * pressure_chamber_scale;
	tube_inner_radii_values[27] = engine_cylinder_radius * pressure_chamber_scale * 0.8f;

	tube_color_values[27] = piston_color;
	tube_offsets[27] = standard_offset;


	tube_outer_radii_values[28] = engine_cylinder_radius * pressure_chamber_scale;
	tube_inner_radii_values[28] = engine_cylinder_radius * pressure_chamber_scale * 0.8f;

	tube_color_values[28] = piston_color;
	tube_offsets[28] = standard_offset;


	tube_outer_radii_values[29] = engine_cylinder_radius * pressure_chamber_scale;
	tube_inner_radii_values[29] = engine_cylinder_radius * pressure_chamber_scale * 0.8f;

	tube_color_values[29] = piston_color;
	tube_offsets[29] = standard_offset;


	tube_outer_radii_values[30] = engine_cylinder_radius * pressure_chamber_scale;
	tube_inner_radii_values[30] = engine_cylinder_radius * pressure_chamber_scale * 0.8f;

	tube_color_values[30] = piston_color;
	tube_offsets[30] = standard_offset;


	tube_outer_radii_values[31] = engine_cylinder_radius * pressure_chamber_scale;
	tube_inner_radii_values[31] = engine_cylinder_radius * pressure_chamber_scale * 0.8f;

	tube_color_values[31] = piston_color;
	tube_offsets[31] = standard_offset;











	tube_outer_radii_values[32] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[32] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[32] = piston_ring_color;
	tube_offsets[32] = standard_offset;

	tube_outer_radii_values[33] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[33] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[33] = piston_ring_color;
	tube_offsets[33] = standard_offset;

	tube_outer_radii_values[34] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[34] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[34] = piston_ring_color;
	tube_offsets[34] = standard_offset;

	tube_outer_radii_values[35] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[35] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[35] = piston_ring_color;
	tube_offsets[35] = standard_offset;


	tube_outer_radii_values[36] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[36] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[36] = piston_ring_color;
	tube_offsets[36] = standard_offset;

	tube_outer_radii_values[37] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[37] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[37] = piston_ring_color;
	tube_offsets[37] = standard_offset;

	tube_outer_radii_values[38] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[38] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[38] = piston_ring_color;
	tube_offsets[38] = standard_offset;

	tube_outer_radii_values[39] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[39] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[39] = piston_ring_color;
	tube_offsets[39] = standard_offset;


	tube_outer_radii_values[40] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[40] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[40] = piston_ring_color;
	tube_offsets[40] = standard_offset;

	tube_outer_radii_values[41] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[41] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[41] = piston_ring_color;
	tube_offsets[41] = standard_offset;

	tube_outer_radii_values[42] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[42] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[42] = piston_ring_color;
	tube_offsets[42] = standard_offset;

	tube_outer_radii_values[43] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[43] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[43] = piston_ring_color;
	tube_offsets[43] = standard_offset;


	tube_outer_radii_values[44] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[44] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[44] = piston_ring_color;
	tube_offsets[44] = standard_offset;

	tube_outer_radii_values[45] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[45] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[45] = piston_ring_color;
	tube_offsets[45] = standard_offset;

	tube_outer_radii_values[46] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[46] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[46] = piston_ring_color;
	tube_offsets[46] = standard_offset;

	tube_outer_radii_values[47] = engine_cylinder_radius * pressure_chamber_scale * 1.05f;
	tube_inner_radii_values[47] = engine_cylinder_radius * pressure_chamber_scale * 0.850f;

	tube_color_values[47] = piston_ring_color;
	tube_offsets[47] = standard_offset;





















































		// PUT INITIAL GEOMETRY HERE





















		//pistons, piston rings, and con rods are added to the list when



		timer(0); //update colors, etc, before rendering anything, after this call the piston locations are known

		//NOW DRAWING PISTONS - since the timer function has established all the piston locations





		//PISTON - a tube for the body of it , then the




//TUBES

		//PISTON RING 1

		//PISTON RING 2

		//PISTON RING 3







		//SCALLOPED EDGE - I forgot this is going to have to take place before the cylinder liner or else that's going to be an issue I think









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


	//ranges from 0.0f to 1.0f - representing the rotation of the crankshaft
	float position_in_rotation_single = (float) ( numFrames % frames_per_rotation ) / (float) frames_per_rotation;

	//ranges from 0.0f to 1.0f = representing the combustion cycle
	float position_in_rotation_double = (float) ( numFrames % ( 2 * frames_per_rotation ) ) / (float) ( 2 * frames_per_rotation );


	glm::mat4 rot(1.0f); //initialize with identity
	// because we really don't need to do this rotation initially

	if(numFrames)
	{
		rot = glm::rotate( frame_rotation_increment, glm::vec3( 0.0f, 0.0f, 1.0f ) );
	}



	//ROTATE BEARINGS (30 to 37)

	tube_tvec_values[8] = rot * vec(tube_tvec_values[8], 1.0f);
	tube_bvec_values[8] = rot * vec(tube_bvec_values[8], 1.0f);


	tube_tvec_values[9] = rot * vec(tube_tvec_values[9], 1.0f);
	tube_bvec_values[9] = rot * vec(tube_bvec_values[9], 1.0f);


	tube_tvec_values[10] = rot * vec(tube_tvec_values[10], 1.0f);
	tube_bvec_values[10] = rot * vec(tube_bvec_values[10], 1.0f);


	tube_tvec_values[11] = rot * vec(tube_tvec_values[11], 1.0f);
	tube_bvec_values[11] = rot * vec(tube_bvec_values[11], 1.0f);


	tube_tvec_values[12] = rot * vec(tube_tvec_values[12], 1.0f);
	tube_bvec_values[12] = rot * vec(tube_bvec_values[12], 1.0f);


	tube_tvec_values[13] = rot * vec(tube_tvec_values[13], 1.0f);
	tube_bvec_values[13] = rot * vec(tube_bvec_values[13], 1.0f);


	tube_tvec_values[14] = rot * vec(tube_tvec_values[14], 1.0f);
	tube_bvec_values[14] = rot * vec(tube_bvec_values[14], 1.0f);


	tube_tvec_values[15] = rot * vec(tube_tvec_values[15], 1.0f);
	tube_bvec_values[15] = rot * vec(tube_bvec_values[15], 1.0f);



	//ROTATE BOTTOMS OF CONNECTION RODS (CYLINDERS index 14 to 21)
	tube_tvec_values[16] = rot * vec(tube_tvec_values[16], 1.0f);
	tube_bvec_values[16] = rot * vec(tube_bvec_values[16], 1.0f);


	tube_tvec_values[17] = rot * vec(tube_tvec_values[17], 1.0f);
	tube_bvec_values[17] = rot * vec(tube_bvec_values[17], 1.0f);


	tube_tvec_values[18] = rot * vec(tube_tvec_values[18], 1.0f);
	tube_bvec_values[18] = rot * vec(tube_bvec_values[18], 1.0f);


	tube_tvec_values[19] = rot * vec(tube_tvec_values[19], 1.0f);
	tube_bvec_values[19] = rot * vec(tube_bvec_values[19], 1.0f);


	tube_tvec_values[20] = rot * vec(tube_tvec_values[20], 1.0f);
	tube_bvec_values[20] = rot * vec(tube_bvec_values[20], 1.0f);


	tube_tvec_values[21] = rot * vec(tube_tvec_values[21], 1.0f);
	tube_bvec_values[21] = rot * vec(tube_bvec_values[21], 1.0f);


	tube_tvec_values[22] = rot * vec(tube_tvec_values[22], 1.0f);
	tube_bvec_values[22] = rot * vec(tube_bvec_values[22], 1.0f);


	tube_tvec_values[23] = rot * vec(tube_tvec_values[23], 1.0f);
	tube_bvec_values[23] = rot * vec(tube_bvec_values[23], 1.0f);

	cylinder_1_rod_connection = (tube_tvec_values[16] + tube_bvec_values[16]) / 2.0f;
	cylinder_2_rod_connection = (tube_tvec_values[17] + tube_bvec_values[17]) / 2.0f;
	cylinder_3_rod_connection = (tube_tvec_values[18] + tube_bvec_values[18]) / 2.0f;
	cylinder_4_rod_connection = (tube_tvec_values[19] + tube_bvec_values[19]) / 2.0f;
	cylinder_5_rod_connection = (tube_tvec_values[20] + tube_bvec_values[20]) / 2.0f;
	cylinder_6_rod_connection = (tube_tvec_values[21] + tube_bvec_values[21]) / 2.0f;
	cylinder_7_rod_connection = (tube_tvec_values[22] + tube_bvec_values[22]) / 2.0f;
	cylinder_8_rod_connection = (tube_tvec_values[23] + tube_bvec_values[23]) / 2.0f;













	//UPDATE PRESSURE CHAMBER STUFF (CYLINDERS index 54 to 61)
	// 54, 55, 56, 57 are in bank 1, 58, 59, 60, 61 are in bank 2

	//tvecs stay the same, bvecs will be moving up and down the cylinder

	float disp1 = glm::cos(2.0f * 3.14159265359f * position_in_rotation_single +  1.0f * 3.14159265359f / 4.0f);
	float disp2 = glm::cos(2.0f * 3.14159265359f * position_in_rotation_single +  3.0f * 3.14159265359f / 4.0f);
	float disp3 = glm::cos(2.0f * 3.14159265359f * position_in_rotation_single +  7.0f * 3.14159265359f / 4.0f);
	float disp4 = glm::cos(2.0f * 3.14159265359f * position_in_rotation_single +  5.0f * 3.14159265359f / 4.0f);

	float disp5 = glm::cos(2.0f * 3.14159265359f * position_in_rotation_single -  1.0f * 3.14159265359f / 4.0f);
	float disp6 = glm::cos(2.0f * 3.14159265359f * position_in_rotation_single +  1.0f * 3.14159265359f / 4.0f);
	float disp7 = glm::cos(2.0f * 3.14159265359f * position_in_rotation_single +  5.0f * 3.14159265359f / 4.0f);
	float disp8 = glm::cos(2.0f * 3.14159265359f * position_in_rotation_single +  3.0f * 3.14159265359f / 4.0f);


	cylinder_1_piston_location = (0.9f * disp1 * (bank1_middle_vector - bank1_start_vector)) + bank1_middle_vector + glm::vec3(0.0f, 0.0f, cylinder_1_z_offset);
	cylinder_2_piston_location = (0.9f * disp2 * (bank1_middle_vector - bank1_start_vector)) + bank1_middle_vector + glm::vec3(0.0f, 0.0f, cylinder_2_z_offset);
	cylinder_3_piston_location = (0.9f * disp3 * (bank1_middle_vector - bank1_start_vector)) + bank1_middle_vector + glm::vec3(0.0f, 0.0f, cylinder_3_z_offset);
	cylinder_4_piston_location = (0.9f * disp4 * (bank1_middle_vector - bank1_start_vector)) + bank1_middle_vector + glm::vec3(0.0f, 0.0f, cylinder_4_z_offset);
	cylinder_5_piston_location = (0.9f * disp5 * (bank2_middle_vector - bank2_start_vector)) + bank2_middle_vector + glm::vec3(0.0f, 0.0f, cylinder_5_z_offset);
	cylinder_6_piston_location = (0.9f * disp6 * (bank2_middle_vector - bank2_start_vector)) + bank2_middle_vector + glm::vec3(0.0f, 0.0f, cylinder_6_z_offset);
	cylinder_7_piston_location = (0.9f * disp7 * (bank2_middle_vector - bank2_start_vector)) + bank2_middle_vector + glm::vec3(0.0f, 0.0f, cylinder_7_z_offset);
	cylinder_8_piston_location = (0.9f * disp8 * (bank2_middle_vector - bank2_start_vector)) + bank2_middle_vector + glm::vec3(0.0f, 0.0f, cylinder_8_z_offset);


	cylinder_bvec_values[14] = cylinder_1_piston_location;
	cylinder_bvec_values[15] = cylinder_2_piston_location;
	cylinder_bvec_values[16] = cylinder_3_piston_location;
	cylinder_bvec_values[17] = cylinder_4_piston_location;
	cylinder_bvec_values[18] = cylinder_5_piston_location;
	cylinder_bvec_values[19] = cylinder_6_piston_location;
	cylinder_bvec_values[20] = cylinder_7_piston_location;
	cylinder_bvec_values[21] = cylinder_8_piston_location;


	//ROTATE CRANKSHAFT JOURNALS (CYLINDERS index 22 to 25)
	cylinder_tvec_values[22] = rot * vec(cylinder_tvec_values[22], 1.0f);
	cylinder_bvec_values[22] = rot * vec(cylinder_bvec_values[22], 1.0f);

	cylinder_tvec_values[23] = rot * vec(cylinder_tvec_values[23], 1.0f);
	cylinder_bvec_values[23] = rot * vec(cylinder_bvec_values[23], 1.0f);

	cylinder_tvec_values[24] = rot * vec(cylinder_tvec_values[24], 1.0f);
	cylinder_bvec_values[24] = rot * vec(cylinder_bvec_values[24], 1.0f);

	cylinder_tvec_values[25] = rot * vec(cylinder_tvec_values[25], 1.0f);
	cylinder_bvec_values[25] = rot * vec(cylinder_bvec_values[25], 1.0f);




	//draw the piston tops (cylinder the whole diameter of the chamber)
	cylinder_bvec_values[26] = cylinder_1_piston_location + bank1_piston_offset * -0.2f;
	cylinder_tvec_values[26] = cylinder_1_piston_location + bank1_piston_offset * 0.75f;

	cylinder_bvec_values[27] = cylinder_2_piston_location + bank1_piston_offset * -0.2f;
	cylinder_tvec_values[27] = cylinder_2_piston_location + bank1_piston_offset * 0.75f;

	cylinder_bvec_values[28] = cylinder_3_piston_location + bank1_piston_offset * -0.2f;
	cylinder_tvec_values[28] = cylinder_3_piston_location + bank1_piston_offset * 0.75f;

	cylinder_bvec_values[29] = cylinder_4_piston_location + bank1_piston_offset * -0.2f;
	cylinder_tvec_values[29] = cylinder_4_piston_location + bank1_piston_offset * 0.75f;

	cylinder_bvec_values[30] = cylinder_5_piston_location + bank2_piston_offset * -0.2f;
	cylinder_tvec_values[30] = cylinder_5_piston_location + bank2_piston_offset * 0.75f;

	cylinder_bvec_values[31] = cylinder_6_piston_location + bank2_piston_offset * -0.2f;
	cylinder_tvec_values[31] = cylinder_6_piston_location + bank2_piston_offset * 0.75f;

	cylinder_bvec_values[32] = cylinder_7_piston_location + bank2_piston_offset * -0.2f;
	cylinder_tvec_values[32] = cylinder_7_piston_location + bank2_piston_offset * 0.75f;

	cylinder_bvec_values[33] = cylinder_8_piston_location + bank2_piston_offset * -0.2f;
	cylinder_tvec_values[33] = cylinder_8_piston_location + bank2_piston_offset * 0.75f;



	//draw the piston body (tube connected to the top of the piston)


	tube_tvec_values[24] = cylinder_1_piston_location + bank1_piston_offset * 2.40f;
	tube_bvec_values[24] = cylinder_1_piston_location + bank1_piston_offset * 0.75f;

	tube_tvec_values[25] = cylinder_2_piston_location + bank1_piston_offset * 2.40f;
	tube_bvec_values[25] = cylinder_2_piston_location + bank1_piston_offset * 0.75f;

	tube_tvec_values[26] = cylinder_3_piston_location + bank1_piston_offset * 2.40f;
	tube_bvec_values[26] = cylinder_3_piston_location + bank1_piston_offset * 0.75f;

	tube_tvec_values[27] = cylinder_4_piston_location + bank1_piston_offset * 2.40f;
	tube_bvec_values[27] = cylinder_4_piston_location + bank1_piston_offset * 0.75f;

	tube_tvec_values[28] = cylinder_5_piston_location + bank2_piston_offset * 2.40f;
	tube_bvec_values[28] = cylinder_5_piston_location + bank2_piston_offset * 0.75f;

	tube_tvec_values[29] = cylinder_6_piston_location + bank2_piston_offset * 2.40f;
	tube_bvec_values[29] = cylinder_6_piston_location + bank2_piston_offset * 0.75f;

	tube_tvec_values[30] = cylinder_7_piston_location + bank2_piston_offset * 2.40f;
	tube_bvec_values[30] = cylinder_7_piston_location + bank2_piston_offset * 0.75f;

	tube_tvec_values[31] = cylinder_8_piston_location + bank2_piston_offset * 2.40f;
	tube_bvec_values[31] = cylinder_8_piston_location + bank2_piston_offset * 0.75f;



	//draw the scalloped cutout (up and down the z-axis)

	cylinder_tvec_values[34] = cylinder_1_piston_location + bank1_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f,  0.04f);
	cylinder_bvec_values[34] = cylinder_1_piston_location + bank1_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f, -0.04f);

	cylinder_tvec_values[35] = cylinder_2_piston_location + bank1_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f,  0.04f);
	cylinder_bvec_values[35] = cylinder_2_piston_location + bank1_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f, -0.04f);

	cylinder_tvec_values[36] = cylinder_3_piston_location + bank1_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f,  0.04f);
	cylinder_bvec_values[36] = cylinder_3_piston_location + bank1_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f, -0.04f);

	cylinder_tvec_values[37] = cylinder_4_piston_location + bank1_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f,  0.04f);
	cylinder_bvec_values[37] = cylinder_4_piston_location + bank1_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f, -0.04f);

	cylinder_tvec_values[38] = cylinder_5_piston_location + bank2_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f,  0.04f);
	cylinder_bvec_values[38] = cylinder_5_piston_location + bank2_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f, -0.04f);

	cylinder_tvec_values[39] = cylinder_6_piston_location + bank2_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f,  0.04f);
	cylinder_bvec_values[39] = cylinder_6_piston_location + bank2_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f, -0.04f);

	cylinder_tvec_values[40] = cylinder_7_piston_location + bank2_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f,  0.04f);
	cylinder_bvec_values[40] = cylinder_7_piston_location + bank2_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f, -0.04f);

	cylinder_tvec_values[41] = cylinder_8_piston_location + bank2_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f,  0.04f);
	cylinder_bvec_values[41] = cylinder_8_piston_location + bank2_piston_offset * 1.5f + glm::vec3(0.0f, 0.0f, -0.04f);




	//draw the piston rings (added after the piston body to draw over it)

	tube_tvec_values[32] = cylinder_1_piston_location + bank1_piston_offset * 0.85f;
	tube_bvec_values[32] = cylinder_1_piston_location + bank1_piston_offset * 0.65f;

	tube_tvec_values[33] = cylinder_2_piston_location + bank1_piston_offset * 0.85f;
	tube_bvec_values[33] = cylinder_2_piston_location + bank1_piston_offset * 0.65f;

	tube_tvec_values[34] = cylinder_3_piston_location + bank1_piston_offset * 0.85f;
	tube_bvec_values[34] = cylinder_3_piston_location + bank1_piston_offset * 0.65f;

	tube_tvec_values[35] = cylinder_4_piston_location + bank1_piston_offset * 0.85f;
	tube_bvec_values[35] = cylinder_4_piston_location + bank1_piston_offset * 0.65f;

	tube_tvec_values[36] = cylinder_5_piston_location + bank2_piston_offset * 0.85f;
	tube_bvec_values[36] = cylinder_5_piston_location + bank2_piston_offset * 0.65f;

	tube_tvec_values[37] = cylinder_6_piston_location + bank2_piston_offset * 0.85f;
	tube_bvec_values[37] = cylinder_6_piston_location + bank2_piston_offset * 0.65f;

	tube_tvec_values[38] = cylinder_7_piston_location + bank2_piston_offset * 0.85f;
	tube_bvec_values[38] = cylinder_7_piston_location + bank2_piston_offset * 0.65f;

	tube_tvec_values[39] = cylinder_8_piston_location + bank2_piston_offset * 0.85f;
	tube_bvec_values[39] = cylinder_8_piston_location + bank2_piston_offset * 0.65f;








	tube_tvec_values[40] = cylinder_1_piston_location + bank1_piston_offset * 1.2f;
	tube_bvec_values[40] = cylinder_1_piston_location + bank1_piston_offset * 1.4f;

	tube_tvec_values[41] = cylinder_2_piston_location + bank1_piston_offset * 1.2f;
	tube_bvec_values[41] = cylinder_2_piston_location + bank1_piston_offset * 1.4f;

	tube_tvec_values[42] = cylinder_3_piston_location + bank1_piston_offset * 1.2f;
	tube_bvec_values[42] = cylinder_3_piston_location + bank1_piston_offset * 1.4f;

	tube_tvec_values[43] = cylinder_4_piston_location + bank1_piston_offset * 1.2f;
	tube_bvec_values[43] = cylinder_4_piston_location + bank1_piston_offset * 1.4f;

	tube_tvec_values[44] = cylinder_5_piston_location + bank2_piston_offset * 1.2f;
	tube_bvec_values[44] = cylinder_5_piston_location + bank2_piston_offset * 1.4f;

	tube_tvec_values[45] = cylinder_6_piston_location + bank2_piston_offset * 1.2f;
	tube_bvec_values[45] = cylinder_6_piston_location + bank2_piston_offset * 1.4f;

	tube_tvec_values[46] = cylinder_7_piston_location + bank2_piston_offset * 1.2f;
	tube_bvec_values[46] = cylinder_7_piston_location + bank2_piston_offset * 1.4f;

	tube_tvec_values[47] = cylinder_8_piston_location + bank2_piston_offset * 1.2f;
	tube_bvec_values[47] = cylinder_8_piston_location + bank2_piston_offset * 1.4f;



	//the con rod itself





	glm::vec3 z_axis = glm::vec3(0.0f, 0.0f, 1.0f);	//known vector

	//these vary by cylinder, based on the rotation of the engine
	glm::vec3 cylinder_1_con_rod_vector =(cylinder_1_rod_connection) - (cylinder_1_piston_location + bank1_piston_offset * 1.5f);
	glm::vec3 cylinder_1_perpendicular_vector = glm::cross(z_axis, cylinder_1_con_rod_vector);

	glm::vec3 cylinder_2_con_rod_vector = (cylinder_2_rod_connection) - (cylinder_2_piston_location + bank1_piston_offset * 1.5f);
	glm::vec3 cylinder_2_perpendicular_vector = glm::cross(z_axis, cylinder_2_con_rod_vector);

	glm::vec3 cylinder_3_con_rod_vector = (cylinder_3_rod_connection) - (cylinder_3_piston_location + bank1_piston_offset * 1.5f);
	glm::vec3 cylinder_3_perpendicular_vector = glm::cross(z_axis, cylinder_3_con_rod_vector);

	glm::vec3 cylinder_4_con_rod_vector = (cylinder_4_rod_connection) - (cylinder_4_piston_location + bank1_piston_offset * 1.5f);
	glm::vec3 cylinder_4_perpendicular_vector = glm::cross(z_axis, cylinder_4_con_rod_vector);

	glm::vec3 cylinder_5_con_rod_vector = (cylinder_5_rod_connection) - (cylinder_5_piston_location + bank2_piston_offset * 1.5f);
	glm::vec3 cylinder_5_perpendicular_vector = glm::cross(z_axis, cylinder_5_con_rod_vector);

	glm::vec3 cylinder_6_con_rod_vector = (cylinder_6_rod_connection) - (cylinder_6_piston_location + bank2_piston_offset * 1.5f);
	glm::vec3 cylinder_6_perpendicular_vector = glm::cross(z_axis, cylinder_6_con_rod_vector);

	glm::vec3 cylinder_7_con_rod_vector = (cylinder_7_rod_connection) - (cylinder_7_piston_location + bank2_piston_offset * 1.5f);
	glm::vec3 cylinder_7_perpendicular_vector = glm::cross(z_axis, cylinder_7_con_rod_vector);

	glm::vec3 cylinder_8_con_rod_vector = (cylinder_8_rod_connection) - (cylinder_8_piston_location + bank2_piston_offset * 1.5f);
	glm::vec3 cylinder_8_perpendicular_vector = glm::cross(z_axis, cylinder_8_con_rod_vector);


//	Con rod vector determined by:
//
//						   //
//						  //		 //
//						 //			//
//						//	o	 //	<---location of piston (top end of con rod)
//							 /	//
//						  /
//						 /
//						o			<---location of the crank (bottom end of the con rod)
//
//
//
//	and the rest is determined by:
//
// 			^
// 			|	<--- known con rod vector
//		 	|
// 	 <--x	<--- known z unit vector (towards you)
// 		^
// 		|_______ this perpendicular_vector is the cross of z and the con rod vector
//
//
//				This con rod vector changes direction a lot, as neither end is fixed -
//				because we need a perpendicular vector to place some of the relevant
//				points, I came up with this solution - take the vector along the con
//				rod's length, then cross it with a known unit vector on the z axis
//				(that is, lengthways down the crank, coming out of the screen at the x)
//				- the result of this cross product gives a vector which is used to
//				locate some of the points of a cuboid or triangle


//triangular base pieces
triangle_point1_values[0] = cylinder_1_rod_connection - 0.45f * cylinder_1_con_rod_vector;
triangle_point2_values[0] = cylinder_1_rod_connection + 0.20f * cylinder_1_perpendicular_vector;
triangle_point3_values[0] = cylinder_1_rod_connection - 0.20f * cylinder_1_perpendicular_vector;

triangle_point1_values[1] = cylinder_2_rod_connection - 0.45f * cylinder_2_con_rod_vector;
triangle_point2_values[1] = cylinder_2_rod_connection + 0.20f * cylinder_2_perpendicular_vector;
triangle_point3_values[1] = cylinder_2_rod_connection - 0.20f * cylinder_2_perpendicular_vector;

triangle_point1_values[2] = cylinder_3_rod_connection - 0.45f * cylinder_3_con_rod_vector;
triangle_point2_values[2] = cylinder_3_rod_connection + 0.20f * cylinder_3_perpendicular_vector;
triangle_point3_values[2] = cylinder_3_rod_connection - 0.20f * cylinder_3_perpendicular_vector;

triangle_point1_values[3] = cylinder_4_rod_connection - 0.45f * cylinder_4_con_rod_vector;
triangle_point2_values[3] = cylinder_4_rod_connection + 0.20f * cylinder_4_perpendicular_vector;
triangle_point3_values[3] = cylinder_4_rod_connection - 0.20f * cylinder_4_perpendicular_vector;

triangle_point1_values[4] = cylinder_5_rod_connection - 0.45f * cylinder_5_con_rod_vector;
triangle_point2_values[4] = cylinder_5_rod_connection + 0.20f * cylinder_5_perpendicular_vector;
triangle_point3_values[4] = cylinder_5_rod_connection - 0.20f * cylinder_5_perpendicular_vector;

triangle_point1_values[5] = cylinder_6_rod_connection - 0.45f * cylinder_6_con_rod_vector;
triangle_point2_values[5] = cylinder_6_rod_connection + 0.20f * cylinder_6_perpendicular_vector;
triangle_point3_values[5] = cylinder_6_rod_connection - 0.20f * cylinder_6_perpendicular_vector;

triangle_point1_values[6] = cylinder_7_rod_connection - 0.45f * cylinder_7_con_rod_vector;
triangle_point2_values[6] = cylinder_7_rod_connection + 0.20f * cylinder_7_perpendicular_vector;
triangle_point3_values[6] = cylinder_7_rod_connection - 0.20f * cylinder_7_perpendicular_vector;

triangle_point1_values[7] = cylinder_8_rod_connection - 0.45f * cylinder_8_con_rod_vector;
triangle_point2_values[7] = cylinder_8_rod_connection + 0.20f * cylinder_8_perpendicular_vector;
triangle_point3_values[7] = cylinder_8_rod_connection - 0.20f * cylinder_8_perpendicular_vector;



glUniform3fv( triangle_point1_location, NUM_TRIANGLES, glm::value_ptr( triangle_point1_values[0] ) );
glUniform3fv( triangle_point2_location, NUM_TRIANGLES, glm::value_ptr( triangle_point2_values[0] ) );
glUniform3fv( triangle_point3_location, NUM_TRIANGLES, glm::value_ptr( triangle_point3_values[0] ) );


//actual con rods themselves
glm::vec3 z_pos = glm::vec3(0.0f, 0.0f,  0.01f);
glm::vec3 z_neg = glm::vec3(0.0f, 0.0f, -0.01f);

glm::vec3 top_loc = cylinder_1_rod_connection - 0.95f * cylinder_1_con_rod_vector;
glm::vec3 bot_loc = cylinder_1_rod_connection - 0.10f * cylinder_1_con_rod_vector;



cuboid_a_values[8] = top_loc + 0.1f * cylinder_1_perpendicular_vector + z_pos;
cuboid_b_values[8] = bot_loc + 0.1f * cylinder_1_perpendicular_vector + z_pos;
cuboid_c_values[8] = top_loc - 0.1f * cylinder_1_perpendicular_vector + z_pos;
cuboid_d_values[8] = bot_loc - 0.1f * cylinder_1_perpendicular_vector + z_pos;
cuboid_e_values[8] = top_loc + 0.1f * cylinder_1_perpendicular_vector + z_neg;
cuboid_f_values[8] = bot_loc + 0.1f * cylinder_1_perpendicular_vector + z_neg;
cuboid_g_values[8] = top_loc - 0.1f * cylinder_1_perpendicular_vector + z_neg;
cuboid_h_values[8] = bot_loc - 0.1f * cylinder_1_perpendicular_vector + z_neg;


top_loc = cylinder_2_rod_connection - 0.95f * cylinder_2_con_rod_vector;
bot_loc = cylinder_2_rod_connection - 0.10f * cylinder_2_con_rod_vector;


cuboid_a_values[9] = top_loc + 0.1f * cylinder_2_perpendicular_vector + z_pos;
cuboid_b_values[9] = bot_loc + 0.1f * cylinder_2_perpendicular_vector + z_pos;
cuboid_c_values[9] = top_loc - 0.1f * cylinder_2_perpendicular_vector + z_pos;
cuboid_d_values[9] = bot_loc - 0.1f * cylinder_2_perpendicular_vector + z_pos;
cuboid_e_values[9] = top_loc + 0.1f * cylinder_2_perpendicular_vector + z_neg;
cuboid_f_values[9] = bot_loc + 0.1f * cylinder_2_perpendicular_vector + z_neg;
cuboid_g_values[9] = top_loc - 0.1f * cylinder_2_perpendicular_vector + z_neg;
cuboid_h_values[9] = bot_loc - 0.1f * cylinder_2_perpendicular_vector + z_neg;



top_loc = cylinder_3_rod_connection - 0.95f * cylinder_3_con_rod_vector;
bot_loc = cylinder_3_rod_connection - 0.10f * cylinder_3_con_rod_vector;


cuboid_a_values[10] = top_loc + 0.1f * cylinder_3_perpendicular_vector + z_pos;
cuboid_b_values[10] = bot_loc + 0.1f * cylinder_3_perpendicular_vector + z_pos;
cuboid_c_values[10] = top_loc - 0.1f * cylinder_3_perpendicular_vector + z_pos;
cuboid_d_values[10] = bot_loc - 0.1f * cylinder_3_perpendicular_vector + z_pos;
cuboid_e_values[10] = top_loc + 0.1f * cylinder_3_perpendicular_vector + z_neg;
cuboid_f_values[10] = bot_loc + 0.1f * cylinder_3_perpendicular_vector + z_neg;
cuboid_g_values[10] = top_loc - 0.1f * cylinder_3_perpendicular_vector + z_neg;
cuboid_h_values[10] = bot_loc - 0.1f * cylinder_3_perpendicular_vector + z_neg;


top_loc = cylinder_4_rod_connection - 0.95f * cylinder_4_con_rod_vector;
bot_loc = cylinder_4_rod_connection - 0.10f * cylinder_4_con_rod_vector;


cuboid_a_values[11] = top_loc + 0.1f * cylinder_4_perpendicular_vector + z_pos;
cuboid_b_values[11] = bot_loc + 0.1f * cylinder_4_perpendicular_vector + z_pos;
cuboid_c_values[11] = top_loc - 0.1f * cylinder_4_perpendicular_vector + z_pos;
cuboid_d_values[11] = bot_loc - 0.1f * cylinder_4_perpendicular_vector + z_pos;
cuboid_e_values[11] = top_loc + 0.1f * cylinder_4_perpendicular_vector + z_neg;
cuboid_f_values[11] = bot_loc + 0.1f * cylinder_4_perpendicular_vector + z_neg;
cuboid_g_values[11] = top_loc - 0.1f * cylinder_4_perpendicular_vector + z_neg;
cuboid_h_values[11] = bot_loc - 0.1f * cylinder_4_perpendicular_vector + z_neg;


top_loc = cylinder_5_rod_connection - 0.95f * cylinder_5_con_rod_vector;
bot_loc = cylinder_5_rod_connection - 0.10f * cylinder_5_con_rod_vector;


cuboid_a_values[12] = top_loc + 0.1f * cylinder_5_perpendicular_vector + z_pos;
cuboid_b_values[12] = bot_loc + 0.1f * cylinder_5_perpendicular_vector + z_pos;
cuboid_c_values[12] = top_loc - 0.1f * cylinder_5_perpendicular_vector + z_pos;
cuboid_d_values[12] = bot_loc - 0.1f * cylinder_5_perpendicular_vector + z_pos;
cuboid_e_values[12] = top_loc + 0.1f * cylinder_5_perpendicular_vector + z_neg;
cuboid_f_values[12] = bot_loc + 0.1f * cylinder_5_perpendicular_vector + z_neg;
cuboid_g_values[12] = top_loc - 0.1f * cylinder_5_perpendicular_vector + z_neg;
cuboid_h_values[12] = bot_loc - 0.1f * cylinder_5_perpendicular_vector + z_neg;

top_loc = cylinder_6_rod_connection - 0.95f * cylinder_6_con_rod_vector;
bot_loc = cylinder_6_rod_connection - 0.10f * cylinder_6_con_rod_vector;


cuboid_a_values[13] = top_loc + 0.1f * cylinder_6_perpendicular_vector + z_pos;
cuboid_b_values[13] = bot_loc + 0.1f * cylinder_6_perpendicular_vector + z_pos;
cuboid_c_values[13] = top_loc - 0.1f * cylinder_6_perpendicular_vector + z_pos;
cuboid_d_values[13] = bot_loc - 0.1f * cylinder_6_perpendicular_vector + z_pos;
cuboid_e_values[13] = top_loc + 0.1f * cylinder_6_perpendicular_vector + z_neg;
cuboid_f_values[13] = bot_loc + 0.1f * cylinder_6_perpendicular_vector + z_neg;
cuboid_g_values[13] = top_loc - 0.1f * cylinder_6_perpendicular_vector + z_neg;
cuboid_h_values[13] = bot_loc - 0.1f * cylinder_6_perpendicular_vector + z_neg;

top_loc = cylinder_7_rod_connection - 0.95f * cylinder_7_con_rod_vector;
bot_loc = cylinder_7_rod_connection - 0.10f * cylinder_7_con_rod_vector;


cuboid_a_values[14] = top_loc + 0.1f * cylinder_7_perpendicular_vector + z_pos;
cuboid_b_values[14] = bot_loc + 0.1f * cylinder_7_perpendicular_vector + z_pos;
cuboid_c_values[14] = top_loc - 0.1f * cylinder_7_perpendicular_vector + z_pos;
cuboid_d_values[14] = bot_loc - 0.1f * cylinder_7_perpendicular_vector + z_pos;
cuboid_e_values[14] = top_loc + 0.1f * cylinder_7_perpendicular_vector + z_neg;
cuboid_f_values[14] = bot_loc + 0.1f * cylinder_7_perpendicular_vector + z_neg;
cuboid_g_values[14] = top_loc - 0.1f * cylinder_7_perpendicular_vector + z_neg;
cuboid_h_values[14] = bot_loc - 0.1f * cylinder_7_perpendicular_vector + z_neg;

top_loc = cylinder_8_rod_connection - 0.95f * cylinder_8_con_rod_vector;
bot_loc = cylinder_8_rod_connection - 0.10f * cylinder_8_con_rod_vector;


cuboid_a_values[15] = top_loc + 0.1f * cylinder_8_perpendicular_vector + z_pos;
cuboid_b_values[15] = bot_loc + 0.1f * cylinder_8_perpendicular_vector + z_pos;
cuboid_c_values[15] = top_loc - 0.1f * cylinder_8_perpendicular_vector + z_pos;
cuboid_d_values[15] = bot_loc - 0.1f * cylinder_8_perpendicular_vector + z_pos;
cuboid_e_values[15] = top_loc + 0.1f * cylinder_8_perpendicular_vector + z_neg;
cuboid_f_values[15] = bot_loc + 0.1f * cylinder_8_perpendicular_vector + z_neg;
cuboid_g_values[15] = top_loc - 0.1f * cylinder_8_perpendicular_vector + z_neg;
cuboid_h_values[15] = bot_loc - 0.1f * cylinder_8_perpendicular_vector + z_neg;














	//UPDATE THE COLORS - THIS WILL REQUIRE MORE SINUSOIDAL TYPE STUFF

	// firing order is defined as 1-8-7-3-6-5-4-2 (from the nissan VK engine)

	// I:intake
	// C:compression
	// F:firing and expansion
	// E:exhaust


	//TIME STEP  : -0- -1- -2- -3- -4- -5- -6- -7-

	//CYLINDER 1 :  F   F   E   E   I   I   C   C
	//CYLINDER 2 :  F   E   E   I   I   C   C   F
	//CYLINDER 3 :  I   C   C   F   F   E   E   I
	//CYLINDER 4 :  E   E   I   I 	C   C   F   F

	//CYLINDER 5 :  E   I   I   C   C   F   F   E
	//CYLINDER 6 :  I   I   C   C   F   F   E   E
	//CYLINDER 7 :  C   C   F   F   E   E   I   I
	//CYLINDER 8 :  C   F   F   E   E   I   I   C





	//UPDATE LOCATIONS OF ALL PISTON COMPONENTS






	//UPDATE THE GPU-SIDE VALUES OF ALL CYLINDERS

	glUniform3fv(cylinder_tvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_tvec_values[0] ) );
	glUniform3fv(cylinder_bvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_bvec_values[0] ) );
	glUniform4fv(cylinder_colors_location, NUM_CYLINDERS, glm::value_ptr( cylinder_color_values[0] ) );

	//THEN SEND ALL THE TUBE VALUES TO THE GPU
	glUniform3fv(tube_tvec_location, NUM_TUBES, glm::value_ptr( tube_tvec_values[0] ) );
	glUniform3fv(tube_bvec_location, NUM_TUBES, glm::value_ptr( tube_bvec_values[0] ) );




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



	//THEN THE CON RODS - THIS RELIES ON THE VALUES COMPUTED EARLIER IN THE TIMER FUNCTION (WHICH MATTERS MOST DURING INITIALIZATION)

	//COMPUTE THE VECTOR ALONG THE CON ROD - THIS IS BETWEEN THE PISTON_LOCATION AND THE ASSOCIATED ROD LOCATION

	//TAKE THE CROSS PRODUCT OF THE VECTOR ALONG THE CON ROD WITH VEC3(0.0F, 0.0F, 1.0F) - THIS IS PERPENDICULAR TO ALL CON RODS,
	// WHICH ARE ALL IN THE XY PLANE - THIS GIVES A THIRD VECTOR WHICH WILL BE USED TO LOCATE THE OFFSETS OF THE 8 VERTICIES OF THE CON ROD







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
