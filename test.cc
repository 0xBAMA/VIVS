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
																//evaluates to:
	glm::vec3(   1.0f,   0.75f,   0.1f),		//0
	glm::vec3(   1.0f,   0.75f,  -0.1f),		//1
	glm::vec3(   1.0f,  -0.75f,   0.1f),		//2
	glm::vec3(   1.0f,  -0.75f,  -0.1f),		//3
	glm::vec3(  -1.0f,   0.75f,   0.1f),		//4
	glm::vec3(  -1.0f,   0.75f,  -0.1f),		//5
	glm::vec3(  -1.0f,  -0.75f,   0.1f),		//6
	glm::vec3(  -1.0f,  -0.75f,  -0.1f),		//7
	glm::vec3(   1.0f,    0.1f,  0.75f),		//8
	glm::vec3(   1.0f,   -0.1f,  0.75f),		//9
	glm::vec3(   1.0f,    0.1f, -0.75f),		//10
	glm::vec3(   1.0f,   -0.1f, -0.75f),		//11
	glm::vec3(  -1.0f,    0.1f,  0.75f),		//12
	glm::vec3(  -1.0f,   -0.1f,  0.75f),		//13
	glm::vec3(  -1.0f,    0.1f, -0.75f),		//14
	glm::vec3(  -1.0f,   -0.1f, -0.75f),		//15
	glm::vec3(  0.75f,    1.0f,   0.1f),		//16
	glm::vec3(  0.75f,    1.0f,  -0.1f),		//17
	glm::vec3( -0.75f,    1.0f,   0.1f),		//18
	glm::vec3( -0.75f,    1.0f,  -0.1f),		//19
	glm::vec3(  0.75f,   -1.0f,   0.1f),		//20
	glm::vec3(  0.75f,   -1.0f,  -0.1f),		//21
	glm::vec3( -0.75f,   -1.0f,   0.1f),		//22
	glm::vec3( -0.75f,   -1.0f,  -0.1f),		//23
	glm::vec3(   0.1f,    1.0f,  0.75f),		//24
	glm::vec3(  -0.1f,    1.0f,  0.75f),		//25
	glm::vec3(   0.1f,    1.0f, -0.75f),		//26
	glm::vec3(  -0.1f,    1.0f, -0.75f),		//27
	glm::vec3(   0.1f,   -1.0f,  0.75f),		//28
	glm::vec3(  -0.1f,   -1.0f,  0.75f),		//29
	glm::vec3(   0.1f,   -1.0f, -0.75f),		//30
	glm::vec3(  -0.1f,   -1.0f, -0.75f),		//31
	glm::vec3(  0.75f,    0.1f,   1.0f),		//32
	glm::vec3(  0.75f,   -0.1f,   1.0f),		//33
	glm::vec3( -0.75f,    0.1f,   1.0f),		//34
	glm::vec3( -0.75f,   -0.1f,   1.0f),		//35
	glm::vec3(  0.75f,    0.1f,  -1.0f),		//36
	glm::vec3(  0.75f,   -0.1f,  -1.0f),		//37
	glm::vec3( -0.75f,    0.1f,  -1.0f),		//38
	glm::vec3( -0.75f,   -0.1f,  -1.0f),		//39
	glm::vec3(   0.1f,   0.75f,   1.0f),		//40
	glm::vec3(  -0.1f,   0.75f,   1.0f),		//41
	glm::vec3(   0.1f,  -0.75f,   1.0f),		//42
	glm::vec3(  -0.1f,  -0.75f,   1.0f),		//43
	glm::vec3(   0.1f,   0.75f,  -1.0f),		//44
	glm::vec3(  -0.1f,   0.75f,  -1.0f),		//45
	glm::vec3(   0.1f,  -0.75f,  -1.0f),		//46
	glm::vec3(  -0.1f,  -0.75f,  -1.0f)			//47


};


//this is the code from Inigo's site

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

	cout << calcOrder(glm::vec3(0.0f, 0.0f, 0.0f)) << endl;

}
