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

		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjectionMatrix;

	private:

		void RecalculateViewMatrix();


		

		glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 m_Target = glm::vec3(0.0f);
		glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_Direction;
		glm::vec3 m_Right;

		//float m_Rotation = 0.0f;
		float PAng = 0.0f, YAng = 0.0f;

		glm::quat m_Rotation = glm::quat();

	//public:

	//	//inline const float* GetViewMatrix()				const { return &m_ViewMatrix;		}
	//	//inline const float* GetProjectionMatrix()		const { return &m_ProjectionMatrix;	}
	//	//												
	//	//inline const vec3 GetX()						const { return m_X;					}
	//	//inline const vec3 GetY()						const { return m_Y;					}
	//	//inline const vec3 GetZ()						const { return m_Z;					}
	//	//inline const vec3 GetPosition()					const { return m_Position;			}
	//	//inline const vec3 GetReference()				const { return m_Reference;			}
	//	//inline const mat4x4 GetProjectionMatrixMAT4()	const { return m_ProjectionMatrix;	}
	//	//inline const mat4x4 GetViewMatrixMAT4()		const { return m_ViewMatrix; }

	//	inline const glm::vec3 GetPosition()				const { return m_Pos; }

	//	inline const float* GetViewMatrix()					const { return glm::value_ptr(m_viewMat); }
	//	inline const float* GetProjectionMatrix()			const { return glm::value_ptr(m_ProjMat);	}


	//	inline const glm::mat4 GetProjectionMatrixMAT4()	const { return m_ProjMat; }
	//	inline const glm::mat4 GetViewMatrixMAT4()			const { return m_viewMat; }

	//	inline const float GetCameraMoveSpeed()				const { return m_CameraMoveSpeed; }
	//	inline const float GetCameraScrollSpeed()			const { return m_CameraScrollSpeed; }
	//	inline const float GetFOV()							const { return m_FOV; }
	//	inline const float GetNearPlane()					const { return m_NearPlane; }
	//	inline const float GetFarPlane()					const { return m_FarPlane; }


	//public:

	//	void SetMoveSpeed(float speed)		{ m_CameraMoveSpeed = speed; }
	//	void SetScrollSpeed(float speed)	{ m_CameraScrollSpeed = speed; }

	//	void SetFOV(float FOV);
	//	void SetNearPlane(float nPlane);
	//	void SetFarPlane(float fPlane);

	//	void Look(const glm::vec3 &Position, const glm::vec3 &Reference, bool RotateAroundReference = false);
	//	void LookAt(const glm::vec3 &Spot);
	//	void Move(const glm::vec3 &Movement);
	//	void OrbitAroundReference(const glm::vec3& Reference);

	//	void CalculateProjection();

	//	//Save/Load
	//	virtual void SaveModuleData(json& JSONFile) const override;
	//	virtual void LoadModuleData(json& JSONFile) override;

	//private:

	//	void CalculateViewMatrix();
	//	void CalculateMouseRotation(const glm::vec3& pos, const glm::vec3& ref);
	//	void Zoom(float dt);	

	//private:

	//	void UpdateVectors();

	//	glm::vec3 m_Front, m_Up, m_Right/*, m_Direction*/;
	//	glm::vec3 /*m_Target,*/ m_Pos, m_WorldUp;
	//	float m_YawAngle, m_PitchAngle;

	//	glm::mat4 m_viewMat, m_ProjMat;

	//	//vec3 m_X, m_Y, m_Z;
	//	//vec3 m_Position, m_Reference;
	//	//mat4x4 m_ViewMatrix, m_ViewMatrixInverse;
	//	//mat4x4 m_ProjectionMatrix;

	//	float m_CameraMoveSpeed;
	//	float m_CameraScrollSpeed;

	//	float m_NearPlane = 0.125f;;
	//	float m_FarPlane = 512.0f;
	//	float m_FOV = 60.0f;
	};
}

#endif