#include "Providers/cnpch.h"
#include "Camera.h"

#include "Application.h"
#include "Renderer/GLRenderer3D.h"

namespace Cronos {

	// -----------------------------------------------------------------
	Camera::Camera()
	{
		m_CamFrustum.type = PerspectiveFrustum;
		m_Orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		m_ViewMatrix = m_ProjectionMatrix = glm::mat4(1.0f);

		m_Position = glm::vec3(0.0f, 3.0f, 5.0f);
		m_Target = glm::vec3(0.0f);
		m_Up = glm::vec3(0.0f, 1.0f, 0.0f);

		Recalculate();

		m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
		m_Front = glm::vec3(0.0f, 0.0f, 1.0f);

		Look(m_Position, m_Target, true);
	}

	// -----------------------------------------------------------------
	void Camera::Look(const glm::vec3 & pos, const glm::vec3 & target, bool RotateAroundReference)
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

	void Camera::LookAt(const glm::vec3 & spot)
	{
		m_Target = spot;

		m_Front = glm::normalize(m_Position - m_Target);
		m_Right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), m_Front));
		m_Up = glm::cross(m_Front, m_Right);

		Recalculate();
	}

	// -----------------------------------------------------------------
	void Camera::Move(CameraMovement direction, bool speedUp, float dt)
	{
		float speed = m_MoveSpeed;
		if(speedUp)
			speed *= 2.0f;

		//Movement --------------------------------------------
		glm::vec3 movement = glm::vec3(0.0f);
		switch (direction)
		{
			case CameraMovement::CAMMOVE_FORWARD:
				movement -= m_Front * speed * dt;
				break;
			case CameraMovement::CAMMOVE_BACKWARDS:
				movement += m_Front * speed * dt;
				break;
			case CameraMovement::CAMMOVE_LEFT:
				movement -= m_Right * speed * dt;
				break;
			case CameraMovement::CAMMOVE_RIGHT:
				movement += m_Right * speed * dt;
				break;
			case CameraMovement::CAMMOVE_UP:
				movement += m_Up * speed * dt;
				break;
			case CameraMovement::CAMMOVE_DOWN:
				movement -= m_Up * speed * dt;
				break;
			case CameraMovement::CAMMOVE_NONE:
				break;
			default:
				break;
		}

		m_Position += movement;
		m_Target += movement;
	}

	void Camera::Zoom(int ZMovement, float dt)
	{
		float scroll = m_ScrollSpeed * dt * 0.6f;
		glm::vec3 newPos(0.0f, 0.0f, 0.0f);

		if (ZMovement > 0)
			newPos -= m_ScrollSpeed * m_Front;
		else
			newPos += m_ScrollSpeed * m_Front;

		m_Position += newPos;
		m_Target += newPos;
	}

	void Camera::Panning(float XMovement, float YMovement, float dt)
	{
		float xOffset = XMovement * (m_MoveSpeed / 3.0f) * dt;
		float yOffset = YMovement * (m_MoveSpeed / 3.0f) * dt;

		glm::vec3 newPos(0.0f, 0.0f, 0.0f);
		newPos += -m_Right * xOffset;
		newPos += m_Up * yOffset;
		//newPos.y += -App->input->GetMouseYMotion() / 2.0f * dt;

		m_Position += newPos;
		m_Target += newPos;
	}

	void Camera::Focus()
	{
		if (App->EditorGUI->GetCurrentGameObject() != nullptr && App->EditorGUI->GetCurrentGameObject()->isActive())
		{
			//TODO: Properly perform the focus!
			//TransformComponent* LookAtComp = App->EditorGUI->GetCurrentGameObject()->GetComponent<TransformComponent>();
			//
			//AABB GO_BB = LookAtComp->GetAABB();
			//glm::vec3 LookPos = LookAtComp->GetCentralAxis();
			//
			//glm::vec3 size = GO_BB.getMax() - GO_BB.getMin();
			//size *= 1.5f;
			//
			//glm::vec3 pos = glm::vec3(LookPos.x * size.x, LookPos.y + 1.5f, LookPos.z * size.z) + m_Front * 6.0f;
			//glm::vec3 ref = glm::vec3(LookPos.x, LookPos.y + 0.5f, LookPos.z + 0.5f);

			//Look(pos, ref, true);
		}
		else
			LookAt(glm::vec3(0.0f));
	}

	// -----------------------------------------------------------------
	void Camera::Recalculate()
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
			m_AspectRatio.x / m_AspectRatio.y, m_NearPlane, m_FarPlane);

		//camFrustum
		m_CamFrustum.pos = float3(m_Position.x, m_Position.y, m_Position.z);
		m_CamFrustum.front = -float3(m_Front.x, m_Front.y, m_Front.z);
		m_CamFrustum.up = float3(m_Up.x, m_Up.y, m_Up.z);
		m_CamFrustum.Transform(math::Quat(m_Orientation.x, m_Orientation.y, m_Orientation.z, m_Orientation.w));

		m_CamFrustum.nearPlaneDistance = m_NearPlane;
		m_CamFrustum.farPlaneDistance = m_FarPlane;

		m_CamFrustum.verticalFov = glm::radians(m_FOV);
		m_CamFrustum.horizontalFov = 2.0f * glm::atan(glm::tan(glm::radians(m_FOV)*0.5f) * (m_AspectRatio.x/m_AspectRatio.y));
	}

	void Camera::DrawFrustum()
	{
		float3 corners[8];
		m_CamFrustum.GetCornerPoints(corners);

		glm::vec3 glmCorners[8];
		glm::vec3 blueColor = glm::vec3(0.0f, 0.0f, 1.0f);

		for (uint i = 0; i < 8; i += 2)
		{
			glm::vec3 min = glm::vec3(corners[i].x, corners[i].y, corners[i].z);
			glm::vec3 max = glm::vec3(corners[i + 1].x, corners[i + 1].y, corners[i + 1].z);

			glmCorners[i] = min;
			glmCorners[i + 1] = max;

			App->renderer3D->DrawLine(max, min, blueColor, 2.0f);
		}

		//near plane
		App->renderer3D->DrawLine(glmCorners[2], glmCorners[0], blueColor, 2.0f);
		App->renderer3D->DrawLine(glmCorners[6], glmCorners[2], blueColor, 2.0f);
		App->renderer3D->DrawLine(glmCorners[0], glmCorners[4], blueColor, 2.0f);
		App->renderer3D->DrawLine(glmCorners[4], glmCorners[6], blueColor, 2.0f);

		//Far plane
		App->renderer3D->DrawLine(glmCorners[1], glmCorners[3], blueColor, 2.0f);
		App->renderer3D->DrawLine(glmCorners[3], glmCorners[7], blueColor, 2.0f);
		App->renderer3D->DrawLine(glmCorners[5], glmCorners[1], blueColor, 2.0f);
		App->renderer3D->DrawLine(glmCorners[7], glmCorners[5], blueColor, 2.0f);
	}

	// -----------------------------------------------------------------
	void Camera::SetFOV(float FOV)
	{
		m_FOV = FOV;
		Recalculate();
	}

	void Camera::SetNearPlane(float nPlane)
	{
		m_NearPlane = nPlane;
		Recalculate();
	}

	void Camera::SetFarPlane(float fPlane)
	{
		m_FarPlane = fPlane;
		Recalculate();
	}

	void Camera::SetAspectRatio(glm::vec2 AspectRatio)
	{
		m_AspectRatio = AspectRatio;
		m_CamFrustum.verticalFov = glm::radians(GetFOV());
		m_CamFrustum.horizontalFov = 2.0f * glm::atan(glm::tan(glm::radians(GetFOV())*0.5f) * m_AspectRatio.x / m_AspectRatio.y);
		Recalculate();
	}
}