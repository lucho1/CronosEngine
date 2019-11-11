#include "Providers/cnpch.h"

#include "Application.h"
#include "Modules/EngineCamera.h"
#include "MaterialComponent.h"

namespace Cronos
{
	MaterialComponent::MaterialComponent(GameObject* attachedGO)
		: Component(ComponentType::MATERIAL, attachedGO)
	{
	}

	MaterialComponent::~MaterialComponent()
	{
		RELEASE(m_AmbientTexture);
		//RELEASE(m_ShaderAttached);
		m_ShaderAttached = nullptr;
		//m_AmbientTexture = nullptr;
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

		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded && m_TexturesContainer[TextureType::DIFFUSE] != nullptr)
		{
			//m_AmbientTexture->Bind(m_AmbientTexture->GetTextureID());
			m_TexturesContainer[TextureType::DIFFUSE]->Bind(m_TexturesContainer[TextureType::DIFFUSE]->GetTextureID());

			m_ShaderAttached->SetUniform1i("u_DiffuseTexture", m_TexturesContainer[TextureType::DIFFUSE]->GetTextureID());
		}
		else
		{
			glColor3f(White.r, White.g, White.b);
			m_ShaderAttached->Unbind();
		}
	}

	void MaterialComponent::Unbind()
	{
		m_TexturesContainer[TextureType::DIFFUSE]->Unbind();
		//m_AmbientTexture->Unbind();
		m_ShaderAttached->Unbind();
	}

	void MaterialComponent::SetTexture(Texture* texture, TextureType type)
	{
		CRONOS_ASSERT((type != TextureType::MAX_TEXTURES || type != TextureType::NONE), "Invalid Texture Type passed!");
		std::unordered_map<TextureType, Texture*>::iterator itemFound = m_TexturesContainer.find(type);

		if (itemFound != m_TexturesContainer.end())
		{
			if (itemFound->second != texture)
			{
				RELEASE(m_TexturesContainer[type]);
				m_TexturesContainer[type] = texture;
			}
		}
		else
			m_TexturesContainer[type] = texture;
	}

	/*

	void MeshComponent::SetTextures(std::vector<Texture*> newTexture, TextureType textureType)
	{
		for (uint i = 0; i < m_TexturesVector.size() && m_TexturesVector[i] != nullptr; i++)
		{
			if (m_TexturesVector[i]->GetTextureType() == textureType)
			{
				RELEASE(m_TexturesVector[i]);
				m_TexturesVector.erase(m_TexturesVector.begin() + i);
			}
		}

		m_TexturesVector.insert(m_TexturesVector.end(), newTexture.begin(), newTexture.end());
	}
	
	*/
}