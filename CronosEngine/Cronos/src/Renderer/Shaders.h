#ifndef _SHADERS_H_
#define _SHADERS_H_

#include <glm/glm.hpp>

namespace Cronos
{
	struct ShaderProgram
	{
		std::string vertexShader;
		std::string fragmentShader;
	};

	class Shader
	{
	public:

		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetUniformVec4f(const std::string& name, glm::vec4& vector4f);
		void SetUniformVec3f(const std::string& name, glm::vec3& vector3f);
		void SetUniformVec2f(const std::string& name, glm::vec2& vector2f);
		void SetUniformMat4f(const std::string &name, const glm::mat4& mat);
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);

	private:

		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& SourceShaderCode);
		void Compile(const std::unordered_map<GLenum, std::string>& ShadersUMap);

		//ShaderProgram ParseShader(const std::string filepath);
		//uint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		//uint CompileShader(uint type, const std::string& source);

		int GetUniformLocation(const std::string& name);

	private:

		uint m_ID;
		std::string m_Path;
		std::unordered_map<std::string, int> m_UniformLocationCache;
	};
}

#endif