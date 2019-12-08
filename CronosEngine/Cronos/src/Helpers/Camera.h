#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Providers/Globals.h"

#define MIN_FOV 15.0f
#define MAX_FOV 120.0f

namespace Cronos {

	enum class CameraMovement
	{
		CAMMOVE_NONE = -1,
		CAMMOVE_FORWARD,
		CAMMOVE_BACKWARDS,
		CAMMOVE_RIGHT,
		CAMMOVE_LEFT,
		CAMMOVE_UP,
		CAMMOVE_DOWN
	};

	class Camera
	{
	public:

		Camera();
		~Camera() {}

	public:

		//Getters
		inline const glm::mat4 GetViewMatrix()			const { return m_ViewMatrix; }
		inline const glm::mat4 GetProjectionMatrix()	const { return m_ProjectionMatrix; }

		inline const glm::vec2 GetAspectRatio()			const { return m_AspectRatio; }
		inline const glm::vec3 GetPosition()			const { return m_Position; }
		inline const glm::vec3 GetTarget()				const { return m_Target; }
		inline const glm::vec3 GetFrontVector()			const { return m_Front; }
		inline const glm::vec3 GetUpVector()			const { return m_Up; }
		inline const glm::vec3 GetRightVector()			const { return m_Right; }

		inline const glm::quat GetOrientation()			const { return m_Orientation; }

		inline const float GetNearPlane()				const { return m_NearPlane; }
		inline const float GetFarPlane()				const { return m_FarPlane; }
		inline const float GetFOV()						const { return m_FOV; }

		inline const float GetCameraMoveSpeed()			const { return m_MoveSpeed; }
		inline const float GetCameraScrollSpeed()		const { return m_ScrollSpeed; }

		inline math::Frustum* GetFrustum()				{ return &m_CamFrustum; }

	public:

		//Setters
		void SetFOV(float FOV);
		void SetNearPlane(float nPlane);
		void SetFarPlane(float fPlane);

		void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
		void SetScrollSpeed(float speed) { m_ScrollSpeed = speed; }

		void SetAspectRatio(glm::vec2 AspectRatio);
		
	public:

		//Camera Methods
		void Look(const glm::vec3& pos, const glm::vec3& target, bool RotateAroundReference = false);
		void LookAt(const glm::vec3& spot);

		void DrawFrustum();

		void Move(CameraMovement direction, bool speedUp, float dt);
		void Zoom(int ZMovement, float dt);
		void Panning(float XMovement, float YMovement, float dt);
		void Focus();

	protected:

		//Camera Methods
		virtual void Recalculate();	

	private:

		//Camera Calculation Data
		glm::vec2 m_AspectRatio = glm::vec2(1.6f, 0.9f);
		
		glm::vec3 m_Front;
		glm::vec3 m_Right;
		glm::vec3 m_Up;

		glm::quat m_Orientation;
		
	protected:

		//Camera Calculation Data for childs
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

		glm::vec3 m_Position = glm::vec3(0.0f, 3.0f, 5.0f);
		glm::vec3 m_Target = glm::vec3(0.0f);

		math::Frustum m_CamFrustum;

	private:

		//Camera Common data
		float m_NearPlane = 1.0f, m_FarPlane = 100.0f, m_FOV = 60.0f;
		float m_MoveSpeed = 10.0f, m_ScrollSpeed = 3.5f;

		float m_FocusDistance = 20.0f;		
	};
}

#endif