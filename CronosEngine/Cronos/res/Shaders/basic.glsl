#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_Model;

out vec2 v_TexCoords;
out vec3 v_Normal;
out vec3 v_FragPos;

void main()
{
	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
	v_TexCoords = a_TexCoords;
	v_Normal = a_Normal;
	v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
}


#type fragment
#version 330 core

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
out vec4 color;

//Light Structure
struct Light
{
	vec3 LightPos;
	vec3 LightColor;
};

uniform Light u_Light = Light(vec3(0), vec3(1));

//Object's colors and textures
uniform bool u_TextureEmpty = true;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
//uniform sampler2D u_NormalMap;

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
	//Lighting calculations ------------------------------------
	//Diffuse light calculation
	vec3 normalVec = normalize(v_Normal);
	vec3 lightDir = normalize(u_Light.LightPos - v_FragPos);
	float diffImpact = max(dot(normalVec, lightDir), 0.0);
	vec3 diffuseLight = diffImpact * u_Light.LightColor;
	//----------------------------------------------------------

	vec4 LightResult = vec4(u_Light.LightColor + diffuseLight, 1.0) * u_AmbientColor;
	
	
	if (!u_TextureEmpty)
		color = (texture(u_DiffuseTexture, v_TexCoords)) * LightResult;
	else
		color = LightResult;

	if (u_drawZBuffer)
	{
		float depth = (LinearizeZ(gl_FragCoord.z) / u_CamPlanes.y);
		color = vec4(vec3(depth), 1.0);
	}
}		