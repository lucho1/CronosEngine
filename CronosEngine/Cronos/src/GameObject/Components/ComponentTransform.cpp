#include"Providers/cnpch.h"
#include "ComponentTransform.h"

namespace Cronos {


	ComponentTransform::ComponentTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale,bool active)
		:Component(ComponentType::Transform,active),m_Position(pos),m_Rotation(rot),m_Scale(scale)
	{
	}

	ComponentTransform::~ComponentTransform()
	{
	}

	void ComponentTransform::OnStart()
	{

	}

	void ComponentTransform::Update()
	{

	}

}