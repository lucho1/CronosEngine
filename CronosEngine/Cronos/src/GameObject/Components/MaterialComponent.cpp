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

		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded && m_TexturesContainer[TextureType::DIFFUSE] != nullptr)
		{
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
}