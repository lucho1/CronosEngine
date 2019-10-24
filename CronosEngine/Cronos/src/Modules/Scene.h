#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "Providers/Globals.h"
#include "Renderer/Primitive.h"
#include "Renderer/Shaders.h"

namespace Cronos {

	class Scene : public Module
	{
	public:

		Scene(Application* app, bool start_enabled = true);
		~Scene();

		virtual bool OnStart() override;
		virtual bool OnCleanUp() override;

		virtual update_status OnPreUpdate(float dt) override;
		virtual update_status OnUpdate(float dt) override;
		virtual update_status OnPostUpdate(float dt) override;


		Shader* BasicTestShader; //To test

	private:

		Plane m_FloorPlane;
		glm::mat4 modelMatDef = glm::mat4(1.0f);
	};

}

#endif
