#include "Providers/cnpch.h"

#include "Application.h"
#include "EngineCamera.h"

#include "GameObject/Components/TransformComponent.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "mmgr/mmgr.h"

namespace Cronos {

	EngineCamera::EngineCamera(Application* app, bool start_enabled) : Module(app, "Module Camera 3D", start_enabled)
	{
	}

	EngineCamera::~EngineCamera()
	{
	}

	// -----------------------------------------------------------------
	bool EngineCamera::OnStart()
	{
		LOG("Setting up the camera");

		m_Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		m_ViewMatrix = m_ProjectionMatrix = glm::mat4(1.0f);

		m_Position = glm::vec3(0.0f, 3.0f, 5.0f);
		m_Target = glm::vec3(0.0f);

		Recalculate();		

		m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
		m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		m_Front = glm::vec3(0.0f, 0.0f, 1.0f);

		Look(m_Position, m_Target, false);
		return true;
	}

	bool EngineCamera::OnCleanUp()
	{
		LOG("Cleaning camera");
		return true;
	}

	update_status EngineCamera::OnUpdate(float dt)
	{
		if (App->EditorGUI->isHoveringWinGame())
		{
			if (App->input->isMouseScrolling())
				Zoom(dt);

			if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
				CameraPanning(dt);

			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
			{
				Move(dt);
				//m_Position = m_Target + Rotate(m_Position, m_Target);
				m_Position = m_Target + MouseRotation(m_Position, m_Target);
				LookAt(m_Target);
			}

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				//Rotate(m_Position, m_Target);
				m_Position = m_Target + MouseRotation(m_Position, m_Target);

				if (App->EditorGUI->GetCurrentGameObject() != nullptr && App->EditorGUI->GetCurrentGameObject()->isActive())
				{
					//glm::vec3 GO_refPos = App->EditorGUI->GetCurrentGameObject()->GetComponent<TransformComponent>()->GetCentralAxis();
					glm::vec3 GO_refPos = App->EditorGUI->GetCurrentGameObject()->GetComponent<TransformComponent>()->GetTranslation();
					LookAt(GO_refPos);
				}
				else
					LookAt(glm::vec3(0.0f));
			}

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_UP)
				Look(m_Position, m_Target, false);
		}
		
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			Focus();

		/*if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
			m_Rotation += 10.0f;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
			m_Rotation -= 10.0f;*/

		/*if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			float pitch = (float)App->input->GetMouseYMotion();
			float yaw = (float)App->input->GetMouseXMotion();

			PAng += glm::sin(glm::radians(pitch)) * glm::cos(glm::radians(yaw)) * 3.0f;
			YAng += glm::sin(glm::radians(yaw)) * 3.0f;

			m_Rotation = glm::quat(glm::vec3(-glm::radians(PAng), -glm::radians(YAng), 0.0f));
		}*/

		// Recalculate -------------
		Recalculate();
		return UPDATE_CONTINUE;
	}

	update_status EngineCamera::OnPostUpdate(float dt)
	{
		if (m_ChangeProj)
		{
			Recalculate();
			m_ChangeProj = false;
		}

		return UPDATE_CONTINUE;
	}

	// -----------------------------------------------------------------
	//glm::vec3 EngineCamera::Rotate(const glm::vec3& pos, const glm::vec3& ref)
	//{
	//	float dx = (float)App->input->GetMouseXMotion() * 0.01f;
	//	float dy = (float)-App->input->GetMouseYMotion() * 0.01f;

	//	//dx = glm::radians(dx);

	//	m_Orientation = glm::rotate(m_Orientation, dx, glm::vec3(0.0f, 1.0f, 0.0f));

	//	glm::vec3 axis = m_Target - m_Position;
	//	axis.y = 0;
	//	m_Orientation = glm::rotate(m_Orientation, dy, glm::normalize(axis));

	//	m_Orientation = glm::normalize(m_Orientation);


	//	//m_Front = glm::normalize(pos - target);
	//	//m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Front));
	//	//m_Up = glm::cross(m_Front, m_Right);



	//	return glm::vec3(0.0f);
	//}
	
	void EngineCamera::Move(float dt)
	{
		//Applying extra impulse if LShift is pressed
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN)
			m_MoveSpeed *= 2.0f;
		else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
			m_MoveSpeed /= 2.0f;

		//Movement --------------------------------------------
		glm::vec3 movement = glm::vec3(0.0f);

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			movement -= m_Front * m_MoveSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			movement += m_Front * m_MoveSpeed * dt;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			movement -= m_Right * m_MoveSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			movement += m_Right * m_MoveSpeed * dt;

		/*if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			movement -= glm::normalize(glm::cross(m_Front, m_Up)) * m_MoveSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			movement += glm::normalize(glm::cross(m_Front, m_Up)) * m_MoveSpeed * dt;*/


		//Take a look here and previous camera if Up movement doesn't work
		if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT)
			movement += m_Up * m_MoveSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT)
			movement -= m_Up * m_MoveSpeed * dt;

		m_Position += movement;
		m_Target += movement;
	}

	void EngineCamera::Zoom(float dt)
	{
		float scroll = m_ScrollSpeed * dt * 0.6f;
		glm::vec3 newPos(0.0f, 0.0f, 0.0f);

		if (App->input->GetMouseZ() > 0)
			newPos -= m_ScrollSpeed * m_Front;
		else
			newPos += m_ScrollSpeed * m_Front;

		m_Position += newPos;
		m_Target += newPos;
	}

	void EngineCamera::CameraPanning(float dt)
	{
		float xOffset = (float)App->input->GetMouseXMotion() * (m_MoveSpeed / 3.0f) * dt;
		float yOffset = (float)App->input->GetMouseYMotion() * (m_MoveSpeed / 3.0f) * dt;

		glm::vec3 newPos(0.0f, 0.0f, 0.0f);
		newPos += -m_Right * xOffset;
		newPos += m_Up * yOffset;
		//newPos.y += -App->input->GetMouseYMotion() / 2.0f * dt;

		m_Position += newPos;
		m_Target += newPos;
	}

	void EngineCamera::Focus()
	{
		if (App->EditorGUI->GetCurrentGameObject() != nullptr && App->EditorGUI->GetCurrentGameObject()->isActive())
		{
			//TODO: Properly perform the focus!
			TransformComponent* LookAtComp = App->EditorGUI->GetCurrentGameObject()->GetComponent<TransformComponent>();

			AABB GO_BB = LookAtComp->GetAABB();
			glm::vec3 LookPos = LookAtComp->GetCentralAxis();

			glm::vec3 size = GO_BB.getMax() - GO_BB.getMin();
			size *= 1.5f;

			glm::vec3 pos = glm::vec3(LookPos.x * size.x, LookPos.y + 1.5f, LookPos.z * size.z) + m_Front * 6.0f;
			glm::vec3 ref = glm::vec3(LookPos.x, LookPos.y + 0.5f, LookPos.z + 0.5f);

			Look(pos, ref, true);
		}
		else
			LookAt(glm::vec3(0.0f));
	}

	glm::vec3 EngineCamera::MouseRotation(const glm::vec3 & pos, const glm::vec3 & ref)
	{
		float dx = -App->input->GetMouseXMotion() * 0.005f;
		float dy = -App->input->GetMouseYMotion() * 0.005f;

		glm::vec3 refPos = pos - ref;
		refPos = glm::rotate(refPos, dx, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 refPosY = glm::rotate(refPos, dy, m_Right);

		float dotProd = glm::dot(glm::normalize(refPosY), glm::vec3(0.0f, 1.0f, 0.0f));
		if (dotProd > -0.95f && dotProd < 0.95f)
			refPos = refPosY;

		return refPos;
	}

	void EngineCamera::Look(const glm::vec3& pos, const glm::vec3& target, bool RotateAroundReference)
	{
		m_Position = pos;
		m_Target = target;

		m_Front = glm::normalize(pos - target);
		m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Front));
		m_Up = glm::cross(m_Front, m_Right);

		if (!RotateAroundReference)
		{
			m_Target = m_Position;
			m_Position += m_Front * 0.005f;
		}

		Recalculate();
	}

	void EngineCamera::LookAt(const glm::vec3& spot)
	{
		m_Target = spot;

		m_Front = glm::normalize(m_Position - m_Target);
		m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Front));
		m_Up = glm::cross(m_Front, m_Right);

		Recalculate();
	}

	// -----------------------------------------------------------------
	void EngineCamera::Recalculate()
	{
		glm::mat4 camTransform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::mat4_cast(m_Orientation);

		m_ViewMatrix = glm::inverse(camTransform);
		m_ViewMatrix = glm::lookAt(m_Position, m_Target, m_Up);

		if (m_FOV < MIN_FOV)
			m_FOV = MIN_FOV;
		else if (m_FOV > MAX_FOV)
			m_FOV = MAX_FOV;

		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV),
			(float)App->window->GetWidth() / (float)App->window->GetHeight(), m_NearPlane, m_FarPlane);
	}

	// -----------------------------------------------------------------
	void EngineCamera::SetFOV(float FOV)
	{
		m_FOV = FOV;
		Recalculate();
	}
	void EngineCamera::SetNearPlane(float nPlane)
	{
		m_NearPlane = nPlane;
		Recalculate();
	}
	void EngineCamera::SetFarPlane(float fPlane)
	{
		m_FarPlane = fPlane;
		Recalculate();
	}


	// -----------------------------------------------------------------
	//Save/Load
	void EngineCamera::SaveModuleData(json& JSONFile) const
	{
		JSONFile["EngineCamera"]["CameraMoveSpeed"] = m_MoveSpeed;
		JSONFile["EngineCamera"]["CameraScrollSpeed"] = m_ScrollSpeed;
		JSONFile["EngineCamera"]["FOV"] = m_FOV;
		JSONFile["EngineCamera"]["NearPlane"] = m_NearPlane;
		JSONFile["EngineCamera"]["FarPlane"] = m_FarPlane;

		JSONFile["EngineCamera"]["InitialPosition"][0] = m_Position.x;
		JSONFile["EngineCamera"]["InitialPosition"][1] = m_Position.y;
		JSONFile["EngineCamera"]["InitialPosition"][2] = m_Position.z;

		JSONFile["EngineCamera"]["InitialLookAt"][0] = m_Target.x;
		JSONFile["EngineCamera"]["InitialLookAt"][1] = m_Target.y;
		JSONFile["EngineCamera"]["InitialLookAt"][2] = m_Target.z;
	}

	void EngineCamera::LoadModuleData(json& JSONFile)
	{
		glm::vec3 initialPos = glm::vec3(	JSONFile["EngineCamera"]["InitialPosition"][0],
											JSONFile["EngineCamera"]["InitialPosition"][1],
											JSONFile["EngineCamera"]["InitialPosition"][2]);
		
		glm::vec3 initialRefence = glm::vec3(	JSONFile["EngineCamera"]["InitialLookAt"][0],
												JSONFile["EngineCamera"]["InitialLookAt"][1],
												JSONFile["EngineCamera"]["InitialLookAt"][2]);
	
		m_MoveSpeed = JSONFile["EngineCamera"]["CameraMoveSpeed"];
		m_ScrollSpeed = JSONFile["EngineCamera"]["CameraScrollSpeed"];
		m_FOV = JSONFile["EngineCamera"]["FOV"];
		m_NearPlane = JSONFile["EngineCamera"]["NearPlane"];
		m_FarPlane = JSONFile["EngineCamera"]["FarPlane"];
	
		m_Target = initialRefence;
		m_Position = initialPos;
	}
}
