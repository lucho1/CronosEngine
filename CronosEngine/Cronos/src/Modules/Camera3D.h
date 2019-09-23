#ifndef _CAMERA3D_H_
#define _CAMERA3D_H_

#include "Module.h"
#include "Globals.h"
#include "glmath.h"

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
		void LookAt(const vec3 &Spot);
		void Move(const vec3 &Movement);

	public:

		const float* GetViewMatrix()		const { return &m_ViewMatrix; }

		const vec3 GetX()					const { return m_X; }
		const vec3 GetY()					const { return m_Y; }
		const vec3 GetZ()					const { return m_Z; }

		const float GetCameraMoveSpeed()	const { return m_CameraMoveSpeed; }

	private:

		void CalculateViewMatrix();

	public:

		vec3 m_X, m_Y, m_Z, m_Position, m_Reference;

	private:

		mat4x4 m_ViewMatrix, m_ViewMatrixInverse;
		float m_CameraMoveSpeed = 5.0f;
	};

}

#endif