#ifndef _CAMERA3D_H_
#define _CAMERA3D_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "glmath.h"


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

		inline const float* GetViewMatrix()				const { return &m_ViewMatrix;		}
		inline const float* GetProjectionMatrix()		const { return &m_ProjectionMatrix;	}
														
		inline const vec3 GetX()						const { return m_X;					}
		inline const vec3 GetY()						const { return m_Y;					}
		inline const vec3 GetZ()						const { return m_Z;					}
		inline const vec3 GetPosition()					const { return m_Position;			}
		inline const vec3 GetReference()				const { return m_Reference;			}
		inline const mat4x4 GetProjectionMatrixMAT4()	const { return m_ProjectionMatrix;	}
		inline const mat4x4 GetViewMatrixMAT4()			const { return m_ViewMatrix; }

		inline const float GetCameraMoveSpeed()			const { return m_CameraMoveSpeed;	}
		inline const float GetCameraScrollSpeed()		const { return m_CameraScrollSpeed; }
		inline const float GetFOV()						const { return m_FOV; }
		inline const float GetNearPlane()				const { return m_NearPlane; }
		inline const float GetFarPlane()				const { return m_FarPlane; }


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

		//Save/Load
		virtual void SaveModuleData(json& JSONFile) const override;
		virtual void LoadModuleData(json& JSONFile) override;

	private:

		void CalculateViewMatrix();
		const vec3 CalculateMouseRotation(const vec3& pos, const vec3& ref);
		void Zoom(float dt);	

	private:

		vec3 m_X, m_Y, m_Z;
		vec3 m_Position, m_Reference;
		mat4x4 m_ViewMatrix, m_ViewMatrixInverse;
		mat4x4 m_ProjectionMatrix;

		float m_CameraMoveSpeed;
		float m_CameraScrollSpeed;

		float m_NearPlane;
		float m_FarPlane;
		float m_FOV;
	};
}

#endif