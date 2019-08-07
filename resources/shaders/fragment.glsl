#version 450 core

in  vec4 color;
out vec4 fColor;

void main()
{

	//use gl_PointCoord used to find distance from the upper left corner of the point primitive

	float dist = distance(gl_PointCoord, vec2(0.5f, 0.5f));

	if(dist < 0.5f)
	{
  	fColor = color;
	}
	else
	{
		fColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}

}
