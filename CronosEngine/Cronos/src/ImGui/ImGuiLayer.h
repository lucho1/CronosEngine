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
		void setDocking();
		void UpdateDocking();
		
		//void ImGuiTransformMenu(GameObject currentObject) {};

	private:
		float m_Time = 0.0f;

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