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
// const int image_height = 768/2;
// const int image_width = 1366/2;

const int image_height = 768;
const int image_width = 1366;


//How many verticies to use, to represent all the voxels
const int points_per_side = 125;
const int NumVertices = points_per_side * points_per_side * points_per_side;

long int numFrames = 0;

//and the array to hold them
vec points[NumVertices];





// UNIFORMS

//based upon the layout qualifiers in the vertex shader
// const int vColor_index = 0;
int vPosition_index = 1;

int view_position = 2;
int rotation_position = 3;

// shape parameters

int sphere_center_position;
int sphere_radius_position;


int numTriangles = 10;

// not worrying about location, using glGetUniformLocation in the initialization to get the values
int point1_position;
int point2_position;
int point3_position;

int thickness_position;


// used to hold the geometry values CPU-side

//SPHERE
float sphere_radius_value;
glm::vec3 sphere_center_value;

//TRIANGLE
glm::vec3 point1;
glm::vec3 point2;
glm::vec3 point3;

float thickness;




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






GLuint shader_handle;

GLuint texture; //handle for the texture



// bool planetest(glm::vec3 plane_point, glm::vec3 plane_normal, glm::vec3 test_point);



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

	view_position = glGetUniformLocation( shader_handle, "view" );
	rotation_position = glGetUniformLocation( shader_handle, "rotation" );


	glUniformMatrix4fv( view_position, 1, GL_FALSE,  glm::value_ptr( projection ) );
	glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );


// GEOMETRY

// SPHERE VALUES

	sphere_center_position = glGetUniformLocation( shader_handle, "sphere_center" );
	sphere_radius_position = glGetUniformLocation( shader_handle, "sphere_radius" );

// INITIAL SPHERE DATA

	sphere_center_value = glm::vec3( 0.0f, 0.0f, 0.0f );
	sphere_radius_value = 0.7f;

	glUniform3fv(sphere_center_position, 1, glm::value_ptr( sphere_center_value ) );
	glUniform1fv(sphere_radius_position, 1, &sphere_radius_value );


// TRIANGLE VALUES

	point1_position = glGetUniformLocation( shader_handle, "triangle.point1" );
	point2_position = glGetUniformLocation( shader_handle, "triangle.point2" );
	point3_position = glGetUniformLocation( shader_handle, "triangle.point3" );

	thickness_position = glGetUniformLocation( shader_handle, "triangle.thickness" );

	cout << endl << point1_position << " " << point2_position << " " << point3_position << " " << thickness_position << endl;


// INITIAL TRIANGLE DATA

	// point1 = glm::vec3(  0.3f, -0.1f, -0.1f );
	// point2 = glm::vec3( -0.1f,  0.3f, -0.1f );
	// point3 = glm::vec3( -0.1f, -0.1f,  0.3f );

	point1 = glm::vec3(  0.0f, -0.2f, -0.2f );
	point2 = glm::vec3( -0.2f,  0.0f, -0.2f );
	point3 = glm::vec3( -0.2f, -0.2f,  0.0f );

	thickness = 0.05f;

 	glUniform3fv( point1_position, 1, glm::value_ptr( point1 ) );
	glUniform3fv( point2_position, 1, glm::value_ptr( point2 ) );
	glUniform3fv( point3_position, 1, glm::value_ptr( point3 ) );


	glUniform1fv( thickness_position, 1, &thickness);

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

	//rotate the points making up the triangle - this requires the use of a 4 unit vector, to use the 4x4 rotation matrix
	// (still simpler than writing out the whole rotation matrix)


	if(rotate_triangle)
	{

		vec point1_temp = glm::rotate(0.01f, glm::vec3(1.0f, 0.0f, 0.0f)) * vec(point1, 1.0f);
		vec point2_temp = glm::rotate(0.01f, glm::vec3(1.0f, 0.0f, 0.0f)) * vec(point2, 1.0f);
		vec point3_temp = glm::rotate(0.01f, glm::vec3(1.0f, 0.0f, 0.0f)) * vec(point3, 1.0f);

		point1 = point1_temp; // they don't do the .xyz swizzle thing in the glm library, but this works to get the first three elements
		point2 = point2_temp;
		point3 = point3_temp;

	 	glUniform3fv( point1_position, 1, glm::value_ptr( point1 ) );
		glUniform3fv( point2_position, 1, glm::value_ptr( point2 ) );
		glUniform3fv( point3_position, 1, glm::value_ptr( point3 ) );

	}

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

			glUniformMatrix4fv( view_position, 1, GL_FALSE,  glm::value_ptr( projection ) );
			break;

		case 'z':
			//zoom out
			left 		*= 1 / 0.9;
			right 	*= 1 / 0.9;
			top 		*= 1 / 0.9;
			bottom 	*= 1 / 0.9;
			zNear 	*= 1 / 0.9;
			zFar 		*= 1 / 0.9;

			projection = glm::ortho(left, right, top, bottom, zNear, zFar);

			glUniformMatrix4fv( view_position, 1, GL_FALSE,  glm::value_ptr( projection ) );
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
			break;

		// CONTROLING THE ROTATION OF THE BLOCK

		case 'f': //reset the block's rotation

			x_rot = 0.0f;
			y_rot = 45.0f;
			z_rot = 90.0f;

			rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );

			break;

		case 'e': // output the values of the current rotation

			cout << "xrot " << x_rot << " yrot " << y_rot << " zrot " << z_rot << endl;

			break;

		case 't':	// add to the block's x rotation

			x_rot += 0.01f;

			rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );


			break;

		case 'c': // subtract from the block's x rotation

			x_rot -= 0.01f;

			rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );

			break;

		case 'g': // add to the block's y rotation

			y_rot += 0.01f;

			rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );

			break;

		case 'd': // subtract from the block's y rotation

			y_rot -= 0.01f;

			rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );

			break;

		case 'v': // add to the block's z rotation

			z_rot += 0.01f;

			rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );

			break;

		case 'r': // subtract from the block's z rotation

			z_rot -= 0.01f;

			rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );

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



	cout << "\rGLUT Initialization Complete." << endl;


	glewExperimental = GL_TRUE;
	glewInit();

	cout << "OpenGL Context established, version is: " << glGetString( GL_VERSION ) << endl;





	cout << "Shader Compilation Starting...";

	Shader theShader( "resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl" );

	shader_handle = theShader.Program;

	cout << "\rShader Compilation Complete.  " << endl;




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


// // PLANETEST for computing the various components of the triangle
//
// bool planetest(glm::vec3 plane_point, glm::vec3 plane_normal, glm::vec3 test_point)
// {
//   //return false if the point is above the plane
// 	//return true if the point is below the plane
//
// 	float result = 0.0;
//
// 	//equation of plane
//
// 	// a (x-x1) + b (y-y1) + c (z-z1) = 0
//
// 	float a = plane_normal.x;
// 	float b = plane_normal.y;
// 	float c = plane_normal.z;
//
// 	float x1 = plane_point.x;
// 	float y1 = plane_point.y;
// 	float z1 = plane_point.z;
//
// 	float x = test_point.x;
// 	float y = test_point.y;
// 	float z = test_point.z;
//
// 	result = a * (x - x1) + b * (y - y1) + c * (z - z1);
//
// 	return (result < 0)?true:false;
// }
