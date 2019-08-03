#version 450 core

// layout(location = 0) in vec4 vColor;
layout(location = 1) in vec3 vPosition;

layout(location = 2) uniform mat4 view;
layout(location = 3) uniform mat4 rotation;

out vec4 color;




//DEFINES

#define RENDER_HEIGHTMAP	false		//renders a heightmap held in a texture

#define RENDER_SPHERES 		true		//renders a list of spheres
#define RENDER_TUBES			true		//renders a list of tubes - (a cylinder with an inner radius)
#define RENDER_CYLINDERS 	true		//renders a list of cylinders
#define RENDER_TRIANGLES 	true		//renders a list of triangles
#define RENDER_QUAD_HEX 	true		//renders a list of cuboids



//eventually I want to do this as a set of compute shaders, one for each of these primitives or maybe batches of these primitives

//	the manual config on the numbers is a bit of a pain right now, but for-loops
//	of a variable size are illegal in GLSL code. Also have to keep a separate
//	bool (above) telling whether or not to render because it does not allow for
//	the use of a zero value for the number of iterations of a for loop

#define NUM_SPHERES   1						//how long is the list of spheres?
#define NUM_TUBES			1					//how long is the list of tubes?
#define NUM_CYLINDERS 1					//how long is the list of cylinders?
#define NUM_TRIANGLES 1						//how long is the list of triangles?
#define NUM_QUAD_HEXS 2					//how long is the list of cuboids?





//SPHERE VALUES
uniform vec3 sphere_center[NUM_SPHERES];
uniform float sphere_radius[NUM_SPHERES];

uniform vec4 sphere_colors[NUM_SPHERES];





//CYLINDER VALUES
uniform vec3 cylinder_tvec[NUM_CYLINDERS];
uniform vec3 cylinder_bvec[NUM_CYLINDERS];

uniform float cylinder_radii[NUM_CYLINDERS];

uniform vec4 cylinder_colors[NUM_CYLINDERS];

uniform vec3 cylinder_offsets[NUM_CYLINDERS];




//TUBE VALUES
uniform vec3 tube_tvec[NUM_TUBES];
uniform vec3 tube_bvec[NUM_TUBES];

uniform float tube_inner_radii[NUM_TUBES];
uniform float tube_outer_radii[NUM_TUBES];

uniform vec4 tube_colors[NUM_TUBES];

uniform vec3 tube_offsets[NUM_TUBES];




//TRIANGLE VALUES
uniform vec3 triangle_point1[NUM_TRIANGLES];
uniform vec3 triangle_point2[NUM_TRIANGLES];
uniform vec3 triangle_point3[NUM_TRIANGLES];

uniform float triangle_thickness[NUM_TRIANGLES];

uniform vec4 triangle_colors[NUM_TRIANGLES];

uniform vec3 triangle_offsets[NUM_TRIANGLES];






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

uniform vec3 cuboid_offsets[NUM_QUAD_HEXS];















// int how_many_being_drawn = 0;
// vec4 sum = vec4(0.0f, 0.0f, 0.0f, 0.0f);





uniform sampler2D ourTexture;



bool planetest(vec3 plane_point, vec3 plane_normal, vec3 test_point);



