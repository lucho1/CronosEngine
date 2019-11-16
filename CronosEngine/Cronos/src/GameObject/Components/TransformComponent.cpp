#include"Providers/cnpch.h"
#include "TransformComponent.h"
#include "GameObject/GameObject.h"

#include "Application.h"
//#include "MaterialComponent.h"

namespace Cronos {

	TransformComponent::TransformComponent(GameObject* attachedGO, bool active)
		: Component(ComponentType::TRANSFORM, attachedGO, active)
	{
		m_TransformationVector = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_TransformationMatrix = glm::mat4(1.0f);

		
	}

	TransformComponent::~TransformComponent()
	{

	}

	void TransformComponent::OnStart()
	{

	}

	void TransformComponent::Update(float dt)
	{
		m_TransformationVector = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_TransformationMatrix = glm::mat4(1.0f);

		m_TransformationVector = m_TransformationMatrix * m_TransformationVector;
		//App->scene->BasicTestShader->SetUniformMat4f("u_Model", m_TransformationMatrix);

		/*if (GetParent()->GetComponent<MaterialComponent>() != nullptr)
			GetParent()->GetComponent<MaterialComponent>()->m_ShaderAttached->SetUniformMat4f("u_Model", m_TransformationMatrix);*/

		

		//DrawCentralAxis();
		
	}

	void TransformComponent::DrawCentralAxis()
	{
		float linelength = 1.0f;
		glm::vec3 axis = GetCentralAxis();

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
	}
}