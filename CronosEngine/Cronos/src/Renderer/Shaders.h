#ifndef _SHADERS_H_
#define _SHADERS_H_

#include <glm/glm.hpp>

namespace Cronos
{
	class Shader
	{
	public:

		Shader(const std::string& vertexShader, const std::string& fragmentShader);
		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void Unbind() const;
		bool UserCompile(const std::string& filepath);

		void SetUniformVec2f(const std::string& name, glm::vec2& vector2f);
		void SetUniformVec3f(const std::string& name, glm::vec3& vector3f);
		void SetUniformVec4f(const std::string& name, glm::vec4& vector4f);
		void SetUniformMat4f(const std::string &name, const glm::mat4& mat);
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float value);
		void SetUniformBool(const std::string& name, bool value);

		void setPath(std::string path) { m_Path = path; }
		void setName(std::string name) { m_name = name + ".glsl"; }


		std::string GetShaderTextFormat()	const { return ShaderText; }
		std::string GetShaderPath()			const { return m_Path; }
		std::string GetShaderName()			const { return m_name; }
		inline uint GetID()					const { return m_ID; }

	private:

		std::string ReadFile(const std::string& filepath);
	
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& SourceShaderCode);
		bool UserPreProcess(const std::string& SourceShaderCode, std::unordered_map<GLenum, std::string>& uMap);

		void Compile(const std::unordered_map<GLenum, std::string>& ShadersUMap);

		int GetUniformLocation(const std::string& name);

	private:

		std::string ShaderText;
		std::string m_name;

		uint m_ID;
		std::string m_Path;
		std::unordered_map<std::string, int> m_UniformLocationCache;
	};
}

#endif