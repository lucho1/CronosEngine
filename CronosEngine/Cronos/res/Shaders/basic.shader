#shader vertex
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


#shader fragment
#version 330 core

out vec4 color;
in vec2 v_TexCoords;

uniform sampler2D u_AmbientTexture;
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_NormalMap;
uniform sampler2D u_HeightMap;
uniform sampler2D u_LightMap;

uniform vec4 u_AmbientColor = vec4(1.0,1.0,1.0,1.0);

uniform int u_TextureEmpty = 1;

uniform vec2 u_CamPlanes; //x for near plane, y for far plane
uniform int u_drawZBuffer = 0;

float LinearizeZ(float depth)
{
	float z = depth*2.0 - 1.0;
	return (2.0*u_CamPlanes.x*u_CamPlanes.y)/(u_CamPlanes.y + u_CamPlanes.x - z*(u_CamPlanes.y - u_CamPlanes.x));
}

void main()
{
	vec4 texColor = vec4(0.8, 0.8, 0.8, 1.0);
	if (u_TextureEmpty == 0)
	{
		texColor = (texture(u_DiffuseTexture, v_TexCoords)) * u_AmbientColor;
		color = texColor;
	}
	else
	{
		color = u_AmbientColor;
	}

	if (u_drawZBuffer == 1)
	{
		float depth = (LinearizeZ(gl_FragCoord.z) / u_CamPlanes.y);
		color = vec4(vec3(depth), 1.0);
	}
}
		