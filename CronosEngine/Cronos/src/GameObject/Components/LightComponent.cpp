#include "Providers/cnpch.h"
#include "LightComponent.h"

#include "Application.h"

#include "GameObject/GameObject.h"
#include "TransformComponent.h"

namespace Cronos
{
	const std::string GetLightUniform(const LightType LType)
	{
		switch (LType)
		{
			case LightType::DIRECTIONAL:	return "u_DirLight";
			case LightType::POINTLIGHT:		return "u_PointLight";
		}

		CRONOS_WARN(0, "Couldn't convert light type into uniform string!");
		return "";
	}

	LightComponent::LightComponent(GameObject * attachedGO)
		: Component(ComponentType::LIGHT, attachedGO)
	{
		m_LightType = LightType::DIRECTIONAL;
		GetParent()->GetComponent<MaterialComponent>()->SetMaterial(*App->renderer3D->GetLightMaterial());
	}

	LightComponent::~LightComponent()
	{
		//App->renderer3D->PopLight(this);
		m_LightType = LightType::NONE;
	}

	void LightComponent::SetLightType(LightType type)
	{
		m_LightType = type;
		m_ChangeLightType = true;
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
			SetLightToZero(shader);
			return;
		}
		if (m_ChangeLightType)
		{
			SetLightToZero(shader);
			m_ChangeLightType = false;
		}

		std::string lType = GetLightUniform(m_LightType);
		shader->SetUniformVec3f(lType + ".LightColor", m_LightColor);
		shader->SetUniform1f(lType + ".LightIntensity", m_LightIntensity);

		switch (m_LightType)
		{
			case LightType::POINTLIGHT:
			{
				glm::vec3 pos;
				glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4());

				shader->SetUniformVec3f(lType + ".LightPos", pos);

				shader->SetUniform1f(lType + ".LightAtt_K", m_LightAttK);
				shader->SetUniform1f(lType + ".LightAtt_L", m_LightAttL);
				shader->SetUniform1f(lType + ".LightAtt_Q", m_LightAttQ);
				break;
			}
			case LightType::DIRECTIONAL:
			{
				shader->SetUniformVec3f(lType + ".LightDir", m_LightDirection);
				break;
			}
			default:
			{
				SetLightToZero(shader);
				break;
			}
		}

	}

	void Cronos::LightComponent::SetLightToZero(Shader* shader)
	{
		shader->SetUniformVec3f("u_DirLight.LightColor", glm::vec3(0.0f));
		shader->SetUniformVec3f("u_DirLight.LightDir", glm::vec3(0.0f));
		shader->SetUniform1f("u_DirLight.LightIntensity", 0.0f);

		shader->SetUniformVec3f("u_PointLight.LightColor", glm::vec3(0.0f));
		shader->SetUniform1f("u_PointLight.LightIntensity", 0.0f);

		shader->SetUniformVec3f("u_PointLight.LightPos", glm::vec3(0.0f));
		shader->SetUniform1f("u_PointLight.LightAtt_K", m_LightAttK);
		shader->SetUniform1f("u_PointLight.LightAtt_L", m_LightAttL);
		shader->SetUniform1f("u_PointLight.LightAtt_Q", m_LightAttQ);
	}
}