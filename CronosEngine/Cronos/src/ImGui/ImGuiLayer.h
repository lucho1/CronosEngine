#pragma once

#include "Core/Module.h"
#include "imgui.h"
#include "Modules/Filesystem.h"
#include "GameObject/GameObject.h"

struct ImGuiTextBuffer;

namespace Cronos {

	class FrameBuffer;

	enum class ShadingMode
	{
		Shaded = 0,
		Wireframe,
		ShadedWireframe,
		MaxElements
	};

	enum class ConfigMenus {
		Application = 0,
		Window,
		Hardware,
		Software,
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

		virtual update_status OnPreUpdate(float dt) override;
		virtual update_status OnUpdate(float dt) override;
		//virtual update_status OnPostUpdate(float dt) override;

		virtual bool OnStart() override;

		void AssetImguiIterator(Directories a);

		Directories* m_CurrentDir;
		AssetItems* m_CurrentAssetSelected = nullptr;


		std::stringstream TestLog;

		void GetInput(uint key, uint state, bool mouse=false);
		ShadingMode GetCurrentShading() const { return m_currentShadingMode; }

		void AddLog(std::string log);

		inline bool isHoveringWinGame() const { return HoverGameWin; }

	private:

		void setDocking();
		void UpdateDocking();

		void GUIDrawMainBar();
		void GUIDrawInspectorMenu(GameObject* CurrentGameObject);
		void GUIDrawAssetLabelInspector();
		void GUIDrawTransformPMenu(GameObject* CurrentGameObject);
		void GUIDrawMaterialsMenu();
		void GUIDrawHierarchyPanel();
		void GUIDrawAssetPanel();
		void GUIDrawNodeEditorPanel();
		void GUIDrawConsolePanel();
		void GUIDrawConfigurationPanel();
		void GUIDrawPerformancePanel();
		void GUIDrawSupportExitOptions();
		void GUIDrawAboutPanel();
		void GUIDrawSceneWindow();
		void GUIDrawWidgetMenu();
		//Config Menus
		void GUIDrawConfigApplicationMenu();
		void GUIDrawConfigWindowMenu();
		void GUIDrawConfigHardwareMenu();
		void GUIDrawConfigSoftwareMenu();
		void GUIDrawConfigRendererMenu();
		void GUIDrawConfigInputMenu();
		void GUIDrawConfigAudioMenu();
		void GUIDrawConfigTexturesMenu();


		void AcumulateLogDT();

		//void ImGuiTransformMenu(GameObject currentObject) {};

	private:

		FrameBuffer* m_SceneWindow = nullptr;

		float m_Time = 0.0f;
		bool ShowMainMenu = true;
		bool ShowDrawGameWindow = true;
		bool ShowInspectorPanel = true;
		bool ShowHierarchyMenu = true;
		bool ShowAssetMenu = true;
		bool ShowNodeEditorPanel = false;
		bool ShowConsolePanel = true;
		bool ShowPerformancePanel = false;
		bool ShowDemoWindow = false;
		bool ShowAboutPanel = false;
		bool ShowConfigurationPanel = false;
		bool ShowExitOpitonsPopUp = false;
		bool AssetLabelInspector = false;

		bool HoverGameWin = false;

		std::string m_ShadingModesLabel[3];

		uint PlayPauseTempImage;

		update_status current_status= update_status::UPDATE_CONTINUE;

		ImGuiTextBuffer LogBuffer;
		ImGuiTextBuffer LogInputs;

		std::vector<float>ms_log;
		std::vector<float>fps_log;

		Directories* AssetDirectories;
		Directories* LastDir;

		std::string LicenseString;

		std::vector <Directories*> DirectoriesArray;

		void HierarchyIterator(GameObject GameObjects);
		ImGuiTreeNodeFlags Treenode_flags;

		ShadingMode m_currentShadingMode;
		GameObject* CurrentGameObject=nullptr;
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
