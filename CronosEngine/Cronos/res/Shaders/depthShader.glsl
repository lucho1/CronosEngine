#type vertex
#version 430 core

//Vertex Layouts
layout(location = 0) in vec3 a_Position;

//Uniforms
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main()
{
	gl_Position = u_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
}


#type fragment
#version 430 core

void main()
{
}		