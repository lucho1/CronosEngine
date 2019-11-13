#include "Providers/cnpch.h"

#include "Application.h"
#include "Modules/EngineCamera.h"
#include "MaterialComponent.h"

namespace Cronos
{
	std::string UniformNameFromTextureType(TextureType textureType)
	{
		std::string ret = "";

		switch (textureType)
		{
			case TextureType::AMBIENT:		ret = "u_AmbientTexture"; break;
			case TextureType::DIFFUSE:		ret = "u_DiffuseTexture"; break;
			case TextureType::SPECULAR:		ret = "u_SpecularTexture"; break;
			case TextureType::NORMALMAP:	ret = "u_NormalMap"; break;
			case TextureType::HEIGHTMAP:	ret = "u_HeightMap"; break;
			case TextureType::LIGHTMAP:		ret = "u_LightMap"; break;
		}

		CRONOS_ASSERT(ret != "", "COULDN'T CONVERT TO ASSIMP TEXTURE TYPE!");
		return ret;
	}


	MaterialComponent::MaterialComponent(GameObject* attachedGO)
		: Component(ComponentType::MATERIAL, attachedGO)
	{
	}

	MaterialComponent::~MaterialComponent()
	{
		std::unordered_map<TextureType, Texture*>::iterator TextureItem = m_TexturesContainer.begin();
		for (; TextureItem != m_TexturesContainer.end(); TextureItem++)
		{
			RELEASE(TextureItem->second);
			m_TexturesContainer.erase(TextureItem);
		}

		m_TexturesContainer.clear();
		RELEASE(m_ShaderAttached);
	}

	void MaterialComponent::Bind(bool bindShader)
	{
		if (!isEnabled() || m_ShaderAttached == nullptr)
			return;

		if (GetParent()->m_IsPrimitive == true)
			bindShader = false;

		if (bindShader)
		{
			m_ShaderAttached->Bind();
			m_ShaderAttached->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrixMAT4());
			m_ShaderAttached->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrixMAT4());
			m_ShaderAttached->SetUniformMat4f("u_Model", glm::mat4(1.0f));
		}

		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded)
		{
			std::unordered_map<TextureType, Texture*>::iterator it = m_TexturesContainer.begin();
			for (; it != m_TexturesContainer.end() && (it->second) != nullptr; it++)
			{
				uint TextureID = (uint)(it->first);
				m_ShaderAttached->SetUniform1i(UniformNameFromTextureType(it->first), TextureID);
				(*it->second).Bind(TextureID);
			}
		}
		else
		{
			glColor3f(White.r, White.g, White.b);
			m_ShaderAttached->Unbind();
		}
	}

	void MaterialComponent::Unbind()
	{
		std::unordered_map<TextureType, Texture*>::iterator it = m_TexturesContainer.begin();
		for (; it != m_TexturesContainer.end() && (it->second) != nullptr; it++)
			(*it->second).Unbind();

		m_ShaderAttached->Unbind();
	}

	void MaterialComponent::SetTexture(Texture* texture, TextureType type)
	{
		CRONOS_ASSERT((type != TextureType::MAX_TEXTURES || type != TextureType::NONE), "Invalid Texture Type passed!");
		if (texture == nullptr || type == TextureType::ICON)
		{
			LOG("Texture was nullptr or Icon type!");
			return;
		}

		std::list<Texture*>::iterator itemFound_inTexturesList = std::find(App->scene->m_TexturesLoaded.begin(), App->scene->m_TexturesLoaded.end(), texture);
		std::unordered_map<TextureType, Texture*>::iterator itemFound = m_TexturesContainer.find(type);

		if (itemFound != m_TexturesContainer.end() && itemFound->second != texture)
		{
			if (itemFound_inTexturesList == App->scene->m_TexturesLoaded.end())
				App->scene->m_TexturesLoaded.push_back(texture);

			m_TexturesContainer[type] = texture;
		}
		else if(itemFound == m_TexturesContainer.end())
		{
			if (itemFound_inTexturesList == App->scene->m_TexturesLoaded.end())
				App->scene->m_TexturesLoaded.push_back(texture);

			m_TexturesContainer.insert(std::pair(type, texture));
		}
	}
}