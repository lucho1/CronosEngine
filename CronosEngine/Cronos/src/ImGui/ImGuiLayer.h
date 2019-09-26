#pragma once

#include "Core/Module.h"
#include "Modules/Filesystem.h"

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
		
		//Config Menus
		void GUIDrawConfigApplicationMenu();
		void GUIDrawConfigWindowMenu();
		void GUIDrawConfigHardwareMenu();
		void GUIDrawConfigRendererMenu();
		void GUIDrawConfigInputMenu();
		void GUIDrawConfigAudioMenu();
		void GUIDrawConfigTexturesMenu();


		//void ImGuiTransformMenu(GameObject currentObject) {};

	private:

		float m_Time = 0.0f;
		bool ShowMainMenu = true;
		bool ShowInspectorPanel = true;
		bool ShowHierarchyMenu = true;
		bool ShowAssetMenu = true;
		bool ShowNodeEditorPanel = false;
		bool ShowConsolePanel = true;
		bool ShowDemoWindow = false;
		bool ShowConfigurationPanel = false;

		Directories* AssetDirectories;

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