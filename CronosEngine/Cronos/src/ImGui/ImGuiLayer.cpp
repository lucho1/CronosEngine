
#include "cnpch.h"

#include "ImGuiLayer.h"

#include "imgui.h"
#include "OpenGL/imgui_impl_sdl.h"
#include "OpenGL/imgui_impl_opengl3.h"



#include "imnodes.h"

#include "Core/Application.h"
//#include "SDL/include/SDL.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
//#include <Hazel/stb_image.h>

//#define SRC_DIR "C:/Users/rleon/Documents/Dev/Engine/Hazel"

namespace Cronos {

	static ImGuiDockNodeFlags dockspace_flags;
	ImGuiWindowFlags window_flags;
	

	int my_image_width = 504, my_image_height = 507;
	//unsigned char* my_image_data = stbi_load("../Hazel/src/Textures/Texture_Sample.jpg", &my_image_width, &my_image_height, NULL, 4);
	GLuint my_opengl_texture;

#define TOTEX (void*)(intptr_t)

	inline int make_id(int node, int attribute) { return (node << 16) | attribute; } //temporary
	
	ImGuiLayer::ImGuiLayer(Application* app, bool start_enabled) : Module(app, "ImGuiLayer")
	{
		
	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	bool ImGuiLayer::OnStart()
	{

		ImGui::CreateContext();
		ImGui::StyleColorsCustom();
		imnodes::Initialize();

		ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
		ImGui_ImplOpenGL3_Init();

		ImGuiIO& io = ImGui::GetIO();

		std::string File = "../Cronos/vendor/imgui/misc/fonts/DroidSans.ttf";
		io.Fonts->AddFontFromFileTTF(File.c_str(), 14.0f);


		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		setDocking();
		//m_RootDirectory = std::filesystem::path("D:/Documentos/Desktop");


		AssetDirectories = App->filesystem->GetAssetDirectories();
		m_CurrentDir = AssetDirectories;
		//TEMPORARY

		glGenTextures(1, &my_opengl_texture);
		glBindTexture(GL_TEXTURE_2D, my_opengl_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, my_image_width, my_image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, my_image_data);

		return true;
	}
	static int iterations = 0;

	static void testIterator(Directories a ) {
		for (auto& c : a.childs) {

			testIterator(*c);
		}
	}

	void ImGuiLayer::AssetImguiIterator(Directories a) {
		for (auto& c : a.childs) {
			std::string temp = c->m_Directories.filename().string();
			if (ImGui::TreeNodeEx(temp.c_str())) {
				
				AssetImguiIterator(*c);	
				ImGui::TreePop();
			}
			if (ImGui::IsItemClicked()) { //TODO NEEDS TO FIX
				c->isClicked = true;
			}
			if (c->isClicked) {
				m_CurrentDir = c;
				c->isClicked = false;
			}
		}
	}

	update_status ImGuiLayer::OnUpdate(float dt)
	{
		for (auto& a : AssetDirectories->childs) {
			iterations++;
			testIterator(*a);
		}

		int test = DirectoriesArray.size();
		static bool DockspaceInitiate;
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(App->window->GetWidth(), App->window->GetHeight());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(App->window->window);
		ImGui::NewFrame();

		// DockSpace
		UpdateDocking();
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &DockspaceInitiate, window_flags);
		ImGui::PopStyleVar();

		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");

			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		//Only to test
		static bool show = true;
		//ImGui::ShowDemoWindow(&show);
		//ImGui::End();


		GUIDrawMainBar();
		if (ShowInspectorPanel)			GUIDrawInspectorMenu();
		if (ShowHierarchyMenu)			GUIDrawHierarchyPanel();
		if (ShowAssetMenu)				GUIDrawAssetPanel();
		if (ShowNodeEditorPanel)		GUIDrawNodeEditorPanel();
		if (ShowConsolePanel)			GUIDrawConsolePanel();
		if (ShowDemoWindow)				ImGui::ShowDemoWindow(&ShowDemoWindow);
		if (ShowConfigurationPanel)		GUIDrawConfigurationPanel();

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		return UPDATE_CONTINUE;
	}


