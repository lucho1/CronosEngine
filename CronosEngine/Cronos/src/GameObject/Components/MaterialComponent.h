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

		Shader* m_ShaderAttached = nullptr;

	private:

		
		std::unordered_map<TextureType, Texture*>m_TexturesContainer;
	};
}

#endif