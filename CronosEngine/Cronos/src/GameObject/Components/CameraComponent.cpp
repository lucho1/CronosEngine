#include "Providers/cnpch.h"
#include "CameraComponent.h"

#include "TransformComponent.h"
#include "GameObject/GameObject.h"

namespace Cronos {

	CameraComponent::CameraComponent(GameObject* attachedGO)
		: Component(ComponentType::CAMERA, attachedGO)
	{
	}

	void CameraComponent::Update(float dt)
	{
		Recalculate();
		DrawFrustum();
	}

	// -----------------------------------------------------------------
	void CameraComponent::Recalculate()
	{
		m_Position = GetParent()->GetComponent<TransformComponent>()->GetTranslation();

		glm::mat4 camTransform = glm::translate(glm::mat4(1.0f), GetPosition()) *
			glm::mat4_cast(GetOrientation());

		m_ViewMatrix = glm::inverse(camTransform);
		m_ViewMatrix = glm::lookAt(GetPosition(), GetTarget(), GetUpVector());

		if (GetFOV() < MIN_FOV)
			SetFOV(MIN_FOV);
		else if (GetFOV() > MAX_FOV)
			SetFOV(MAX_FOV);

		m_ProjectionMatrix = glm::perspective(glm::radians(GetFOV()),
			GetAspectRatio().x / GetAspectRatio().y, GetNearPlane(), GetFarPlane());

		//Frustum
		glm::vec3 eulerRot = glm::radians(GetParent()->GetComponent<TransformComponent>()->GetOrientation());
		math::Quat q1 = math::Quat({ 1, 0, 0 }, eulerRot.x);
		math::Quat q2 = math::Quat({ 0, 1, 0 }, eulerRot.y);
		math::Quat q3 = math::Quat({ 0, 0, 1 }, eulerRot.z);
				
		m_CamFrustum.pos = float3(GetPosition().x, GetPosition().y, GetPosition().z);
		m_CamFrustum.front = -float3(GetFrontVector().x, GetFrontVector().y, GetFrontVector().z);
		m_CamFrustum.up = float3(GetUpVector().x, GetUpVector().y, GetUpVector().z);

		m_CamFrustum.Transform(q1*q2*q3);

		m_CamFrustum.nearPlaneDistance = GetNearPlane();
		m_CamFrustum.farPlaneDistance = GetFarPlane();

		m_CamFrustum.verticalFov = glm::radians(GetFOV());
		m_CamFrustum.horizontalFov = 2.0f * glm::atan(glm::tan(glm::radians(GetFOV())*0.5f) * GetAspectRatio().x / GetAspectRatio().y);
	}
}