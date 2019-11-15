#include "Providers/cnpch.h"

#include "Providers/Globals.h"
#include "Application.h"
#include "EngineCamera.h"

#include "GameObject/Components/TransformComponent.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	EngineCamera::EngineCamera(Application* app, bool start_enabled) : Module(app, "Module Camera 3D", start_enabled)
	{
		m_Pos = glm::vec3(0.0f, 3.0f, 5.0f);
		m_Target = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
		
		m_Direction = glm::normalize(m_Pos - m_Target);
		

		m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
		m_Up = glm::cross(m_Direction, m_Right);

		m_viewMat = glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
		//m_viewMat = glm::lookAt(m_Pos, m_Direction, m_Up);


		/*m_X = vec3(1.0f, 0.0f, 0.0f);
		m_Y = vec3(0.0f, 1.0f, 0.0f);
		m_Z = vec3(0.0f, 0.0f, 1.0f);

		m_Position = vec3(0.0f, 3.0f, 5.0f);
		m_Reference = vec3(0.0f, 0.0f, 0.0f);*/

		CalculateViewMatrix();
	}

	EngineCamera::~EngineCamera()
	{}

	// -----------------------------------------------------------------
	bool EngineCamera::OnStart()
	{
		LOG("Setting up the camera");
		SetFOV(m_FOV);
		SetNearPlane(m_NearPlane);
		SetFarPlane(m_FarPlane);

		Look(m_Pos, m_Target, true);
		return true;
	}

	// -----------------------------------------------------------------
	bool EngineCamera::OnCleanUp()
	{
		LOG("Cleaning camera");
		return true;
	}

	// -----------------------------------------------------------------
	update_status EngineCamera::OnUpdate(float dt)
	{

		if (App->EditorGUI->isHoveringWinGame()) {


			if (App->input->isMouseScrolling())
				Zoom(dt);

			if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
			{
				glm::vec3 newPos(0.0f, 0.0f, 0.0f);
				newPos += -m_Right * (float)App->input->GetMouseXMotion() * dt * m_CameraMoveSpeed/2.0f;
				newPos += m_Up * (float)App->input->GetMouseYMotion() * dt * m_CameraMoveSpeed/2.0f;
				//newPos.y += -App->input->GetMouseYMotion() / 2.0f * dt;

				m_Pos += newPos;
				m_Target += newPos;
			}

			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {

				//m_CameraMoveSpeed *= dt;

				//Aplying extra impulse if LShift is pressed
				if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN)
					m_CameraMoveSpeed *= 2.0f;
				else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
					m_CameraMoveSpeed /= 2.0f;

				// Camera Movement On Keys WASD + Right Mouse Button ------------------------------
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) m_Pos += m_CameraMoveSpeed * m_Front * dt;
				if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) m_Pos -= m_CameraMoveSpeed * m_Front * dt;
				if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) m_Pos -= glm::normalize(glm::cross(m_Front, m_Up)*m_CameraMoveSpeed) * dt;
				if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) m_Pos += glm::normalize(glm::cross(m_Front, m_Up)*m_CameraMoveSpeed) * dt;
				//glm::vec3 newPos(0.0f, 0.0f, 0.0f);
				//if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) newPos.y += m_CameraMoveSpeed * dt;
				//if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) newPos.y -= m_CameraMoveSpeed * dt;

				//// Note that the vectors m_X/m_Y/m_Z contain the current axis of the camera
				//if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= m_Direction * m_CameraMoveSpeed * dt;
				//if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += m_Direction * m_CameraMoveSpeed * dt;

				//if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= m_Right * m_CameraMoveSpeed * dt;
				//if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += m_Right * m_CameraMoveSpeed * dt;

				//m_Pos += newPos;
				//m_Target += newPos;

				//// Moving camera position orbiting
				////m_Pos = m_Target + CalculateMouseRotation(m_Pos, m_Target);

				//// Finally, we look to the reference
				//LookAt(m_Target);
			}

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				// Orbiting around center --- CalculateMouseRotation() is the reference postion
				//m_Pos = m_Target + CalculateMouseRotation(m_Pos, m_Target);
				
				if (App->EditorGUI->GetCurrentGameObject() != nullptr && App->EditorGUI->GetCurrentGameObject()->isActive())
				{
					glm::vec3 GO_refPos = App->EditorGUI->GetCurrentGameObject()->GetComponent<TransformComponent>()->GetCentralAxis();
					OrbitAroundReference(GO_refPos);
				}
				else
					OrbitAroundReference(glm::vec3(0.0f));
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			if (App->EditorGUI->GetCurrentGameObject() != nullptr && App->EditorGUI->GetCurrentGameObject()->isActive())
			{
				AABB GO_BB = App->EditorGUI->GetCurrentGameObject()->GetComponent<TransformComponent>()->GetAABB();
				glm::vec3 posToLook = App->EditorGUI->GetCurrentGameObject()->GetComponent<TransformComponent>()->GetCentralAxis();

				glm::vec3 size = GO_BB.getMax() - GO_BB.getMin();
				size *= 1.5f;

				glm::vec3 pos = glm::vec3(posToLook.x * size.x, posToLook.y + 1.5f, posToLook.z * size.z) + m_Direction * 6.0f;
				glm::vec3 ref = glm::vec3(posToLook.x, posToLook.y + 0.5f, posToLook.z + 0.5f);

				Look(pos, ref, true);
			}
			else
				LookAt(glm::vec3(0.0f));
		}

		// Recalculate matrix -------------
		CalculateViewMatrix();
		return UPDATE_CONTINUE;
	}

	// -----------------------------------------------------------------
	//const glm::vec3 EngineCamera::CalculateMouseRotation(const glm::vec3& pos, const glm::vec3& ref)
	//{
	//	// Mouse motion ----------------
	//	// There is another way to do it in Physics handout 1
	//	// Applying horizontal orbit
	//	glm::vec3 ref_pos = m_Pos - ref;
	//	ref_pos = rotate(ref_pos, -App->input->GetMouseXMotion() * 0.5f, vec3(0.0f, 1.0f, 0.0f));

	//	// Applying vertical orbit
	//	glm::vec3 ref_pos_dy = glm::rotate(ref_pos, -App->input->GetMouseYMotion() * 0.5f, m_Right);

	//	// We check the vertical orbit to not do a complete orbit
	//	float dotProd = dot(normalize(ref_pos_dy), glm::vec3(0.0f, 1.0f, 0.0f));
	//	if (dotProd > -0.95f && dotProd < 0.95f)
	//		ref_pos = ref_pos_dy;

	//	return ref_pos;
	//}


	// -----------------------------------------------------------------
	void EngineCamera::Look(const glm::vec3 &Position, const glm::vec3 &Reference, bool RotateAroundReference)
	{
		m_Pos = Position;
		m_Target = Reference;

		m_Direction = glm::normalize(m_Pos - m_Target);
		m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
		m_Up = glm::cross(m_Direction, m_Right);

		if (!RotateAroundReference)
		{
			m_Target = m_Pos;
			m_Pos += m_Direction * 0.05f;
		}

		/*m_Position = Position;
		m_Reference = Reference;

		m_Z = normalize(Position - Reference);
		m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
		m_Y = cross(m_Z, m_X);

		if (!RotateAroundReference)
		{
			m_Reference = m_Position;
			m_Position += m_Z * 0.05f;
		}*/

		CalculateViewMatrix();
	}

	// -----------------------------------------------------------------
	void EngineCamera::LookAt(const glm::vec3 &Spot)
	{
		m_Target = Spot;

		m_Direction = glm::normalize(m_Pos - m_Target);
		m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
		m_Up = glm::cross(m_Direction, m_Right);


		/*m_Reference = Spot;

		m_Z = normalize(m_Position - m_Reference);
		m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
		m_Y = cross(m_Z, m_X);*/

		CalculateViewMatrix();
	}


	// -----------------------------------------------------------------
	void EngineCamera::Move(const glm::vec3 &Movement)
	{
		m_Pos += Movement;
		m_Target += Movement;

		//m_Position += Movement;
		//m_Reference += Movement;

		CalculateViewMatrix();
	}

	// -----------------------------------------------------------------
	void EngineCamera::Zoom(float dt) {

		glm::vec3 newPos(0.0f, 0.0f, 0.0f);

		if (App->input->GetMouseZ() > 0)
			newPos -= m_CameraScrollSpeed * m_Direction * dt;
		else
			newPos += m_CameraScrollSpeed * m_Direction * dt;

		m_Pos += newPos;
		//m_Position += newPos;
	}

	// -----------------------------------------------------------------
	void EngineCamera::OrbitAroundReference(const glm::vec3 &Reference)
	{
		m_Target = Reference;

		m_Direction = glm::normalize(m_Pos - m_Target);
		m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Direction));
		m_Up = glm::cross(m_Direction, m_Right);

		/*m_Reference = Reference;

		m_Z = normalize(m_Position - Reference);
		m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
		m_Y = cross(m_Z, m_X);*/

		CalculateViewMatrix();
	}


	// -----------------------------------------------------------------
	void EngineCamera::CalculateViewMatrix()
	{
		/*m_ViewMatrix = mat4x4(	m_X.x, m_Y.x, m_Z.x, 0.0f,
								m_X.y, m_Y.y, m_Z.y, 0.0f,
								m_X.z, m_Y.z, m_Z.z, 0.0f,
								-dot(m_X, m_Position), -dot(m_Y, m_Position), -dot(m_Z, m_Position), 1.0f);

		m_ViewMatrixInverse = inverse(m_ViewMatrix);*/


		m_viewMat = glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);

		//m_viewMat = glm::lookAt(m_Pos, m_Direction, m_Up);
	}

	// -----------------------------------------------------------------
	void EngineCamera::CalculateProjection()
	{
		if (App->scene->BasicTestShader == nullptr)
			return;

		if (m_FOV < MIN_FOV)
			m_FOV = MIN_FOV;
		else if (m_FOV > MAX_FOV)
			m_FOV = MAX_FOV;

		else if (m_FOV >= MIN_FOV && m_FOV <= MAX_FOV) {

			m_ProjMat = glm::perspective(m_FOV, App->window->GetAspectRatio(), m_NearPlane, m_FarPlane);
			App->scene->BasicTestShader->SetUniformMat4f("u_Proj", m_ProjMat);

			//glViewport(0, 0, (GLint)App->window->GetWindowWidth();, (GLint)App->window->GetWindowHeight()); //If something with the proj.mat. goes wrong, uncomment this.

			//glMatrixMode(GL_PROJECTION);
			//glLoadIdentity();

			////m_ProjMat = glm::perspective(m_FOV, App->window->GetAspectRatio(), m_NearPlane, m_FarPlane);

			////glm::value_ptr(m_ProjMat);

			//glLoadMatrixf(glm::value_ptr(m_ProjMat));


			////m_ProjectionMatrix = perspective(m_FOV, App->window->GetAspectRatio(), m_NearPlane, m_FarPlane);
			////glLoadMatrixf(&m_ProjectionMatrix);

			//glMatrixMode(GL_MODELVIEW);
			//glLoadIdentity();
		}
	}

	// -----------------------------------------------------------------
	//Save/Load
	void EngineCamera::SaveModuleData(json& JSONFile) const
	{
		JSONFile["EngineCamera"]["CameraMoveSpeed"] = m_CameraMoveSpeed;
		JSONFile["EngineCamera"]["CameraScrollSpeed"] = m_CameraScrollSpeed;
		JSONFile["EngineCamera"]["FOV"] = m_FOV;
		JSONFile["EngineCamera"]["NearPlane"] = m_NearPlane;
		JSONFile["EngineCamera"]["FarPlane"] = m_FarPlane;

		JSONFile["EngineCamera"]["InitialPosition"][0] = m_Pos.x;
		JSONFile["EngineCamera"]["InitialPosition"][1] = m_Pos.y;
		JSONFile["EngineCamera"]["InitialPosition"][2] = m_Pos.z;

		JSONFile["EngineCamera"]["InitialLookAt"][0] = m_Target.x;
		JSONFile["EngineCamera"]["InitialLookAt"][1] = m_Target.y;
		JSONFile["EngineCamera"]["InitialLookAt"][2] = m_Target.z;
	}

	void EngineCamera::LoadModuleData(json& JSONFile)
	{
		//vec3 initialPos = vec3(	JSONFile["EngineCamera"]["InitialPosition"][0],
		//						JSONFile["EngineCamera"]["InitialPosition"][1],
		//						JSONFile["EngineCamera"]["InitialPosition"][2]);
		//
		//vec3 initialRefence = vec3(	JSONFile["EngineCamera"]["InitialLookAt"][0],
		//							JSONFile["EngineCamera"]["InitialLookAt"][1],
		//							JSONFile["EngineCamera"]["InitialLookAt"][2]);

		m_CameraMoveSpeed = JSONFile["EngineCamera"]["CameraMoveSpeed"];
		m_CameraScrollSpeed = JSONFile["EngineCamera"]["CameraScrollSpeed"];
		m_FOV = JSONFile["EngineCamera"]["FOV"];
		m_NearPlane = JSONFile["EngineCamera"]["NearPlane"];
		m_FarPlane = JSONFile["EngineCamera"]["FarPlane"];

		//m_Reference = initialRefence;
		//m_Position = initialPos;
	}

	// -----------------------------------------------------------------
	void EngineCamera::SetFOV(float FOV)
	{
		m_FOV = FOV;
		CalculateProjection();
	}
	void EngineCamera::SetNearPlane(float nPlane)
	{
		m_NearPlane = nPlane;
		CalculateProjection();
	}
	void EngineCamera::SetFarPlane(float fPlane)
	{
		m_FarPlane = fPlane;
		CalculateProjection();
	}
}
