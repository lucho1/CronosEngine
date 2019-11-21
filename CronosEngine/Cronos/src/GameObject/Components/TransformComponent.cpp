#include"Providers/cnpch.h"
#include "TransformComponent.h"
#include "GameObject/GameObject.h"

#include "Application.h"

#include <glm/gtx//euler_angles.hpp>
//#include "MaterialComponent.h"

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
		App->scene->BasicTestShader->Bind();
		App->scene->BasicTestShader->SetUniformMat4f("u_Model", m_GlobalTransformationMatrix);
		App->scene->BasicTestShader->Unbind();
	}

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
		m_LocalTransformationMatrix = glm::translate(glm::mat4(1.0f), m_Translation) *
			glm::mat4_cast(m_Orientation) * glm::scale(glm::mat4(1.0f), m_Scale);

		//Update childs' transform
		GameObject* GOAttached_Parent = GetParent()->GetParentGameObject();
		if (GOAttached_Parent != nullptr)
			m_GlobalTransformationMatrix = GOAttached_Parent->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * m_LocalTransformationMatrix;
		else
			m_GlobalTransformationMatrix = m_LocalTransformationMatrix;

		for (auto child : GetParent()->m_Childs)
			child->GetComponent<TransformComponent>()->UpdateTransform();
	}

	//Central Axis
	void TransformComponent::DrawCentralAxis()
	{
		float linelength = 1.0f;
		glm::vec3 axis = GetCentralAxis();

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));

		glLineWidth(5.0f);
		glBegin(GL_LINES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(axis.x, axis.y, axis.z);
		glVertex3f(axis.x + linelength, axis.y, axis.z);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(axis.x, axis.y, axis.z);
		glVertex3f(axis.x, axis.y + linelength, axis.z);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(axis.x, axis.y, axis.z);
		glVertex3f(axis.x, axis.y, axis.z + linelength);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnd();
		glLineWidth(2.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
}