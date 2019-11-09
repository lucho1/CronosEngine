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

		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded && m_AmbientTexture != nullptr)
		{
			m_AmbientTexture->Bind(m_AmbientTexture->GetTextureID());
			m_ShaderAttached->SetUniform1i("u_DiffuseTexture", m_AmbientTexture->GetTextureID());
		}
		else
		{
			glColor3f(White.r, White.g, White.b);
			m_ShaderAttached->Unbind();
		}
	}

	void MaterialComponent::Unbind()
	{
		m_AmbientTexture->Unbind();
		m_ShaderAttached->Unbind();

		/*std::vector< Texture*>::iterator item2 = m_TexturesVector.begin();
		item2 = m_TexturesVector.begin();
		for (; item2 != m_TexturesVector.end() && (*item2) != NULL; item2++)
			(*item2)->Unbind();

		shader->Unbind();*/
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