#version 450 core

// layout(location = 0) in vec4 vColor;
layout(location = 1) in vec4 vPosition;

layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 rotation;

out vec4 color;




#define NUM_SPHERES   1
#define NUM_TRIANGLES 1
#define NUM_QUAD_HEXS 1
#define NUM_CYLINDERS 1



//SPHERE VALUES
uniform vec3 sphere_center[NUM_SPHERES];
uniform float sphere_radius[NUM_SPHERES];

uniform vec4 sphere_colors[NUM_SPHERES];




//TRIANGLE VALUES
uniform vec3 triangle_point1[NUM_TRIANGLES];
uniform vec3 triangle_point2[NUM_TRIANGLES];
uniform vec3 triangle_point3[NUM_TRIANGLES];

uniform float triangle_thickness[NUM_TRIANGLES];

uniform vec4 triangle_colors[NUM_TRIANGLES];






//QUAD HEX/CUBOID VALUES
uniform vec3 cuboid_a[NUM_QUAD_HEXS];
uniform vec3 cuboid_b[NUM_QUAD_HEXS];
uniform vec3 cuboid_c[NUM_QUAD_HEXS];
uniform vec3 cuboid_d[NUM_QUAD_HEXS];
uniform vec3 cuboid_e[NUM_QUAD_HEXS];
uniform vec3 cuboid_f[NUM_QUAD_HEXS];
uniform vec3 cuboid_g[NUM_QUAD_HEXS];
uniform vec3 cuboid_h[NUM_QUAD_HEXS];

uniform vec4 cuboid_colors[NUM_QUAD_HEXS];






//CYLINDER VALUES
uniform vec3 cylinder_tvec[NUM_CYLINDERS];
uniform vec3 cylinder_bvec[NUM_CYLINDERS];

uniform float cylinder_radii[NUM_CYLINDERS];

uniform vec4 cylinder_colors[NUM_CYLINDERS];








int how_many_being_drawn = 0;
vec4 sum = vec4(0.0f, 0.0f, 0.0f, 0.0f);





uniform sampler2D ourTexture;



bool planetest(vec3 plane_point, vec3 plane_normal, vec3 test_point);



