#type vertex
#version 330 core

//Layouts
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

//Uniforms
uniform mat4 u_View;
uniform mat4 u_Proj;
uniform mat4 u_Model;

uniform float u_Time = 0.0;
uniform float u_MaxTime = 10.0;
uniform float u_Amplitude = 5.0f;
uniform float u_WaveLength = 1.0f;

//Variables
const float pi = 3.14159;

//Data sent to fragment shader (are they called "varying"? I think they are)
out float v_VertHeight;
out float v_MaxHeight;
out vec2 v_TexCoords;

void main()
{
	float height = u_Amplitude*sin(2.0*pi*((u_Time/u_MaxTime) - (a_Position.x/u_WaveLength)));
	v_VertHeight = height;
	v_MaxHeight = u_Amplitude;
	
	gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
	gl_Position.y += height;
	v_TexCoords = a_TexCoords;
}


#type fragment
#version 330 core

//Unused in shader (but here because the material sends the uniform, so for debug I'll let them here to not to redo many stuff)
uniform sampler2D u_AmbientTexture;
uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecularTexture;
uniform sampler2D u_NormalMap;
uniform sampler2D u_HeightMap;
uniform sampler2D u_LightMap;

uniform int u_TextureEmpty = 1;

//Uniforms
uniform vec4 u_AmbientColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform float u_ColorGradingOffset = 0.0f; //Keep it between 0 and 1
uniform sampler2D u_WaterTexture;

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

	color = texture(u_WaterTexture, v_TexCoords) * u_AmbientColor;
	//color.r *= 1.0 - (1.0 - (colorGrading + normalisedHeight));
	//color.g *= 1.0 - (1.0 - (colorGrading + normalisedHeight));
	color.r *= (colorGrading + normalisedHeight);
	color.g *= (colorGrading + normalisedHeight);

	//if(color.r <= 0.0)
	//	color.r = 0.2;
	//if(color.g <= 0.0)
	//	color.g = 0.2;
}