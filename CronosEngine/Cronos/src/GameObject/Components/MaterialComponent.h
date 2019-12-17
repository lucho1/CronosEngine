#ifndef _MATERIALCOMPONENT_H_
#define _MATERIALCOMPONENT_H_

#include "Component.h"
#include "Renderer/Shaders.h"
#include "Renderer/Textures.h"

namespace Cronos
{
	class MaterialComponent : public Component
	{
	public:

		MaterialComponent(GameObject* attachedGO);
		~MaterialComponent();

		void Bind(bool bindMaterial);
		void Unbind();

		void SetTexture(Texture* texture, TextureType type);
		void SetShader(Shader* shader) { m_ShaderAttached = shader; }

		inline const std::unordered_map<TextureType, Texture*>GetTextures() const { return m_TexturesContainer; }
		static ComponentType GetType() { return ComponentType::MATERIAL; };

		void SetColor(const glm::vec4& col) { m_AmbientColor = col; }
		const glm::vec4 GetColor() const { return m_AmbientColor; }

	private:
		
		Shader* m_ShaderAttached = nullptr;
		
		std::unordered_map<TextureType, Texture*>m_TexturesContainer;
		glm::vec4 m_AmbientColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};
}

#endif