#include"Providers/cnpch.h"
#include "TransformComponent.h"
#include "GameObject/GameObject.h"

#include "Application.h"

namespace Cronos {

	TransformComponent::TransformComponent(GameObject* attachedGO, bool active)
		: Component(ComponentType::TRANSFORM, attachedGO, active)
	{
		m_EulerAngles = glm::vec3(0.0f);
		m_Orientation = glm::quat(1.0f, glm::vec3(0.0f));
		m_Scale = m_Translation = glm::vec3(0.0f);

		m_GlobalTransformationMatrix = m_LocalTransformationMatrix = glm::mat4(1.0f);
		UpdateTransform();
	}

	TransformComponent::~TransformComponent()
	{
	}

	void TransformComponent::Update(float dt)
	{
	}

	void TransformComponent::SetPosition(glm::vec3 position)
	{
		//AABBPos = m_Translation;
		m_Translation = position;
		UpdateTransform();
	}

	void TransformComponent::SetScale(glm::vec3 scale)
	{
		//AABBScale = m_Scale;
		m_Scale = scale;
		UpdateTransform();
	}

	//Set the orientation of the object (pass Euler Angles in degrees!!)
	void TransformComponent::SetOrientation(glm::vec3 euler_angles)
	{
		//AABBOrientation = m_Orientation;
		glm::vec3 EA_Rad = glm::radians(euler_angles);
		glm::quat rot = glm::quat(EA_Rad - m_EulerAngles);

		m_Orientation = m_Orientation * rot;
		m_EulerAngles = EA_Rad;
		UpdateTransform();
	}

	void TransformComponent::Rotate(glm::vec3 euler_angles)
	{
		glm::vec3 EA_Rad = glm::radians(euler_angles);
		glm::quat newOrientation = glm::quat(EA_Rad);

		m_Orientation = m_Orientation * newOrientation;
		m_EulerAngles = EA_Rad;
	}

	void TransformComponent::Move(glm::vec3 translation)
	{
		m_Translation += translation;
		UpdateTransform();
	}

	void TransformComponent::Scale(glm::vec3 scale)
	{
		m_Scale += scale;
		UpdateTransform();
	}

	void TransformComponent::UpdateTransform()
	{
		//Local transform
		m_LocalTransformationMatrix = glm::translate(glm::mat4(1.0f), m_Translation) *
			glm::mat4_cast(m_Orientation) * glm::scale(glm::mat4(1.0f), m_Scale);

		//Update global transform
		GameObject* GOAttached_Parent = GetParent()->GetParentGameObject();
		if (GOAttached_Parent != nullptr)
			m_GlobalTransformationMatrix = GOAttached_Parent->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * m_LocalTransformationMatrix;
		else
			m_GlobalTransformationMatrix = m_LocalTransformationMatrix;
		
		glm::vec3 pos = glm::vec3(0.0f);
		glm::decompose(m_GlobalTransformationMatrix, glm::vec3(), glm::quat(), pos, glm::vec3(), glm::vec4());

		//Set OOBB (which will set AABB)
		GetParent()->SetOOBBTransform(pos, m_Orientation, m_Scale);

		//Update childs' transform
		for (auto child : GetParent()->m_Childs)
			child->GetComponent<TransformComponent>()->UpdateTransform();		
	}
}