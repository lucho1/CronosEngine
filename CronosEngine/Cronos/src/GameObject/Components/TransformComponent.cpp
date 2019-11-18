#include"Providers/cnpch.h"
#include "TransformComponent.h"
#include "GameObject/GameObject.h"

#include "Application.h"
//#include "MaterialComponent.h"

namespace Cronos {

	TransformComponent::TransformComponent(GameObject* attachedGO, bool active)
		: Component(ComponentType::TRANSFORM, attachedGO, active)
	{
		m_TransformationMatrix = glm::mat4(1.0f);
		DecomposeTransformation();
	}

	TransformComponent::~TransformComponent()
	{
	}

	void TransformComponent::OnStart()
	{
	}

	void TransformComponent::Update(float dt)
	{
		//m_TransformationVector = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//m_TransformationMatrix = glm::mat4(1.0f);
		//
		//m_TransformationVector = m_TransformationMatrix * m_TransformationVector;

		App->scene->BasicTestShader->Bind();
		App->scene->BasicTestShader->SetUniformMat4f("u_Model", m_TransformationMatrix);
		App->scene->BasicTestShader->Unbind();

		if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT)
			Move(glm::vec3(1, 1, 0));
		if (App->input->GetKey(SDL_SCANCODE_K) == KEY_REPEAT)
			Scale(glm::vec3(2, 2, 2));

		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_REPEAT)
			SetPosition(glm::vec3(0, 0, 0));
		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT)
			SetScale(glm::vec3(1, 1, 1));

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

	void TransformComponent::SetPosition(glm::vec3 position)
	{
		m_TransformationMatrix = glm::translate(glm::mat4(1.0f), position);
		DecomposeTransformation();
	}

	void TransformComponent::SetScale(glm::vec3 scale)
	{
		m_TransformationMatrix = glm::scale(glm::mat4(1.0f), scale);
		DecomposeTransformation();
	}

	void TransformComponent::SetOrientation(glm::vec3 eulerAxAngles)
	{
		//glm::quat rot = glm::quat(eulerAxAngles);
		//glm::mat4 rotMat = glm::mat4(rot);
		glm::quat rot = glm::quat(eulerAxAngles);


		glm::mat4 transform = m_TransformationMatrix * glm::mat4_cast(rot);
		m_TransformationMatrix = transform;
		//m_TransformationMatrix *= rot;
		DecomposeTransformation();

		//transform = glm::translate(position) * glm::mat4_cast(rotation) * glm::scale(scale);
	}

	void TransformComponent::Move(glm::vec3 translation)
	{
		m_TransformationMatrix = glm::translate(m_TransformationMatrix, translation);
		DecomposeTransformation();
	}

	void TransformComponent::Scale(glm::vec3 scale)
	{
		m_TransformationMatrix = glm::scale(m_TransformationMatrix, scale);
		DecomposeTransformation();
	}

	void TransformComponent::Rotate(glm::vec3 eulerAxAngles)
	{
		//glm::quat myquaternion = glm::quat(glm::vec3(angle.x, angle.y, angle.z));
		glm::quat rot = glm::quat(eulerAxAngles);

		glm::mat4 transform = m_TransformationMatrix * glm::mat4_cast(rot);
		m_TransformationMatrix = transform;

		//m_TransformationMatrix *= rot;
		DecomposeTransformation();

	}

	void TransformComponent::DecomposeTransformation()
	{
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(m_TransformationMatrix, m_Scale, m_Orientation, m_Position, skew, perspective);
		m_Orientation = glm::conjugate(m_Orientation);

		m_Orientation_eulerAngles = glm::vec3(glm::asin(m_Orientation.x / 2), glm::asin(m_Orientation.y / 2), glm::asin(m_Orientation.z / 2));
	}
}