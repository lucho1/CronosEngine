
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

	int my_image_width=504, my_image_height=507;
	//unsigned char* my_image_data = stbi_load("../Hazel/src/Textures/Texture_Sample.jpg", &my_image_width, &my_image_height, NULL, 4);
	GLuint my_opengl_texture;

#define TOTEX (void*)(intptr_t)

inline int make_id(int node, int attribute) { return (node << 16) | attribute; }

	ImGuiLayer::ImGuiLayer(Application* app,bool start_enabled) : Module(app,"ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::UpdateDocking() {

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
	}

	void InitializeNodeEditor() {

	}

	update_status ImGuiLayer::OnUpdate(float dt)
	{

		ImGuiIO& io = ImGui::GetIO();
		//Application& app = Application::Get();
		io.DisplaySize = ImVec2(App->window->GetWidth(), App->window->GetHeight());
		//io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());
		//

		//float time = (float)glfwGetTime();
		//io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
		//m_Time = time;
		//
	
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(App->window->window);
		ImGui::NewFrame();

		static bool show = true;

		UpdateDocking();
		ImGui::SetNextWindowBgAlpha(0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &show, window_flags);
		ImGui::PopStyleVar();

		// DockSpace
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}



		ImGui::ShowDemoWindow(&show);

		//ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("Project", &show, ImGuiWindowFlags_MenuBar))
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

			// left
			const char* SceneLabel = "Scenes";
			static int selected = 0;
			ImGui::BeginChild("left panel", ImVec2(150, 0), true);
			if (ImGui::TreeNode("Assets"))
			{
				if (ImGui::TreeNode(SceneLabel)) {
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}

			ImGui::EndChild();
			ImGui::SameLine();

			// right
			ImGui::BeginGroup();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
			ImGui::Text("MyObject: %d", selected);
			ImGui::Separator();

			ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");

			ImGui::EndChild();

			ImGui::EndGroup();
		}
		static bool showNodeEditor = false;

		ImGui::End();

		ImGui::BeginMainMenuBar();
		{
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
				if (ImGui::MenuItem("NodeEditor")) {
					showNodeEditor = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug")) {
				ImGui::MenuItem("New");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window")) {
				ImGui::MenuItem("New");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				ImGui::MenuItem("New");
				ImGui::EndMenu();
			}
		}

		ImGui::EndMainMenuBar();
		ImGui::Begin("Hierarchy"); 
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
		//dockspace_flags = ImGuiDockNodeFlags_AutoHideTabBar;
		ImGui::SetNextWindowSize(ImVec2(500, 400));
		ImGui::Begin("Inspector", &show);
		ImGui::Checkbox(" ", &show); ImGui::SameLine();
		static char buf1[64] = "Target"; ImGui::InputText("###", buf1, 64, ImGuiInputTextFlags_CharsNoBlank);
		ImGui::Separator();
		if(ImGui::CollapsingHeader("Transform"))
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
		if (ImGui::CollapsingHeader("Material"))
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
			ImVec2 FramePadding(100.0f,3.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, FramePadding); ImGui::SameLine();
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
					//ImGui::AlignTextToFramePadding();
			}
			ImGui::SameLine();
			ImGui::Text("\n   Metallic");

			ImGui::ImageButton(NULL, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials); ImGui::SameLine();
			//ImGui::AlignTextToFramePadding();
			ImGui::Text("\n   Roughtness");
			if (ImGui::Button("Hello")) {
				ImGui::OpenPopup("Context");
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::Separator();
		}
		ImGui::End();

		if (showNodeEditor) {
			ImGui::PushStyleColor(ImGuiCol_TitleBg|ImGuiCol_TitleBgActive,ImVec4(0.0f,0.0f,0.0f,1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,2);
			ImGui::Begin("Node Editor",&showNodeEditor);
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

				if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(1)&&ImGui::IsWindowHovered())
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

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		return UPDATE_CONTINUE;
	}

	void ImGuiLayer::setDocking() {

		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove ;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			
		}

		//window_flags |= ImGuiWindowFlags_NoBackground;
	}

	bool ImGuiLayer::OnStart()
	{

		ImGui::CreateContext();
		ImGui::StyleColorsCustom();
		imnodes::Initialize();

		ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
		ImGui_ImplOpenGL3_Init();

		current_id_ = 0;
		//ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();

		std::string File = "../Cronos/vendor/imgui/misc/fonts/DroidSans.ttf";
		io.Fonts->AddFontFromFileTTF(File.c_str(), 14.0f/*, NULL, io.Fonts->GetGlyphRangesDefault()*/);

		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	     io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
		//io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		setDocking();

		//TEMPORARY

		glGenTextures(1, &my_opengl_texture);
		glBindTexture(GL_TEXTURE_2D, my_opengl_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, my_image_width, my_image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, my_image_data);



	/*	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;*/

		//ImGui_ImplOpenGL3_Init("#version 410 core");

		return true;
	}

	void ImGuiLayer::OnDetach()
	{
	}

	//void ImGuiLayer::OnEvent(Event& event)
	//{
	//	EventDispatcher dispatcher(event);
	//	dispatcher.Dispatch<MouseButtonPressedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
	//	dispatcher.Dispatch<MouseButtonReleasedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
	//	dispatcher.Dispatch<MouseMovedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
	//	dispatcher.Dispatch<MouseScrolledEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::OnMouseScrollEvent));
	//	dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
	//	dispatcher.Dispatch<KeyReleasedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
	//	dispatcher.Dispatch<KeyTypedEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
	//	dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
	//}

	//bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent & e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.MouseDown[e.GetMouseButton()] = true;

	//	return false;

	//}

	//bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent & e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.MouseDown[e.GetMouseButton()] = false;

	//	return false;
	//}

	//bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent & e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.MousePos = ImVec2(e.GetX(), e.GetY());	

	//	return false;
	//}

	//bool ImGuiLayer::OnMouseScrollEvent(MouseScrolledEvent & e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.MouseWheel += e.GetYOffset();
	//	io.MouseWheelH += e.GetXOffset();

	//	return false;
	//}

	//bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent & e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.KeysDown[e.GetKeyCode()] = true;

	//	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	//	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	//	io.KeyAlt= io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	//	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	//	return false;
	//}

	//bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent & e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.KeysDown[e.GetKeyCode()] = false;

	//	return false;
	//}

	//bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e) 
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	int keycode = e.GetKeyCode();

	//	if (keycode > 0 && keycode < 0x10000)
	//		io.AddInputCharacter((unsigned short)keycode);

	//	return false;
	//}

	//bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent & e)
	//{
	//	ImGuiIO& io = ImGui::GetIO();
	//	io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
	//	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	//	glViewport(0, 0, e.GetWidth(), e.GetHeight());

	//	return false;
	//}

}