#include "cnpch.h"
#include "Application.h"

namespace Cronos {

	Application::Application(int FPSCap) : m_FPSCap(FPSCap)
	{
		window = new SDLWindow(this);
		input = new Input(this);
		audio = new Audio(this);
		scene = new Scene(this);
		renderer3D = new GLRenderer3D(this);
		camera = new Camera3D(this);
		filesystem = new Filesystem(this);

		EditorGUI = new ImGuiLayer(this);

		// The order of calls is very important!
		// Modules will Init() Start() and Update in this order
		// They will CleanUp() in reverse order

		// Main Modules
		AddModule(window);
		AddModule(renderer3D);
		AddModule(camera);
		AddModule(input);
		AddModule(audio);


		// Scenes
		AddModule(scene);
		AddModule(filesystem);
		AddModule(EditorGUI);

		if (m_FPSCap > 0)
			m_CappedMS = 1000 / FPSCap;

	}

	Application::~Application()
	{
		for (auto& element : m_ModulesList)
			if (element)
				delete element; //TODO: Does this work?
	}

	bool Application::OnInit()
	{
		bool ret = true;


		for (auto& element : m_ModulesList)
			if (ret)
				ret = element->OnInit();

		// After all Init calls we call Start() in all modules
		LOG("Application Start --------------");
		for (auto& element : m_ModulesList)
			if (ret)
				ret = element->OnStart();

		return ret;
	}


	// ---------------------------------------------
	void Application::PrepareUpdate()
	{
		//For performance information purposes
		//m_FrameCount++;
		// m_LastSecFrameCount++;

		m_Timestep = mt_LastFrameTime.ReadSec();
		mt_LastFrameTime.Start();
	}

	// ---------------------------------------------
	void Application::FinishUpdate()
	{
		//For performance information purposes
		//if(mt_LastSecFrameTime.Read() > 1000) 
		//{
		//	mt_LastSecFrameTime.Start();
		//	uint32 tmp_LastSecFrameCount = m_LastSecFrameCount;
		//	m_LastSecFrameCount = 0;
		//}

		//float m_AverageFPS = float(m_FrameCount) / mt_StartingTime.ReadSec();
		//Cool info gotten from previous info: Secs since start (mt_StartingTime.ReadSec())
		//|| Frames in last update (current_framecount - last_frame count), probably you'll need to declare a new value or something

		uint32 LastFrameMS = mt_LastFrameTime.Read();
		if (m_CappedMS > 0 && (int)LastFrameMS < m_CappedMS)
			SDL_Delay(m_CappedMS - LastFrameMS);

	}

	// Call PreUpdate, Update and PostUpdate on all modules
	update_status Application::OnUpdate()
	{
		update_status ret = UPDATE_CONTINUE;
		PrepareUpdate();

		for (auto& element : m_ModulesList)
			if (ret)
				ret = element->OnPreUpdate(m_Timestep);

		for (auto& element : m_ModulesList)
			if (ret)
				ret = element->OnUpdate(m_Timestep);

		for (auto& element : m_ModulesList)
			if (ret)
				ret = element->OnPostUpdate(m_Timestep);

		FinishUpdate();
		return ret;
	}

	bool Application::OnCleanUp()
	{
		bool ret = true;

		for (auto& element : m_ModulesList)
			if (ret)
				ret = element->OnCleanUp();

		return ret;
	}

	void Application::AddModule(Module* mod)
	{
		m_ModulesList.push_back(mod);
	}

}