void main()
{
  gl_Position = view * rotation * vPosition;
	// This is the same, regardless of anything that happens subsequently

	// color = vColor;








	//SPHERE

	// float the_distance = distance( vPosition, vec4( sphere_center, 1.0f ) );
	//
	// if( the_distance < ( 0.75 * sphere_radius ) ) // INNERMOST LAYER
	// {
	// 	// color = vec4(1.0f, 0.3f, 0.5f, 1.0f);
	// 	color = vec4(0.4, 0.5, the_distance, 0.01f);
	// }
	// else if( the_distance < sphere_radius) // THE NEXT LAYER
	// {
	// 	// color = vec4(0.3, 0.2, pow(the_distance, 3), 0.5f);
	// 	color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	// }


	for(int i = 0; i < NUM_SPHERES; i++)
	{
		if( distance( vPosition.xyz, sphere_center[i]) < sphere_radius[i] )
		{
			how_many_being_drawn++;
			sum += sphere_colors[i];
		}
	}











	//HEIGHTMAP
	float height = texture( ourTexture, 1.5 * (vPosition.yz + vec2( 0.35f, 0.35f ) ) ).r;

	if(height != 0)
	{
		if(vPosition.x + 0.30 < height * 0.05 && vPosition.x > -0.34)
		{
			// color = vec4( height, 0.2f, 0.1f, 0.7f);

			how_many_being_drawn++;
			sum += vec4( height, 0.2f, 0.1f, 0.7f);

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



// TRIANGLES (TRIANGULAR PRISM WITH ADJUSTABLE THICKNESS)

	for(int i = 0; i < NUM_TRIANGLES; i++)
	{

		//calculate the center of the triangle
		calculated_triangle_center = ( triangle_point1[i] + triangle_point2[i] + triangle_point3[i] ) / 3.0f;


		//calculate the top normal vector of the triangle

		//    ^  < - - -normal
		//		|
		//		|
		//	1 .______. 2
		//		\							taking the cross product of the two sides (1-2 and 1-3)
		//		 \							will give either the normal or the inverse of the normal
		//      \								check this against the center point of the triangle to determine
		//			 * 3							and invert it if neccesary (depending on the relative positions
		//													of these three points)

		calculated_top_normal = normalize( cross( triangle_point1[i] - triangle_point2[i], triangle_point1[i] - triangle_point3[i] ) );
		calculated_top_normal = planetest( triangle_point1[i] + triangle_thickness[i] * calculated_top_normal, calculated_top_normal, calculated_triangle_center ) ? calculated_top_normal : ( calculated_top_normal * -1.0f );

		//calculate the side normal vectors

		//			   ^
		//			   |  < - - top normal
		//       _________
		//      |\       /| ^
		//      | \ top / |	| thickness
		//			|  \   /  | v
		//      \   \ /  /
		//       \   |  /
		//        \  | /
		//         \ |/
		//          \/
		//
		//	looking at this from one of the edges:
		//
		//   ^
		//   | < - - - - the triangle's top normal
		//   *-------> < - - - vector representing the side being considered
		//
		//   take the cross product of these two vectors, then do a similar test involving the center point of the triangle to invert it if neccesary

		calculated_side_1_2_normal = normalize( cross( calculated_top_normal, triangle_point2[i] - triangle_point1[i] ) );
		calculated_side_1_2_normal = planetest( triangle_point1[i], calculated_side_1_2_normal, calculated_triangle_center) ? calculated_side_1_2_normal : ( calculated_side_1_2_normal * -1.0f );

		calculated_side_2_3_normal = normalize( cross( calculated_top_normal, triangle_point3[i] - triangle_point2[i] ) );
		calculated_side_2_3_normal = planetest( triangle_point2[i], calculated_side_2_3_normal, calculated_triangle_center) ? calculated_side_2_3_normal : ( calculated_side_2_3_normal * -1.0f );

		calculated_side_3_1_normal = normalize( cross( calculated_top_normal, triangle_point1[i] - triangle_point3[i] ) );
		calculated_side_3_1_normal = planetest( triangle_point3[i], calculated_side_3_1_normal, calculated_triangle_center) ? calculated_side_3_1_normal : ( calculated_side_3_1_normal * -1.0f );


		// do the tests - for each of the normals, top, bottom, and the three sides,
		//	use the planetest function to determine whether the current point is
		//	'below' all 5 planes - if it is, it is inside this triangular prism


		draw_triangles[i] = planetest( triangle_point1[i] + ( triangle_thickness[i] / 2.0f ) * calculated_top_normal, calculated_top_normal, vPosition.xyz ) &&
		planetest( triangle_point1[i] - ( triangle_thickness[i] / 2.0f ) * calculated_top_normal, -1.0f * calculated_top_normal, vPosition.xyz ) &&
		planetest( triangle_point1[i], calculated_side_1_2_normal, vPosition.xyz ) &&
		planetest( triangle_point2[i], calculated_side_2_3_normal, vPosition.xyz ) &&
		planetest( triangle_point3[i], calculated_side_3_1_normal, vPosition.xyz );

		if(draw_triangles[i])
		{
			how_many_being_drawn++;
			sum += vec4(1.0f, 1.0f, 1.0f, 1.0f); //triangle_colors[i];
		}
	}















//QUADRILATERAL HEXAHEDRON (CUBOID)

	// 	point location reference
	//
	// 	   e-------g    +y
	// 	  /|      /|		 |
	// 	 / |     / |     |___+x
	// 	a-------c  |    /
	// 	|  f----|--h   +z
	// 	| /     | /
	//  |/      |/
	// 	b-------d

	//		the four points making up each of the 6 faces must be coplanar - if not,
	// 			the shape will not come out as intended (there would be two potential
	//			planes for each face, and only one of them is used to represent that face)

	//		that being said, there is still a degree of freedom allowing a lot of
	//			potential for non-cube shapes, making use of trapezoidal or
	//			rhombus-shaped faces which need not be parallel to one another.

	//		the algorithm is very similar to the one used for the triangle - it can
	//			be generalized to any convex polyhedron - concave shapes do not work
	//			as there are areas that should be within the shape that will not pass
	//			all the requisite plane tests, which will exclude some of the area that
	//			should lie within the shape

	vec3 quad_hex_center;

	vec3 quad_hex_top_normal;
	vec3 quad_hex_bottom_normal;
	vec3 quad_hex_left_normal;
	vec3 quad_hex_right_normal;
	vec3 quad_hex_front_normal;
	vec3 quad_hex_back_normal;

	bool draw_cuboid[NUM_QUAD_HEXS];


	for(int i = 0; i < NUM_QUAD_HEXS; i++)
	{
		quad_hex_center = (cuboid_a[i] + cuboid_b[i] + cuboid_c[i] + cuboid_d[i] + cuboid_e[i] + cuboid_f[i] + cuboid_g[i] + cuboid_h[i]) / 8.0f;

		//TOP - using ACE
		quad_hex_top_normal = normalize( cross( cuboid_a[i] - cuboid_c[i], cuboid_e[i] - cuboid_c[i] ) );
		quad_hex_top_normal = planetest( cuboid_a[i], quad_hex_top_normal, quad_hex_center) ? quad_hex_top_normal : ( quad_hex_top_normal * -1.0f );

		//BOTTOM - using BFD
		quad_hex_bottom_normal = normalize( cross( cuboid_b[i] - cuboid_f[i], cuboid_d[i] - cuboid_f[i] ) );
		quad_hex_bottom_normal = planetest( cuboid_b[i], quad_hex_bottom_normal, quad_hex_center) ? quad_hex_bottom_normal : ( quad_hex_bottom_normal * -1.0f );

		//LEFT - using FEA
		quad_hex_left_normal = normalize( cross( cuboid_f[i] - cuboid_e[i], cuboid_a[i] - cuboid_e[i] ) );
		quad_hex_left_normal = planetest( cuboid_f[i], quad_hex_left_normal, quad_hex_center) ? quad_hex_left_normal : ( quad_hex_left_normal * -1.0f );

		//RIGHT - using CGH
		quad_hex_right_normal = normalize( cross( cuboid_c[i] - cuboid_g[i], cuboid_h[i] - cuboid_g[i] ) );
		quad_hex_right_normal = planetest( cuboid_c[i], quad_hex_right_normal, quad_hex_center) ? quad_hex_right_normal : ( quad_hex_right_normal * -1.0f );

		//FRONT - using ABD
		quad_hex_front_normal = normalize( cross( cuboid_a[i] - cuboid_b[i], cuboid_d[i] - cuboid_b[i] ) );
		quad_hex_front_normal = planetest( cuboid_a[i], quad_hex_front_normal, quad_hex_center) ? quad_hex_front_normal : ( quad_hex_front_normal * -1.0f );

		//BACK - using GHF
		quad_hex_back_normal = normalize( cross( cuboid_g[i] - cuboid_h[i], cuboid_f[i] - cuboid_h[i] ) );
		quad_hex_back_normal = planetest( cuboid_g[i], quad_hex_back_normal, quad_hex_center) ? quad_hex_back_normal : ( quad_hex_back_normal * -1.0f );


		draw_cuboid[i] =  planetest(cuboid_a[i], quad_hex_top_normal, vPosition.xyz) &&
			planetest( cuboid_b[i], quad_hex_bottom_normal, vPosition.xyz) &&
			planetest( cuboid_f[i], quad_hex_left_normal, vPosition.xyz) &&
			planetest( cuboid_c[i], quad_hex_right_normal, vPosition.xyz) &&
			planetest( cuboid_a[i], quad_hex_front_normal, vPosition.xyz) &&
			planetest( cuboid_g[i], quad_hex_back_normal, vPosition.xyz);


		if(draw_cuboid[i])
		{
			how_many_being_drawn++;
			sum += cuboid_colors[i];
		}
	}


















//CYLINDERS

	//	 ,----.
	//	(   *  )
	//	|`----'|
	//	|      |
	//	|      |
	//	|      |
	//	|,----.|
	//	(   *  )
	//	 `----'

	//	two points in space are used to represent the centers of the two circular
	//		faces of the cylinder. A line is established between the two points -
	//		this line serves two functions -

	//	first, normals for the planes of the circular ends can be computed

	//	second, it's used to check the distance from the current vertex to the line

	//	if the vertex passes both tests (it is between the two planes and within
	//		the radius of the cylinder) it can be said to be inside the cylinder

	vec3 cylinder_tvec_normal;
	vec3 cylinder_bvec_normal;

	vec3 cylinder_center;


	for(int i = 0; i < NUM_CYLINDERS; i++)
	{
		cylinder_center = ( cylinder_bvec[i] + cylinder_tvec[i] ) / 2.0f;

		cylinder_tvec_normal = cylinder_bvec[i] - cylinder_tvec[i];
		cylinder_tvec_normal = planetest( cylinder_tvec[i], cylinder_tvec_normal, cylinder_center) ? cylinder_tvec_normal : (cylinder_tvec_normal * 1.0f);

		cylinder_bvec_normal = cylinder_bvec[i] - cylinder_tvec[i];
		cylinder_bvec_normal = planetest( cylinder_bvec[i], cylinder_bvec_normal, cylinder_center) ? cylinder_bvec_normal : (cylinder_bvec_normal * 1.0f);


		if( planetest(cylinder_bvec[i], cylinder_bvec_normal, vPosition.xyz) && planetest(cylinder_tvec[i], cylinder_tvec_normal, vPosition.xyz) )
		{

			if((length( cross( cylinder_tvec[i] - cylinder_bvec[i], cylinder_bvec[i] - vPosition.xyz ) ) / length( cylinder_tvec[i] - cylinder_bvec[i] )) < cylinder_radii[i])
			{
				//distance from point to line from http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html

				how_many_being_drawn++;
				sum += cylinder_colors[i];

			}
		}
	}







	if(how_many_being_drawn > 0)
	{// at least one shape is being drawn

		color = sum / how_many_being_drawn;

	}
	else
	{
		color = vec4(0.0f, 0.0f, 0.0f, 0.03f);
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
