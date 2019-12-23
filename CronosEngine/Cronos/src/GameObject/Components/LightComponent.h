#ifndef _LIGHTCOMPONENT_H_
#define _LIGHTCOMPONENT_H_

#include "Component.h"
#include "Renderer/Shaders.h"
#include "MaterialComponent.h"

namespace Cronos
{
	enum class LightType { NONE = -1, DIRECTIONAL, POINTLIGHT };

	class LightComponent : public Component
	{
	public:

		LightComponent(GameObject* attachedGO);
		~LightComponent();
		
		static ComponentType GetType() { return ComponentType::LIGHT; }
		
	public:

		//Setters
		void SetLightType(LightType type);
		void SetLightDirection(const glm::vec3& direction) { m_LightDirection = direction; }
		void SetAttenuationFactors(const glm::vec3& attenuationFactorsKLQ) { m_LightAttK = attenuationFactorsKLQ.x; m_LightAttL = attenuationFactorsKLQ.y, m_LightAttQ = attenuationFactorsKLQ.z; }
		void SetLightColor(const glm::vec3& color);
		void SetLightIntensity(const float& intensity);

		//Getters
		inline const LightType GetLightType()	const { return m_LightType; }
		inline const glm::vec3 GetLightColor()	const { return m_LightColor; }
		inline const float GetLightIntensity()	const { return m_LightIntensity; }
		inline const glm::vec3 GetLightDirection() const { return m_LightDirection; }
		inline const glm::vec3 GetLightAttenuationFactors() const { return { m_LightAttK, m_LightAttL, m_LightAttQ }; }

	public:

		void SendUniformsLightData(Shader* shader);
		void SendUniformsLightData(Shader* shader, uint pLight_Index);

	private:

		void SetLightToZero(Shader* shader);

	private:

		LightType m_LightType = LightType::NONE;

		glm::vec3 m_LightColor = glm::vec3(1.0f);
		glm::vec3 m_LightDirection = glm::vec3(0.0f);
		float m_LightIntensity = 0.5f;
		float m_LightAttK = 1.0f, m_LightAttL = 0.09f, m_LightAttQ = 0.032f;

		bool m_ChangeLightType = false;
	};
}

#endif