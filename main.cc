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
#define NUM_TRIANGLES 1
#define NUM_QUAD_HEXS 1
#define NUM_CYLINDERS 2


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






//CYLINDER VALUES
GLuint cylinder_tvec_location;
glm::vec3 cylinder_tvec_values[NUM_CYLINDERS];

GLuint cylinder_bvec_location;
glm::vec3 cylinder_bvec_values[NUM_CYLINDERS];


GLuint cylinder_radii_location;
float cylinder_radii_values[NUM_CYLINDERS];

GLuint cylinder_colors_location;
vec cylinder_color_values[NUM_CYLINDERS];


//for animating the progress bar thing
glm::vec3 base_point = glm::vec3(0.0f, 0.0f, -0.295f);
glm::vec3 full_bar = glm::vec3(0.0f, 0.0f, 0.295f * 2.0f);




// ROTATION AND PROJECTION

float x_rot = 0.0f;
float y_rot = 1.0f;
float z_rot = 2.0f;


glm::mat4 rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
// glm::mat4 rotation = glm::mat4( 1.0 );



float base_scale = 0.85;

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






GLuint shader_handle;

GLuint texture; //handle for the texture



void update_rotation();


void generate_points()
{
	// float total_edge_length = 1.0f;

	float total_edge_length = 0.8f;
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

	// SPHERE VALUES

		sphere_center_location = glGetUniformLocation( shader_handle, "sphere_center" );
		sphere_radius_location = glGetUniformLocation( shader_handle, "sphere_radius" );
		sphere_colors_location = glGetUniformLocation( shader_handle, "sphere_colors" );

	// INITIAL SPHERE DATA

		sphere_center_value[0] = glm::vec3( 0.0f, 0.0f, 0.0f );
		sphere_radius_value[0] = 0.22f;
		sphere_colors_values[0] = vec(0.5f, 0.0f, 0.1f, 0.2f);

		glUniform3fv(sphere_center_location, NUM_SPHERES, glm::value_ptr( sphere_center_value[0] ) );
		glUniform1fv(sphere_radius_location, NUM_SPHERES, &sphere_radius_value[0] );
		glUniform4fv(sphere_colors_location, NUM_SPHERES, glm::value_ptr( sphere_colors_values[0] ) );












//TRIANGLES

	// TRIANGLE VALUES

		triangle_point1_location = glGetUniformLocation( shader_handle, "triangle_point1" );
		triangle_point2_location = glGetUniformLocation( shader_handle, "triangle_point2" );
		triangle_point3_location = glGetUniformLocation( shader_handle, "triangle_point3" );

		triangle_colors_location = glGetUniformLocation( shader_handle, "triangle_colors" );

		triangle_thickness_location = glGetUniformLocation( shader_handle, "triangle_thickness" );


	// INITIAL TRIANGLE DATA


	//this is just to maintain a placeholder

		triangle_point1_values[0] = glm::vec3(  0.0f, -0.2f, -0.2f );
		triangle_point2_values[0] = glm::vec3( -0.2f,  0.0f, -0.2f );
		triangle_point3_values[0] = glm::vec3( -0.2f, -0.2f,  0.0f );

		triangle_color_values[0] = glm::vec4( 0.99f, 0.25f, 0.29f, 1.0f );

		thickness[0] = 0.04f;

	 	glUniform3fv( triangle_point1_location, NUM_TRIANGLES, glm::value_ptr( triangle_point1_values[0] ) );
		glUniform3fv( triangle_point2_location, NUM_TRIANGLES, glm::value_ptr( triangle_point2_values[0] ) );
		glUniform3fv( triangle_point3_location, NUM_TRIANGLES, glm::value_ptr( triangle_point3_values[0] ) );

		glUniform3fv( triangle_colors_location, NUM_TRIANGLES, glm::value_ptr( triangle_color_values[0] ) );


		glUniform1fv( triangle_thickness_location, NUM_TRIANGLES, &thickness[0]);














//QUADRILATERAL HEXAHEDRON (CUBOID)

	cuboid_a_location = glGetUniformLocation( shader_handle, "cuboid_a");
	cuboid_b_location = glGetUniformLocation( shader_handle, "cuboid_b");
	cuboid_c_location = glGetUniformLocation( shader_handle, "cuboid_c");
	cuboid_d_location = glGetUniformLocation( shader_handle, "cuboid_d");
	cuboid_e_location = glGetUniformLocation( shader_handle, "cuboid_e");
	cuboid_f_location = glGetUniformLocation( shader_handle, "cuboid_f");
	cuboid_g_location = glGetUniformLocation( shader_handle, "cuboid_g");
	cuboid_h_location = glGetUniformLocation( shader_handle, "cuboid_h");

	cuboid_colors_location = glGetUniformLocation( shader_handle, "cuboid_colors");


	float p =  0.05f;
	float n = -0.05f;

	glm::vec3 cuboid_offset = glm::vec3(0.2, 0.2, 0.2);

	cuboid_a_values[0] = glm::vec3( n, p, p) + cuboid_offset;
	cuboid_b_values[0] = glm::vec3( n, n, p) + cuboid_offset;
	cuboid_c_values[0] = glm::vec3( p, p, p) + cuboid_offset;
	cuboid_d_values[0] = glm::vec3( p, n, p) + cuboid_offset;
	cuboid_e_values[0] = glm::vec3( n, p, n) + cuboid_offset;
	cuboid_f_values[0] = glm::vec3( n, n, n) + cuboid_offset;
	cuboid_g_values[0] = glm::vec3( p, p, n) + cuboid_offset;
	cuboid_h_values[0] = glm::vec3( p, n, n) + cuboid_offset;

	cuboid_color_values[0] = vec(0.0f, 0.0f, 0.0f, 1.0f);


	glUniform3fv(cuboid_a_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_a_values[0] ) );
	glUniform3fv(cuboid_b_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_b_values[0] ) );
	glUniform3fv(cuboid_c_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_c_values[0] ) );
	glUniform3fv(cuboid_d_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_d_values[0] ) );
	glUniform3fv(cuboid_e_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_e_values[0] ) );
	glUniform3fv(cuboid_f_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_f_values[0] ) );
	glUniform3fv(cuboid_g_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_g_values[0] ) );
	glUniform3fv(cuboid_h_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_h_values[0] ) );

	glUniform4fv(cuboid_colors_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_color_values[0] ) );













