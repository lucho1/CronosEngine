#include "Providers/cnpch.h"
#include "mmgr/mmgr.h"

#include "Providers/Globals.h"
#include "Application.h"
#include "EngineCamera.h"

namespace Cronos {

	EngineCamera::EngineCamera(Application* app, bool start_enabled) : Module(app, "Module Camera 3D", start_enabled)
	{
		CalculateViewMatrix();

		m_X = vec3(1.0f, 0.0f, 0.0f);
		m_Y = vec3(0.0f, 1.0f, 0.0f);
		m_Z = vec3(0.0f, 0.0f, 1.0f);

		m_Position = vec3(0.0f, 0.0f, 5.0f);
		m_Reference = vec3(0.0f, 0.0f, 0.0f);

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

		Look(m_Position, m_Reference);
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
		if (App->input->isMouseScrolling())
			Zoom(dt);

		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
		{
			vec3 newPos(0.0f, 0.0f, 0.0f);
			newPos += m_X * App->input->GetMouseXMotion() / 2.0f * dt;
			newPos.y += -App->input->GetMouseYMotion() / 2.0f * dt;

			m_Position += newPos;
			m_Reference += newPos;
		}

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {

			//Aplying extra impulse if LShift is pressed
			if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN)
				m_CameraMoveSpeed *= 2.0f;
			else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
				m_CameraMoveSpeed /= 2.0f;

			// Camera Movement On Keys WASD + Right Mouse Button ------------------------------
			vec3 newPos(0.0f, 0.0f, 0.0f);
			if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT) newPos.y += m_CameraMoveSpeed * dt;
			if (App->input->GetKey(SDL_SCANCODE_G) == KEY_REPEAT) newPos.y -= m_CameraMoveSpeed * dt;

			// Note that the vectors m_X/m_Y/m_Z contain the current axis of the camera
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= m_Z * m_CameraMoveSpeed * dt;
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += m_Z * m_CameraMoveSpeed * dt;

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= m_X * m_CameraMoveSpeed * dt;
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += m_X * m_CameraMoveSpeed * dt;

			m_Position += newPos;
			m_Reference += newPos;

			// Moving camera position orbiting
			m_Position = m_Reference + CalculateMouseRotation(m_Position, m_Reference);

			// Finally, we look to the reference
			LookAt(m_Reference);
		}

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {

			// Orbiting around center --- CalculateMouseRotation() is the reference postion
			m_Position = m_Reference + CalculateMouseRotation(m_Position, m_Reference);
			OrbitAroundReference(vec3(0, 0, 0));
		}

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			LookAt(vec3(0.0f, 0.0f, 0.0f)); //TODO: Make this with objects, not with center!!

		// Recalculate matrix -------------
		CalculateViewMatrix();

		return UPDATE_CONTINUE;
	}

	// -----------------------------------------------------------------
	const vec3 EngineCamera::CalculateMouseRotation(const vec3& pos, const vec3& ref)
	{
		// Mouse motion ----------------
		// There is another way to do it in Physics handout 1
		// Applying horizontal orbit
		vec3 ref_pos = pos - ref;
		ref_pos = rotate(ref_pos, -App->input->GetMouseXMotion() * 0.5f, vec3(0.0f, 1.0f, 0.0f));

		// Applying vertical orbit
		vec3 ref_pos_dy = rotate(ref_pos, -App->input->GetMouseYMotion() * 0.5f, m_X);

		// We check the vertical orbit to not do a complete orbit
		float dotProd = dot(normalize(ref_pos_dy), vec3(0.0f, 1.0f, 0.0f));
		if (dotProd > -0.95f && dotProd < 0.95f)
			ref_pos = ref_pos_dy;

		return ref_pos;
	}


	// -----------------------------------------------------------------
	void EngineCamera::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
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
	void EngineCamera::LookAt(const vec3 &Spot)
	{
		m_Reference = Spot;

		m_Z = normalize(m_Position - m_Reference);
		m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
		m_Y = cross(m_Z, m_X);

		CalculateViewMatrix();
	}


	// -----------------------------------------------------------------
	void EngineCamera::Move(const vec3 &Movement)
	{
		m_Position += Movement;
		m_Reference += Movement;

		CalculateViewMatrix();
	}

	// -----------------------------------------------------------------
	void EngineCamera::Zoom(float dt) {

		vec3 newPos(0.0f, 0.0f, 0.0f);

		if (App->input->GetMouseZ() > 0)
			newPos -= m_CameraScrollSpeed * m_Z * dt;
		else
			newPos += m_CameraScrollSpeed * m_Z * dt;

		m_Position += newPos;
	}

	// -----------------------------------------------------------------
	void EngineCamera::OrbitAroundReference(const vec3 &Reference)
	{
		this->m_Reference = Reference;

		m_Z = normalize(m_Position - Reference);
		m_X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), m_Z));
		m_Y = cross(m_Z, m_X);

		CalculateViewMatrix();
	}


	// -----------------------------------------------------------------
	void EngineCamera::CalculateViewMatrix()
	{
		m_ViewMatrix = mat4x4(m_X.x, m_Y.x, m_Z.x, 0.0f,
			m_X.y, m_Y.y, m_Z.y, 0.0f,
			m_X.z, m_Y.z, m_Z.z, 0.0f,
			-dot(m_X, m_Position), -dot(m_Y, m_Position), -dot(m_Z, m_Position), 1.0f);

		m_ViewMatrixInverse = inverse(m_ViewMatrix);
	}

	// -----------------------------------------------------------------
	void EngineCamera::CalculateProjection()
	{
		if (m_FOV < MIN_FOV)
			m_FOV = MIN_FOV;
		else if (m_FOV > MAX_FOV)
			m_FOV = MAX_FOV;

		else if (m_FOV >= MIN_FOV && m_FOV <= MAX_FOV) {

			//glViewport(0, 0, (GLint)App->window->GetWindowWidth();, (GLint)App->window->GetWindowHeight()); //If something with the proj.mat. goes wrong, uncomment this.

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			m_ProjectionMatrix = perspective(m_FOV, App->window->GetAspectRatio(), m_NearPlane, m_FarPlane);
			glLoadMatrixf(&m_ProjectionMatrix);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
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
	}

	void EngineCamera::LoadModuleData(json& JSONFile)
	{
		vec3 initialPos = vec3(	JSONFile["EngineCamera"]["InitialPosition"][0],
								JSONFile["EngineCamera"]["InitialPosition"][1],
								JSONFile["EngineCamera"]["InitialPosition"][2]);

		vec3 initialRefence = vec3(	JSONFile["EngineCamera"]["InitialLookAt"][0],
									JSONFile["EngineCamera"]["InitialLookAt"][1],
									JSONFile["EngineCamera"]["InitialLookAt"][2]);

		m_CameraMoveSpeed = JSONFile["EngineCamera"]["CameraMoveSpeed"];
		m_CameraScrollSpeed = JSONFile["EngineCamera"]["CameraScrollSpeed"];
		m_FOV = JSONFile["EngineCamera"]["FOV"];
		m_NearPlane = JSONFile["EngineCamera"]["NearPlane"];
		m_FarPlane = JSONFile["EngineCamera"]["FarPlane"];

		m_Reference = initialRefence;
		m_Position = initialPos;
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