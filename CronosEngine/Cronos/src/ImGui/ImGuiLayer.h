#pragma once

#include "Core/Module.h"


namespace Cronos {

	enum class ItemType 
	{
		ITEM_NONE=0,
		ITEM_FBX,
		ITEM_OBJ,
		ITEM_MATERIAL,
		ITEM_SHADER,
		ITEM_SCRIPT,
		ITEM_FOLDER,
		ITEM_TEXTURE
	};

	class AssetItems {
	public:
		AssetItems(std::filesystem::path m_Path);

		ItemType type = ItemType::ITEM_NONE;
		std::string m_Elements;
		void DrawIcons();

		int GetElementSize();

	private:
		std::string m_Extension;
		int m_ElementSize;
	};

	class Directories {
	public:

		Directories(std::filesystem::path m_Path);

		std::filesystem::path m_Directories;
		std::string m_LabelDirectories;
		int m_DepthID;
		int m_ID;
		std::vector<AssetItems> m_Container;
		std::list<Directories*>childs;

	};

	class ImGuiLayer : public Module
	{
	public:

		ImGuiLayer(Application* app, bool start_enabled=true);
		~ImGuiLayer();
		
		update_status OnUpdate(float dt) override;
		
		bool OnStart() override;

		void AssetImguiIterator(Directories a);

		Directories* m_CurrentDir;

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
		//void ImGuiTransformMenu(GameObject currentObject) {};

	
		Directories* LoadCurrentDirectories(std::filesystem::path filepath);

	private:

		std::filesystem::path m_RootDirectory; //Temporary
		std::string m_LabelRootDirectory;

		float m_Time = 0.0f;
		bool ShowMainMenu = true;
		bool ShowInspectorPanel = true;
		bool ShowHierarchyMenu = true;
		bool ShowAssetMenu = true;
		bool ShowNodeEditorPanel = false;

		Directories* AssetDirectories;

		std::vector <Directories*> DirectoriesArray;

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