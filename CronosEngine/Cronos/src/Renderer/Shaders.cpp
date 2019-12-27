#include "Providers/cnpch.h"
#include "Shaders.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	GLenum StringToShaderType(const std::string& typeString)
	{
		if (typeString == "vertex")
			return GL_VERTEX_SHADER;
		if (typeString == "fragment" || typeString == "pixel")
			return GL_FRAGMENT_SHADER;

		CRONOS_ASSERT(false, "Invalid string Shader Type passed")
		return 0;
	}

	//Constructors
	Shader::Shader(const std::string& filepath)
	{
		std::string ShaderSourceCode = ReadFile(filepath);
		ShaderText = ShaderTextModifiable= ShaderSourceCode;
		Compile(PreProcess(ShaderSourceCode));
		m_Path = filepath;
	}

	Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		std::unordered_map<GLenum, std::string> SourceShaderUMap;
		SourceShaderUMap[GL_VERTEX_SHADER] = vertexShader;
		SourceShaderUMap[GL_FRAGMENT_SHADER] = fragmentShader;
		Compile(SourceShaderUMap);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ID);
	}

	//Shader Methods
	void Shader::Bind() const
	{
		glUseProgram(m_ID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	//Uniforms
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

	void Shader::SetUniformVec2f(const std::string& name, glm::vec2& vector2f)
	{
		glUniform2f(GetUniformLocation(name), vector2f.x, vector2f.y);
	}

	void Shader::SetUniformVec3f(const std::string& name, glm::vec3& vector3f)
	{
		glUniform3f(GetUniformLocation(name), vector3f.x, vector3f.y, vector3f.z);
	}

	void Shader::SetUniformVec4f(const std::string& name, glm::vec4& vector4f)
	{
		glUniform4f(GetUniformLocation(name), vector4f.x, vector4f.y, vector4f.z, vector4f.a);
	}

	void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& mat)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]); //glm::value_ptr(trans)
	}

	void Shader::SetUniform1i(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniform1f(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniformBool(const std::string& name, bool value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}
	
	//Shader Read & Creation (.glsl version)
	std::string Shader::ReadFile(const std::string & filepath)
	{
		std::string ret;

		std::ifstream in(filepath, std::ios::in, std::ios::binary);
		if (in)
		{
			//We seek for the end of file so we can set the returning string size
			in.seekg(0, std::ios::end);
			ret.resize(in.tellg());

			//We now seek for the file beginning to put it at the string (passing the string's first position and its size)
			in.seekg(0, std::ios::beg);
			in.read(&ret[0], ret.size());

			//Finally close the file
			in.close();
		}
		else
			CRONOS_WARN(false, "Couldn't open the file: " + filepath);

		return ret;
	}

	std::unordered_map<GLenum, std::string> Shader::PreProcess(const std::string & SourceShaderCode)
	{
		std::unordered_map<GLenum, std::string> ret;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = SourceShaderCode.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			size_t end_of_line = SourceShaderCode.find_first_of("\r\n", pos);
			CRONOS_ASSERT(end_of_line != std::string::npos, "Shader Syntax Error");

			size_t begin_pos = pos + typeTokenLength + 1;
			std::string shaderType = SourceShaderCode.substr(begin_pos, end_of_line - begin_pos);
			CRONOS_ASSERT(StringToShaderType(shaderType), "Invalid Shader Type");

			size_t next_line_pos = SourceShaderCode.find_first_not_of("\r\n", end_of_line);
			pos = SourceShaderCode.find(typeToken, next_line_pos);
			ret[StringToShaderType(shaderType)] = SourceShaderCode.substr(next_line_pos,
													pos - (next_line_pos == std::string::npos ? SourceShaderCode.size() - 1 : next_line_pos));
		}

		return ret;
	}

	void Shader::Compile(const std::unordered_map<GLenum, std::string>& ShadersUMap)
	{
		std::vector<GLenum> shaderIDs_vec(ShadersUMap.size());
		GLuint program = glCreateProgram();

		for (auto element : ShadersUMap)
		{
			GLenum shaderType = element.first;
			const std::string& shaderString = element.second;

			GLuint id = glCreateShader(shaderType);

			const GLchar* shaderSourceString = shaderString.c_str();
			glShaderSource(id, 1, &shaderSourceString, 0);
			glCompileShader(id);

			GLint result = 0;
			glGetShaderiv(id, GL_COMPILE_STATUS, &result);
			if (result == GL_FALSE)
			{
				GLint length = 0;
				glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
				
				std::vector<GLchar> message(length);
				glGetShaderInfoLog(id, length, &length, &message[0]);
				glDeleteShader(id);

				LOG("Error in Shader compilation: %s", message.data());
				CRONOS_ASSERT(false, "Shader Compilation Failure!");
				break;
			}

			glAttachShader(program, id);
			shaderIDs_vec.push_back(id);
		}

		m_ID = program;
		glLinkProgram(program);
		glValidateProgram(program);

		GLint result = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&result);
		if (result == GL_FALSE)
		{
			GLint length;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
			
			std::vector<GLchar> message(length);
			glGetProgramInfoLog(program, length, &length, &message[0]);
			
			glDeleteProgram(program);
			for (auto element : shaderIDs_vec)
				glDeleteShader(element);

			LOG("Error in Shader Program Linking: %s", message.data());
			CRONOS_ASSERT(false, "Shader Linking Failure!");
			return;
		}
		
		for (auto element : shaderIDs_vec)
		{
			glDetachShader(program, element);
			glDeleteShader(element);
		}
	}
}