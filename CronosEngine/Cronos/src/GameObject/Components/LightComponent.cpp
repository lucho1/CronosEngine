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
		App->renderer3D->PopLight(this);
		m_LightType = LightType::NONE;
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
		if (!isEnabled())
			return;

		if (!GetParent()->isActive())
		{
			shader->SetUniformVec3f("u_PointLight.LightColor", glm::vec3(0.0f));
			shader->SetUniform1f("u_PointLight.LightIntensity", 0.0f);
			return;
		}

		glm::vec3 pos;
		glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4());
		
		shader->SetUniformVec3f("u_PointLight.LightPos", pos);
		//shader->SetUniformVec3f("u_DirLight.LightDir", m_LightDirection);
		shader->SetUniformVec3f("u_PointLight.LightColor", m_LightColor);
		
		shader->SetUniform1f("u_PointLight.LightIntensity", m_LightIntensity);
		
		shader->SetUniform1f("u_PointLight.LightAtt_K", m_LightAttK);
		shader->SetUniform1f("u_PointLight.LightAtt_L", m_LightAttL);
		shader->SetUniform1f("u_PointLight.LightAtt_Q", m_LightAttQ);
	}	
}