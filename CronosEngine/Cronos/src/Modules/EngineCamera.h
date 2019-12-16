#ifndef _CAMERA3D_H_
#define _CAMERA3D_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "Helpers/Camera.h"

namespace Cronos {

	class EngineCamera : public Module, public Camera
	{
	public:

		//Module Methods
		EngineCamera(Application* app, bool start_enabled = true);
		~EngineCamera();

		virtual bool			OnStart() override;
		virtual update_status	OnUpdate(float dt) override;
		virtual update_status	OnPostUpdate(float dt) override;
		virtual bool			OnCleanUp() override;

		//Save/Load
		void SaveModuleData(json& JSONFile) const;
		void LoadModuleData(json& JSONFile);

	public:

		void ChangeProjection() { m_ChangeProj = true; }
		inline Camera* GetCamera() { return this; }

	private:

		//Camera Methods
		glm::vec3 MouseRotation(const glm::vec3& pos, const glm::vec3& ref);
		
		//Returns a ray (vec3) that goes from camera pos in forward direction and has length of far plane distance
		const glm::vec3 RaycastForward();
		GameObject* OnClickSelection();

	private:

		//Camera Common data
		bool m_ChangeProj = false;
	};
}

#endif
