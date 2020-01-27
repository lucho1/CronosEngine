#type vertex
#version 420 core

//Layouts
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

//Uniforms
layout(std140, binding = 0) uniform u_CameraData
{
	mat4 u_View;
	mat4 u_Proj;
};

uniform mat4 u_Model;

uniform float u_Time = 0.0;
uniform float u_MaxTime = 10.0;
uniform float u_Amplitude = 5.0;
uniform float u_WaveLength = 1.0;
uniform float u_Velocity = 10.0;
uniform float u_FOAMVelocity = 0.5;
uniform vec2 u_FOAMDirection = vec2(0, 0);
uniform float u_WaveMovementMultiplicator = 2.0;

//Variables
const float pi = 3.14159;

//Data sent to fragment shader (are they called "varying"? I think they are)
out float v_VertHeight;
out float v_MaxHeight;
out vec2 v_TexCoords;

float random(vec2 p)
{
     vec2 K1 = vec2(23.14069263277926, 2.665144142690225);

   //return fract( cos( mod( 12345678., 256. * dot(p,K1) ) ) ); // ver1
     return fract(cos(dot(p,K1)) * 12345.6789); // ver3
}

float random2(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	float height = u_Amplitude*sin(2.0*pi*((u_Time/u_MaxTime)*u_Velocity - (a_Position.x/u_WaveLength)));
	float dX = sin(2.0*a_Position.x + u_Time);
	float dZ = sin(2.0*a_Position.y + u_Time);
	//float Xf = a_Position.x/u_WaveLength - 0.5;
	//float Yf = a_Position.y/u_Amplitude - 0.5;
	//
	//vec2 vec = vec2(Xf, Yf);
	//float height = random2(vec) * 2.0*pi*(u_Time/u_MaxTime);
	vec2 tCoordsDir = clamp(normalize(u_FOAMDirection), 0.0, 1.0);

	vec2 tCoords = a_TexCoords;
	tCoords.x -= u_FOAMVelocity * u_Time/u_MaxTime * tCoordsDir.x;
	tCoords.y -= u_FOAMVelocity * u_Time/u_MaxTime * tCoordsDir.y;

	v_VertHeight = height;
	v_MaxHeight = u_Amplitude;	
	v_TexCoords = tCoords;

	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position.x, a_Position.y, a_Position.z + dX*dZ*u_WaveMovementMultiplicator, 1.0);
	gl_Position.y += height;
}


#type fragment
#version 420 core

//Uniforms
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;

uniform int u_TextureEmpty = 1;

//Uniforms
uniform vec4 u_AmbientColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform float u_ColorGradingOffset = 0.0f; //Keep it between 0 and 1

//Data sent from vertex shader
in float v_VertHeight;
in float v_MaxHeight;
in vec2 v_TexCoords;

//Color output
out vec4 color;

void main()
{
	float normalisedHeight = v_VertHeight/(v_MaxHeight + 2.0);
	float colorGrading = u_ColorGradingOffset;

	if(colorGrading >= 1.0)
		colorGrading = 1.0;
	if(colorGrading <= 0.0)
		colorGrading = 0.0;

	color = texture(u_DiffuseTexture, v_TexCoords) * u_AmbientColor + texture(u_SpecularTexture, v_TexCoords) * 0.2;
	color.r *= (colorGrading + normalisedHeight);
	color.g *= (colorGrading + normalisedHeight);
}