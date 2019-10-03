#include "cnpch.h"
#include "Globals.h"
#include "Application.h"
#include "ImGui/ImGuiLayer.h"
#include "Camera3D.h"

namespace Cronos {

	Camera3D::Camera3D(Application* app, bool start_enabled) : Module(app, "Module Camera 3D", start_enabled)
	{
		CalculateViewMatrix();

		m_X = vec3(1.0f, 0.0f, 0.0f);
		m_Y = vec3(0.0f, 1.0f, 0.0f);
		m_Z = vec3(0.0f, 0.0f, 1.0f);

		m_Position = vec3(0.0f, 0.0f, 5.0f);
		m_Reference = vec3(0.0f, 0.0f, 0.0f);
	}

	Camera3D::~Camera3D()
	{}

	// -----------------------------------------------------------------
	bool Camera3D::OnStart()
	{
		App->EditorGUI->AddLog("Setting up the Camera");
		LOG("Setting up the camera");
		return true;
	}

	// -----------------------------------------------------------------
	bool Camera3D::OnCleanUp()
	{
		App->EditorGUI->AddLog("Cleaning Camera");
		LOG("Cleaning camera");
		return true;
	}

	// -----------------------------------------------------------------
	update_status Camera3D::OnUpdate(float dt)
	{
		// OnKeys WASD keys -----------------------------------
		vec3 newPos(0.0f, 0.0f, 0.0f);
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += m_CameraMoveSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) newPos.y -= m_CameraMoveSpeed * dt;

		// Note that the vectors m_X/m_Y/m_Z contain the current axis of the camera
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= m_Z * m_CameraMoveSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += m_Z * m_CameraMoveSpeed * dt;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= m_X * m_CameraMoveSpeed * dt;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += m_X * m_CameraMoveSpeed * dt;

		m_Position += newPos;
		m_Reference += newPos;

		// Mouse motion ----------------
		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
		{
			//You have another way to do it in Physics handout 1
			// Applying horizontal orbit
			vec3 ref_pos = m_Position - m_Reference;
			ref_pos = rotate(ref_pos, -App->input->GetMouseXMotion() * 0.5f, vec3(0.0f, 1.0f, 0.0f));

			// Applying vertical orbit
			vec3 ref_pos_dy = rotate(ref_pos, -App->input->GetMouseYMotion() * 0.5f, m_X);

			// We check the vertical orbit to not do a complete orbit
			float dotProd = dot(normalize(ref_pos_dy), vec3(0.0f, 1.0f, 0.0f));
			if (dotProd > -0.95f && dotProd < 0.95f)
				ref_pos = ref_pos_dy;

			// Moving camera position orbiting
			m_Position = m_Reference + ref_pos;

			// Finally, we look to the reference
			LookAt(m_Reference);
		}

		// Recalculate matrix -------------
		CalculateViewMatrix();

		return UPDATE_CONTINUE;
	}

	// -----------------------------------------------------------------
	void Camera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
	{
		this->m_Position = Position;
		this->m_Reference = Reference;

		m_Z = normalize(Position - Reference);
		m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
		m_Y = cross(m_Z, m_X);

		if (!RotateAroundReference)
		{
			this->m_Reference = this->m_Position;
			this->m_Position += m_Z * 0.05f;
		}

		CalculateViewMatrix();
	}

	// -----------------------------------------------------------------
	void Camera3D::LookAt(const vec3 &Spot)
	{
		m_Reference = Spot;

		m_Z = normalize(m_Position - m_Reference);
		m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
		m_Y = cross(m_Z, m_X);

		CalculateViewMatrix();
	}


	// -----------------------------------------------------------------
	void Camera3D::Move(const vec3 &Movement)
	{
		m_Position += Movement;
		m_Reference += Movement;

		CalculateViewMatrix();
	}

	// -----------------------------------------------------------------
	void Camera3D::CalculateViewMatrix()
	{
		m_ViewMatrix = mat4x4(m_X.x, m_Y.x, m_Z.x, 0.0f,
			m_X.y, m_Y.y, m_Z.y, 0.0f,
			m_X.z, m_Y.z, m_Z.z, 0.0f,
			-dot(m_X, m_Position), -dot(m_Y, m_Position), -dot(m_Z, m_Position), 1.0f);


		m_ViewMatrixInverse = inverse(m_ViewMatrix);
	}

}