//CYLINDERS

	cylinder_tvec_location = glGetUniformLocation( shader_handle, "cylinder_tvec");
	cylinder_bvec_location = glGetUniformLocation( shader_handle, "cylinder_bvec");
	cylinder_radii_location = glGetUniformLocation( shader_handle, "cylinder_radii");
	cylinder_colors_location = glGetUniformLocation( shader_handle, "cylinder_colors");




	cylinder_tvec_values[0] = glm::vec3( 0.0f, 0.0f, -0.3f);
	cylinder_bvec_values[0] = glm::vec3( 0.0f, 0.0f,  0.3f);
	cylinder_radii_values[0] = 0.07f;
	cylinder_color_values[0] = vec(0.0f, 0.0f, 0.4f, 0.1f);

	cylinder_tvec_values[1] = glm::vec3( 0.0f, 0.0f, -0.295f);
	cylinder_bvec_values[1] = glm::vec3( 0.0f, 0.0f,  0.295f);
	cylinder_radii_values[1] = 0.06f;
	cylinder_color_values[1] = vec(0.75f, 0.0f, 0.3f, 1.0f);



	glUniform3fv(cylinder_tvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_tvec_values[0] ) );
	glUniform3fv(cylinder_bvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_bvec_values[0] ) );
	glUniform1fv(cylinder_radii_location, NUM_CYLINDERS, &cylinder_radii_values[0] );
	glUniform4fv(cylinder_colors_location, NUM_CYLINDERS, glm::value_ptr( cylinder_color_values[0] ) );




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

	// //rotate the points making up the triangle - this requires the use of a 4 unit vector, to use the 4x4 rotation matrix
	// // (still simpler than writing out the whole rotation matrix)
	//
	// // need to use vec4s to multiply by a 4x4 rotation matrix
	// vec triangle_point1_temp;
	// vec triangle_point2_temp;
	// vec triangle_point3_temp;
	//
	// // variable axis of rotation
	// glm::vec3 axis;
	//
	// if(rotate_triangle)
	// {
	// 	for(int i = 0; i < NUM_TRIANGLES; i++)
	// 	{
	//
	// 		switch(i % 3)
	// 		{
	// 			case 0:
	// 				axis = glm::vec3(1.0f, 0.0f, 0.0f);
	// 				break;
	// 			case 1:
	// 				axis = glm::vec3(0.0f, 1.0f, 0.0f);
	// 				break;
	// 			case 2:
	// 				axis = glm::vec3(0.0f, 0.0f, 1.0f);
	// 				break;
	// 		}
	//
	// 		// axis = glm::vec3(0.0f, 0.0f, 1.0f);
	//
	// 		triangle_point1_temp = glm::rotate(0.01f, axis) * vec(triangle_point1_values[i], 1.0f);
	// 		triangle_point2_temp = glm::rotate(0.01f, axis) * vec(triangle_point2_values[i], 1.0f);
	// 		triangle_point3_temp = glm::rotate(0.01f, axis) * vec(triangle_point3_values[i], 1.0f);
	//
	// 		triangle_point1_values[i] = triangle_point1_temp; // they don't do the .xyz swizzle thing in the glm library, but this works to get the first three elements
	// 		triangle_point2_values[i] = triangle_point2_temp;
	// 		triangle_point3_values[i] = triangle_point3_temp;
	//
	// 	 	glUniform3fv( triangle_point1_location, NUM_TRIANGLES, glm::value_ptr( triangle_point1_values[0] ) );
	// 		glUniform3fv( triangle_point2_location, NUM_TRIANGLES, glm::value_ptr( triangle_point2_values[0] ) );
	// 		glUniform3fv( triangle_point3_location, NUM_TRIANGLES, glm::value_ptr( triangle_point3_values[0] ) );
	//
	// 	}
	// }
	//
	// vec cuboid_a_temp;
	// vec cuboid_b_temp;
	// vec cuboid_c_temp;
	// vec cuboid_d_temp;
	// vec cuboid_e_temp;
	// vec cuboid_f_temp;
	// vec cuboid_g_temp;
	// vec cuboid_h_temp;
	//
	//
	// if(rotate_hexahedrons)
	// {
	// 	for(int i = 0; i < NUM_QUAD_HEXS; i++)
	// 	{
	//
	// 		axis = glm::vec3(1.0f, 0.0f, 0.0f);
	//
	// 		cuboid_a_temp = glm::rotate(0.01f, axis) * vec(cuboid_a_values[i], 1.0f);
	// 		cuboid_b_temp = glm::rotate(0.01f, axis) * vec(cuboid_b_values[i], 1.0f);
	// 		cuboid_c_temp = glm::rotate(0.01f, axis) * vec(cuboid_c_values[i], 1.0f);
	// 		cuboid_d_temp = glm::rotate(0.01f, axis) * vec(cuboid_d_values[i], 1.0f);
	// 		cuboid_e_temp = glm::rotate(0.01f, axis) * vec(cuboid_e_values[i], 1.0f);
	// 		cuboid_f_temp = glm::rotate(0.01f, axis) * vec(cuboid_f_values[i], 1.0f);
	// 		cuboid_g_temp = glm::rotate(0.01f, axis) * vec(cuboid_g_values[i], 1.0f);
	// 		cuboid_h_temp = glm::rotate(0.01f, axis) * vec(cuboid_h_values[i], 1.0f);
	//
	//
	// 		cuboid_a_values[i] = cuboid_a_temp;
	// 		cuboid_b_values[i] = cuboid_b_temp;
	// 		cuboid_c_values[i] = cuboid_c_temp;
	// 		cuboid_d_values[i] = cuboid_d_temp;
	// 		cuboid_e_values[i] = cuboid_e_temp;
	// 		cuboid_f_values[i] = cuboid_f_temp;
	// 		cuboid_g_values[i] = cuboid_g_temp;
	// 		cuboid_h_values[i] = cuboid_h_temp;
	//
	// 	}
	//
	// 	glUniform3fv( cuboid_a_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_a_values[0] ) );
	// 	glUniform3fv( cuboid_b_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_b_values[0] ) );
	// 	glUniform3fv( cuboid_c_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_c_values[0] ) );
	// 	glUniform3fv( cuboid_d_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_d_values[0] ) );
	// 	glUniform3fv( cuboid_e_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_e_values[0] ) );
	// 	glUniform3fv( cuboid_f_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_f_values[0] ) );
	// 	glUniform3fv( cuboid_g_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_g_values[0] ) );
	// 	glUniform3fv( cuboid_h_location, NUM_QUAD_HEXS, glm::value_ptr( cuboid_h_values[0] ) );
	//
	// }

	float amount = (0.5f * std::sin(0.02f * numFrames) + 0.5f);

	// ranging from 0.0 to 1.0

	cylinder_bvec_values[1] = base_point + amount * full_bar;
	glUniform3fv(cylinder_bvec_location, NUM_CYLINDERS, glm::value_ptr( cylinder_bvec_values[0] ) );


	cylinder_color_values[1] = vec(amount, 1.0f - amount , 0.0f, 1.0f);
	glUniform4fv(cylinder_colors_location, NUM_CYLINDERS, glm::value_ptr( cylinder_color_values[0] ) );




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
