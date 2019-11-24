#include "Providers/cnpch.h"

#include "Application.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	Application::Application(int FPSCap) : m_FPSCap(FPSCap)
	{
		window = new SDLWindow(this);
		input = new Input(this);
		audio = new Audio(this);
		scene = new Scene(this);
		renderer3D = new GLRenderer3D(this);
		engineCamera = new EngineCamera(this);
		filesystem = new Filesystem(this);
		textureManager = new TextureManager(this);
		EditorGUI = new ImGuiLayer(this);
		resourceManager = new ResourceManager(this);

		// The order of calls is very important!
		// Modules will Init() Start() and Update in this order
		// They will CleanUp() in reverse order

		// Main Modules
		AddModule(window);
		AddModule(renderer3D);
		AddModule(resourceManager);
		AddModule(engineCamera);
		AddModule(input);
		AddModule(audio);
		AddModule(textureManager);
	

		// Scenes
		AddModule(filesystem);

		AddModule(scene);
		AddModule(EditorGUI);
	}

	Application::~Application()
	{
		for (auto&& element : m_ModulesList)
			RELEASE(element);

		m_ModulesList.clear();
	}

	bool Application::OnInit()
	{
		bool ret = true;
		COMPILATIONLOGINFO;

		//Loading...
		m_DefaultConfigurationFilepath = "res/configuration/config.json";
		LoadJsonFile(m_DefaultConfigurationFilepath.c_str());
		mt_SaveTimer.Start();

		int i = 0;
		//Init all Modules
		for (auto& element : m_ModulesList)
		{
			if (ret) {
				EditorGUI->AddLog(("Initializing " + element->m_ModuleName));
				ret = element->OnInit();
			}
		}

		// After all Init calls we call Start() in all modules
		LOG("-------------- Application Start --------------");
		i = 0;
		for (auto& element : m_ModulesList)
		{
			if (ret)
				ret = element->OnStart();
		}

		mt_StartingTime.Start();
		m_GameTimer.Stop();

		return ret;
	}


	// ---------------------------------------------
	void Application::PrepareUpdate()
	{
		if (m_GT_Play)
		{
			if (m_GameTimer.IsActive() && m_GameTimer.IsPaused())
				m_GameTimer.Play();
			else
			{
				m_GameTimer.Start();
				m_Slower_dt = false;
				m_Faster_dt = false;
				m_NextFrameUpdate = false;

				//SavePlayedScene CHECK IF SCENE IS NULLPTR FIRST!!!!!!!!!
			}
		}

		if (m_GT_Stop)
		{
			m_GameTimer.Stop();
			m_Slower_dt = false;
			m_Faster_dt = false;
			m_NextFrameUpdate = false;

			//LoadPlayedScene CHECK IF SCENE IS NULLPTR FIRST!!!!!!!!!
		}

		if (m_GT_Pause)
			m_GameTimer.Pause();

		if (m_GT_Faster && m_GameTimer.IsActive()) m_Faster_dt = !m_Faster_dt;
		if (m_GT_Slower && m_GameTimer.IsActive()) m_Slower_dt = !m_Slower_dt;
		if (m_GT_NextFrame && m_GameTimer.IsActive()) m_NextFrameUpdate = !m_NextFrameUpdate;

		m_GT_Play = m_GT_Pause = m_GT_Stop = false;
		m_GT_Slower = m_GT_Faster = m_GT_NextFrame = false;
		m_GT_SaveScene = false;

		m_GameTimer_Time = m_GameTimer.ReadSec();


		//For performance information purposes
		m_FrameCount++;
		m_LastSecFrameCount++;

		m_Game_dt = m_Timestep = mt_LastFrameTime.ReadSec();
		mt_LastFrameTime.Start();

		if (m_Slower_dt) m_Game_dt = m_Timestep / 2.0f;
		if (m_Faster_dt) m_Game_dt = m_Timestep * 2.0f;
		if (m_GameTimer.IsActive() && m_GameTimer.IsPaused()) m_Game_dt = 0.0f;
		if (m_NextFrameUpdate && !m_GameTimer.IsPaused())
		{
			m_Game_dt = m_Timestep;
			m_NextFrameUpdate = false;
		}

		//SavePlayedScene with a button CHECK IF SCENE IS NULLPTR FIRST!!!!!!!!!
	}

	// ---------------------------------------------
	void Application::FinishUpdate()
	{
		if (m_MustLoad)
			LoadJsonFile(m_DefaultConfigurationFilepath.c_str());
		if (m_MustSave || mt_SaveTimer.ReadSec() > SaveTime)
			SaveJsonFile(m_DefaultConfigurationFilepath.c_str());


		//For performance information purposes
		if(mt_LastSecFrameTime.Read() > 1000)
		{
			mt_LastSecFrameTime.Start();
			m_PREV_LastSecFrameCount = m_LastSecFrameCount;
			m_LastSecFrameCount = 0;
		}

		m_AverageFPS = float(m_FrameCount) / mt_StartingTime.ReadSec();
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

		if (input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
			m_GT_Play = true;
		if (input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
			m_GT_Pause = true;
		if (input->GetKey(SDL_SCANCODE_I) == KEY_DOWN)
			m_GT_Stop = true;
		if (input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
			m_GT_Faster = true;
		if (input->GetKey(SDL_SCANCODE_Y) == KEY_DOWN)
			m_GT_Slower = true;
		if (input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
			m_GT_NextFrame = true;

		//Save scene?

		PrepareUpdate();

		for (auto& element : m_ModulesList)
		{
			if (ret == UPDATE_CONTINUE)
				ret = element->OnPreUpdate(m_Timestep);
		}
		for (auto& element : m_ModulesList)
		{
			if (ret == UPDATE_CONTINUE)
			{
				if(element == scene /*|| element == engineCamera*/) //Uncomment the thing with camera to test Play/Pause/Stop thing
					ret = element->OnUpdate(m_Game_dt);
				else
					ret = element->OnUpdate(m_Timestep);
			}
		}
		for (auto& element : m_ModulesList)
		{
			if (ret == UPDATE_CONTINUE)
				ret = element->OnPostUpdate(m_Timestep);
		}

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


	// SERIALIZATION ---------------------------------------------
	void Application::LoadEngineData() const
	{
		m_MustLoad = true;
	}

	void Application::SaveEngineData() const
	{
		m_MustSave = true;
	}


	//Save/Load JSON File
	void Application::LoadJsonFile(const char* filePath)
	{
		//If the path is nullptr, we might not want to break, but to issue a warning
		if (filePath == nullptr) {

			CRONOS_WARN(filePath != nullptr, ("Unable to find Path to load: " + std::string(filePath)));
			return;
		}

		//Load a file from the given path into an input file stream (ifstream)
		std::ifstream InputFile_Stream;
		InputFile_Stream.open(filePath);

		//Same than above
		if (InputFile_Stream.is_open() == false) {

			CRONOS_WARN(InputFile_Stream.is_open(), "Unable to Open file to load!");
			InputFile_Stream.close();
			return;
		}

		//Load the file into the json file and close the file
		m_JSONConfigFile = json::parse(InputFile_Stream);
		InputFile_Stream.close();

		//Now copy all the json file data into our code data
		m_AppName = m_JSONConfigFile["Application"]["Name"].get<std::string>();
		m_AppVersion = m_JSONConfigFile["Application"]["Version"].get<std::string>();

		m_AppOrganization = m_JSONConfigFile["Application"]["Organization"].get<std::string>();
		m_AppAuthors = m_JSONConfigFile["Application"]["Authors"].get<std::string>();
		m_FPSCap = m_JSONConfigFile["Application"]["FPS Cap"].get<int>();
		SaveTime = m_JSONConfigFile["Application"]["SaveTime"].get<float>();

		SetFPSCap(m_FPSCap);
		SetAppTitle(m_AppName);
		SetAppVersion(m_AppVersion);
		SetAppOrganization(m_AppOrganization);
		SetAppAuthors(m_AppAuthors);

		//Call all modules to load
		for (auto element : m_ModulesList)
			element->LoadModuleData(m_JSONConfigFile);

		m_MustLoad = false;
	}


	void Application::SaveJsonFile(const char* filePath) const
	{
		if (filePath == nullptr) {

			CRONOS_WARN(filePath != nullptr, ("Unable to find Path to save: " + std::string(filePath)));
			return;
		}

		//As we did first in Load(), we create now an output file stream (ofstream) to save there the json file
		std::ofstream OutputFile_Stream;
		OutputFile_Stream.open(filePath);

		if (OutputFile_Stream.is_open() == false) {

			CRONOS_WARN(OutputFile_Stream.is_open(), "Unable to Open file to save!");
			OutputFile_Stream.close();
			return;
		}

		//Auxiliar JSON File to which to save all data, then call all modules to save
		json aux_JSONFile;
		aux_JSONFile["Application"]["Name"] = m_AppName;
		aux_JSONFile["Application"]["Version"] = m_AppVersion;

		aux_JSONFile["Application"]["Organization"] = m_AppOrganization;
		aux_JSONFile["Application"]["Authors"] = m_AppAuthors;
		aux_JSONFile["Application"]["FPS Cap"] = m_FPSCap;
		aux_JSONFile["Application"]["SaveTime"] = SaveTime;

		//Call modules to Save into json aux file
		for (auto element : m_ModulesList)
			element->SaveModuleData(aux_JSONFile);


		OutputFile_Stream << std::setw(4) << aux_JSONFile << std::endl;
		OutputFile_Stream.close();
		m_JSONConfigFile = aux_JSONFile;
		m_MustSave = false;
		mt_SaveTimer.Start();
	}


	//"Setters"
	const void Application::RequestBrowser(const char* WebDirection)
	{
		ShellExecuteA(NULL, "open", WebDirection, NULL, NULL, SW_SHOWNORMAL);
	}

	void Application::SetFPSCap(int FPScap)
	{
		if (FPScap > 70)
			int a = 0;

		if (FPScap > 0) {
			m_CappedMS = 1000 / FPScap;
			m_FPSCap = FPScap;
		}
		else
			LOG("FPS CAP must be bigger than 0!!")
	}

	void Application::SetAppTitle(const std::string& name)
	{
		m_AppName = name;
		if(App->window->window != nullptr)
			App->window->SetTitle(name.c_str());
	}

	void Application::SetAppVersion(const std::string& version)
	{
		m_AppVersion = version;
	}

	void Application::SetAppOrganization(const std::string& org)
	{
		m_AppOrganization = org;
	}

	void Application::SetAppAuthors(const std::string& authors)
	{
		m_AppAuthors = authors;
	}
}
