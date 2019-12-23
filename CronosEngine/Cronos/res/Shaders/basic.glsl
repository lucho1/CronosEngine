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
	v_TexCoords = a_TexCoords;
	v_CamPos = u_CameraPosition;
	v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
	v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;

	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
}


#type fragment
#version 330 core

float randomNum(float num) { return fract(sin(dot(vec2(num), vec2(12.9898,78.233))) * 43758.5453); }

uniform float u_ShaderPlaybackTime;
int m_AssertTimes = 0;
vec4 AssertColorOutput()
{
	//This is basically to set a blinking output color to denote an assertion on shader run time.
	vec4 blinkColor = vec4(1.0);
	blinkColor = vec4(randomNum(u_ShaderPlaybackTime), randomNum(u_ShaderPlaybackTime/2), randomNum(u_ShaderPlaybackTime/3), 1.0);
	return (mod(u_ShaderPlaybackTime, 1.0) > 0.5) ? blinkColor : vec4(1.0);
}

#define CNSH_ASSERT(condition) while(!condition) { ++m_AssertTimes; break; }
#define COLOROUTPUT_EXIT() while(m_AssertTimes != 0) { color = AssertColorOutput(); break; }

#define MAX_POINTLIGHTS 165
#define MAX_DIRLIGHTS 8

//Input variables
in vec2 v_TexCoords;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec3 v_CamPos;

//Output variables
out vec4 color;


//Light Structure
struct DirLight 
{
	vec3 LightDir;
	vec3 LightColor;

	float LightIntensity;
};

uniform DirLight u_DirLightsArray[MAX_DIRLIGHTS] = DirLight[MAX_DIRLIGHTS](DirLight(vec3(0), vec3(1), 0.0));
uniform int u_CurrentDirLights = 0;

struct PointLight
{
	vec3 LightPos;
	vec3 LightColor;

	float LightIntensity;

	float LightAtt_K;
	float LightAtt_L;
	float LightAtt_Q;
};

uniform PointLight u_PointLightsArray[MAX_POINTLIGHTS] = PointLight[MAX_POINTLIGHTS](PointLight(vec3(0), vec3(1), 0.0, 1.0, 0.09, 0.032));
uniform int u_CurrentPointLights = 0;

//Material Stuff
uniform vec4 u_AmbientColor;
uniform float u_Shininess = 32.0;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
//uniform sampler2D u_NormalMap;

uniform bool u_TextureEmpty = true;

//ZBuffer rendering -------------------------------------------------------------------------------------------------
uniform vec2 u_CamPlanes; //x for near plane, y for far plane
uniform bool u_drawZBuffer = false;

float LinearizeZ(float depth)
{
	float z = depth*2.0 - 1.0;
	return (2.0*u_CamPlanes.x*u_CamPlanes.y)/(u_CamPlanes.y + u_CamPlanes.x - z*(u_CamPlanes.y - u_CamPlanes.x));
}

//Light Calculations -----------------------------------------------------------------------------------------------
vec4 CalculateLightResult(bool hasTextures, vec4 LColor, float diff, float spec)
{
	//Result
	if(hasTextures)
	{
		vec4 ambient = LColor * texture(u_DiffuseTexture, v_TexCoords) * u_AmbientColor;
		vec4 diffuse = diff * texture(u_DiffuseTexture, v_TexCoords);
		vec4 specular = spec * texture(u_SpecularTexture, v_TexCoords);

		return (ambient + diffuse + specular);
	}
	else
	{
		vec4 ambient = LColor * u_AmbientColor;
		vec4 diffuse = LColor * diff;
		vec4 specular = LColor * spec;

		return (ambient + diffuse + specular);
	}
}

//Dir Light Calculation
vec4 CalculateDirectionalLight(DirLight dLight, vec3 normal, vec3 viewDirection, bool hasTextures)
{
	vec3 lightDir = normalize(dLight.LightDir);
	
	//Diffuse Component
	float diffImpact = max(dot(normal, lightDir), 0.0);
	
	//Specular component
	vec3 reflectDirection = reflect(-lightDir, normal);
	float specImpact = pow(max(dot(viewDirection, reflectDirection), 0.0), u_Shininess);

	//Result
	vec4 LightRes = CalculateLightResult(hasTextures, vec4(dLight.LightColor, 1.0), diffImpact, specImpact);
	return (LightRes * dLight.LightIntensity);
}

//Point Light Calculation
vec4 CalculatePointLight(PointLight pLight, vec3 normal, vec3 FragPos, vec3 viewDirection, bool hasTextures)
{
	vec3 lightDir = normalize(pLight.LightPos - FragPos);

	//Diffuse Component
	float diffImpact = max(dot(normal, lightDir), 0.0);

	//Specular Component
	vec3 reflectDirection = reflect(-lightDir, normal);
	float specImpact = pow(max(dot(viewDirection, reflectDirection), 0.0), u_Shininess);

	//Attenuation Calculation
	float d = length(pLight.LightPos - FragPos);
	float lightAttenuation = 1.0/ (pLight.LightAtt_K + pLight.LightAtt_L * d + pLight.LightAtt_Q *(d * d));

	//Result
	vec4 LightRes = CalculateLightResult(hasTextures, vec4(pLight.LightColor, 1.0), diffImpact, specImpact);
	return (LightRes * lightAttenuation * pLight.LightIntensity);
}

//------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------
void main()
{
	if (u_drawZBuffer)
	{
		float depth = (LinearizeZ(gl_FragCoord.z) / u_CamPlanes.y);
		color = vec4(vec3(depth), 1.0);
	}
	else
	{
		CNSH_ASSERT((u_CurrentPointLights <= MAX_POINTLIGHTS && u_CurrentPointLights >= 0));
		CNSH_ASSERT((u_CurrentDirLights <= MAX_DIRLIGHTS && u_CurrentDirLights >= 0));

		//Generic Light Calculations
		vec3 normalVec = normalize(v_Normal);
		vec3 viewDirection = normalize(v_CamPos - v_FragPos);

		//Color Output
		vec4 colorOutput = vec4(vec3(0.0), 1.0);

		for(int i = 0; i < u_CurrentDirLights; ++i)
			colorOutput += CalculateDirectionalLight(u_DirLightsArray[i], normalVec, viewDirection, !u_TextureEmpty);		

		for(int i = 0; i < u_CurrentPointLights; ++i)
			colorOutput += CalculatePointLight(u_PointLightsArray[i], normalVec, v_FragPos, viewDirection, !u_TextureEmpty);

		color = colorOutput;
		COLOROUTPUT_EXIT();
	}
}		