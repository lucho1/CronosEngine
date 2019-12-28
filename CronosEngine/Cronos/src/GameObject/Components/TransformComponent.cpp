#include"Providers/cnpch.h"
#include "TransformComponent.h"

#include "GameObject/GameObject.h"
#include "Renderer/GLRenderer3D.h"

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

	void TransformComponent::Update(float dt)
	{
		//Update Parents' AABBs in function of their childs' AABBs
		if (GetParent()->m_Childs.size() > 0)
		{
			math::AABB aabb;
			aabb.SetNegativeInfinity(); //What if I comment this??

			for (auto child : GetParent()->m_Childs)
				aabb.Enclose(child->GetAABB());

			if (GetParent()->GetComponent<MeshComponent>())
				aabb.Enclose(GetParent()->GetAABB());

			GetParent()->SetAABB(aabb);
		}
	}

	//Transform setters ---------------------------------------------------------------------------
	void TransformComponent::SetPosition(glm::vec3 position)
	{
		m_Translation = position;
		UpdateTransform();
	}

	void TransformComponent::SetScale(glm::vec3 scale)
	{
		m_Scale = scale;
		UpdateTransform();
	}

	//Set the orientation of the object (pass Euler Angles in degrees!!)
	void TransformComponent::SetOrientation(glm::vec3 euler_angles)
	{	
		glm::vec3 EA_Rad = glm::radians(euler_angles);
		glm::quat rot = glm::quat(EA_Rad - m_EulerAngles);
		
		m_Orientation = m_Orientation * rot;			
		m_EulerAngles = EA_Rad;
		UpdateTransform();
	}

	//Transform adders ----------------------------------------------------------------------------
	void TransformComponent::Rotate(glm::vec3 euler_angles)
	{
		glm::vec3 EA_Rad = glm::radians(euler_angles);
		glm::quat newOrientation = glm::quat(EA_Rad);

		m_Orientation = newOrientation;
		m_EulerAngles = EA_Rad;
		UpdateTransform();
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

	//Update Transform ------------------------------------------------------------------------------
	void TransformComponent::UpdateTransform()
	{
		//Update Local transform
		m_LocalTransformationMatrix = glm::translate(glm::mat4(1.0f), m_Translation) *
			glm::mat4_cast(m_Orientation) * glm::scale(glm::mat4(1.0f), m_Scale);

		//Update global transform
		GameObject* GOAttached_Parent = GetParent()->GetParentGameObject();
		if (GOAttached_Parent != nullptr)
			m_GlobalTransformationMatrix = GOAttached_Parent->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * m_LocalTransformationMatrix;
		else
			m_GlobalTransformationMatrix = m_LocalTransformationMatrix;	

		//Set OOBB transform (which will set AABB one) - Parents one will be set in update
		GetParent()->SetOOBBTransform(m_GlobalTransformationMatrix);

		//Update Octree
		if (GetParent())
		{
			App->renderer3D->RemoveFromTree(GetParent());
			App->renderer3D->InsertInTree(GetParent());
		}

		//Update childs' transform
		for (auto child : GetParent()->m_Childs)
			child->GetComponent<TransformComponent>()->UpdateTransform();
	}
}