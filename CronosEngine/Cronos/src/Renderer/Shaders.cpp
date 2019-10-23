#include "Providers/cnpch.h"
#include "Shaders.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		m_ID = CreateShader(vertexShader, fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_ID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::SetUniformVec3f(const std::string& name, glm::vec3& vector3f)
	{
		glUniform3f(glGetUniformLocation(m_ID, name.c_str()), vector3f.x, vector3f.y, vector3f.z);
	}

	void Shader::SetUniformMat4f(const std::string& name, const mat4x4& mat)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat/*&mat[0][0]*/);
	}

	void Shader::SetUniform1i(const std::string & name, int value)
	{
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	uint Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		//Vertex
		uint vShader = glCreateShader(GL_VERTEX_SHADER);
		const char* sSource = vertexShader.c_str();
		glShaderSource(vShader, 1, &sSource, NULL);
		glCompileShader(vShader);

		int success;
		char info[512];
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vShader, 512, NULL, info);
			LOG("Error in VShader compilation: ", info);
		}

		//Fragment
		uint fShader = glCreateShader(GL_FRAGMENT_SHADER);
		sSource = fragmentShader.c_str();
		glShaderSource(fShader, 1, &sSource, NULL);
		glCompileShader(fShader);
		
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fShader, 512, NULL, info);
			LOG("Error in FShader compilation: ", info);
		}

		
		//Shader Link
		uint ShaderProgram = glCreateProgram();
		glAttachShader(ShaderProgram, vShader);
		glAttachShader(ShaderProgram, fShader);
		glLinkProgram(ShaderProgram);

		glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(ShaderProgram, 512, NULL, info);
			LOG("Error in Shader Linking: ", info);
		}

		glDeleteShader(vShader);
		glDeleteShader(fShader);

		return ShaderProgram;
	}
}