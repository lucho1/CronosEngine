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
		void SetMaterial(uint index); //This is temporary too, to assign materials with an index from renderer's material list
		void SetShader(Shader& shader);
		void SetColor(const glm::vec4& col);
		void SetShininess(const float& shininess);
		void SetTexture(Texture* texture, TextureType type);

		//Getters
		static ComponentType GetType()										{ return ComponentType::MATERIAL; }
		inline const std::unordered_map<TextureType, Texture*>GetTextures()	const { return m_Material->GetTextures(); }
		inline const glm::vec4 GetColor()									const { return m_Material->GetMaterialColor(); }
		inline const float GetShininess()									const { return m_Material->GetMaterialShininess(); }
		inline const Material* GetMaterial()								const { return m_Material; }


		//Temporary index to find own material. TODO: Change this, materials should be serialized
		uint m_MaterialIndex = 0;

	private:

		Material* m_Material = nullptr;
	};
}

#endif