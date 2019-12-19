#ifndef _LIGHTCOMPONENT_H_
#define _LIGHTCOMPONENT_H_

#include "Component.h"
#include "Renderer/Shaders.h"


namespace Cronos
{
	enum class LightType { NONE = -1, DIRECTIONAL };

	class LightComponent : public Component
	{
	public:

		LightComponent(GameObject* attachedGO);
		~LightComponent();

		virtual void Update(float dt);

	public:

		//Setters
		void SetLightType(LightType type) { m_LightType = type; }
		void SetLightColor(const glm::vec3& color) { m_LightColor = color; }

		//Getters
		static ComponentType GetType() { return ComponentType::LIGHT; };
		inline const LightType GetLightType()	const { return m_LightType; }
		inline const glm::vec3 GetLightColor()	const { return m_LightColor; }

	public:

		void SendUniformsLightData(Shader* shader);

	private:

		LightType m_LightType = LightType::NONE;
		glm::vec3 m_LightColor = glm::vec3(1.0f);
	};
}

#endif