#include"Providers/cnpch.h"
#include "ComponentTransform.h"

namespace Cronos {


	ComponentTransform::ComponentTransform(GameObject* parentGo, bool active)
		:Component(ComponentType::Transform,parentGo,active)
	{
	}

	ComponentTransform::~ComponentTransform()
	{
	}

	void ComponentTransform::OnStart()
	{

	}

	void ComponentTransform::Update(float dt)
	{

	}

}