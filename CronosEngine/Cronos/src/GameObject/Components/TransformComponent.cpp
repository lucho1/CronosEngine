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
		m_Position = m_Orientation_eulerAnglesRAD = m_Orientation_eulerAnglesDEG = glm::vec3(0.0f);
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
		/*if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
			Scale(glm::vec3(1.2, 1.2, 1.2));*/

		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
			SetPosition(glm::vec3(0, 0, 0));
		//if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
		//	SetScale(glm::vec3(1, 1, 1));

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
		/*glm::vec3 resEuler = eulerAxAngles - m_Orientation_eulerAnglesDEG;
		glm::bvec3 vec = glm::lessThanEqual(glm::abs(resEuler), glm::vec3(glm::epsilon<float>()));

		if (glm::all(vec) || eulerAxAngles == m_Orientation_eulerAnglesDEG)
			return;

		glm::vec3 euler_inRadians = glm::radians(eulerAxAngles);

		float xd = (float)((int)(euler_inRadians.x * 100+.5))/100;

		if (vec.x)
			euler_inRadians.x = m_Orientation_eulerAnglesDEG.x;
		else
			euler_inRadians.x = (float)((int)(euler_inRadians.x * 100 + .5)) / 100;
		if (vec.y)
			euler_inRadians.y = m_Orientation_eulerAnglesDEG.y;
		else
			euler_inRadians.y = (float)((int)(euler_inRadians.y * 100 + .5)) / 100;
		if (vec.z)
			euler_inRadians.z = m_Orientation_eulerAnglesDEG.z;
		else
			euler_inRadians.z = (float)((int)(euler_inRadians.z * 100 + .5)) / 100;


		if (euler_inRadians == m_Orientation_eulerAnglesRAD)
			return;*/

		
		/*glm::mat4 mat = glm::eulerAngleXYZ(euler_inRadians.x, euler_inRadians.y, euler_inRadians.z);

		glm::quat rot = glm::rotate(m_Orientation, euler_inRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rot = glm::rotate(rot, euler_inRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rot = glm::rotate(rot, euler_inRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));*/



		//glm::quat resRot = rot * m_Orientation;
		glm::vec3 increase = glm::radians(eulerAxAngles - m_Orientation_eulerAnglesDEG);


		glm::quat q1 = glm::quat(increase.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat q2 = glm::quat(increase.y, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat q3 = glm::quat(increase.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::quat rot = q1*q2*q3;

		/*
		Quaternion QuatAroundX = Quaternion( Vector3(1.0,0.0,0.0), EulerAngle.x );
Quaternion QuatAroundY = Quaternion( Vector3(0.0,1.0,0.0), EulerAngle.y );
Quaternion QuatAroundZ = Quaternion( Vector3(0.0,0.0,1.0), EulerAngle.z );
Quaternion finalOrientation = QuatAroundX * QuatAroundY * QuatAroundZ;
		*/

		glm::quat res = rot * m_Orientation;

		glm::mat4 mat = glm::mat4(1.0f) * glm::mat4_cast(rot);

		glm::mat4 transform = m_TransformationMatrix * mat;
		//glm::vec3 euler_inRadians = glm::radians(eulerAxAngles);
		//
		//glm::mat4 mat = glm::eulerAngleXYZ(euler_inRadians.x, euler_inRadians.y, euler_inRadians.z);
		//glm::mat4 transform = m_TransformationMatrix * mat;
		//glm::mat4 transform = m_TransformationMatrix * glm::mat4_cast(rot);
		m_TransformationMatrix = transform;
		DecomposeTransformation();
	}

	void TransformComponent::Rotate(glm::vec3 eulerAxAngles)
	{
		glm::quat rot = glm::quat(glm::radians(eulerAxAngles));
		glm::mat4 transform = m_TransformationMatrix * glm::mat4_cast(rot);
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

	void TransformComponent::DecomposeTransformation()
	{
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(m_TransformationMatrix, m_Scale, m_Orientation, m_Position, skew, perspective);
		m_Orientation = glm::conjugate(m_Orientation);

		//Set euler angles
		glm::quat q = m_Orientation;
		float pitch = -glm::atan(2 * (q.w * q.x + q.y * q.z), 1 - 2 * (q.x*q.x + q.y*q.y));
		float yaw = -glm::asin(2 * (q.w * q.y - q.z * q.x));
		float roll = -glm::atan(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y*q.y + q.z*q.z));

		if (glm::abs(pitch - m_Orientation_eulerAnglesRAD.x) <= glm::epsilon<float>() == false)
		{
			m_Orientation_eulerAnglesRAD.x = (float)((int)(pitch * 100 + .5) / 100);
			float roundedVal = (float)((int)(glm::degrees(pitch) * 100 + .5) / 100);
			m_Orientation_eulerAnglesDEG.x = roundedVal;
		}
		if (glm::abs(yaw - m_Orientation_eulerAnglesRAD.y) <= glm::epsilon<float>() == false)
		{
			m_Orientation_eulerAnglesRAD.y = (float)((int)(yaw * 100 + .5) / 100);
			float roundedVal = (float)((int)(glm::degrees(yaw) * 100 + .5) / 100);
			m_Orientation_eulerAnglesDEG.y = roundedVal;
		}
		if (glm::abs(roll - m_Orientation_eulerAnglesRAD.z) <= glm::epsilon<float>() == false)
		{
			m_Orientation_eulerAnglesRAD.z = (float)((int)(roll * 100 + .5) / 100);
			float roundedVal = (float)((int)(glm::degrees(roll) * 100 + .5) / 100);
			m_Orientation_eulerAnglesDEG.z = roundedVal;
		}
	}
}