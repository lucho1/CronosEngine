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

		m_LocalTransformationMatrix = glm::mat4(1.0f);
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
	void TransformComponent::SetOrientation(glm::vec3 eulerAxAngles)
	{
		////eulerAxAngles = glm::radians(eulerAxAngles);
		//glm::mat4 rotM = glm::yawPitchRoll(eulerAxAngles.y, eulerAxAngles.x, eulerAxAngles.z);
		//
		//rotMat = rotMat * rotM;
		//UpdateTransform();
		//
		//return;

		//NEW TRY ----------------------------
		

		//if (glm::epsilonEqual(eulerAxAngles.y, 1.5f, 0.4f) || glm::epsilonEqual(1.5f, eulerAxAngles.y, 0.4f))
		//{
		//	//eulerAxAngles.y += 0.5f;

		//	//glm::mat4 YCorrectionMatrix = glm::yawPitchRoll(eulerAxAngles.y, eulerAxAngles.x, eulerAxAngles.z);
		//	////YCorrectionMatrix = glm::mat4_cast(m_Orientation);
		//	////glm::mat4 m = glm::mat4
		//	//
		//	//m_Orientation = glm::quat(YCorrectionMatrix);

		//	/*glm::quat YCorrection_q = glm::quat(YCorrectionMatrix);

		//	m_Orientation.y = YCorrection_q.y;*/

		//	glm::quat qPitch = glm::angleAxis(eulerAxAngles.x, glm::vec3(1, 0, 0));

		//	glm::quat qYaw = glm::angleAxis(eulerAxAngles.y, glm::vec3(0, 1, 0));
		//	//eulerAxAngles.y = 0.0f;
		//	glm::quat qRoll = glm::angleAxis(eulerAxAngles.z, glm::vec3(0, 0, 1));

		//	qPitch.y = qPitch.z = qYaw.x = qYaw.z = qRoll.x = qRoll.y = 0.0f;

		//	///x,y,z are in radians
		//	m_Orientation = qRoll * qYaw * qPitch;
		//}
		//else
		//	m_Orientation = glm::quat(eulerAxAngles);

		//if (glm::epsilonEqual(eulerAxAngles.y, 1.5f, 0.2f))
		//{
		//	/*glm::mat4 rotM = glm::yawPitchRoll(eulerAxAngles.y, eulerAxAngles.x, eulerAxAngles.z);
		//	rotMat = rotMat * rotM;
		//	m_Orientation = glm::quat(rotMat);*/
		//
		//	glm::mat4 expectedRot = glm::yawPitchRoll(eulerAxAngles.y, eulerAxAngles.x, eulerAxAngles.z);
		//
		//	m_Orientation = glm::quat(expectedRot);
		//
		//	//glm::quat convQuat = glm::quat(eulerAxAngles);
		//	//glm::mat4 actualRot = glm::mat4(convQuat);
		//
		//}
		//else
		//	m_Orientation = glm::quat(eulerAxAngles);

		

		//UpdateTransform();
		glm::quat qPitch = glm::angleAxis(eulerAxAngles.x, glm::vec3(1, 0, 0));
		
		glm::quat qYaw = glm::angleAxis(eulerAxAngles.y, glm::vec3(0, 1, 0));
		//eulerAxAngles.y = 0.0f;
		glm::quat qRoll = glm::angleAxis(eulerAxAngles.z, glm::vec3(0, 0, 1));
		//
		//qPitch.y = qPitch.z = qYaw.x = qYaw.z = qRoll.x = qRoll.y = 0.0f;
		//qYaw.x = qRoll.x = 0;
		//
		/////x,y,z are in radians
		//m_Orientation = qPitch * qYaw * qRoll;		
		
		
		//m_Orientation = glm::quat(eulerAxAngles);

		//NEW TRY ----------------------------

		//glm::quat qPitch = glm::angleAxis(eulerAxAngles.x, glm::vec3(1, 0, 0));
		//
		//glm::quat qYaw = glm::angleAxis(eulerAxAngles.y, glm::vec3(0, 1, 0));
		////eulerAxAngles.y = 0.0f;
		//glm::quat qRoll = glm::angleAxis(eulerAxAngles.z, glm::vec3(0, 0, 1));

		//qPitch.y = qPitch.z = qYaw.x = qYaw.z = qRoll.x = qRoll.y = 0.0f;

		/////x,y,z are in radians
		//m_Orientation = qYaw * qPitch * qRoll;
		//m_Orientation = rotQuat;
		//m_Orientation = qYaw * glm::quat(eulerAxAngles);

		//m_Orientation = qYaw * qPitch * qRoll;
		m_Orientation = qRoll * qYaw * qPitch; //"WORKS!"

		UpdateTransform();

		//------------------------------------
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
		//m_LocalTransformationMatrix = glm::translate(glm::mat4(1.0f), m_Translation) *
		//	rotMat * glm::scale(glm::mat4(1.0f), m_Scale);

		m_LocalTransformationMatrix = glm::translate(glm::mat4(1.0f), m_Translation) *
			glm::mat4_cast(m_Orientation) * glm::scale(glm::mat4(1.0f), m_Scale);
		
		m_Rotation_InEulerAngles = glm::eulerAngles(m_Orientation);

		//m_Rotation_InEulerAngles = ConvertMatToEulerAnglesXYZ(rotMat);


		//Set euler angles
		//glm::quat q = m_Orientation;
		//float pitch = glm::atan(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x*q.x + q.y*q.y));
		//float yaw = glm::asin(2 * (q.w * q.y - q.z * q.x));
		//float roll = glm::atan(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y*q.y + q.z*q.z));

		//m_Rotation_InEulerAngles = glm::vec3(pitch, yaw, roll);

		//float pitch = glm::atan(2 * q.y*q.w - 2 * q.x*q.z, 1 - 2 * q.y*q.y - 2 * q.z*q.z);
		//float yaw = glm::asin(2 * (q.x * q.y + q.z * q.w));
		//float roll = glm::atan(2*q.x*q.w - 2*q.y*q.z, 1-2*q.x*q.x - 2*q.z*q.z);


		/*if (m_Rotation_InEulerAngles.y > 1.50f && m_Rotation_InEulerAngles.y < 1.55f)
			m_Rotation_InEulerAngles.y = 1.60f;*/

		//glm::quat q2 = glm::quat(rotMat);
		//glm::vec3 eu = glm::eulerAngles(q2);

		//m_Rotation_InEulerAngles = glm::vec3(pitch, yaw, roll);


		// yaw (z-axis rotation)
		/*float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		float yaw = std::atan2(siny_cosp, cosy_cosp);*/

		//m_Rotation_InEulerAngles.y = yaw;


		//m_Rotation_InEulerAngles = eu;


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