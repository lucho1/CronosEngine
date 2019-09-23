#pragma once

#include "Core/Module.h"


namespace Cronos {


	class ImGuiLayer : public Module
	{
	public:
		ImGuiLayer(Application* app, bool start_enabled=true);
		~ImGuiLayer();
		
		update_status OnUpdate(float dt) override;
		
		bool OnStart() override;
		//void OnUpdate();
		//void OnAttach();
		void OnDetach();


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

	private:
		float m_Time = 0.0f;
		bool ShowMainMenu = true;
		bool ShowInspectorPanel = true;
		bool ShowHierarchyMenu = true;
		bool ShowAssetMenu = true;
		bool ShowNodeEditorPanel = false;


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