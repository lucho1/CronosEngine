#ifndef _MATERIALCOMPONENT_H_
#define _MATERIALCOMPONENT_H_

#include "Component.h"
#include "Renderer/Shaders.h"
#include "Renderer/Textures.h"

#include "Renderer/Material.h"

namespace Cronos
{
	class MaterialComponent : public Component
	{
	public:

		MaterialComponent(GameObject* attachedGO);
		~MaterialComponent() {}

		void Bind();
		void Unbind() { m_Material->Unbind(); }

		//Setters
		void SetMaterial(Material& material);
		void SetShader(Shader& shader);
		void SetColor(const glm::vec4& col);
		void SetTexture(Texture* texture, TextureType type);

		//Getters
		static ComponentType GetType()										{ return ComponentType::MATERIAL; };
		inline const std::unordered_map<TextureType, Texture*>GetTextures()	const { return m_Material->GetTextures(); }
		inline const glm::vec4 GetColor()									const { return m_Material->GetMaterialColor(); }
		inline const Material* GetMaterial()								const { return m_Material; }


	private:

		Material* m_Material = nullptr;
	};
}

#endif