#include "Providers/cnpch.h"
#include "LightComponent.h"

namespace Cronos
{
	LightComponent::LightComponent(GameObject * attachedGO)
		: Component(ComponentType::LIGHT, attachedGO)
	{
		m_LightType = LightType::DIRECTIONAL;
	}

	LightComponent::~LightComponent()
	{
		m_LightType = LightType::NONE;
	}

	void LightComponent::Update(float dt)
	{
	}
}