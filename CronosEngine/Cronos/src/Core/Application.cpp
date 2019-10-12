#include "Providers/cnpch.h"

#include "Application.h"
#include "Helpers/RNGen.h"

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

		EditorGUI = new ImGuiLayer(this);

		// The order of calls is very important!
		// Modules will Init() Start() and Update in this order
		// They will CleanUp() in reverse order

		// Main Modules
		AddModule(window);
		AddModule(renderer3D);
		AddModule(engineCamera);
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

		//Not a Module!
		//SystemInfo AppSystemInfo;
		//Json file
		m_DefaultConfigurationFilepath = "res/configuration/config.json";
		mt_SaveTimer.Start();
		//LoadJsonFile("res/configuration/config.json");
		//SaveJsonFile("res/configuration/config.json");

		for (auto& element : m_ModulesList)
			if (ret) {
				EditorGUI->AddLog(("Initializing Module" + element->m_ModuleName));
				ret = element->OnInit();
			}
		// After all Init calls we call Start() in all modules
		LOG("Application Start --------------");
		for (auto& element : m_ModulesList)
			if (ret)
				ret = element->OnStart();

		mt_StartingTime.Start();

		//Not a Module!
		//SystemInfo AppSystemInfo;

		return ret;
	}


	// ---------------------------------------------
	void Application::PrepareUpdate()
	{
		//For performance information purposes
		m_FrameCount++;
		m_LastSecFrameCount++;

		m_Timestep = mt_LastFrameTime.ReadSec();
		mt_LastFrameTime.Start();
	}

	// ---------------------------------------------
	void Application::FinishUpdate()
	{
		if (m_MustLoad)
			LoadJsonFile(m_DefaultConfigurationFilepath.c_str());
		if (m_MustSave || mt_SaveTimer.ReadSec() > 30.0f)
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
		PrepareUpdate();

		for (auto& element : m_ModulesList)
			if (ret==UPDATE_CONTINUE)
				ret = element->OnPreUpdate(m_Timestep);

		for (auto& element : m_ModulesList)
			if (ret == UPDATE_CONTINUE)
				ret = element->OnUpdate(m_Timestep);

		for (auto& element : m_ModulesList)
			if (ret == UPDATE_CONTINUE)
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


	// SERIALIZATION ---------------------------------------------
	void Application::LoadEngineData() const
	{
		m_MustLoad = true;
	}

	void Application::SaveEngineData() const
	{
		m_MustSave = false;
	}


	//Save/Load JSON File
	void Application::LoadJsonFile(const char* filePath) const
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

		//Now copy all the json file data into our code data, then call all modules to load
		std::string name = m_JSONConfigFile["Application"]["Name"];
		std::string version = m_JSONConfigFile["Application"]["Version"];

		//Call all modules to load


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
		aux_JSONFile["Application"]["Name"] = "CronosEngine";
		aux_JSONFile["Application"]["Version"] = "v0.1";

		//TODO: Do with the loaded data what you have to do to make it useful
		//TODO: Create a default time to save

		//Call modules to Save into json aux file







		OutputFile_Stream << std::setw(4) << aux_JSONFile << std::endl;
		OutputFile_Stream.close();
		m_JSONConfigFile = aux_JSONFile;
		m_MustSave = false;
		mt_SaveTimer.Start();
	}

	const void Application::RequestBrowser(const char* WebDirection)
	{
		ShellExecuteA(NULL, "open", WebDirection, NULL, NULL, SW_SHOWNORMAL);
	}

	void Application::SetFPSCap(int FPScap)
	{
		if (FPScap > 0) {
			m_CappedMS = 1000 / FPScap;
			m_FPSCap = FPScap;
		}
		else
			LOG("FPS CAP must be bigger than 0!!")
	}

}
