#shader vertex
#version 330 core

layout(location = 0) in vec4 position;	// casting vec2** to vec4 since glPosition is a vec4 

void main()
{
	gl_Position = position;
};


#shader fragment
#version 330 core

out vec4 colour;
uniform vec4 u_Colour;	// the u_ indicates that this is a uniform

void main()
{
	colour = u_Colour;	// vec4(1.0, 1.0, 0.0, 1.0);	// RGBA, RED
};
