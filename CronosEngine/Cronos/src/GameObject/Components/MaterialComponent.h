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

		void Bind(bool bindShader);
		void Unbind();


		void SetShader(Shader* shader) { m_ShaderAttached = shader; }
		void SetTexture(Texture* texture) { m_AmbientTexture = texture; }
		static ComponentType GetType() { return ComponentType::MATERIAL; };

	private:

		//	enum class TextureType { NONE = -1, ICON, AMBIENT, DIFFUSE, SPECULAR, NORMALMAP, HEIGHTMAP, LIGHTMAP, MAX_TEXTURES};
		Shader* m_ShaderAttached = nullptr;
		Texture* m_AmbientTexture = nullptr;
	};
}

#endif