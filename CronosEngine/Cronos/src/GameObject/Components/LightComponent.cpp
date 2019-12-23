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
			case LightType::DIRECTIONAL:	return "u_DirLightsArray";
			case LightType::POINTLIGHT:		return "u_PointLightsArray";
		}

		CRONOS_WARN(0, "Couldn't convert light type into uniform string!");
		return "";
	}

	LightComponent::LightComponent(GameObject * attachedGO)
		: Component(ComponentType::LIGHT, attachedGO)
	{
		m_LightType = LightType::POINTLIGHT;
		GetParent()->GetComponent<MaterialComponent>()->SetMaterial(*App->renderer3D->GetLightMaterial());
		App->renderer3D->m_PointLightsVec.push_back(this);
	}

	LightComponent::~LightComponent()
	{
		m_LightType = LightType::NONE;
	}

	void LightComponent::SetLightType(LightType type)
	{
		if (m_LightType == type)
			return;

		if (m_LightType == LightType::POINTLIGHT)
		{
			std::vector<LightComponent*>* PLights = &App->renderer3D->m_PointLightsVec;
			PLights->erase(std::find(PLights->begin(), PLights->end(), this));
		}
		else if (m_LightType == LightType::DIRECTIONAL)
		{
			std::vector<LightComponent*>* DLights = &App->renderer3D->m_DirectionalLightsVec;
			DLights->erase(std::find(DLights->begin(), DLights->end(), this));
		}

		if (type == LightType::POINTLIGHT)
			App->renderer3D->m_PointLightsVec.push_back(this);
		else if (type == LightType::DIRECTIONAL)
			App->renderer3D->m_DirectionalLightsVec.push_back(this);

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

	void LightComponent::SendUniformsLightData(Shader* shader, uint lightIndex)
	{
		if (!isEnabled())
			return;

		if (!GetParent()->isActive())
		{
			SetLightToZero(shader, lightIndex);
			return;
		}
		if (m_ChangeLightType)
		{
			SetLightToZero(shader, lightIndex);
			m_ChangeLightType = false;
		}

		char indexedCharsArray[10];
		sprintf(indexedCharsArray, "[%i]", lightIndex);		
		std::string indexedStr_LArray = GetLightUniform(m_LightType) + indexedCharsArray;		

		shader->SetUniformVec3f(indexedStr_LArray + ".LightColor", m_LightColor);
		shader->SetUniform1f(indexedStr_LArray + ".LightIntensity", m_LightIntensity);

		switch (m_LightType)
		{
			case LightType::POINTLIGHT:
			{
				glm::vec3 pos;
				glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4());
				shader->SetUniformVec3f(indexedStr_LArray + ".LightPos", pos);

				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_K", m_LightAttK);
				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_L", m_LightAttL);
				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_Q", m_LightAttQ);
				break;
			}
			case LightType::DIRECTIONAL:
			{
				shader->SetUniformVec3f(indexedStr_LArray + ".LightDir", m_LightDirection);
				break;
			}
			default:
			{
				SetLightToZero(shader, lightIndex);
				break;
			}
		}		
	}

	void Cronos::LightComponent::SetLightToZero(Shader* shader, uint lightIndex)
	{
		char indexedCharsArray[10];
		sprintf(indexedCharsArray, "[%i]", lightIndex);

		std::string indArrSTR = indexedCharsArray;
		std::string indexed_DirLStr = "u_DirLight" + indArrSTR;
		std::string indexed_PLStr = "u_PointLight" + indArrSTR;

		shader->SetUniformVec3f(indexed_DirLStr + ".LightColor", glm::vec3(0.0f));
		shader->SetUniformVec3f(indexed_DirLStr + ".LightDir", glm::vec3(0.0f));
		shader->SetUniform1f(indexed_DirLStr + ".LightIntensity", 0.0f);

		shader->SetUniformVec3f(indexed_PLStr + ".LightColor", glm::vec3(0.0f));
		shader->SetUniform1f(indexed_PLStr + ".LightIntensity", 0.0f);

		shader->SetUniformVec3f(indexed_PLStr + ".LightPos", glm::vec3(0.0f));
		shader->SetUniform1f(indexed_PLStr + ".LightAtt_K", m_LightAttK);
		shader->SetUniform1f(indexed_PLStr + ".LightAtt_L", m_LightAttL);
		shader->SetUniform1f(indexed_PLStr + ".LightAtt_Q", m_LightAttQ);
	}
}