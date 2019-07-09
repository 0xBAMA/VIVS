#version 450 core

// layout(location = 0) in vec4 vColor;
layout(location = 1) in vec4 vPosition;

layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 rotation;

out vec4 color;

layout(location = 4)  uniform vec3 sphere_center;
layout(location = 5)  uniform float sphere_radius;



#define NUM_TRIANGLES 6

// going to need to read up on uniform blocks

uniform vec3 point1[NUM_TRIANGLES];
uniform vec3 point2[NUM_TRIANGLES];
uniform vec3 point3[NUM_TRIANGLES];

uniform float thickness;






uniform sampler2D ourTexture;



bool planetest(vec3 plane_point, vec3 plane_normal, vec3 test_point);



void main()
{
  gl_Position = view * rotation * vPosition;
	// This is the same, regardless of anything that happens subsequently

	// color = vColor;
	// color = vec4( 0.0f, 0.0f, 0.0f, 0.0f );




	//SPHERE

	float the_distance = distance( vPosition, vec4( sphere_center, 1.0f ) );

	if( the_distance < ( 0.75 * sphere_radius ) ) // INNERMOST LAYER
	{
		// color = vec4(1.0f, 0.3f, 0.5f, 1.0f);
		color = vec4(0.4, 0.5, the_distance, 0.01f);
	}
	else if( the_distance < sphere_radius) // THE NEXT LAYER
	{
		// color = vec4(0.3, 0.2, pow(the_distance, 3), 0.5f);
		color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}


	//HEIGHTMAP
	float height = texture( ourTexture, 1.5 * (vPosition.yz + vec2( 0.35f, 0.35f ) ) ).r;

	if(height != 0)
	{
		if(vPosition.x + 0.30 < height * 0.05 && vPosition.x > -0.34)
		{
			color = vec4( height, 0.2f, 0.1f, 0.7f);
		}
	}




	//TRIANGLE

	// to aleviate the redundancy of computing all the cross products, etc, for every point,
	// the normals are computed CPU-side and sent in as uniform variables then the value of
	// vPosition is checked against 5 different planes which represent the top, bottom, and
	// three sides of the triangle - if it is 'below' all 5 planes, the point can be said to
	// be 'inside' the triangle.

	vec3 calculated_triangle_center;
	vec3 calculated_top_normal;
	vec3 calculated_side_1_2_normal;
	vec3 calculated_side_2_3_normal;
	vec3 calculated_side_3_1_normal;

	bool draw_triangles[NUM_TRIANGLES];
	vec4 colors[NUM_TRIANGLES];

	int how_many_being_drawn = 0;
	vec4 sum = vec4(0.0f, 0.0f, 0.0f, 0.0f);


// TRIANGLES

	for(int i = 0; i < NUM_TRIANGLES; i++)
	{

		colors[i] = vec4(0.75f + 0.25 * sin(i + 1.5), 0.25f + 0.25 * sin(i), 0.5f + 0.5 * cos(i + 2.0), 1.0f);

		//calculate the center of the triangle
		calculated_triangle_center = ( point1[i] + point2[i] + point3[i] ) / 3.0f;

		//calculate the top normal vector of the triangle
		calculated_top_normal = normalize( cross( point1[i] - point2[i], point1[i] - point3[i] ) );
		calculated_top_normal = planetest( point1[i] + thickness * calculated_top_normal, calculated_top_normal, calculated_triangle_center ) ? calculated_top_normal : ( calculated_top_normal * -1.0f );

		//calculate the side normal vectors

		//       *
		//      / \
		//     /   \
		//    /     \
		//   *-------* <--- the side being considered
		//
		//	looking at this edge-on:
		//
		//   *
		//   | <----------- the triangle's top normal
		//   *-------* <--- vector representing the side being considered
		//
		//   take the cross product of these two vectors, then do a similar test involving the center point of the triangle to invert it if neccesary

		calculated_side_1_2_normal = normalize( cross( calculated_top_normal, point2[i] - point1[i] ) );
		calculated_side_1_2_normal = planetest( point1[i], calculated_side_1_2_normal, calculated_triangle_center) ? calculated_side_1_2_normal : ( calculated_side_1_2_normal * -1.0f );

		calculated_side_2_3_normal = normalize( cross( calculated_top_normal, point3[i] - point2[i] ) );
		calculated_side_2_3_normal = planetest( point2[i], calculated_side_2_3_normal, calculated_triangle_center) ? calculated_side_2_3_normal : ( calculated_side_2_3_normal * -1.0f );

		calculated_side_3_1_normal = normalize( cross( calculated_top_normal, point1[i] - point3[i] ) );
		calculated_side_3_1_normal = planetest( point3[i], calculated_side_3_1_normal, calculated_triangle_center) ? calculated_side_3_1_normal : ( calculated_side_3_1_normal * -1.0f );

		// do the tests
		draw_triangles[i] = planetest( point1[i] + ( thickness / 2.0f ) * calculated_top_normal, calculated_top_normal, vPosition.xyz ) &&
		planetest( point1[i] - ( thickness / 2.0f ) * calculated_top_normal, -1.0f * calculated_top_normal, vPosition.xyz ) &&
		planetest( point1[i], calculated_side_1_2_normal, vPosition.xyz ) &&
		planetest( point2[i], calculated_side_2_3_normal, vPosition.xyz ) &&
		planetest( point3[i], calculated_side_3_1_normal, vPosition.xyz );

		if(draw_triangles[i])
		{
			how_many_being_drawn++;
			sum += colors[i];
		}
	}




	if(how_many_being_drawn > 0)
	{
		color = sum / how_many_being_drawn;
		// color = colors[how_many_being_drawn-1];
	}


}











bool planetest(vec3 plane_point, vec3 plane_normal, vec3 test_point)
{
  //return false if the point is above the plane
	//return true if the point is below the plane

	float result = 0.0;

	//equation of plane

	// a (x-x1) + b (y-y1) + c (z-z1) = 0

	float a = plane_normal.x;
	float b = plane_normal.y;
	float c = plane_normal.z;

	float x1 = plane_point.x;
	float y1 = plane_point.y;
	float z1 = plane_point.z;

	float x = test_point.x;
	float y = test_point.y;
	float z = test_point.z;

	result = a * (x - x1) + b * (y - y1) + c * (z - z1);

	return (result < 0) ? true : false;
}
