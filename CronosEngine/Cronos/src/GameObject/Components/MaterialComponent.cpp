#include "Providers/cnpch.h"

#include "Application.h"
#include "Modules/EngineCamera.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"

namespace Cronos
{
	MaterialComponent::MaterialComponent(GameObject* attachedGO)
		: Component(ComponentType::MATERIAL, attachedGO)
	{
		m_Material = new Material();
	}

	void MaterialComponent::Bind()
	{
		if (!isEnabled())
			return;

		if (m_Material == nullptr)
		{
			CRONOS_WARN(0, "Component's Material is null");
			return;
		}

		m_Material->Bind(!GetParent()->m_IsPrimitive);

		/*if (GetParent()->m_IsPrimitive == true)
			bindMaterial = false;

		if (bindMaterial)
		{
			if(m_TexturesContainer.size() > 0)
				App->scene->BasicTestShader->SetUniform1i("u_TextureEmpty", false);
			else
				App->scene->BasicTestShader->SetUniform1i("u_TextureEmpty", true);

			App->scene->BasicTestShader->SetUniformVec4f("u_AmbientColor", m_AmbientColor);
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
		}
		else
		{
			App->scene->BasicTestShader->SetUniform1i("u_TextureEmpty", true);
			App->scene->BasicTestShader->SetUniformVec4f("u_AmbientColor", m_AmbientColor);
		}

		if(App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe)
			App->scene->BasicTestShader->SetUniform1i("u_TextureEmpty", true);*/
	}

	void MaterialComponent::SetMaterial(Material & material)
	{
		m_Material = &material;
	}
	void MaterialComponent::SetShader(Shader & shader)
	{		
		m_Material->SetShader(shader);
	}
	void MaterialComponent::SetColor(const glm::vec4 & col)
	{
		m_Material->SetColor(col);
	}
	void MaterialComponent::SetTexture(Texture * texture, TextureType type)
	{
		m_Material->SetTexture(texture, type);
	}
}