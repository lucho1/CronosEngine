#ifndef _CAMERA3D_H_
#define _CAMERA3D_H_

#include "Module.h"
#include "Providers/Globals.h"

#define MIN_FOV 15.0f
#define MAX_FOV 120.0f

namespace Cronos {

	class EngineCamera : public Module
	{
	public:

		//Module Methods
		EngineCamera(Application* app, bool start_enabled = true);
		~EngineCamera();

		virtual bool			OnStart() override;
		virtual update_status	OnUpdate(float dt) override;
		virtual update_status	OnPostUpdate(float dt) override;
		virtual bool			OnCleanUp() override;

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

	public:

		//Setters
		void SetFOV(float FOV);
		void SetNearPlane(float nPlane);
		void SetFarPlane(float fPlane);

		void ChangeProjection() { m_ChangeProj = true; }

	private:

		//Camera Methods
		void RecalculateMatrices();
		void RecalculateVectors();

		void Move(float dt);
		void Zoom(float dt);
		void CameraPanning(float dt);
		void Focus();
		glm::vec3 Rotate(const glm::vec3& pos, const glm::vec3& ref);

	private:

		//Camera Calculation Data
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

		glm::vec3 m_Position;
		glm::vec3 m_Target;
		
		glm::vec3 m_Front;
		glm::vec3 m_Right;
		glm::vec3 m_Up;

		//float m_Rotation = 0.0f;
		float PAng = 0.0f, YAng = 0.0f;
		glm::quat m_Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		//float m_Rotation = 0.0f;

	private:

		//Camera Common data
		float m_NearPlane = 1.0f, m_FarPlane = 100.0f , m_FOV = 60.0f;
		float m_MoveSpeed = 10.0f, m_ScrollSpeed = 3.5f;
		bool m_ChangeProj = false;

		float m_FocusDistance = 20.0f;
	};
}

#endif