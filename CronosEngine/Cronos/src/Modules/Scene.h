#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "Globals.h"

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

	};

}

#endif