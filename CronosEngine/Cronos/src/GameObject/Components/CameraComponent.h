#ifndef _CAMERACOMPONENT_H_
#define _CAMERACOMPONENT_H_

#include "Component.h"

namespace Cronos {

	class CameraComponent : public Component
	{
	public:

		CameraComponent(GameObject* attachedGO);
		~CameraComponent() {}

		virtual void Update(float dt) { deltaTime = dt; Recalculate(); DrawFrustum(); }

	public:

		//Getters
		inline const glm::mat4 GetViewMatrix() const { return m_ViewMatrix; }
		inline const glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }

		inline const glm::vec3 GetPosition() const { return m_Position; }
		inline const glm::vec3 GetTarget() const { return m_Target; }
		inline const glm::vec3 GetFrontVector() const { return m_Front; }
		inline const glm::vec3 GetUpVector() const { return m_Up; }
		inline const glm::vec3 GetRightVector() const { return m_Right; }

		inline const float GetNearPlane() const { return m_NearPlane; }
		inline const float GetFarPlane() const { return m_FarPlane; }
		inline const float GetFOV() const { return m_FOV; }


		//const CnFrustum& GetFrustum() const { return m_CamFrustum; }
		static ComponentType GetType() { return ComponentType::CAMERA; };

		void DrawFrustum();

	public:

		//Setters
		void SetFOV(float FOV);
		void SetNearPlane(float nPlane);
		void SetFarPlane(float fPlane);
		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }

	private:

		//Camera Methods
		void Look(const glm::vec3& pos, const glm::vec3& target, bool RotateAroundReference = false);
		void LookAt(const glm::vec3& spot);

		void Recalculate();
		


		//void Move(glm::vec3 movement);
		//void Zoom(float zMovement);
		//void CameraPanning(glm::vec2 panMove);
		//void Focus(GameObject* focusTarget);

	private:

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

		glm::vec3 m_Position;
		glm::vec3 m_Target;

		glm::vec3 m_Front;
		glm::vec3 m_Right;
		glm::vec3 m_Up;

		glm::quat m_Orientation;

		float m_NearPlane = 1.0f, m_FarPlane = 100.0f, m_FOV = 60.0f;
		float m_MoveSpeed = 10.0f;
		glm::vec2 m_AspectRatio = glm::vec2(1.6f, 0.9f);

		float deltaTime = 0.0f;

		Frustum m_CamFrustum;
	};
}

#endif