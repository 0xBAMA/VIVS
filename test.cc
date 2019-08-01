#include <iostream> //terminal I/O
using std::cout;
using std::endl;


int main()
{
	for(int i = 70; i < 90 ; i++)
	{//output information on cylinders


		cout << endl << "cylinder_tvec_values[" << i << "] = glm::vec3(0.0f, 0.0f, 0.0f);" << endl << "cylinder_bvec_values[" << i << "] = glm::vec3(0.0f, 0.0f, 0.0f);" << endl << "cylinder_radii_values[" << i << "] = 0.0f;" << endl << "cylinder_color_values[" << i << "] = ambient_color;" << endl <<"cylinder_offsets[" << i << "] = standard_offset;";
		cout << endl;

	}
}
