#ifndef _SHADERS_H_
#define _SHADERS_H_

#include <glm/glm.hpp>
#include <glmath.h>

namespace Cronos
{
	class Shader
	{
	public:

		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetUniformVec3f(const std::string& name, glm::vec3& vector3f);
		void SetUniformMat4f(const std::string &name, const mat4x4 &mat);
		void SetUniform1i(const std::string& name, int value);

	private:

		uint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	private:

		uint m_ID;
		std::string m_Path;

	};
}

#endif