#version 450 core

in  vec4 color;
out vec4 fColor;

void main()
{

	//use gl_PointCoord used to find distance from the upper left corner of the point primitive

	if(color.a == 0)
	{
  	fColor = color;
	}
	else
	{
		fColor = vec4(color.rgb, color.a *  (1.0f - pow(distance( vec2(0.5f, 0.5f), gl_PointCoord), 2.0f)));
	}

}
