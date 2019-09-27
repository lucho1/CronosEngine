#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "Module.h"

#include "Modules/SDLWindow.h"
#include "Modules/Input.h"
#include "Modules/Audio.h"
#include "Modules/Scene.h"
#include "Modules/EngineCamera.h"
#include "Modules/Filesystem.h"

#include "Renderer/GLRenderer3D.h"
#include "ImGui/ImGuiLayer.h"

namespace Cronos {

	class Application {

	public:

		SDLWindow* window;
		Input* input;
		Audio* audio;
		Scene* scene;
		GLRenderer3D* renderer3D;
		EngineCamera* engineCamera;
		ImGuiLayer* EditorGUI;
		Filesystem* filesystem;

	private:

		std::vector<Module*> m_ModulesList;

		//Timestep stuff
		float m_Timestep = 0.0f;
		Timer mt_LastFrameTime;

		///Framerate Debug Info.
		//Timer mt_StartingTime; //Time since starting NEEDS TO BE STARTED!!!!!!
		//Timer mt_LastSecFrameTime; //Timer to check the frames in a second (at each second) NEEDS TO BE STARTED!!!!!
		//uint m_FrameCount = 0; //Frames since starting
		//uint32 m_LastSecFrameCount = 0; //Frames in last second

		//FPS CAP
		int m_CappedMS = -1;
		int m_FPSCap = -1;

	public:

		Application(int FPSCap = -1);
		~Application();

		bool OnInit();
		update_status OnUpdate();
		bool OnCleanUp();

		inline const float GetDeltaTime() const { return m_Timestep; }
		inline const int GetFPSCap() const { return m_FPSCap; }

	private:

		void AddModule(Module* mod);
		void PrepareUpdate();
		void FinishUpdate();
	};

	extern Application* App;

}

#endif