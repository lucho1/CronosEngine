#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_Model;

out vec2 v_TexCoords;

void main()
{
	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
	v_TexCoords = a_TexCoords;
}


#type fragment
#version 330 core

//Light Structure
struct Light
{
	vec3 LightPos;
	vec3 LightColor;
};

uniform Light u_Light;

in vec2 v_TexCoords;
out vec4 color;


//Object's colors and textures
uniform bool u_TextureEmpty = true;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
//uniform sampler2D u_NormalMap;
//uniform sampler2D u_HeightMap;
//uniform sampler2D u_LightMap;

uniform vec4 u_AmbientColor;

//ZBuffer rendering
uniform vec2 u_CamPlanes; //x for near plane, y for far plane
uniform bool u_drawZBuffer = false;

float LinearizeZ(float depth)
{
	float z = depth*2.0 - 1.0;
	return (2.0*u_CamPlanes.x*u_CamPlanes.y)/(u_CamPlanes.y + u_CamPlanes.x - z*(u_CamPlanes.y - u_CamPlanes.x));
}

//--------------------------------------------------------------------------------
void main()
{
	vec4 ambientColorLighted = vec4(u_Light.LightColor, 1.0);
	
	if (!u_TextureEmpty)
		color = (texture(u_DiffuseTexture, v_TexCoords)) * u_AmbientColor;
	else
		color = u_AmbientColor;

	if (u_drawZBuffer)
	{
		float depth = (LinearizeZ(gl_FragCoord.z) / u_CamPlanes.y);
		color = vec4(vec3(depth), 1.0);
	}
}



		