#include "Providers/cnpch.h"
#include "LightComponent.h"

#include "Application.h"

#include "GameObject/GameObject.h"
#include "Renderer/GLRenderer3D.h"
#include "TransformComponent.h"

namespace Cronos
{
	const std::string GetLightUniform(const LightType LType)
	{
		switch (LType)
		{
			case LightType::DIRECTIONAL:	return "u_DirLightsArray";
			case LightType::POINTLIGHT:		return "u_PointLightsArray";
			case LightType::SPOTLIGHT:		return "u_SPLightsArray";
		}

		CRONOS_WARN(0, "Couldn't convert light type into uniform string!");
		return "";
	}

	LightComponent::LightComponent(GameObject * attachedGO)
		: Component(ComponentType::LIGHT, attachedGO)
	{
		m_LightMaterial = new Material(*App->renderer3D->GetLightMaterial());
		GetParent()->GetComponent<MaterialComponent>()->SetMaterial(*m_LightMaterial);

		m_LightType = LightType::POINTLIGHT;
		App->renderer3D->m_PointLightsVec.push_back(this);
	}

	LightComponent::~LightComponent()
	{
		m_LightType = LightType::NONE;
		RELEASE(m_LightMaterial);
	}

	void LightComponent::Update(float dt)
	{
		glm::vec3 pos;
		glm::quat q;
		glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), q, pos, glm::vec3(), glm::vec4());
		glm::vec3 vecOrientation = glm::vec3(2 * (q.x * q.z + q.w * q.y), 2 * (q.y * q.z - q.w * q.x), 1 - 2 * (q.x * q.x + q.y * q.y));
		
		m_PLightComp.m_LightPosition = m_SLightComp.m_LightPosition = pos;
		m_SLightComp.m_LightDirection = -vecOrientation;
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
		else if (m_LightType == LightType::SPOTLIGHT)
		{
			std::vector<LightComponent*>* SpLights = &App->renderer3D->m_SpotLightsVec;
			SpLights->erase(std::find(SpLights->begin(), SpLights->end(), this));
		}

		if (type == LightType::POINTLIGHT)
			App->renderer3D->m_PointLightsVec.push_back(this);
		else if (type == LightType::DIRECTIONAL)
			App->renderer3D->m_DirectionalLightsVec.push_back(this);
		else if(type == LightType::SPOTLIGHT)
			App->renderer3D->m_SpotLightsVec.push_back(this);

		m_LightType = type;
		m_ChangeLightType = true;
	}

	void LightComponent::SetLightColor(const glm::vec3& color)
	{
		m_PLightComp.m_LightColor = m_SLightComp.m_LightColor = m_DLightComp.m_LightColor = color;
		GetParent()->GetComponent<MaterialComponent>()->SetColor({ glm::vec3(color), 1.0f });
	}

	void LightComponent::SetLightIntensity(const float& intensity)
	{
		if (intensity < 0.0f || intensity > 1.0f)
			return;

		m_PLightComp.m_LightIntensity =
		m_SLightComp.m_LightIntensity =
		m_DLightComp.m_LightIntensity = intensity;
	}



	void LightComponent::SendUniformsLightData(Shader* shader, uint lightIndex)
	{
		if (!isEnabled())
			return;

		//App->renderer3D->GetSSBO().PassData(sizeof(DirectionalLight), );

		if (!GetParent()->isActive())
		{
			//SetLightToZero(shader, lightIndex, m_LightType);
			return;
		}
		if (m_ChangeLightType)
		{
			//SetLightToZero(shader, lightIndex, m_LightType);
			m_ChangeLightType = false;
		}

		char indexedCharsArray[10];
		sprintf(indexedCharsArray, "[%i]", lightIndex);
		std::string indexedStr_LArray = GetLightUniform(m_LightType) + indexedCharsArray;

		shader->SetUniformVec3f(indexedStr_LArray + ".LightColor", glm::vec3(1.0f));
		shader->SetUniform1f(indexedStr_LArray + ".LightIntensity", 1.0f);

		switch (m_LightType)
		{
			case LightType::POINTLIGHT:
			{
				glm::vec3 pos;
				glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4());
				shader->SetUniformVec3f(indexedStr_LArray + ".LightPos", pos);

				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_K", 1.0f);
				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_L", 0.0f);
				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_Q", 0.0f);
				break;
			}
			case LightType::DIRECTIONAL:
			{
				shader->SetUniformVec3f(indexedStr_LArray + ".LightDir", glm::vec3(0.0));
				break;
			}
			case LightType::SPOTLIGHT:
			{
				glm::vec3 pos;
				glm::quat q;
				glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), q, pos, glm::vec3(), glm::vec4());
				shader->SetUniformVec3f(indexedStr_LArray + ".LightPos", pos);
				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_K", 1.0f);
				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_L", 0.0f);
				shader->SetUniform1f(indexedStr_LArray + ".LightAtt_Q", 0.0f);


				glm::vec3 vecOrientation = glm::vec3(2 * (q.x*q.z + q.w*q.y), 2 * (q.y*q.z - q.w*q.x), 1 - 2 * (q.x*q.x + q.y*q.y));
				shader->SetUniformVec3f(indexedStr_LArray + ".LightDir", -vecOrientation);
				shader->SetUniform1f(indexedStr_LArray + ".cutoffAngleCos", glm::cos(glm::radians(12.5f)));
				shader->SetUniform1f(indexedStr_LArray + ".outerCutoffAngleCos", glm::cos(glm::radians(60.0f)));

				break;
			}
			default:
			{
				//SetLightToZero(shader, lightIndex, m_LightType);
				break;
			}
		}
	}


	//void LightComponent::SendUniformsLightData(Shader* shader, uint lightIndex)
	//{
	//	if (!isEnabled())
	//		return;
	//
	//	if (!GetParent()->isActive())
	//	{
	//		SetLightToZero(shader, lightIndex, m_LightType);
	//		return;
	//	}
	//	if (m_ChangeLightType)
	//	{
	//		SetLightToZero(shader, lightIndex, m_LightType);
	//		m_ChangeLightType = false;
	//	}
	//
	//	char indexedCharsArray[10];
	//	sprintf(indexedCharsArray, "[%i]", lightIndex);		
	//	std::string indexedStr_LArray = GetLightUniform(m_LightType) + indexedCharsArray;		
	//
	//	shader->SetUniformVec3f(indexedStr_LArray + ".LightColor", m_LightColor);
	//	shader->SetUniform1f(indexedStr_LArray + ".LightIntensity", m_LightIntensity);
	//
	//	switch (m_LightType)
	//	{
	//		case LightType::POINTLIGHT:
	//		{
	//			glm::vec3 pos;
	//			glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4());
	//			shader->SetUniformVec3f(indexedStr_LArray + ".LightPos", pos);
	//
	//			shader->SetUniform1f(indexedStr_LArray + ".LightAtt_K", m_LightAttK);
	//			shader->SetUniform1f(indexedStr_LArray + ".LightAtt_L", m_LightAttL);
	//			shader->SetUniform1f(indexedStr_LArray + ".LightAtt_Q", m_LightAttQ);
	//			break;
	//		}
	//		case LightType::DIRECTIONAL:
	//		{
	//			shader->SetUniformVec3f(indexedStr_LArray + ".LightDir", m_LightDirection);
	//			break;
	//		}
	//		case LightType::SPOTLIGHT:
	//		{
	//			glm::vec3 pos;
	//			glm::quat q;
	//			glm::decompose(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix(), glm::vec3(), q, pos, glm::vec3(), glm::vec4());
	//			shader->SetUniformVec3f(indexedStr_LArray + ".LightPos", pos);
	//			shader->SetUniform1f(indexedStr_LArray + ".LightAtt_K", m_LightAttK);
	//			shader->SetUniform1f(indexedStr_LArray + ".LightAtt_L", m_LightAttL);
	//			shader->SetUniform1f(indexedStr_LArray + ".LightAtt_Q", m_LightAttQ);
	//
	//
	//			glm::vec3 vecOrientation = glm::vec3(2 * (q.x*q.z + q.w*q.y), 2 * (q.y*q.z - q.w*q.x), 1-2 * (q.x*q.x + q.y*q.y));
	//			shader->SetUniformVec3f(indexedStr_LArray + ".LightDir", -vecOrientation);
	//			shader->SetUniform1f(indexedStr_LArray + ".cutoffAngleCos", glm::cos(glm::radians(m_LightCutoffAngle)));
	//			shader->SetUniform1f(indexedStr_LArray + ".outerCutoffAngleCos", glm::cos(glm::radians(m_LightOuterCutoffAngle)));
	//
	//			break;
	//		}
	//		default:
	//		{
	//			SetLightToZero(shader, lightIndex, m_LightType);
	//			break;
	//		}
	//	}		
	//}

	/*void Cronos::LightComponent::SetLightToZero(Shader* shader, uint lightIndex, LightType lType)
	{
		char indexedCharsArray[10];
		sprintf(indexedCharsArray, "[%i]", lightIndex);
		std::string indArrSTR = indexedCharsArray;

		switch (lType)
		{
			case LightType::DIRECTIONAL:
			{
				std::string indexed_DirLStr = "u_DirLight" + indArrSTR;

				shader->SetUniformVec3f(indexed_DirLStr + ".LightDir", glm::vec3(0.0f));
				shader->SetUniformVec3f(indexed_DirLStr + ".LightColor", glm::vec3(0.0f));
				shader->SetUniform1f(indexed_DirLStr + ".LightIntensity", 0.0f);
				break;
			}
			case LightType::POINTLIGHT:
			{
				std::string indexed_PLStr = "u_PointLight" + indArrSTR;

				shader->SetUniformVec3f(indexed_PLStr + ".LightPos", glm::vec3(0.0f));
				shader->SetUniformVec3f(indexed_PLStr + ".LightColor", glm::vec3(0.0f));
				shader->SetUniform1f(indexed_PLStr + ".LightIntensity", 0.0f);

				shader->SetUniform1f(indexed_PLStr + ".LightAtt_K", m_LightAttK);
				shader->SetUniform1f(indexed_PLStr + ".LightAtt_L", m_LightAttL);
				shader->SetUniform1f(indexed_PLStr + ".LightAtt_Q", m_LightAttQ);
				break;
			}
			case LightType::SPOTLIGHT:
			{
				std::string indexed_SPLStr = "u_SPLightsArray" + indArrSTR;

				shader->SetUniformVec3f(indexed_SPLStr + ".LightPos", glm::vec3(0.0f));
				shader->SetUniformVec3f(indexed_SPLStr + ".LightColor", glm::vec3(0.0f));

				shader->SetUniform1f(indexed_SPLStr + ".LightIntensity", 0.0f);

				shader->SetUniform1f(indexed_SPLStr + ".LightAtt_K", m_LightAttK);
				shader->SetUniform1f(indexed_SPLStr + ".LightAtt_L", m_LightAttL);
				shader->SetUniform1f(indexed_SPLStr + ".LightAtt_Q", m_LightAttQ);

				shader->SetUniformVec3f(indexed_SPLStr + ".LightDir", glm::vec3(0.0f));
				shader->SetUniform1f(indexed_SPLStr + ".cutoffAngleCos", 0.0f);
				shader->SetUniform1f(indexed_SPLStr + ".outerCutoffAngleCos", 0.0f);
				break;
			}
		}
	}*/
}