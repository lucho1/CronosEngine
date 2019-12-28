#include "Providers/cnpch.h"
#include "Material.h"

#include "Application.h"
#include "GLRenderer3D.h"
#include "ImGui/ImGuiLayer.h"
#include "Modules/Scene.h"

#include "ImGui/ImGuiLayer.h"
#include "Modules/Scene.h"

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
		m_MaterialName = "New Material";
		m_ID = App->m_RandomNumGenerator.GetIntRN();

		m_MaterialShader = App->renderer3D->GetBasicShader();
		App->renderer3D->AddMaterialToList(this);
	}

	Material::~Material()
	{
		if (m_MaterialTextures.size() > 0)
		{
			//for (uint i = 0; i < m_MaterialTextures.size(); i++)
			//{
			//	for (auto itr = m_MaterialTextures.begin(); itr != m_MaterialTextures.end();)
			//	{
			//		RELEASE(itr->second);
			//		itr = m_MaterialTextures.erase(itr);
			//	}
			//}

			m_MaterialTextures.clear();
		}
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
		m_MaterialShader->SetUniform1f("u_Shininess", m_MaterialShininess);
		
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

	
	Texture * Material::GetTextureType(TextureType type)
	{
		std::unordered_map<TextureType, Texture*>::const_iterator iterator = m_MaterialTextures.find(type);

		if (iterator == m_MaterialTextures.end()) {
			return nullptr;
		}
		else
			return iterator->second;

		return nullptr;
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