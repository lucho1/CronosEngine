#ifndef _CAMERA3D_H_
#define _CAMERA3D_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "glmath.h"

#define MIN_FOV 15.0f
#define MAX_FOV 120.0f

namespace Cronos {

	class EngineCamera : public Module
	{
	public:

		EngineCamera(Application* app, bool start_enabled = true);
		~EngineCamera();

		virtual bool			OnStart() override;
		virtual update_status	OnUpdate(float dt) override;
		virtual bool			OnCleanUp() override;

	public:

		inline const float* GetViewMatrix()			const { return &m_ViewMatrix;		}
		inline const float* GetProjectionMatrix()	const { return &m_ProjectionMatrix;	}

		inline const vec3 GetX()					const { return m_X;					}
		inline const vec3 GetY()					const { return m_Y;					}
		inline const vec3 GetZ()					const { return m_Z;					}
		inline const vec3 GetPosition()				const { return m_Position;			}
		inline const vec3 GetReference()			const { return m_Reference;			}

		inline const float GetCameraMoveSpeed()		const { return m_CameraMoveSpeed;	}

	public:

		void SetMoveSpeed(float speed)		{ m_CameraMoveSpeed = speed; }
		void SetScrollSpeed(float speed)	{ m_CameraScrollSpeed = speed; }

		void SetFOV(float FOV);
		void SetNearPlane(float nPlane);
		void SetFarPlane(float fPlane);

		void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
		void LookAt(const vec3 &Spot);
		void Move(const vec3 &Movement);
		void OrbitAroundReference(const vec3& Reference);

		void CalculateProjection();

	private:

		void CalculateViewMatrix();
		const vec3 CalculateMouseRotation(const vec3& pos, const vec3& ref);
		void Zoom(float dt);	

	private:

		vec3 m_X, m_Y, m_Z;
		vec3 m_Position, m_Reference;
		mat4x4 m_ViewMatrix, m_ViewMatrixInverse;
		mat4x4 m_ProjectionMatrix;

		float m_CameraMoveSpeed = 5.0f;
		float m_CameraScrollSpeed = 20.0f;

		float m_NearPlane = 0.125f;
		float m_FarPlane = 512.0f;
		float m_FOV = 60.0f;
	};
}

#endif