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
const int points_per_side = 100;
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


// not worrying about location, using glGetUniformLocation in the initialization to get the values
int point1_position;
int point2_position;
int point3_position;

int thickness_position;


// used to hold the geometry values CPU-side

//SPHERE
float sphere_radius_value;
glm::vec3 sphere_center_value;


#define NUM_TRIANGLES 12

//TRIANGLE
glm::vec3 point1[NUM_TRIANGLES];
glm::vec3 point2[NUM_TRIANGLES];
glm::vec3 point3[NUM_TRIANGLES];

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

	point1_position = glGetUniformLocation( shader_handle, "point1" );
	point2_position = glGetUniformLocation( shader_handle, "point2" );
	point3_position = glGetUniformLocation( shader_handle, "point3" );

	thickness_position = glGetUniformLocation( shader_handle, "thickness" );


// INITIAL TRIANGLE DATA

	point1[0] = glm::vec3(  0.0f, -0.2f, -0.2f );
	point2[0] = glm::vec3( -0.2f,  0.0f, -0.2f );
	point3[0] = glm::vec3( -0.2f, -0.2f,  0.0f );

	point1[1] = glm::vec3(  0.0f, -0.2f, -0.2f );
	point2[1] = glm::vec3( -0.2f,  0.0f, -0.2f );
	point3[1] = glm::vec3( -0.2f, -0.2f,  0.0f );

	point1[2] = glm::vec3(  0.0f, -0.2f, -0.2f );
	point2[2] = glm::vec3( -0.2f,  0.0f, -0.2f );
	point3[2] = glm::vec3( -0.2f, -0.2f,  0.0f );


	point1[3] = glm::vec3(  0.0f,  0.2f,  0.2f );
	point2[3] = glm::vec3(  0.2f,  0.0f,  0.2f );
	point3[3] = glm::vec3(  0.2f,  0.2f,  0.0f );

	point1[4] = glm::vec3(  0.0f,  0.2f,  0.2f );
	point2[4] = glm::vec3(  0.2f,  0.0f,  0.2f );
	point3[4] = glm::vec3(  0.2f,  0.2f,  0.0f );

	point1[5] = glm::vec3(  0.0f,  0.2f,  0.2f );
	point2[5] = glm::vec3(  0.2f,  0.0f,  0.2f );
	point3[5] = glm::vec3(  0.2f,  0.2f,  0.0f );


	point1[6] = glm::vec3(  0.3f, -0.1f, -0.1f );
	point2[6] = glm::vec3( -0.1f,  0.3f, -0.1f );
	point3[6] = glm::vec3( -0.1f, -0.1f,  0.3f );

	point1[7] = glm::vec3(  0.3f, -0.1f, -0.1f );
	point2[7] = glm::vec3( -0.1f,  0.3f, -0.1f );
	point3[7] = glm::vec3( -0.1f, -0.1f,  0.3f );

	point1[8] = glm::vec3(  0.3f, -0.1f, -0.1f );
	point2[8] = glm::vec3( -0.1f,  0.3f, -0.1f );
	point3[8] = glm::vec3( -0.1f, -0.1f,  0.3f );


	point1[9] = glm::vec3(  -0.3f,  0.1f,  0.1f );
	point2[9] = glm::vec3(  0.1f,  -0.3f,  0.1f );
	point3[9] = glm::vec3(  0.1f,  0.1f,  -0.3f );

	point1[10] = glm::vec3(  -0.3f,  0.1f,  0.1f );
	point2[10] = glm::vec3(  0.1f,  -0.3f,  0.1f );
	point3[10] = glm::vec3(  0.1f,  0.1f,  -0.3f );

	point1[11] = glm::vec3(  -0.3f,  0.1f,  0.1f );
	point2[11] = glm::vec3(  0.1f,  -0.3f,  0.1f );
	point3[11] = glm::vec3(  0.1f,  0.1f,  -0.3f );

	thickness = 0.05f;

 	glUniform3fv( point1_position, NUM_TRIANGLES, glm::value_ptr( point1[0] ) );
	glUniform3fv( point2_position, NUM_TRIANGLES, glm::value_ptr( point2[0] ) );
	glUniform3fv( point3_position, NUM_TRIANGLES, glm::value_ptr( point3[0] ) );


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

	// need to use vec4s to multiply by a 4x4 rotation matrix
	vec point1_temp;
	vec point2_temp;
	vec point3_temp;

	// variable axis of rotation
	glm::vec3 axis;

	if(rotate_triangle)
	{
		for(int i = 0; i < NUM_TRIANGLES; i++)
		{

			switch(i % 3)
			{
				case 0:
					axis = glm::vec3(1.0f, 0.0f, 0.0f);
					break;
				case 1:
					axis = glm::vec3(0.0f, 1.0f, 0.0f);
					break;
				case 2:
					axis = glm::vec3(0.0f, 0.0f, 1.0f);
					break;
			}

			point1_temp = glm::rotate(0.01f, axis) * vec(point1[i], 1.0f);
			point2_temp = glm::rotate(0.01f, axis) * vec(point2[i], 1.0f);
			point3_temp = glm::rotate(0.01f, axis) * vec(point3[i], 1.0f);

			point1[i] = point1_temp; // they don't do the .xyz swizzle thing in the glm library, but this works to get the first three elements
			point2[i] = point2_temp;
			point3[i] = point3_temp;
		}

	 	glUniform3fv( point1_position, NUM_TRIANGLES, glm::value_ptr( point1[0] ) );
		glUniform3fv( point2_position, NUM_TRIANGLES, glm::value_ptr( point2[0] ) );
		glUniform3fv( point3_position, NUM_TRIANGLES, glm::value_ptr( point3[0] ) );

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
			left 		*= ( 1 / 0.9 );
			right 	*= ( 1 / 0.9 );
			top 		*= ( 1 / 0.9 );
			bottom 	*= ( 1 / 0.9 );
			zNear 	*= ( 1 / 0.9 );
			zFar 		*= ( 1 / 0.9 );

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




void update_rotation()
{ // uses global x rotation, y rotation, z rotation
	rotation = glm::rotate( x_rot, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(y_rot, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(z_rot, glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv( rotation_position, 1, GL_FALSE,  glm::value_ptr( rotation ) );
}
