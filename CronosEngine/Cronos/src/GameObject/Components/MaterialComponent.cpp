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
		SetMaterial(*App->renderer3D->GetDefaultMaterial());
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

		glm::mat4 transform = GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix();
		m_Material->Bind(!GetParent()->m_IsPrimitive, transform);
	}

	void MaterialComponent::SetMaterial(Material& material)
	{
		if (&material == nullptr)
		{
			CRONOS_WARN(0, "Couldn't Assign Material, the one passed is NULL -- Function: SetMaterial()")
			return;
		}

		m_Material = &material;
	}

	void MaterialComponent::SetMaterial(uint index)
	{
		if (App->renderer3D->GetMaterialInMaterialsList(index) == nullptr)
		{
			CRONOS_WARN(0, "Couldn't Assign Material, the one passed is NULL -- Function: SetMaterial()")
				return;
		}

		m_Material = App->renderer3D->GetMaterialInMaterialsList(index);
	}

	void MaterialComponent::SetShader(Shader & shader)
	{	
		if (m_Material == nullptr || m_Material->GetMaterialID() == App->renderer3D->GetDefaultMaterialID())
		{
			CRONOS_WARN(0, "Operation Failed, current Material is NULL or is default (unmodifiable) -- Function: SetShader()")
			return;
		}

		m_Material->SetShader(shader);
	}

	void MaterialComponent::SetColor(const glm::vec4 & col)
	{
		if (m_Material == nullptr || m_Material->GetMaterialID() == App->renderer3D->GetDefaultMaterialID())
		{
			CRONOS_WARN(0, "Operation Failed, current Material is NULL or is default (unmodifiable) -- Function: SetColor()")
			return;
		}

		m_Material->SetColor(col);
	}

	void MaterialComponent::SetTexture(Texture * texture, TextureType type)
	{
		if (m_Material == nullptr || m_Material->GetMaterialID() == App->renderer3D->GetDefaultMaterialID())
		{
			CRONOS_WARN(0, "Operation Failed, current Material is NULL or is default (unmodifiable) -- Function: SetTexture()")
			return;
		}

		m_Material->SetTexture(texture, type);
	}
}