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

	void LightComponent::SetLightColor(const glm::vec3& color)
	{
		m_LightColor = color;
		GetParent()->GetComponent<MaterialComponent>()->SetColor({ glm::vec3(color), 1.0f });
	}

	void LightComponent::SetLightIntensity(const float& intensity)
	{
		if (intensity < 0.0f || intensity > 1.0f)
			return;

		m_LightIntensity = intensity;
	}

	void LightComponent::SendUniformsLightData(Shader* shader)
	{
		glm::vec3 pos;
		glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4());

		shader->SetUniformVec3f("u_Light.LightPos", pos);
		shader->SetUniform1f("u_Light.LightIntensity", m_LightIntensity);
		shader->SetUniformVec3f("u_Light.LightColor", m_LightColor);
	}
}