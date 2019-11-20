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
		m_Rotation_InEulerAngles = glm::vec3(0.0f);
		m_Orientation = glm::quat(1.0f, glm::vec3(0.0f));
		m_Translation = glm::vec3(0.0f);
		m_Scale = glm::vec3(1.0f);

		m_TransformationMatrix = glm::mat4(1.0f);
		UpdateTransform();
	}

	TransformComponent::~TransformComponent()
	{
	}

	void TransformComponent::OnStart()
	{
	}

	void TransformComponent::Update(float dt)
	{
		App->scene->BasicTestShader->Bind();
		App->scene->BasicTestShader->SetUniformMat4f("u_Model", m_TransformationMatrix);
		App->scene->BasicTestShader->Unbind();
	}

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
	void TransformComponent::SetOrientation(glm::vec3 eulerAxAngles)
	{
		//eulerAxAngles = glm::radians(eulerAxAngles);
		glm::mat4 rotM = glm::yawPitchRoll(eulerAxAngles.y, eulerAxAngles.x, eulerAxAngles.z);

		rotMat = rotMat * rotM;
		UpdateTransform();
		//m_Rotation_InEulerAngles = glm::eulerAngles(glm::quat_cast(rotMat));
		//m_Rotation_InEulerAngles = glm::extractEulerAngleXYZ(rotMat);



		/*glm::vec3 euler_inRadians = glm::radians(eulerAxAngles);
		glm::quat newRot;
		double cy = cos(euler_inRadians.y * 0.5);
		double sy = sin(euler_inRadians.y * 0.5);
		double cp = cos(euler_inRadians.x * 0.5);
		double sp = sin(euler_inRadians.x * 0.5);
		double cr = cos(euler_inRadians.z * 0.5);
		double sr = sin(euler_inRadians.z * 0.5);
		newRot.w = cy * cp * cr + sy * sp * sr;
		newRot.x = cy * cp * sr - sy * sp * cr;
		newRot.y = sy * cp * sr + cy * sp * cr;
		newRot.z = sy * cp * cr - cy * sp * sr;

		m_Orientation = newRot * m_Orientation;
		UpdateTransform();*/
	}

	void TransformComponent::Rotate(glm::vec3 eulerAxAngles)
	{
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
		//m_TransformationMatrix = glm::translate(glm::mat4(1.0f), m_Translation) *
		//	glm::scale(glm::mat4(1.0f), m_Scale) * glm::mat4_cast(m_Orientation);

		m_TransformationMatrix = glm::translate(glm::mat4(1.0f), m_Translation) *
			glm::scale(glm::mat4(1.0f), m_Scale) * rotMat;
		//Set euler angles
		glm::quat q = m_Orientation;
		float pitch = -glm::atan(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x*q.x + q.y*q.y));
		float yaw = -glm::asin(2 * (q.w * q.y - q.z * q.x));
		float roll = -glm::atan(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y*q.y + q.z*q.z));

		glm::vec3 eu = glm::eulerAngles(q);

		//m_Rotation_InEulerAngles = glm::vec3(pitch, yaw, roll);
		//m_Rotation_InEulerAngles = eu;
	}
}