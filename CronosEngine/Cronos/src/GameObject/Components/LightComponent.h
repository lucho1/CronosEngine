#ifndef _LIGHTCOMPONENT_H_
#define _LIGHTCOMPONENT_H_

#include "Component.h"

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

		//Getters
		static ComponentType GetType() { return ComponentType::LIGHT; };
		inline const LightType GetLightType() const { return m_LightType; }

	private:

		LightType m_LightType = LightType::NONE;
	};
}

#endif