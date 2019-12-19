#include "Providers/cnpch.h"
#include "LightComponent.h"

#include "Application.h"

#include "GameObject/GameObject.h"
#include "TransformComponent.h"

namespace Cronos
{
	LightComponent::LightComponent(GameObject * attachedGO)
		: Component(ComponentType::LIGHT, attachedGO)
	{
		m_LightType = LightType::DIRECTIONAL;
		GetParent()->GetComponent<MaterialComponent>()->SetMaterial(*App->renderer3D->GetLightMaterial());
	}

	LightComponent::~LightComponent()
	{
		m_LightType = LightType::NONE;
	}

	void LightComponent::Update(float dt)
	{
	}

	void LightComponent::SetLightColor(const glm::vec3 & color)
	{
		m_LightColor = color;
		GetParent()->GetComponent<MaterialComponent>()->SetColor({ glm::vec3(color), 1.0f });
	}

	void LightComponent::SendUniformsLightData(Shader* shader)
	{
		glm::vec3 pos;
		glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4());

		shader->SetUniformVec3f("u_Light.LightPos", pos);
		shader->SetUniformVec3f("u_Light.LightColor", m_LightColor);
	}
}