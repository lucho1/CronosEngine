#type vertex
#version 330 core

//Layout variables
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

//Uniform variables
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_Model;

//------------------------------------------------------------------------------------------------------------------
void main()
{
	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
}


#type fragment
#version 330 core


//------------------------------------------------------------------------------------------------------------------
void main()
{
	gl_FragColor = vec4(1.0);
}		