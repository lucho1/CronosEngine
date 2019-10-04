#pragma once

#include "Core/Module.h"
#include "imgui.h"
#include "Modules/Filesystem.h"

struct ImGuiTextBuffer;

namespace Cronos {

	enum class ConfigMenus {
		Application = 0,
		Window,
		Hardware,
		Renderer,
		Input,
		Audio,
		Texture

	};

	class ImGuiLayer : public Module
	{
	public:

		ImGuiLayer(Application* app, bool start_enabled=true);
		~ImGuiLayer();
		
		virtual update_status OnUpdate(float dt) override;
		
		virtual bool OnStart() override;

		void AssetImguiIterator(Directories a);

		Directories* m_CurrentDir;

		std::stringstream TestLog;

		void GetInput(uint key, uint state, bool mouse=false);
		void AddLog(std::string log);

	private:

		void setDocking();
		void UpdateDocking();

		void GUIDrawMainBar();
		void GUIDrawInspectorMenu();
		void GUIDrawTransformPMenu();
		void GUIDrawMaterialsMenu();
		void GUIDrawHierarchyPanel();
		void GUIDrawAssetPanel();
		void GUIDrawNodeEditorPanel();
		void GUIDrawConsolePanel();
		void GUIDrawConfigurationPanel();
		void GUIDrawPerformancePanel();
		void GUIDrawSupportExitOptions();
		//Config Menus
		void GUIDrawConfigApplicationMenu();
		void GUIDrawConfigWindowMenu();
		void GUIDrawConfigHardwareMenu();
		void GUIDrawConfigRendererMenu();
		void GUIDrawConfigInputMenu();
		void GUIDrawConfigAudioMenu();
		void GUIDrawConfigTexturesMenu();


		void AcumulateLogDT();

		//void ImGuiTransformMenu(GameObject currentObject) {};

	private:

		float m_Time = 0.0f;
		bool ShowMainMenu = true;
		bool ShowInspectorPanel = true;
		bool ShowHierarchyMenu = true;
		bool ShowAssetMenu = true;
		bool ShowNodeEditorPanel = false;
		bool ShowConsolePanel = true;
		bool ShowPerformancePanel = false;
		bool ShowDemoWindow = false;
		bool ShowConfigurationPanel = false;
		bool ShowExitOpitonsPopUp = false;

		update_status current_status= update_status::UPDATE_CONTINUE;

		ImGuiTextBuffer LogBuffer;
		ImGuiTextBuffer LogInputs;

		std::vector<float>ms_log;
		std::vector<float>fps_log;

		Directories* AssetDirectories;
		Directories* LastDir;

		std::vector <Directories*> DirectoriesArray;

		ConfigMenus currentMenu= ConfigMenus::Application;

		struct Link
		{
			int start, end;
		};

		struct Color3
		{
			float data[3];
		};

		int current_id_;
		std::unordered_map<int, float> float_nodes_;
		std::unordered_map<int, Color3> color_nodes_;
		std::unordered_map<int, Link> links_;
	
	};


}