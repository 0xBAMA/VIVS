#include <iostream> //terminal I/O
using std::cout;
using std::endl;


#define GLM_FORCE_SWIZZLE			//supposedly ....
#include "resources/glm/glm.hpp" 									// general types
#include "resources/glm/gtc/matrix_transform.hpp" // orthographic view matrix (glm::ortho( left, right, bottom, top, zNear, zFar ))
#include "resources/glm/gtc/type_ptr.hpp" 				// allows the sending of a matrix (for glUniform)
#include "resources/glm/gtx/transform.hpp"				// rotate()


int num_vectors = 48;

glm::vec3 vectors[] = {

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




int main()
{

	for(int i = 0; i < num_vectors; i++)
	{
		cout << vectors[i].x << " " << vectors[i].y << " " << vectors[i].z << " maps to entry " << calcOrder(vectors[i]) << endl;
	}

}
