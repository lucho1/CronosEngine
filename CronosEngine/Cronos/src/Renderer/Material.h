#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Renderer/Shaders.h"
#include "Renderer/Textures.h"

namespace Cronos
{
	class Material
	{
	public:

		Material();
		~Material();

		void Bind(bool bindTextures, const glm::mat4& ModelTransform = glm::mat4(1.0f));
		void Unbind();
		
	public:

		//Getters
		inline const std::unordered_map<TextureType, Texture*>GetTextures()	const { return m_MaterialTextures; }
		inline const glm::vec4 GetMaterialColor()							const { return m_MaterialAmbientColor; }
		inline const uint GetMaterialID()									const { return m_ID; }
		inline const std::string GetMatName()								const { return m_MaterialName; }
		inline const std::string GetMatPath()								const { return m_Path; }
		Texture* GetTextureType(TextureType type);
		
		//Setters
		void SetName(const std::string& name)								{ m_MaterialName = name; }
		void SetShader(Shader& shader)										{ m_MaterialShader = &shader; }
		void SetColor(const glm::vec4& color)								{ m_MaterialAmbientColor = color; }
		void SetTexture(Texture* texture, TextureType type);
		void SetID(uint ID)													{ m_ID = ID; }
		
	
	private:
		
		std::string m_MaterialName;
		std::string m_Path;
		uint m_ID = 0;

		glm::vec4 m_MaterialAmbientColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
		std::unordered_map<TextureType, Texture*> m_MaterialTextures;

		Shader* m_MaterialShader = nullptr;
	};
}

#endif