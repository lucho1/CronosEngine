#ifndef _CAMERA3D_H_
#define _CAMERA3D_H_

#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#define MIN_FOV 15.0f
#define MAX_FOV 120.0f

namespace Cronos {

	class Camera3D : public Module
	{
	public:

		Camera3D(Application* app, bool start_enabled = true);
		~Camera3D();

		virtual bool OnStart() override;
		virtual update_status OnUpdate(float dt) override;
		virtual bool OnCleanUp() override;

		void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
		void OrbitAroundReference(const vec3& Reference);

		void LookAt(const vec3 &Spot);
		void Move(const vec3 &Movement);

	public:

		inline const float* GetViewMatrix()			const { return &m_ViewMatrix; }

		inline const vec3 GetX()					const { return m_X; }
		inline const vec3 GetY()					const { return m_Y; }
		inline const vec3 GetZ()					const { return m_Z; }

		inline const float GetCameraMoveSpeed()	const { return m_CameraMoveSpeed; }

	private:

		void CalculateViewMatrix();
		const vec3 CalculateMouseRotation(const vec3& pos, const vec3& ref);


		//TODO: Change the workflow of this:
		void Zoom(float dt);
		float nearPlane = 0.125f;
		float farPlane = 512.0f;
		float FOV = 60.0f;

	public:

		vec3 m_X, m_Y, m_Z, m_Position, m_Reference;

	private:

		mat4x4 m_ViewMatrix, m_ViewMatrixInverse;
		float m_CameraMoveSpeed = 5.0f;
		float m_CameraScrollSpeed = 20.0f;
	};

}

#endif