void main()
{
  gl_Position = view * rotation * vec4(vPosition, 1.0f);
	// This is the same, regardless of anything that happens subsequently

	color = vec4(0.0f, 0.0f, 0.0f, 0.0f);







	if(RENDER_SPHERES)
	{
		for(int i = 0; i < NUM_SPHERES; i++)
		{
			if( distance( vPosition.xyz, sphere_center[i]) < sphere_radius[i])
			{
				// how_many_being_drawn++;
				// sum += sphere_colors[i];

				color = sphere_colors[i];

			}
		}
	}











	//HEIGHTMAP
	if(RENDER_HEIGHTMAP)
	{
		float height = texture( ourTexture, 1.5 * (vPosition.yz + vec2( 0.35f, 0.35f ) ) ).r;

		if(height != 0)
		{
			if(vPosition.x + 0.30 < height * 0.05 && vPosition.x > -0.34)
			{
				color = vec4( height, 0.2f, 0.1f, 0.7f);

				// how_many_being_drawn++;
				// sum += vec4( height, 0.2f, 0.1f, 0.7f);

			}
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


		if(RENDER_CYLINDERS)
		{

			vec3 cylinder_tvec_normal;
			vec3 cylinder_bvec_normal;

			vec3 cylinder_center;

			vec3 bvec_local, tvec_local;


			for(int i = 0; i < NUM_CYLINDERS; i++)
			{
				bvec_local = cylinder_bvec[i] + cylinder_offsets[i];
				tvec_local = cylinder_tvec[i] + cylinder_offsets[i];


				cylinder_center = ( bvec_local + tvec_local ) / 2.0f;

				cylinder_tvec_normal = bvec_local - tvec_local;
				cylinder_tvec_normal = planetest( tvec_local, cylinder_tvec_normal, cylinder_center) ? cylinder_tvec_normal : (cylinder_tvec_normal * -1.0f);

				cylinder_bvec_normal = bvec_local - tvec_local;
				cylinder_bvec_normal = planetest( bvec_local, cylinder_bvec_normal, cylinder_center) ? cylinder_bvec_normal : (cylinder_bvec_normal * -1.0f);


				if( planetest(bvec_local, cylinder_bvec_normal, vPosition.xyz) && planetest(tvec_local, cylinder_tvec_normal, vPosition.xyz) )
				{

					if((length( cross( tvec_local - bvec_local, bvec_local - vPosition.xyz ) ) / length( tvec_local - bvec_local )) < cylinder_radii[i])
					{
						//distance from point to line from http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html

						// how_many_being_drawn++;
						// sum += cylinder_colors[i];

						color = cylinder_colors[i];

						// if(i < 30)
						// 	color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

					}
				}
			}
		}



	//TUBES

		//	 ,----.
		//	(   *  )
		//	|`----'|
		//	|      |
		//	|      |
		//	|      |
		//	|,----.|
		//	(   *  )
		//	 `----'

		// TUBES ARE JUST CYLINDERS THAT HAVE AN INNER RADIUS



		if(RENDER_TUBES)
		{

			vec3 tube_tvec_normal;
			vec3 tube_bvec_normal;

			vec3 tube_center;

			vec3 bvec_local, tvec_local;


			for(int i = 0; i < NUM_TUBES; i++)
			{
				bvec_local = tube_bvec[i] + tube_offsets[i];
				tvec_local = tube_tvec[i] + tube_offsets[i];


				tube_center = ( bvec_local + tvec_local ) / 2.0f;

				tube_tvec_normal = bvec_local - tvec_local;
				tube_tvec_normal = planetest( tvec_local, tube_tvec_normal, tube_center) ? tube_tvec_normal : (tube_tvec_normal * -1.0f);

				tube_bvec_normal = bvec_local - tvec_local;
				tube_bvec_normal = planetest( bvec_local, tube_bvec_normal, tube_center) ? tube_bvec_normal : (tube_bvec_normal * -1.0f);


				if( planetest(bvec_local, tube_bvec_normal, vPosition.xyz) && planetest(tvec_local, tube_tvec_normal, vPosition.xyz) )
				{

					float point_distance = length( cross( tvec_local - bvec_local, bvec_local - vPosition.xyz ) ) / length( tvec_local - bvec_local );

					if(point_distance < tube_outer_radii[i] && point_distance > tube_inner_radii[i])
					{
						//distance from point to line from http://mathworld.wolfram.com/Point-LineDistance3-Dimensional.html

						// how_many_being_drawn++;
						// sum += cylinder_colors[i];


						color = tube_colors[i];



					}
				}
			}
		}



		//TRIANGLE

		// to aleviate the redundancy of computing all the cross products, etc, for every point,
		// the normals are computed CPU-side and sent in as uniform variables then the value of
		// vPosition is checked against 5 different planes which represent the top, bottom, and
		// three sides of the triangle - if it is 'below' all 5 planes, the point can be said to
		// be 'inside' the triangle.

		// TRIANGLES (TRIANGULAR PRISM WITH ADJUSTABLE THICKNESS)




		if(RENDER_TRIANGLES)
		{
			vec3 calculated_triangle_center;
			vec3 calculated_top_normal;
			vec3 calculated_side_1_2_normal;
			vec3 calculated_side_2_3_normal;
			vec3 calculated_side_3_1_normal;

			bool draw_triangles[NUM_TRIANGLES];





			for(int i = 0; i < NUM_TRIANGLES; i++)
			{

				vec3 point1_local = triangle_point1[i] + triangle_offsets[i];
				vec3 point2_local = triangle_point2[i] + triangle_offsets[i];
				vec3 point3_local = triangle_point3[i] + triangle_offsets[i];



				//calculate the center of the triangle
				calculated_triangle_center = ( point1_local + point2_local + point3_local ) / 3.0f;


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

				calculated_top_normal = normalize( cross( point1_local - point2_local, point1_local - point3_local ) );
				calculated_top_normal = planetest( point1_local + triangle_thickness[i] * calculated_top_normal, calculated_top_normal, calculated_triangle_center ) ? calculated_top_normal : ( calculated_top_normal * -1.0f );

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

				calculated_side_1_2_normal = normalize( cross( calculated_top_normal, point2_local - point1_local ) );
				calculated_side_1_2_normal = planetest( point1_local, calculated_side_1_2_normal, calculated_triangle_center) ? calculated_side_1_2_normal : ( calculated_side_1_2_normal * -1.0f );

				calculated_side_2_3_normal = normalize( cross( calculated_top_normal, point3_local - point2_local ) );
				calculated_side_2_3_normal = planetest( point2_local, calculated_side_2_3_normal, calculated_triangle_center) ? calculated_side_2_3_normal : ( calculated_side_2_3_normal * -1.0f );

				calculated_side_3_1_normal = normalize( cross( calculated_top_normal, point1_local - point3_local ) );
				calculated_side_3_1_normal = planetest( point3_local, calculated_side_3_1_normal, calculated_triangle_center) ? calculated_side_3_1_normal : ( calculated_side_3_1_normal * -1.0f );


				// do the tests - for each of the normals, top, bottom, and the three sides,
				//	use the planetest function to determine whether the current point is
				//	'below' all 5 planes - if it is, it is inside this triangular prism


				draw_triangles[i] = planetest( point1_local + ( triangle_thickness[i] / 2.0f ) * calculated_top_normal, calculated_top_normal, vPosition.xyz ) &&
				planetest( point1_local - ( triangle_thickness[i] / 2.0f ) * calculated_top_normal, -1.0f * calculated_top_normal, vPosition.xyz ) &&
				planetest( point1_local, calculated_side_1_2_normal, vPosition.xyz ) &&
				planetest( point2_local, calculated_side_2_3_normal, vPosition.xyz ) &&
				planetest( point3_local, calculated_side_3_1_normal, vPosition.xyz );

				if(draw_triangles[i])
				{
					// how_many_being_drawn++;
					// sum += vec4(1.0f, 1.0f, 1.0f, 1.0f); //triangle_colors[i];

					color = triangle_colors[i];

				}
			}
		}





//QUADRILATERAL HEXAHEDRON (CUBOID)

	// 	point location reference - it can be rotated, but the faces must be
	//		ACEG, CGDH, EGFH, ABEF, ABCD and BDFH - axes just for reference
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

	//		the algorithm is very similar to the one used for the triangle - I have
	//			seen it said that it can be generalized to any convex polyhedron -
	//			concave shapes do not work as there are areas that should be within the
	//			shape that will not pass all the requisite plane tests, which will
	//			exclude some of the area that should lie within the shape






	if(RENDER_QUAD_HEX)
	{

		vec3 quad_hex_center;

		vec3 quad_hex_top_normal;
		vec3 quad_hex_bottom_normal;
		vec3 quad_hex_left_normal;
		vec3 quad_hex_right_normal;
		vec3 quad_hex_front_normal;
		vec3 quad_hex_back_normal;

		vec3 a_local, b_local, c_local, d_local, e_local, f_local, g_local, h_local;

		bool draw_cuboid[NUM_QUAD_HEXS];


		for(int i = 0; i < NUM_QUAD_HEXS; i++)
		{

			a_local = cuboid_a[i] + cuboid_offsets[i];
			b_local = cuboid_b[i] + cuboid_offsets[i];
			c_local = cuboid_c[i] + cuboid_offsets[i];
			d_local = cuboid_d[i] + cuboid_offsets[i];

			e_local = cuboid_e[i] + cuboid_offsets[i];
			f_local = cuboid_f[i] + cuboid_offsets[i];
			g_local = cuboid_g[i] + cuboid_offsets[i];
			h_local = cuboid_h[i] + cuboid_offsets[i];


			quad_hex_center = (a_local + b_local + c_local + d_local + e_local + f_local + g_local + h_local) / 8.0f;

			// "TOP" (ACEG) - using ACE
			quad_hex_top_normal = normalize( cross( a_local - c_local, e_local - c_local ) );
			quad_hex_top_normal = planetest( a_local, quad_hex_top_normal, quad_hex_center) ? quad_hex_top_normal : ( quad_hex_top_normal * -1.0f );

			// "BOTTOM" (BDFH) - using BFD
			quad_hex_bottom_normal = normalize( cross( b_local - f_local, d_local - f_local ) );
			quad_hex_bottom_normal = planetest( b_local, quad_hex_bottom_normal, quad_hex_center) ? quad_hex_bottom_normal : ( quad_hex_bottom_normal * -1.0f );

			// "LEFT" (ABEF) - using FEA
			quad_hex_left_normal = normalize( cross( f_local - e_local, a_local - e_local ) );
			quad_hex_left_normal = planetest( f_local, quad_hex_left_normal, quad_hex_center) ? quad_hex_left_normal : ( quad_hex_left_normal * -1.0f );

			// "RIGHT" (CDGH) - using CGH
			quad_hex_right_normal = normalize( cross( c_local - g_local, h_local - g_local ) );
			quad_hex_right_normal = planetest( c_local, quad_hex_right_normal, quad_hex_center) ? quad_hex_right_normal : ( quad_hex_right_normal * -1.0f );

			// "FRONT" (ABCD) - using ABD
			quad_hex_front_normal = normalize( cross( a_local - b_local, d_local - b_local ) );
			quad_hex_front_normal = planetest( a_local, quad_hex_front_normal, quad_hex_center) ? quad_hex_front_normal : ( quad_hex_front_normal * -1.0f );

			// "BACK" (EFGH) - using GHF
			quad_hex_back_normal = normalize( cross( g_local - h_local, f_local - h_local ) );
			quad_hex_back_normal = planetest( g_local, quad_hex_back_normal, quad_hex_center) ? quad_hex_back_normal : ( quad_hex_back_normal * -1.0f );


			draw_cuboid[i] =  planetest(a_local, quad_hex_top_normal, vPosition.xyz) &&
				planetest( b_local, quad_hex_bottom_normal, vPosition.xyz) &&
				planetest( f_local, quad_hex_left_normal, vPosition.xyz) &&
				planetest( c_local, quad_hex_right_normal, vPosition.xyz) &&
				planetest( a_local, quad_hex_front_normal, vPosition.xyz) &&
				planetest( g_local, quad_hex_back_normal, vPosition.xyz);


			if(draw_cuboid[i])
			{
				// how_many_being_drawn++;
				// sum += cuboid_colors[i];

				color = cuboid_colors[i];

				// if(i < 14)
				// 	color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			}
		}
	}

























	// if(how_many_being_drawn > 0)
	// {// at least one shape is being drawn
	//
	// 	// color = sum / how_many_being_drawn;
	//
	// }
	// else
	// {
	// 	color = vec4(0.0f, 0.0f, 0.0f, 0.003f);
	// }
}











bool planetest(vec3 plane_point, vec3 plane_normal, vec3 test_point)
{
	// Determines whether a point is above or below a plane

  //		return false if the point is above the plane
	//		return true if the point is below the plane

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
