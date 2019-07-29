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

long int numFrames = 0;


//How many verticies to use, to represent all the voxels (default value)
int points_per_side = 100;
int NumVertices = points_per_side * points_per_side * points_per_side;


//and the array to hold them
const int MaxVerticies = 64000000;
vec points[MaxVerticies];



//trying something new to get around the alpha limitations - keeping
//48 copies of the vertex array

//	http://www.iquilezles.org/www/articles/volumesort/volumesort.htm

// TEXT FROM THAT PAGE:
//
//
// Intro
// Let's say you have a set of objects that you want to alpha blend. Let's say
// the position of these objects is constant. And let's assume also that your
// objects are all positioned along a 2d or 3d grid. Then, you can very easily
// sort this objects at virtually zero performance cost, and this article will
// explain you how.
//
// It might look like the premises are too restrictive, not applicable to "real
// life". However, imagine you have a field of grass, and you want to draw the
// blades in back-to-front order. You can probably afford aligning them into a
// 2D grid, and might be apply random scale and orientation to break the
// regulartity. Or may be you have a cloud rendering engine using billboards,
// and you have to sort them also to properly alpha blend the particles. Or you
// could even have a point-cloud viewer showing some nice Julia sets coming
// froma 1024^3 voxel (like the one in the end of this article).
//
//
//
// In 2D
// To explain the techinque, let's first think about the problem in 2D.
// Let's say you have a grid of objects like the one below. Now let's say you
// are looking to this grid of objects from the view point indicated by the
// orange arrow in the diagram. Now, try to agree with the fact that given this
// situation you could draw your objects line by line, starting from the line a,
// b-... until the line p, q-...
//
// That's obviously because we are looking roughly from bottom to top. Note also
// that because we are a bit skewed to the left, we better draw the objects from
// "left to right" within each line; ie, a, c, d, ... instead of ..., e, d, c, b, a.
//
// Good. In a very similar way, the correct order to render the objects in the
// grid for a view point like the one indicated with the green arrow should be
// left to rigth for the columns, and then botton to top within each column:
// ..., p, k, f, a, ..., q, l, g, b, ...
//
// So it's quite simple. We can determine the order just based on the view
// vector. If instead of "left to right" and "botton to up" we use +x, -x, +y
// and -y, we can easily see that there 8 different possible orders:
// { +x+y, +x-y, -x+y, +x-y, +y+x, +y-x, -y+x, -y-x } (basically we have
// 4 options for the first axis (+x, -x, +y, -y) and the there is only 2
// remaining for the second (+x, -x or +y, -y, depending on the first option).
//
// The transition between one order and another is done on half quadrants, as
// you can see. The figure showing 2D square split in 8 sections shows the areas
// where the same order is valid (you can see the orange and green areas
// corresponding to the arrows we used as example in the previous diagram).
// The trick now is clear: precompute 8 index arrays and save the in video
// memory, one for each possible order. For each rendering pass, take the view
// direction and compute wich of the orders is the good one, and use it to
// render. So, we basically skip any sorting time, and also bus trafic between
// the CPU and the GPU. The only drawback is that we need 8 copies of the index
// arrays in memory instead of 8, and as we will see inmediatly, it is even
// worse in 3D... But again, is so cool to have zero sorting cost!
//
//
//
//
// When the view vector stays in any
// point of a colored area, the order
// is the same.
//
// In 3D
// In 3D the situation is quite the same, we only have one axis more. The
// difference is that now the amount of possible orders is quite bigger. For the
// first axis's order we have 6 options (-x,+x,-y,+y,-z,+z), for the second we
// have 4 (assume we choosed -x, the we still have -y,+y,-z,+z) and 2 for the
// last axis (assuming we chose +z, we still have -y and +y). So that's a total
// of 48 posibilities! This can be a lot of video memory depending of the
// application. There is some simple tricks to help of course. For example, we
// keep the 48 copies in system memory and just upload the one we need. Assuming
// frame to frame coherence, this should happen not to often. We can even have a
// small thread runing in parallel to the rendering just calculating the index
// array instead of precalculating and storing it in system memory. We can even
// anticipate the camera movement and precompute (asynchronously) the next
// expected index array.
//
// Another trick is to have a top level grid to sort cells of objects, and then
// let random ordered drawing of the objects in the cell. If the objects where a
// field of grass, this can work pretty well. Or even, if we allready have an
// octree data structure to do frustum culling and occlussion queries on the
// dataset, we can sort the octree nodes with this technique and then do
// standard CPU sorting in the visible node, or even have precomputed index
// arrays per-node.
//
// Now the view vector can belong to 48 possible sections in the surface of a
// cube, as shown in the picture below.
//
// In 3D, we have 48 areas for the view vector.
//
// Implementation
// To finish the article, a bit of code to show how you can get the order index
// (from 0 to 47) from the 3D view vector. There is probably a more simple
// (read compact) way to do it.
//
// int calcOrder( const vec3 & dir )
// {
//     int signs;
//
//     const int   sx = dir.x<0.0f;
//     const int   sy = dir.y<0.0f;
//     const int   sz = dir.z<0.0f;
//     const float ax = fabsf( dir.x );
//     const float ay = fabsf( dir.y );
//     const float az = fabsf( dir.z );
//
//     if( ax>ay && ax>az )
//     {
//         if( ay>az ) signs = 0 + ((sx<<2)|(sy<<1)|sz);
//         else        signs = 8 + ((sx<<2)|(sz<<1)|sy);
//     }
//     else if( ay>az )
//     {
//         if( ax>az ) signs = 16 + ((sy<<2)|(sx<<1)|sz);
//         else        signs = 24 + ((sy<<2)|(sz<<1)|sx);
//     }
//     else
//     {
//         if( ax>ay ) signs = 32 + ((sz<<2)|(sx<<1)|sy);
//         else        signs = 40 + ((sz<<2)|(sy<<1)|sx);
//     }
//
//     return signs;
// }


























GLuint array_buffers[48];





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

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	glDrawArrays( GL_POINTS, 0, NumVertices );

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
