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

		Look(m_Position, m_Target, true);
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

			static glm::vec3 prevTarget = m_Target;
			if (changeTarget)
				prevTarget = m_Target;

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				changeTarget = false;

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
			{
				m_Target = prevTarget;
				changeTarget = true;
			}
			
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

		//ORBIT
		//m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
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
	//void EngineCamera::CalculateMouseRotation(const glm::vec3& pos, const glm::vec3& ref)
	//{
	//	// Mouse motion ----------------

	//	/*glm::vec3 dir;

	//	glm::vec3 movement = pos - ref;*/

	//	//m_viewMat = glm::rotate(m_viewMat, (float)App->input->GetMouseXMotion(), glm::vec3(1.0f));

	//	float pitch = (float)App->input->GetMouseXMotion(), yaw = (float)App->input->GetMouseYMotion();

	//	m_PitchAngle += pitch;
	//	m_YawAngle += yaw;

	//	UpdateVectors();
	//	//CalculateViewMatrix();

	//	//glm::vec3 a = m_viewMat[3];
	//	//return glm::vec3(0.0f);

	//	/*dir.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
	//	dir.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
	//	dir.y = glm::sin(glm::radians(pitch));

	//	glm::rotate(m_viewMat, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	//	glm::rotate(m_viewMat, yaw, glm::vec3(0.0f, 0.0f, 1.0f));
	//	m_Pos = m_viewMat[3];
	//	CalculateViewMatrix();*/

	//	//return dir;

	//	// There is another way to do it in Physics handout 1
	//	// Applying horizontal orbit
	//	//glm::vec3 ref_pos = m_Pos - ref;
	//	//ref_pos = rotate(m_viewMat, (float)-App->input->GetMouseXMotion() * 0.5f, vec3(0.0f, 1.0f, 0.0f));
	//	//
	//	//glm::rotate(m_viewMat, (float)-App->input->GetMouseXMotion(), glm::vec3(0.0f, 1.0f, 0.0f));

	//	//// Applying vertical orbit
	//	//glm::vec3 ref_pos_dy = glm::rotate(ref_pos, -App->input->GetMouseYMotion() * 0.5f, m_Right);

	//	//// We check the vertical orbit to not do a complete orbit
	//	//float dotProd = dot(normalize(ref_pos_dy), glm::vec3(0.0f, 1.0f, 0.0f));
	//	//if (dotProd > -0.95f && dotProd < 0.95f)
	//	//	ref_pos = ref_pos_dy;

	//	//return ref_pos;
	//}


	//// -----------------------------------------------------------------
	//void EngineCamera::OrbitAroundReference(const glm::vec3 &Reference)
	//{
	//	//m_Target = Reference;

	//	//m_Direction = glm::normalize(m_Pos - m_Target);
	//	////m_Front = glm::normalize(m_Pos - m_Target);
	//	//m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
	//	//m_Up = glm::cross(m_Direction, m_Right);

	//	///*m_Reference = Reference;

	//	//m_Z = normalize(m_Position - Reference);
	//	//m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
	//	//m_Y = cross(m_Z, m_X);*/

	//	//CalculateViewMatrix();
	//}


	//// -----------------------------------------------------------------
	//void EngineCamera::Look(const glm::vec3 &Position, const glm::vec3 &Reference, bool RotateAroundReference)
	//{
	//	//m_Pos = Position;
	//	//m_Target = Reference;

	//	//m_Direction = glm::normalize(m_Pos - m_Target);
	//	//m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
	//	//m_Up = glm::cross(m_Direction, m_Right);

	//	//if (!RotateAroundReference)
	//	//{
	//	//	m_Target = m_Pos;
	//	//	m_Pos += m_Direction * 0.05f;
	//	//}

	//	///*m_Position = Position;
	//	//m_Reference = Reference;

	//	//m_Z = normalize(Position - Reference);
	//	//m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
	//	//m_Y = cross(m_Z, m_X);

	//	//if (!RotateAroundReference)
	//	//{
	//	//	m_Reference = m_Position;
	//	//	m_Position += m_Z * 0.05f;
	//	//}*/

	//	//CalculateViewMatrix();
	//}

	//// -----------------------------------------------------------------
	//void EngineCamera::LookAt(const glm::vec3 &Spot)
	//{
	//	//m_Target = Spot;

	//	//m_Direction = glm::normalize(m_Pos - m_Target);
	//	//m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
	//	//m_Up = glm::cross(m_Direction, m_Right);


	//	///*m_Reference = Spot;

	//	//m_Z = normalize(m_Position - m_Reference);
	//	//m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
	//	//m_Y = cross(m_Z, m_X);*/

	//	//CalculateViewMatrix();
	//}


	//// -----------------------------------------------------------------
	//void EngineCamera::Move(const glm::vec3 &Movement)
	//{
	//	//m_Pos += Movement;
	//	//m_Target += Movement;

	//	////m_Position += Movement;
	//	////m_Reference += Movement;

	//	//CalculateViewMatrix();
	//}

	//// -----------------------------------------------------------------
	//void EngineCamera::Zoom(float dt) {

	//	glm::vec3 newPos(0.0f, 0.0f, 0.0f);

	//	if (App->input->GetMouseZ() > 0)
	//		newPos -= m_CameraScrollSpeed * m_Front * dt;
	//	else
	//		newPos += m_CameraScrollSpeed * m_Front * dt;

	//	m_Pos += newPos;
	//	//m_Position += newPos;
	//}

	//void EngineCamera::UpdateVectors()
	//{
	//	glm::vec3 front;
	//	front.x = cos(glm::radians(m_YawAngle)) * cos(glm::radians(m_PitchAngle));
	//	front.y = sin(glm::radians(m_PitchAngle));
	//	front.z = sin(glm::radians(m_YawAngle)) * cos(glm::radians(m_PitchAngle));

	//	m_Front = glm::normalize(front);

	//	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	//	m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	//	CalculateViewMatrix();
	//}

	//// -----------------------------------------------------------------
	//void EngineCamera::CalculateViewMatrix()
	//{
	//	/*m_ViewMatrix = mat4x4(	m_X.x, m_Y.x, m_Z.x, 0.0f,
	//							m_X.y, m_Y.y, m_Z.y, 0.0f,
	//							m_X.z, m_Y.z, m_Z.z, 0.0f,
	//							-dot(m_X, m_Position), -dot(m_Y, m_Position), -dot(m_Z, m_Position), 1.0f);

	//	m_ViewMatrixInverse = inverse(m_ViewMatrix);*/
	//	

	//	m_viewMat = glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
	//	m_viewMat = glm::inverse(m_viewMat);

	//	/*if (App->scene->BasicTestShader == nullptr)
	//		return;*/

	//	if (App != nullptr && App->scene->BasicTestShader != nullptr)
	//		App->scene->BasicTestShader->SetUniformMat4f("u_View", m_viewMat);

	//	//m_viewMat = glm::lookAt(m_Pos, m_Direction, m_Up);
	//}

	//// -----------------------------------------------------------------
	//void EngineCamera::CalculateProjection()
	//{
	//	if (App->scene->BasicTestShader == nullptr)
	//		return;

	//	if (m_FOV < MIN_FOV)
	//		m_FOV = MIN_FOV;
	//	else if (m_FOV > MAX_FOV)
	//		m_FOV = MAX_FOV;

	//	else if (m_FOV >= MIN_FOV && m_FOV <= MAX_FOV) {

	//		m_ProjMat = glm::perspective(m_FOV, App->window->GetAspectRatio(), m_NearPlane, m_FarPlane);
	//		App->scene->BasicTestShader->SetUniformMat4f("u_Proj", m_ProjMat);

	//		//glViewport(0, 0, (GLint)App->window->GetWindowWidth();, (GLint)App->window->GetWindowHeight()); //If something with the proj.mat. goes wrong, uncomment this.

	//		//glMatrixMode(GL_PROJECTION);
	//		//glLoadIdentity();

	//		////m_ProjMat = glm::perspective(m_FOV, App->window->GetAspectRatio(), m_NearPlane, m_FarPlane);

	//		////glm::value_ptr(m_ProjMat);

	//		//glLoadMatrixf(glm::value_ptr(m_ProjMat));


	//		////m_ProjectionMatrix = perspective(m_FOV, App->window->GetAspectRatio(), m_NearPlane, m_FarPlane);
	//		////glLoadMatrixf(&m_ProjectionMatrix);

	//		//glMatrixMode(GL_MODELVIEW);
	//		//glLoadIdentity();
	//	}
	//}

	//// -----------------------------------------------------------------
	////Save/Load
	//void EngineCamera::SaveModuleData(json& JSONFile) const
	//{
	//	JSONFile["EngineCamera"]["CameraMoveSpeed"] = m_CameraMoveSpeed;
	//	JSONFile["EngineCamera"]["CameraScrollSpeed"] = m_CameraScrollSpeed;
	//	JSONFile["EngineCamera"]["FOV"] = m_FOV;
	//	JSONFile["EngineCamera"]["NearPlane"] = m_NearPlane;
	//	JSONFile["EngineCamera"]["FarPlane"] = m_FarPlane;

	//	JSONFile["EngineCamera"]["InitialPosition"][0] = m_Pos.x;
	//	JSONFile["EngineCamera"]["InitialPosition"][1] = m_Pos.y;
	//	JSONFile["EngineCamera"]["InitialPosition"][2] = m_Pos.z;

	//	//JSONFile["EngineCamera"]["InitialLookAt"][0] = m_Target.x;
	//	//JSONFile["EngineCamera"]["InitialLookAt"][1] = m_Target.y;
	//	//JSONFile["EngineCamera"]["InitialLookAt"][2] = m_Target.z;
	//}

	//void EngineCamera::LoadModuleData(json& JSONFile)
	//{
	//	//vec3 initialPos = vec3(	JSONFile["EngineCamera"]["InitialPosition"][0],
	//	//						JSONFile["EngineCamera"]["InitialPosition"][1],
	//	//						JSONFile["EngineCamera"]["InitialPosition"][2]);
	//	//
	//	//vec3 initialRefence = vec3(	JSONFile["EngineCamera"]["InitialLookAt"][0],
	//	//							JSONFile["EngineCamera"]["InitialLookAt"][1],
	//	//							JSONFile["EngineCamera"]["InitialLookAt"][2]);

	//	m_CameraMoveSpeed = JSONFile["EngineCamera"]["CameraMoveSpeed"];
	//	m_CameraScrollSpeed = JSONFile["EngineCamera"]["CameraScrollSpeed"];
	//	m_FOV = JSONFile["EngineCamera"]["FOV"];
	//	m_NearPlane = JSONFile["EngineCamera"]["NearPlane"];
	//	m_FarPlane = JSONFile["EngineCamera"]["FarPlane"];

	//	//m_Reference = initialRefence;
	//	//m_Position = initialPos;
	//}
}
