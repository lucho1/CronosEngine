#ifndef _SHADERS_H_
#define _SHADERS_H_

#include <glm/glm.hpp>
#include <glmath.h>

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
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetUniformVec3f(const std::string& name, glm::vec3& vector3f);
		void SetUniformMat4f(const std::string &name, const mat4x4 &mat);
		void SetUniformMat4f(const std::string &name, const glm::mat4& mat);
		void SetUniform1i(const std::string& name, int value);

	private:

		uint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		uint CompileShader(uint type, const std::string& source);

		int GetUniformLocation(const std::string& name);

	private:

		uint m_ID;
		std::string m_Path;
		std::unordered_map<std::string, int> m_UniformLocationCache;

	};
}

#endif