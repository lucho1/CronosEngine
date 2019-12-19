#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform vec3 u_CameraPosition;
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_Model;

out vec2 v_TexCoords;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec3 v_CamPos;

void main()
{
	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
	v_TexCoords = a_TexCoords;
	v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
	v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	v_CamPos = u_CameraPosition;
}


#type fragment
#version 330 core

in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec3 v_CamPos;
out vec4 color;

//Light Structure
struct Light
{
	vec3 LightDir;
	vec3 LightPos;
	vec3 LightColor;

	float LightIntensity;
};

uniform Light u_Light = Light(vec3(1), vec3(0), vec3(1), 0.5);

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
	vec3 normalVec = normalize(v_Normal);
	vec3 lightDir = normalize(u_Light.LightPos - v_FragPos);
	//vec3 lightDir = normalize(u_Light.LightDir);

	//Diffuse light calculation
	float diffImpact = max(dot(normalVec, lightDir), 0.0);
	vec3 diffuseLight = diffImpact * u_Light.LightColor;

	//Specular Light calculation
	vec3 viewDirection = normalize(v_CamPos - v_FragPos);
	vec3 reflectDirection = reflect(-lightDir, normalVec);
	float specImpact = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
	vec3 specularLight = u_Light.LightIntensity * specImpact * u_Light.LightColor;
	//----------------------------------------------------------

	vec4 LightResult = vec4(u_Light.LightColor + diffuseLight + specularLight, 1.0) * u_AmbientColor;
	
	
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