#include "Providers/cnpch.h"
#include "Material.h"

#include "Application.h"
#include "GLRenderer3D.h"

#include "GameObject/Components/TransformComponent.h"

namespace Cronos
{
	std::string UniformNameFromTextureType(TextureType textureType)
	{
		std::string ret = "";
		switch (textureType)
		{
			case TextureType::DIFFUSE:		ret = "u_DiffuseTexture"; break;
			case TextureType::SPECULAR:		ret = "u_SpecularTexture"; break;
		}

		CRONOS_ASSERT(ret != "", "Unexisting or Unsupported texture type");
		return ret;
	}


	Material::Material()
	{
		App->renderer3D->AddMaterialToList(this);
		m_MaterialName = "Unnamed Material";
		m_MaterialShader = App->scene->BasicTestShader;
	}


	Material::~Material()
	{
		std::unordered_map<TextureType, Texture*>::iterator TextureItem = m_MaterialTextures.begin();
		for (; TextureItem != m_MaterialTextures.end() && (*TextureItem).second; TextureItem++)
		{
			RELEASE(TextureItem->second);
			m_MaterialTextures.erase(TextureItem);
		}

		m_MaterialTextures.clear();
	}


	void Material::Bind(bool bindTextures, const glm::mat4& ModelTransform)
	{
		if (m_MaterialShader == nullptr)
		{
			CRONOS_WARN(0, "Material Shader not found");
			return;
		}

		m_MaterialShader->Bind();
		m_MaterialShader->SetUniformMat4f("u_Model", ModelTransform);
		m_MaterialShader->SetUniformVec4f("u_AmbientColor", m_MaterialAmbientColor);
		
		if(!bindTextures || m_MaterialTextures.size() <= 0 || App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe)
			m_MaterialShader->SetUniform1i("u_TextureEmpty", true);
		else
		{
			m_MaterialShader->SetUniform1i("u_TextureEmpty", false);

			std::unordered_map<TextureType, Texture*>::iterator it = m_MaterialTextures.begin();
			for (; it != m_MaterialTextures.end() && (it->second) != nullptr; it++)
			{
				uint TextureID = (uint)(it->first);
				m_MaterialShader->SetUniform1i(UniformNameFromTextureType(it->first), TextureID);
				(*it->second).Bind(TextureID);
			}
		}
	}


	void Material::Unbind()
	{
		std::unordered_map<TextureType, Texture*>::iterator it = m_MaterialTextures.begin();
		for (; it != m_MaterialTextures.end() && (it->second) != nullptr; it++)
			(*it->second).Unbind();

		m_MaterialShader->Unbind();
	}

	
	void Material::SetTexture(Texture * texture, TextureType type)
	{
		CRONOS_ASSERT((type != TextureType::MAX_TEXTURES || type != TextureType::NONE), "Invalid Texture Type passed!");
		if (texture == nullptr || type == TextureType::ICON)
		{
			LOG("Texture was nullptr or Icon type!");
			return;
		}

		std::list<Texture*>::iterator TextureLoaded = std::find(App->scene->m_TexturesLoaded.begin(), App->scene->m_TexturesLoaded.end(), texture);
		std::unordered_map<TextureType, Texture*>::iterator TextureInMaterial = m_MaterialTextures.find(type);

		if (TextureInMaterial != m_MaterialTextures.end() && TextureInMaterial->second != texture)
		{
			if (TextureLoaded == App->scene->m_TexturesLoaded.end())
				App->scene->m_TexturesLoaded.push_back(texture);

			m_MaterialTextures[type] = texture;
		}
		else if (TextureInMaterial == m_MaterialTextures.end())
		{
			if (TextureLoaded == App->scene->m_TexturesLoaded.end())
				App->scene->m_TexturesLoaded.push_back(texture);

			m_MaterialTextures.insert(std::pair(type, texture));
		}
	}
}