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

	int Shader::GetUniformLocation(const std::string & name)
	{
		if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
			return m_UniformLocationCache[name];

		int loc = glGetUniformLocation(m_ID, name.c_str());

		if (loc == -1)
			LOG("Warning! Uniform '%s' doesn't exist! (loc == -1)", name.c_str());

		m_UniformLocationCache[name] = loc;
		return loc;
	}

	void Shader::SetUniformVec3f(const std::string& name, glm::vec3& vector3f)
	{
		glUniform3f(GetUniformLocation(name), vector3f.x, vector3f.y, vector3f.z);
	}

	void Shader::SetUniformVec2f(const std::string& name, glm::vec2& vector2f)
	{
		glUniform2f(GetUniformLocation(name), vector2f.x, vector2f.y);
	}

	void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& mat)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]); //glm::value_ptr(trans)
	}

	void Shader::SetUniform1i(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}


	//Shader Creation
	uint Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		uint program = glCreateProgram();
		uint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		uint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);
		glValidateProgram(program);

		int result;
		glGetProgramiv(program, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE) {

			int length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
			char *message = (char*)alloca(length * sizeof(char));

			glGetShaderInfoLog(program, length, &length, message);
			LOG("Error in Shader Program Linking: %s", message);
		}

		glDetachShader(program, vs);
		glDetachShader(program, fs);
		glDeleteShader(vs);
		glDeleteShader(fs);

		if (result == GL_FALSE)
		{
			glDeleteProgram(program);
			return 0;
		}

		return program;
	}

	uint Shader::CompileShader(uint type, const std::string& source)
	{
		uint id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);

		if (result == GL_FALSE) {

			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char *message = (char*)alloca(length * sizeof(char));

			glGetShaderInfoLog(id, length, &length, message);
			LOG("Error in %s Shader compilation: %s", (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment"), message);
			glDeleteShader(id);
			return 0;
		}

		return id;
	}
}