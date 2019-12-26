#include "Providers/cnpch.h"

#include "Application.h"
#include "EngineCamera.h"

#include <glm/gtx/rotate_vector.hpp>
//#include <glm/gtx/euler_angles.hpp>

#include "mmgr/mmgr.h"

namespace Cronos
{
	//Engine Camera stuff ----------------------------------------------
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
		glm::vec2 ar = glm::vec2((float)App->window->GetWidth(), (float)App->window->GetHeight());
		SetAspectRatio(ar);

		Look(GetPosition(), GetTarget(), false);
		App->renderer3D->SetRenderingCamera(*this);
		return true;
	}

	bool EngineCamera::OnCleanUp()
	{
		LOG("Cleaning camera");
		return true;
	}

	update_status EngineCamera::OnUpdate(float dt)
	{
		//DrawFrustum();

		if (App->EditorGUI->isHoveringWinGame())
		{
			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
			{
				//Applying extra impulse if LShift is pressed
				static bool speedup = false;
				if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN)
					speedup = true;
				else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
					speedup = false;

				//Modifying camera speed with mouse central button scroll
				if (App->input->isMouseScrolling())
					ModifySpeedMultiplicator(App->input->GetMouseZ(), dt);

				//Movement --------------------------------------------
				if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
					Move(CameraMovement::CAMMOVE_FORWARD, speedup, dt);
				if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
					Move(CameraMovement::CAMMOVE_BACKWARDS, speedup, dt);

				if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
					Move(CameraMovement::CAMMOVE_LEFT, speedup, dt);
				if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
					Move(CameraMovement::CAMMOVE_RIGHT, speedup, dt);

				//Take a look here and previous camera if Up movement doesn't work
				if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
					Move(CameraMovement::CAMMOVE_UP, speedup, dt);
				if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
					Move(CameraMovement::CAMMOVE_DOWN, speedup, dt);

				//m_Position = m_Target + Rotate(m_Position, m_Target);
				m_Position = GetTarget() + MouseRotation(GetPosition(), GetTarget());
				LookAt(GetTarget());
			}

			//Zoom with middle button of mouse (if scrolling)
			if (App->input->isMouseScrolling() && App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KEY_REPEAT)
				Zoom(App->input->GetMouseZ(), dt);

			//Pan with middle button of mouse
			if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
				Panning((float)App->input->GetMouseXMotion(), (float)App->input->GetMouseYMotion(), dt);
			
			//Select an object with LClick
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
				App->EditorGUI->SetSelectedGameObject(OnClickSelection());

			//ALT Functionality (orbit)
			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				//Rotate(m_Position, m_Target);
				m_Position = GetTarget() + MouseRotation(GetPosition(), GetTarget());

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
				Look(GetPosition(), GetTarget(), false);
		}

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			Focus();
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_UP)
			Look(GetPosition(), GetTarget(), false);

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

	void EngineCamera::ModifySpeedMultiplicator(float ZMovement, float dt)
	{
		m_ScrollingSpeedChange = true;
		float scroll = m_ScrollSpeed * dt * 3.0f;

		if (ZMovement > 0)
			m_SpeedMultiplicator += scroll;
		else
			m_SpeedMultiplicator -= scroll;

		if (m_SpeedMultiplicator > 15.0f)
			m_SpeedMultiplicator = 15.0f;
		if (m_SpeedMultiplicator < 0.3f)
			m_SpeedMultiplicator = 0.3f;
	}

	glm::vec3 EngineCamera::MouseRotation(const glm::vec3 & pos, const glm::vec3 & ref)
	{
		float dx = -App->input->GetMouseXMotion() * 0.005f;
		float dy = -App->input->GetMouseYMotion() * 0.005f;

		glm::vec3 refPos = pos - ref;
		refPos = glm::rotate(refPos, dx, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::vec3 refPosY = glm::rotate(refPos, dy, GetRightVector());

		float dotProd = glm::dot(glm::normalize(refPosY), glm::vec3(0.0f, 1.0f, 0.0f));
		if (dotProd > -0.95f && dotProd < 0.95f)
			refPos = refPosY;

		return refPos;
	}

	GameObject* EngineCamera::OnClickSelection()
	{
		glm::vec3 spawn = RaycastForward();
		math::LineSegment ray = math::LineSegment(float3(GetPosition().x, GetPosition().y, GetPosition().z), float3(spawn.x, spawn.y, spawn.z));
		std::vector<GameObject*> GameObjectSelection;

		//TODO: Once Rendering octree is fixed, do this with objects in visible nodes
		for (auto& GO : App->scene->m_GameObjects)
		{
			if (GO->GetAABB().Intersects(ray))
			{
				std::vector<GameObject*> tmpVec;
				tmpVec = GetObjectFromSelection(GO, ray);

				if(tmpVec.size() > 0)
					GameObjectSelection.insert(GameObjectSelection.begin(), tmpVec.begin(), tmpVec.end());
			}
		}
		
		if (GameObjectSelection.size() > 0)
		{
			QuickSortByCamDistance(GameObjectSelection, GetPosition(), 0, GameObjectSelection.size() - 1);
			return GameObjectSelection[0];
		}

		return nullptr;
	}

	std::vector<GameObject*> EngineCamera::GetObjectFromSelection(GameObject* parent, math::LineSegment rayIntersecting)
	{
		std::vector<GameObject*> retVec;
		if (parent->m_Childs.size() > 0)
		{
			for (auto& child : parent->m_Childs)
			{
				if (child->GetAABB().Intersects(rayIntersecting))
				{
					std::vector<GameObject*> childsVec = GetObjectFromSelection(child, rayIntersecting);					
					if (childsVec.size() > 0)
						retVec.insert(retVec.begin(), childsVec.begin(), childsVec.end());
				}

				if (child->isActive() && child->GetComponent<MeshComponent>() && child->GetAABB().Intersects(*GetFrustum()) && child->GetAABB().Intersects(rayIntersecting))
					retVec.push_back(child);
			}
		}

		if (parent->GetComponent<MeshComponent>() && parent->isActive() && parent->GetAABB().Intersects(*GetFrustum()) && parent->GetAABB().Intersects(rayIntersecting))
			retVec.push_back(parent);

		return retVec;
	}

	//Objects sorter for mouse picking
	void EngineCamera::QuickSortByCamDistance(std::vector<GameObject*>&vec, glm::vec3 camPos, uint left, uint right)
	{
		if (left >= right || right >= vec.size()) return; //Length <= 1 or invalid
		float pivotD = glm::length(vec[right]->GetComponent<TransformComponent>()->GetGlobalTranslation() - camPos);
		uint cnt = left;

		for (uint i = left; i <= right; i++)
		{
			float currentObjD = glm::length(vec[i]->GetComponent<TransformComponent>()->GetGlobalTranslation() - camPos);
			if (currentObjD <= pivotD)
			{
				std::swap(vec[cnt], vec[i]);
				++cnt;
			}
		}

		QuickSortByCamDistance(vec, camPos, left, cnt - 2); //We just need to sort the left part of the vector
		//QuickSortByCamDistance(vec, camPos, cnt, right);
	}

	const glm::vec3 EngineCamera::RaycastForward()
	{
		float mouseX = (2.0f * App->input->GetMouseX()) / App->window->GetWidth() - 1.0f;
		float mouseY = 1.0f - (2.0f * App->input->GetMouseY()) / App->window->GetHeight() - 0.6f;

		//float mouseX = App->input->GetMouseX() / (App->window->GetWidth()  * 0.5f) - 1.0f;
		//float mouseY = App->input->GetMouseY() / (App->window->GetHeight() * 0.5f) - 1.0f;

		glm::vec4 rayClip = glm::vec4(mouseX, mouseY, -1.0f, 1.0f);

		glm::vec4 rayEye = glm::inverse(GetProjectionMatrix()) * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

		glm::vec4 rayWorld = glm::inverse(GetViewMatrix()) * rayEye;
		glm::vec3 dir = glm::normalize(rayWorld); //This are the screen coordinates (of mouse) converted into worldPos as a forward direction

		return GetPosition() + dir * GetFarPlane(); //Ray End -- distance or length = FarPlane, so it gives a vec3 going from pos to farplane in forward direction
	
	
		// -------------------------------------------------------------------------------------------
		// -------------------------------------------------------------------------------------------
		/*//float mouseX = App->input->GetMouseX() / (App->window->GetWidth()  * 0.5f) - 1.0f;
		//float mouseY = App->input->GetMouseY() / (App->window->GetHeight() * 0.5f) - 1.0f;

		float mouseNormX = App->input->GetMouseX() / (float)App->window->GetWidth();
		float mouseNormY = App->input->GetMouseY() / (float)App->window->GetHeight();

		//Normalizing mouse position in range of -1 / 1 // -1, -1 being at the bottom left corner
		mouseNormX = (mouseNormX - 0.5) / 0.5;
		mouseNormY = (mouseNormY - 0.5) / 0.5;

		float mouseX = mouseNormX;
		float mouseY = -mouseNormY;

		glm::mat4 proj = App->engineCamera->GetProjectionMatrix();
		glm::mat4 view = App->engineCamera->GetViewMatrix();

		glm::mat4 invVP = glm::inverse(proj * view);
		glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
		glm::vec4 worldPos = invVP * screenPos;

		glm::vec3 dir = glm::normalize(glm::vec3(worldPos));

		return dir;*/
	}


	// -----------------------------------------------------------------
	//Save/Load
	void EngineCamera::SaveModuleData(json& JSONFile) const
	{
		JSONFile["EngineCamera"]["CameraMoveSpeed"] = GetCameraMoveSpeed();
		JSONFile["EngineCamera"]["CameraScrollSpeed"] = GetCameraScrollSpeed();
		JSONFile["EngineCamera"]["FOV"] = GetFOV();
		JSONFile["EngineCamera"]["NearPlane"] = GetNearPlane();
		JSONFile["EngineCamera"]["FarPlane"] = GetFarPlane();

		JSONFile["EngineCamera"]["InitialPosition"][0] = GetPosition().x;
		JSONFile["EngineCamera"]["InitialPosition"][1] = GetPosition().y;
		JSONFile["EngineCamera"]["InitialPosition"][2] = GetPosition().z;

		JSONFile["EngineCamera"]["InitialLookAt"][0] = GetTarget().x;
		JSONFile["EngineCamera"]["InitialLookAt"][1] = GetTarget().y;
		JSONFile["EngineCamera"]["InitialLookAt"][2] = GetTarget().z;
	}

	void EngineCamera::LoadModuleData(json& JSONFile)
	{
		glm::vec3 initialPos = glm::vec3(		JSONFile["EngineCamera"]["InitialPosition"][0],
												JSONFile["EngineCamera"]["InitialPosition"][1],
												JSONFile["EngineCamera"]["InitialPosition"][2]);

		glm::vec3 initialRefence = glm::vec3(	JSONFile["EngineCamera"]["InitialLookAt"][0],
												JSONFile["EngineCamera"]["InitialLookAt"][1],
												JSONFile["EngineCamera"]["InitialLookAt"][2]);

		SetMoveSpeed(JSONFile["EngineCamera"]["CameraMoveSpeed"]);
		SetScrollSpeed(JSONFile["EngineCamera"]["CameraScrollSpeed"]);
		SetFOV(JSONFile["EngineCamera"]["FOV"]);
		SetNearPlane(JSONFile["EngineCamera"]["NearPlane"]);
		SetFarPlane(JSONFile["EngineCamera"]["FarPlane"]);

		m_Target = initialRefence;
		m_Position = initialPos;
	}
}
