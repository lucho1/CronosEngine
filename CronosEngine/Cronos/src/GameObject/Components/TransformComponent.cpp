#include"Providers/cnpch.h"
#include "TransformComponent.h"

namespace Cronos {

	TransformComponent::TransformComponent(GameObject* parentGo, bool active)
		: Component(ComponentType::TRANSFORM, parentGo, active)
	{

	}

	TransformComponent::~TransformComponent()
	{

	}

	void TransformComponent::OnStart()
	{

	}

	void TransformComponent::Update(float dt)
	{

	}
}