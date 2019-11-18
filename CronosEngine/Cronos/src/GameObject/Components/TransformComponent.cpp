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
		m_Orientation = glm::quat(1.0f, glm::vec3(0.0f));
		m_Position = m_Orientation_eulerAngles = glm::vec3(0.0f);
		m_Scale = glm::vec3(1.0f);
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

		if (App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
			Move(glm::vec3(0.3, 0, 0));
		if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
			Scale(glm::vec3(1.2, 1.2, 1.2));

		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
			SetPosition(glm::vec3(0, 0, 0));
		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
			SetScale(glm::vec3(1, 1, 1));

		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
			SetOrientation(glm::vec3(20.00, 20.00, 20.00));
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			Rotate(glm::vec3(20.00, 20.00, 20.00));

		//if (App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT)
		//	Rotate(glm::vec3(1, 1, 1));

		//if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		//	SetOrientation(glm::vec3(-90, 0, 0));

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
		glm::vec3 resPos = position - m_Position;
		m_TransformationMatrix = glm::translate(m_TransformationMatrix, resPos);
		DecomposeTransformation();
	}

	void TransformComponent::SetScale(glm::vec3 scale)
	{
		glm::vec3 resScale = scale - m_Scale;
		if (glm::any(glm::lessThanEqual(glm::abs(resScale), glm::vec3(glm::epsilon<float>()))))
			return;
		
		m_TransformationMatrix = glm::scale(m_TransformationMatrix, resScale);
		DecomposeTransformation();
	}

	//Set the orientation of the object (pass Euler Angles in degrees!!)
	void TransformComponent::SetOrientation(glm::vec3 eulerAxAngles)
	{
		glm::float1 a = 1.2345678;
		glm::precision(0);
		a += 2.345;

		std::cout << a;

		glm::vec3 euler_inRadians = glm::radians(eulerAxAngles);
		glm::mat4 mat = glm::eulerAngleXYZ(euler_inRadians.x, euler_inRadians.y, euler_inRadians.z);
		glm::quat rot = glm::quat(mat);

		glm::quat resRot = rot * m_Orientation;
		glm::mat4 transform = m_TransformationMatrix * glm::mat4_cast(resRot);
		m_TransformationMatrix = transform;
		DecomposeTransformation();
	}

	void TransformComponent::Move(glm::vec3 translation)
	{
		m_TransformationMatrix = glm::translate(m_TransformationMatrix, translation);
		DecomposeTransformation();
	}

	void TransformComponent::Scale(glm::vec3 scale)
	{
		if (glm::any(glm::lessThanEqual(glm::abs(scale), glm::vec3(glm::epsilon<float>()))))
			return;

		m_TransformationMatrix = glm::scale(m_TransformationMatrix, scale);
		DecomposeTransformation();
	}

	void TransformComponent::Rotate(glm::vec3 eulerAxAngles)
	{
		glm::quat rot = glm::quat(glm::radians(eulerAxAngles));
		glm::mat4 transform = m_TransformationMatrix * glm::mat4_cast(rot);
		m_TransformationMatrix = transform;
		DecomposeTransformation();
	}

	void TransformComponent::DecomposeTransformation()
	{
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(m_TransformationMatrix, m_Scale, m_Orientation, m_Position, skew, perspective);
		m_Orientation = glm::conjugate(m_Orientation);

		glm::quat q = m_Orientation;

		float pitch = glm::atan(2*(q.w * q.x + q.y * q.z), 1-2*(q.x*q.x + q.y*q.y));
		float yaw = glm::asin(2*(q.w * q.y - q.z * q.x));
		float roll = glm::atan(2*(q.w * q.z + q.x * q.y), 1-2*(q.y*q.y + q.z*q.z));

		glm::vec3 Test = glm::eulerAngles(q);

		m_Orientation_eulerAngles = glm::vec3(pitch, yaw, roll);

	}
}