	void ImGuiLayer::setDocking() {

		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		}

	}



	void ImGuiLayer::UpdateDocking() {

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
	}

	void ImGuiLayer::GUIDrawMainBar()
	{
		
		ImGui::BeginMainMenuBar();
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
			if (ImGui::BeginMenu("File")) {
				
				ImGui::MenuItem("New Scene");			
				ImGui::MenuItem("Open Scene");
				ImGui::Separator();
				ImGui::MenuItem("Save");
				ImGui::MenuItem("Save As...");
				ImGui::Separator();
				ImGui::MenuItem("New Project");
				ImGui::MenuItem("Open Project");
				ImGui::MenuItem("Save Project");
				ImGui::Separator();
				if (ImGui::MenuItem("Configuration")) {
					ShowConfigurationPanel = true;
				}

				ImGui::MenuItem("Exit");
			
				ImGui::EndMenu();	
			}
			
			if (ImGui::BeginMenu("Edit")) {
				ImGui::MenuItem("New");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Asset")) {
				ImGui::MenuItem("New");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug")) {
				ImGui::MenuItem("New");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window")) {
				if (ImGui::MenuItem("NodeEditor")) {
					ShowNodeEditorPanel = true;
				}
				if (ImGui::MenuItem("Inspector")) {
					ShowInspectorPanel = true;
				}
				if (ImGui::MenuItem("Hierarchy")) {
					ShowHierarchyMenu = true;
				}
				if (ImGui::MenuItem("Asset")) {
					ShowAssetMenu = true;
				}
				if (ImGui::MenuItem("Console")) {
					ShowConsolePanel = true;
				}
				if (ImGui::MenuItem("Demo Window")) {
					ShowDemoWindow = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				ImGui::MenuItem("New");
				ImGui::EndMenu();
			}
		}
		ImGui::PopStyleVar();
		ImGui::EndMainMenuBar();

	}

	void ImGuiLayer::GUIDrawInspectorMenu()
	{
		ImGui::SetNextWindowSize(ImVec2(500, 400));
		ImGui::Begin("Inspector", &ShowInspectorPanel);
		ImGui::Checkbox(" ", &ShowInspectorPanel); ImGui::SameLine();
		static char buf1[64] = "Target"; ImGui::InputText("###", buf1, 64, ImGuiInputTextFlags_CharsNoBlank);
		ImGui::Separator();

		GUIDrawTransformPMenu();
		GUIDrawMaterialsMenu();

		ImGui::End();

	}

	void ImGuiLayer::GUIDrawTransformPMenu()
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static float f0 = 1.0f, f1 = 2.0f, f2 = 3.0f;
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Position               "); ImGui::SameLine();
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##valueX", &f0, 0.1f); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##valueY", &f1, 0.1f); ImGui::SameLine();
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##valueZ", &f2, 0.1f);
			ImGui::Text("Rotation              "); ImGui::SameLine();
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##value1", &f0, 0.1f); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##value2", &f1, 0.1f); ImGui::SameLine();
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##value3", &f2, 0.1f);
			ImGui::Text("Scale                    "); ImGui::SameLine();
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##value4", &f0, 0.1f); ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##value5", &f1, 0.1f); ImGui::SameLine();
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); ImGui::DragFloat("##value6", &f2, 0.1f);
		}
		ImGui::Separator();
	}

	void ImGuiLayer::GUIDrawMaterialsMenu()
	{
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static bool alpha_preview = true;
			static bool alpha_half_preview = false;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = false;
			static int FramePaddingMaterials = 2;
			static ImVec4 ref_color_v(1.0f, 0.0f, 1.0f, 0.5f);

			static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 255.0f / 255.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.392f, 0.369f, 0.376f, 0.10f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.128f, 0.128f, 0.128f, 0.55f));

			ImGui::ImageButton(NULL, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials); ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("\n   Albedo"); ImGui::SameLine();
			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
			ImVec2 FramePadding(100.0f, 3.0f);
			//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, FramePadding); ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 30));
			ImGui::ColorEdit4(" \n MyColor##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags);
			ImGui::PopStyleVar();


			ImGui::ImageButton(NULL, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials); ImGui::SameLine();
			//ImGui::AlignTextToFramePadding();
			ImGui::Text("\n   Specular"); ImGui::SameLine();
			static float SpecIntensity = 1.0f;
			//ImGui::DragFloatRange2("range", &begin, &end, 0.25f, 0.0f, 100.0f, "Min: %.1f %%", "Max: %.1f %%");
			//ImGui::AlignTextToFramePadding();
			ImGui::PushItemWidth(70); ImGui::SliderFloat("##", &SpecIntensity, 0.0f, 1.0f);

			if (ImGui::ImageButton(TOTEX my_opengl_texture, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials)) {
				ImGui::OpenPopup("Context");
			}
			ImGui::SameLine();
			ImGui::Text("\n   Metallic");

			ImGui::ImageButton(NULL, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials); ImGui::SameLine();
			ImGui::Text("\n   Roughtness");
			if (ImGui::Button("Hello")) {
				ImGui::OpenPopup("Context");
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::Separator();
		}
	}

	void ImGuiLayer::GUIDrawHierarchyPanel()
	{

		ImGui::Begin("Hierarchy", &ShowHierarchyMenu);
		{
			if (ImGui::TreeNode("Tree"))
			{
				for (int x = 0; x < 3; x++)
				{
					bool open1 = ImGui::TreeNode((void*)(intptr_t)x, "Node%d", x);

					if (open1)
					{
						for (int y = 0; y < 3; y++)
						{
							bool open2 = ImGui::TreeNode((void*)(intptr_t)y, "Node%d.%d", x, y);
							if (open2)
							{
								ImGui::Text("Even more contents");
								if (ImGui::TreeNode("Tree in column"))
								{
									ImGui::TreePop();
								}
							}
							if (open2)
								ImGui::TreePop();
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();

	}

	void ImGuiLayer::GUIDrawAssetPanel()
	{
		ImGui::Begin("Project", &ShowAssetMenu, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Create"))
				{
					ImGui::MenuItem("Asset");
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGuiIO& io = ImGui::GetIO();

			static int WindowSize = 150;
			ImGui::BeginChild("left panel", ImVec2(WindowSize, 0),true);
			
			if (ImGui::TreeNode(App->filesystem->GetLabelAssetRoot().c_str())) {
			// left
				for (auto& a : AssetDirectories->childs)
				{
					
					std::string	temp = a->m_Directories.filename().string();
					if (ImGui::TreeNode(temp.c_str())){	
						
						/*if (ImGui::IsItemClicked()) {
							m_CurrentDir = a;
						}*/
						AssetImguiIterator(*a);		
						ImGui::TreePop();
					}	
					if (ImGui::IsItemClicked()) {
						a->isClicked = true;
					}
					if (a->isClicked) {
						m_CurrentDir = a;
					}
					
				}
				ImGui::TreePop();
			}
			ImGui::EndChild();

			const char* SceneLabel = "Scenes";
		

			ImGui::SameLine();

			// right
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			std::string tempstring = m_CurrentDir->m_Directories.filename().string();

		
			ImGui::Text("CurrentWindow: %s", tempstring.c_str());
			ImGui::Separator();
			int spaceCounter = 150;
			for (auto& a : m_CurrentDir->m_Container) {
				a.DrawIcons();
				spaceCounter += a.GetElementSize();
				int b = ImGui::GetWindowWidth();
				if (spaceCounter < ImGui::GetWindowWidth()) {
					ImGui::SameLine();

				}
				else
					spaceCounter = 150;
				//ImGui::Button(a.m_Elements.c_str());
			}

			ImGui::EndChild();
			ImGui::EndGroup();
			
		}
		ImGui::End();
	}

	void ImGuiLayer::GUIDrawNodeEditorPanel()
	{
		ImGui::PushStyleColor(ImGuiCol_TitleBg | ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2);
		if (ImGui::Begin("Node Editor", &ShowNodeEditorPanel)) {
			const int hardcoded_node_id = 1;
			imnodes::BeginNodeEditor();
			{


				/*	imnodes::SetNodeName(hardcoded_node_id, "output node");
					imnodes::BeginNode(hardcoded_node_id);
					const int output_attr_id = 2;
					imnodes::BeginOutputAttribute(output_attr_id);
					ImGui::Indent(40);
					ImGui::Text("output pin");
					imnodes::EndAttribute();
					imnodes::EndNode();

					imnodes::SetNodeName(2, "input node");
					imnodes::BeginNode(2);
					imnodes::BeginInputAttribute(3);
					ImGui::Text("Input pin");
					imnodes::EndAttribute();
					imnodes::EndNode();*/

				for (auto& elem : float_nodes_)
				{
					const float node_width = 150.0f;
					imnodes::BeginNode(elem.first);

					imnodes::BeginInputAttribute(make_id(elem.first, 0));
					ImGui::Text("input");
					imnodes::EndAttribute();
					ImGui::Spacing();
					{
						const float label_width = ImGui::CalcTextSize("number").x;
						ImGui::Text("number");
						ImGui::PushItemWidth(node_width - label_width - 6.0f);
						ImGui::SameLine();
						ImGui::DragFloat("##hidelabel", &elem.second, 0.01f);
						ImGui::PopItemWidth();
					}
					ImGui::Spacing();
					{
						imnodes::BeginOutputAttribute(make_id(elem.first, 1));
						const float label_width = ImGui::CalcTextSize("output").x;
						ImGui::Indent(node_width - label_width - 1.5f);
						ImGui::Text("output");
						imnodes::EndAttribute();
					}

					imnodes::EndNode();
				}

				for (auto& elem : color_nodes_)
				{
					const float node_width = 200.0f;
					imnodes::BeginNode(elem.first);

					imnodes::BeginInputAttribute(make_id(elem.first, 0));
					ImGui::Text("input");
					imnodes::EndAttribute();
					ImGui::Spacing();

					{
						imnodes::BeginOutputAttribute(make_id(elem.first, 1));
						const float label_width = ImGui::CalcTextSize("color").x;
						ImGui::PushItemWidth(node_width - label_width - 6.0f);
						ImGui::ColorEdit3("color", elem.second.data);
						ImGui::PopItemWidth();
						imnodes::EndAttribute();
					}
					ImGui::Spacing();
					{
						imnodes::BeginOutputAttribute(make_id(elem.first, 2));
						const float label_width = ImGui::CalcTextSize("output").x;
						ImGui::Indent(node_width - label_width - 1.5f);
						ImGui::Text("output");
						imnodes::EndAttribute();
					}

					imnodes::EndNode();
				}

				if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
				{
					ImGui::OpenPopup("context menu");
				}

				if (ImGui::BeginPopup("context menu"))
				{
					int new_node = -1;
					ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();

					if (ImGui::MenuItem("drag float node"))
					{
						new_node = current_id_++;
						float_nodes_.insert(std::make_pair(new_node, 0.f));
						imnodes::SetNodeName(new_node, "drag float");
					}

					if (ImGui::MenuItem("color node"))
					{
						new_node = current_id_++;
						color_nodes_.insert(std::make_pair(new_node, Color3{}));
						imnodes::SetNodeName(new_node, "color");
					}

					ImGui::EndPopup();

					if (new_node != -1)
					{
						imnodes::SetNodePos(new_node, click_pos);
					}
				}

				for (const auto linkpair : links_)
				{
					imnodes::Link(
						linkpair.first, linkpair.second.start, linkpair.second.end);
				}

			}
			imnodes::EndNodeEditor();

			int link_start, link_end;
			if (imnodes::IsLinkCreated(&link_start, &link_end))
			{
				links_.insert(
					std::make_pair(current_id_++, Link{ link_start, link_end }));
			}

			int link_id;
			if (imnodes::IsLinkSelected(&link_id))
			{
				//if (ImGui::IsKeyReleased(SDL_SCANCODE_X))
				//{
				links_.erase(link_id);
				//}
			}
			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}
	}

	void ImGuiLayer::GUIDrawConfigurationPanel() {

		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

		ImGui::PushStyleColor(ImGuiCol_TitleBg | ImGuiCol_TitleBgActive, ImVec4(0.392f, 0.369f, 0.376f, 1.00f));
		ImGui::Begin("Configuration", &ShowConfigurationPanel, ImGuiWindowFlags_NoDocking);
		ImGui::PopStyleColor();

		ImGui::BeginGroup();
		int selected = 0;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);
		if (ImGui::Selectable("Application"))
			selected = 1;
		else if (ImGui::Selectable("Window"))
			selected = 2;
		else if (ImGui::Selectable("Hardware"))
			selected = 3;
		else if (ImGui::Selectable("Renderer"))
			selected = 4;
		else if (ImGui::Selectable("Input"))
			selected = 5;
		else if (ImGui::Selectable("Audio"))
			selected = 6;
		else if (ImGui::Selectable("Texture"))
			selected = 7;
		ImGui::PopStyleVar();


		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::EndGroup();


		ImGui::End();

	}



	struct ExampleAppLog
	{
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
		bool                AutoScroll;     // Keep scrolling if already at the bottom

		ExampleAppLog()
		{
			AutoScroll = true;
			Clear();
		}

		void    Clear()
		{
			Buf.clear();
			LineOffsets.clear();
			LineOffsets.push_back(0);
		}

		void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
		{
			int old_size = Buf.size();
			va_list args;
			va_start(args, fmt);
			Buf.appendfv(fmt, args);
			va_end(args);
			for (int new_size = Buf.size(); old_size < new_size; old_size++)
				if (Buf[old_size] == '\n')
					LineOffsets.push_back(old_size + 1);
		}

		void    Draw(const char* title, bool* p_open = NULL)
		{
			if (!ImGui::Begin(title, p_open))
			{
				ImGui::End();
				return;
			}

			// Options menu
			if (ImGui::BeginPopup("Options"))
			{
				ImGui::Checkbox("Auto-scroll", &AutoScroll);
				ImGui::EndPopup();
			}

			// Main window
			if (ImGui::Button("Options"))
				ImGui::OpenPopup("Options");
			ImGui::SameLine();
			bool clear = ImGui::Button("Clear");
			ImGui::SameLine();
			bool copy = ImGui::Button("Copy");
			ImGui::SameLine();
			Filter.Draw("Filter", -100.0f);

			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

			if (clear)
				Clear();
			if (copy)
				ImGui::LogToClipboard();

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			const char* buf = Buf.begin();
			const char* buf_end = Buf.end();
			if (Filter.IsActive())
			{
				// In this example we don't use the clipper when Filter is enabled.
				// This is because we don't have a random access on the result on our filter.
				// A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
				// especially if the filtering function is not trivial (e.g. reg-exp).
				for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
				{
					const char* line_start = buf + LineOffsets[line_no];
					const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
					if (Filter.PassFilter(line_start, line_end))
						ImGui::TextUnformatted(line_start, line_end);
				}
			}
			else
			{
				// The simplest and easy way to display the entire buffer:
				//   ImGui::TextUnformatted(buf_begin, buf_end);
				// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
				// Here we instead demonstrate using the clipper to only process lines that are within the visible area.
				// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
				// Using ImGuiListClipper requires A) random access into your data, and B) items all being the  same height,
				// both of which we can handle since we an array pointing to the beginning of each line of text.
				// When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use the clipper.
				// Storing or skimming through the search result would make it possible (and would be recommended if you want to search through tens of thousands of entries)
				ImGuiListClipper clipper;
				clipper.Begin(LineOffsets.Size);
				while (clipper.Step())
				{
					for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
					{
						const char* line_start = buf + LineOffsets[line_no];
						const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
						ImGui::TextUnformatted(line_start, line_end);
					}
				}
				clipper.End();
			}
			ImGui::PopStyleVar();

			if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);

			ImGui::EndChild();
			ImGui::End();
		}
	};



	void ImGuiLayer::GUIDrawConsolePanel()
	{
		static ExampleAppLog log;

		// For the demo: add a debug button _BEFORE_ the normal log window contents
		// We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
		// Most of the contents of the window will be added by the log.Draw() call.
		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::Begin("Console", &ShowConsolePanel);
		if (ImGui::SmallButton("[Debug] Add 5 entries"))
		{
			static int counter = 0;
			for (int n = 0; n < 5; n++)
			{
				const char* categories[3] = { "info", "warn", "error" };
				const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
				log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
					ImGui::GetFrameCount(), categories[counter % IM_ARRAYSIZE(categories)], ImGui::GetTime(), words[counter % IM_ARRAYSIZE(words)]);
				counter++;
			}
		}

		static std::string toLog = TestLog.str();
		if (!toLog.empty()) {
			log.AddLog(toLog.c_str());
			toLog.clear();
		}
		ImGui::End();

		// Actually call in the regular Log helper (which will Begin() into the same window as we just did)
		log.Draw("Console", &ShowConsolePanel);
	}
}