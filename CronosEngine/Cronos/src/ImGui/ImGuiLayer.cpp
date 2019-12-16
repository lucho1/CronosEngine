#include "Providers/cnpch.h"

#include "ImGuiLayer.h"
#include "imgui.h"

#include "OpenGL/imgui_impl_sdl.h"
#include "OpenGL/imgui_impl_opengl3.h"

#include "imnodes.h"

#include "Core/Application.h"
#include "Modules/SDLWindow.h"
#include "Renderer/Buffers.h"
#include "GameObject/Components/Component.h"
#include "GameObject/Components/TransformComponent.h"
#include "GameObject/Components/CameraComponent.h"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#define sameLine ImGui::SameLine()

namespace Cronos {

	static ImGuiDockNodeFlags dockspace_flags;
	ImGuiWindowFlags window_flags;
	//static char currShaderMode[20];

#define TOTEX (void*)(intptr_t)

	inline int make_id(int node, int attribute) { return (node << 16) | attribute; } //temporary
	void PrimitivesMenu();

	ImGuiLayer::ImGuiLayer(Application* app, bool start_enabled) : Module(app, "ImGuiLayer"), ms_log(100), fps_log(100)
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void RightOptions(GameObject* currentGameObject) {

		std::string ID = std::to_string(currentGameObject->GetGOID());
		static ImVec2 Pos = ImGui::GetCursorPos();

		if (ImGui::BeginPopup(ID.c_str())) {

			if (ImGui::BeginMenu("Rename File")) {

				static char buf1[64];
				sprintf_s(buf1, "%s", currentGameObject->GetName().c_str());

				//ImGui::SetNextItemWidth(ImGui::CalcTextSize(buf1).x + 25);
				if (ImGui::InputText("###", buf1, 64)) {

					currentGameObject->SetName(buf1);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Delete"))
			{
				int cursor = 0;
				for (auto& go : App->scene->m_GameObjects) {
					bool wasActive = go->isActive();
					if (go->GetGOID() == currentGameObject->GetGOID()) {
						App->scene->m_GameObjects.erase(App->scene->m_GameObjects.begin() + cursor);
						if(!wasActive)
						 ImGui::PopStyleColor();
					}
					cursor++;
				}
			}
			/*	if (ImGui::MenuItem("Copy"))
				{
					if (currentGameObject != nullptr)
						App->scene->ToCopy = currentGameObject;
				}
				if (ImGui::MenuItem("Paste"))
				{
					if (App->scene->ToCopy != nullptr && currentGameObject != nullptr)
					{
						GameObject* NewGO = App->filesystem->Load(App->scene->ToCopy->GetGOID());

						NewGO->SetNewID();

						if (App->EditorGUI->GetCurrentGameObject()->GetParentGameObject() == nullptr)
						{
							NewGO->SetParent(App->EditorGUI->GetCurrentGameObject());
							currentGameObject->m_Childs.push_back(NewGO);
						}
						else
						{
							NewGO->SetParent(App->EditorGUI->GetCurrentGameObject()->GetParentGameObject());
							currentGameObject->m_Childs.push_back(NewGO);
						}

						App->filesystem->SaveOwnFormat(App->EditorGUI->GetCurrentGameObject());
					}
				}*/
			ImGui::EndPopup();
		}
	}

	bool ImGuiLayer::OnStart()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsCustom();
		imnodes::Initialize();

		ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
		ImGui_ImplOpenGL3_Init();

		ImGuiIO& io = ImGui::GetIO();
		//std::string File = "../Cronos/vendor/imgui/misc/fonts/DroidSans.ttf";
		std::string File = "res/fonts/DroidSans.ttf";
		//std::string File = "vendor/imgui/misc/fonts/DroidSans.ttf";
		io.Fonts->AddFontFromFileTTF(File.c_str(), 14.0f);
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_NavEnableSetMousePos | ImGuiConfigFlags_NavEnableKeyboard;

		setDocking();

		//Setting FrameBuffer for gameWindow;
		m_SceneWindow = new FrameBuffer();
		m_SceneWindow->Init(1280, 720);

		m_ShadingModesLabel[(int)ShadingMode::Shaded] = "Shaded";
		m_ShadingModesLabel[(int)ShadingMode::ShadedWireframe] = "Shaded Wireframe";
		m_ShadingModesLabel[(int)ShadingMode::Wireframe] = "Wireframe";
		m_currentShadingMode = ShadingMode::Shaded;


		PlayEventImage = App->textureManager->CreateTexture("res/Icons/Widget_Play_Icon.png", TextureType::ICON);
		StopEventImage = App->textureManager->CreateTexture("res/Icons/Widget_Stop_Icon.png", TextureType::ICON);
		PauseEventImage = App->textureManager->CreateTexture("res/Icons/Widget_Pause_Icon.png", TextureType::ICON);
		NextFrameEventImage = App->textureManager->CreateTexture("res/Icons/Widget_NextFr_Icon.png", TextureType::ICON);
		FasterEventImage = App->textureManager->CreateTexture("res/Icons/Widget_Fast_Icon.png", TextureType::ICON);
		SlowerEventImage = App->textureManager->CreateTexture("res/Icons/Widget_Speed_Icon.png", TextureType::ICON);

		//strcpy(currShaderMode, m_ShadingModesLabel[(int)m_currentShadingMode].c_str());
		//Reading License
		FILE* fp = fopen("LICENSE", "r");
		int c;
		while ((c = fgetc(fp)) != EOF) {
			LicenseString += c;
		}
		//LicenseString = std::string("sadasf");

		//Setting temporary root
		AssetDirectories = App->filesystem->GetAssetDirectories();
		m_CurrentDir = AssetDirectories;
		const char* LogString1 = AssetDirectories->m_LabelDirectories.c_str();
		const char* LogString2 = m_CurrentDir->m_LabelDirectories.c_str();
		LOG("	Asset Dir: %s \n	Current Dir: %s", LogString1, LogString2);

		HardwareInfo = SystemInfo(true);
		SoftwareInfo = SystemInfo(false);
		CurrentGameObject = nullptr;
		Draging = nullptr;
		return true;
	}


	void ImGuiLayer::AssetImguiIterator(Directories a) {
		for (auto& c : a.childs) {

			std::string temp = c->m_Directories.filename().string();
			ImGuiTreeNodeFlags Treenode_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick ;

			bool open = ImGui::TreeNodeEx(temp.c_str(),Treenode_flags);
			if (ImGui::IsItemClicked()) {
				m_CurrentDir = c;
			}

			if (open) {
				AssetImguiIterator(*c);
				ImGui::TreePop();
			}
		}
	}

	void ImGuiLayer::HierarchyIterator(GameObject GameObjects)
	{
		for (auto& go : GameObjects.m_Childs) {

			ImGuiTreeNodeFlags Treenode_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			std::string ID = std::to_string(go->GetGOID());

			if (go->isActive() == false) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1.0f));
			}

			if (nodeHirearchySelected == go->GetGOID())
			{
				Treenode_flags |= ImGuiTreeNodeFlags_Selected;
			}
			std::string GameObject_Name = go->GetName();

			if (go->GetCountChilds() <= 0) {
				Treenode_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				ImGui::TreeNodeEx((void*)(intptr_t)go->GetGOID(), Treenode_flags, GameObject_Name.c_str());
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
					GetGameObject(go);
				}
				if (ImGui::BeginDragDropTarget())
				{
					setParentGameObject(go);
				}
				if (ImGui::IsItemClicked()) {
					CurrentGameObject = go;
					nodeHirearchySelected = go->GetGOID();
				}
				if (ImGui::IsItemClicked(1))
				{
					ImGui::OpenPopup(ID.c_str());
				}

				RightOptions(go);
			}
			else {

				bool open = ImGui::TreeNodeEx((void*)(intptr_t)go->GetGOID(), Treenode_flags, GameObject_Name.c_str());

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
					GetGameObject(go);
				}
				if (ImGui::BeginDragDropTarget())
				{
					setParentGameObject(go);
				}

				if (ImGui::IsItemClicked()) {
					CurrentGameObject = go;
					nodeHirearchySelected = go->GetGOID();
				}

				if (ImGui::IsItemClicked(1))
				{
					ImGui::OpenPopup(ID.c_str());
				}

				RightOptions(go);
				if (open) {
					HierarchyIterator(*go);
					ImGui::TreePop();
				}
			}

			if (go->isActive() == false) {
				ImGui::PopStyleColor();
			}

		}
	}

	void ImGuiLayer::setParentGameObject(GameObject* ToParentGo) {

		if (Draging != nullptr) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject")) {
				GameObject* Temp = Draging;
				GameObject* LastParent = Temp->GetParentGameObject();
				//To know if ToParentGo is a child so it will cancel, a parent cannot be a child from his own child.
				for (auto& a : Temp->m_Childs) {
					if (a->GetGOID() == ToParentGo->GetGOID()) {
						ImGui::EndDragDropTarget();
						Draging = nullptr;
						return;
					}
				}
				//To know if we are moving childs
				std::list<GameObject*>::iterator it1;
				if (LastParent != nullptr) {
					for (it1 = LastParent->m_Childs.begin(); it1 != LastParent->m_Childs.end(); ++it1) {
						if ((*it1)->GetGOID() == Temp->GetGOID()) {
							LastParent->m_Childs.erase(it1);
							Temp->SetParent(ToParentGo);
							break;
						}
					}
				}
				//To know if we are moving Fathers;
				else {
					for (unsigned int i = 0; i<App->scene->m_GameObjects.size(); ++i) {
						if (App->scene->m_GameObjects[i]->GetGOID() == Temp->GetGOID()) {
							App->scene->m_GameObjects.erase(App->scene->m_GameObjects.begin() + i);
							Temp->SetParent(ToParentGo);
							break;
						}
					}
				}
				ToParentGo->m_Childs.push_back(Temp);
				Draging = nullptr;
			}
		}
		ImGui::EndDragDropTarget();
	}

	void ImGuiLayer::BreakParentGameObject(GameObject* go) {

		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{

			GameObject* Temp = Draging;
			GameObject* LastParent = Temp->GetParentGameObject();
			if (LastParent == nullptr) {
				Draging = nullptr;
				ImGui::EndDragDropTarget();
				return;
			}
			std::list<GameObject*>::iterator it1;
			for (it1 = LastParent->m_Childs.begin(); it1 != LastParent->m_Childs.end(); ++it1) {
				if ((*it1)->GetGOID() == Temp->GetGOID()) {
					LastParent->m_Childs.erase(it1);
					break;
				}
			}
			go->BreakParent();
			App->scene->m_GameObjects.push_back(go);
			Draging = nullptr;
		}
		ImGui::EndDragDropTarget();
	}

	void ImGuiLayer::GetGameObject(GameObject* Go) {

		ImGui::SetDragDropPayload("GameObject", Go, sizeof(Go->GetGOID()));
		Draging = Go;
		ImGui::Text(Go->GetName().c_str());
		ImGui::EndDragDropSource();
	}

	void ImGuiLayer::SetSelectedGameObject(GameObject* gameObject)
	{
		if (!gameObject)
			return;

		CurrentGameObject = gameObject;
		nodeHirearchySelected = gameObject->GetGOID();
	}

	update_status ImGuiLayer::OnPreUpdate(float dt) {
		if (ShowDrawGameWindow)
			m_SceneWindow->PreUpdate();

		return current_status;
	}

	update_status ImGuiLayer::OnPostUpdate(float dt)
	{
		int test = DirectoriesArray.size();
		static bool DockspaceInitiate;
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(App->window->GetWidth(), App->window->GetHeight());
		io.WantCaptureKeyboard = true;

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

		GUIDrawMainBar();
		GUIDrawWidgetMenu();
		if (ShowInspectorPanel)			GUIDrawInspectorMenu(CurrentGameObject);
		if (ShowHierarchyMenu)			GUIDrawHierarchyPanel();
		if (ShowNodeEditorPanel)		GUIDrawNodeEditorPanel();
		if (ShowConsolePanel)			GUIDrawConsolePanel();
		if (ShowAssetMenu)				GUIDrawAssetPanel();
		if (ShowDemoWindow)				ImGui::ShowDemoWindow(&ShowDemoWindow);
		if (ShowConfigurationPanel)		GUIDrawConfigurationPanel();
		if (ShowPerformancePanel)		GUIDrawPerformancePanel();
		if (ShowAboutPanel)				GUIDrawAboutPanel();
		if (ShowDrawGameWindow)		    GUIDrawSceneWindow();

		if (App->input->getCurrentWinStatus())	GUIDrawSupportExitOptions();
		ImGui::End();

		if (CurrentGameObject != nullptr&&App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) {
			int cursor = 0;
			for (auto& go : App->scene->m_GameObjects) {
				if (go->GetGOID() == CurrentGameObject->GetGOID()) {
					App->scene->m_GameObjects.erase(App->scene->m_GameObjects.begin() + cursor);
					break;
				}
				else {
					DeleteGameObject(go);
				}
				cursor++;
			}
		}


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SDL_GL_SwapWindow(App->window->window);
		return current_status;
	}

	update_status ImGuiLayer::OnUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	void ImGuiLayer::DeleteGameObject(GameObject* go) {

		std::list<GameObject*>::iterator it1;
		for (it1 = go->m_Childs.begin(); it1 != go->m_Childs.end();++it1) {

			if ((*it1)->GetGOID() == CurrentGameObject->GetGOID())
			{
				go->m_Childs.erase(it1);
				return;
			}
			else
				DeleteGameObject((*it1));
		}
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

	void ImGuiLayer::GUIDrawWidgetMenu() {

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		ImGuiWindowFlags WidgetFlags = ImGuiWindowFlags_NoTitleBar|ImGuiDockNodeFlags_AutoHideTabBar | ImGuiWindowFlags_NoMove |ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoScrollWithMouse;

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.1f, 0.1f));
		//ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("##none",nullptr,WidgetFlags);

		ImGui::SameLine(ImGui::GetWindowWidth()/2-172*0.5);
		if (ImGui::ImageButton(TOTEX PlayEventImage->GetTextureID(), ImVec2(56 * 0.5, 46 * 0.5), ImVec2(0, 0), ImVec2(1, 1), -1))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.170f, 0.170f, 0.17f, 1.0f);
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.4f);

			App->m_GT_Play = !App->m_GT_Play;
		} ImGui::SameLine();

		if (ImGui::ImageButton(TOTEX PauseEventImage->GetTextureID(), ImVec2(57 * 0.5, 46 * 0.5), ImVec2(0, 0), ImVec2(1, 1), -1))
		{
			App->m_GT_Pause = !App->m_GT_Pause;
		} ImGui::SameLine();

		if (ImGui::ImageButton(TOTEX NextFrameEventImage->GetTextureID(), ImVec2(57 * 0.5, 46 * 0.5), ImVec2(0, 0), ImVec2(1, 1), -1))
		{
			App->m_GT_NextFrame = !App->m_GT_NextFrame;
		} ImGui::SameLine();

		if (ImGui::ImageButton(TOTEX StopEventImage->GetTextureID(), ImVec2(57 * 0.5, 46 * 0.5), ImVec2(0, 0), ImVec2(1, 1), -1))
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);
			style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

			App->m_GT_Stop = !App->m_GT_Stop;
		} ImGui::SameLine(20);

		if (ImGui::ImageButton(TOTEX SlowerEventImage->GetTextureID(), ImVec2(57 * 0.5, 46 * 0.5), ImVec2(0, 0), ImVec2(1, 1), -1))
		{
			App->m_GT_Slower = !App->m_GT_Slower;
		} ImGui::SameLine(55);

		if (ImGui::ImageButton(TOTEX FasterEventImage->GetTextureID(), ImVec2(57 * 0.5, 46 * 0.5), ImVec2(0, 0), ImVec2(1, 1), -1))
		{
			App->m_GT_Faster = !App->m_GT_Faster;
		}

		ImGui::SameLine();
		ImGui::Text("Game Time: "); ImGui::SameLine();

		/*float value = (int)(var * 100 + .5);
		return (float)value / 100;*/

		float Gtime = App->m_GameTimer_Time; //To round to 2 decimals
		Gtime = (int)(Gtime * 100 + 0.5f);
		Gtime = (float)(Gtime / 100);
		ImGui::Text(std::to_string(Gtime).c_str());


		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void ImGuiLayer::UpdateDocking() {
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
	}

	void ImGuiLayer::GUIDrawSceneWindow()
	{
		static ImGuiWindowFlags GameWindow_flags= ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_MenuBar;

		static ImVec2 SizeGame;
		static ImVec2 LastSize = SizeGame;

		//ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
		static bool ZB_RenderingActive = false;
		ImGui::Begin("Scene",nullptr,GameWindow_flags);
		{
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(m_ShadingModesLabel[(int)m_currentShadingMode].c_str())) {

					for (int i = 0; i < (int)ShadingMode::MaxElements; i++)
					{
						if (ImGui::MenuItem(m_ShadingModesLabel[i].c_str()))
							m_currentShadingMode = (ShadingMode)i;
					}

					ImGui::Separator();
					if (ImGui::MenuItem("Render Z-Buffer", "", &ZB_RenderingActive))
					{
						App->renderer3D->SetZBuffer();
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
			SizeGame = ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y-55);
			if (LastSize.x != SizeGame.x || LastSize.y != SizeGame.y)
			{
				//TODO: When doing this resize it actually does a window resize, and shouldn't be like that
				//but resizing through renderer doesn't works
				m_SceneWindow->OnResize(SizeGame.x, SizeGame.y);
				//App->renderer3D->OnResize(SizeGame.x, SizeGame.y);
				//App->window->OnResize(SizeGame.x, SizeGame.y);
				LastSize = SizeGame;
			}

			ImGui::Image((void*)m_SceneWindow->GetWindowFrame(), SizeGame, ImVec2(0, 1), ImVec2(1, 0));

			if (ImGui::BeginDragDropTarget())
			{
				//ImGui::GetID("Scene");
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Window"))
				{
					int payload_n = *(const int*)payload->Data;

					if (m_CurrentAssetSelected->GetType() == ItemType::ITEM_FBX|| m_CurrentAssetSelected->GetType() == ItemType::ITEM_OBJ) {
						AssetItems* a = (AssetItems*)payload->Data;
						GameObject* ret = App->filesystem->Load(a->GetGameObjectID());

						if (ret != nullptr) {
							App->scene->m_GameObjects.push_back(ret);
							ret->SetNewID();
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
		if (ImGui::IsItemHovered()) {
			HoverGameWin = true;
		}
		else
			HoverGameWin = false;

		m_SceneWindow->PostUpdate();
		ImGui::End();
	}

	void ImGuiLayer::GUIDrawMainBar()
	{
		ImGui::BeginMainMenuBar();
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
			if (ImGui::BeginMenu("File")) {

				if(ImGui::MenuItem("New Scene")) {
					App->scene->mustCleanScene = true;
				}
				if (ImGui::MenuItem("Load")) {
					App->LoadEngineData();
					App->scene->mustLoad = true;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Save")) {
					App->SaveEngineData();
					App->scene->mustSave = true;
				}
				ImGui::MenuItem("Save As...");
				ImGui::Separator();
				ImGui::MenuItem("New Project");
				ImGui::MenuItem("Open Project");
				ImGui::MenuItem("Save Project");
				ImGui::Separator();
				if (ImGui::MenuItem("Configuration")) {
					ShowConfigurationPanel = true;
				}

				if (ImGui::MenuItem("Exit"))
					App->input->updateQuit(true);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {
				ImGui::MenuItem("Undo","CTRL+Z");
				ImGui::MenuItem("Redo", "CTRL+Y");

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("GameObject")) {
				if (ImGui::MenuItem("Empty Object"))
				{
					PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::EMPTY, "Empty", { 1,1,1 });
					App->scene->m_GameObjects.push_back(ret);
				}
				if (ImGui::BeginMenu("3D Object"))
					{
					PrimitivesMenu();
					ImGui::EndMenu();
					}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug")) {
				if (ImGui::MenuItem("Clear Console")) {
					LogBuffer.clear();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window")) {
				if (ImGui::MenuItem("NodeEditor")) {
					ShowNodeEditorPanel = !ShowNodeEditorPanel;
				}
				if (ImGui::MenuItem("Inspector")) {
					ShowInspectorPanel = !ShowInspectorPanel;
				}
				if (ImGui::MenuItem("Hierarchy")) {
					ShowHierarchyMenu = !ShowHierarchyMenu;
				}
				if (ImGui::MenuItem("Asset")) {
					ShowAssetMenu = !ShowAssetMenu;
				}
				if (ImGui::MenuItem("Performance")) {
					ShowPerformancePanel = !ShowPerformancePanel;
				}
				if (ImGui::MenuItem("Console")) {
					ShowConsolePanel = !ShowConsolePanel;
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("Demo Window")) {
					ShowDemoWindow = !ShowDemoWindow;
				}
				if (ImGui::MenuItem("Documentation")) {
					App->RequestBrowser("https://github.com/lucho1/CronosEngine/wiki");
				}
				if (ImGui::MenuItem("Download latest")) {
					App->RequestBrowser("https://github.com/lucho1/CronosEngine/releases");
				}
				if (ImGui::MenuItem("Report a bug")) {
					App->RequestBrowser("https://github.com/lucho1/CronosEngine/issues");
				}
				if (ImGui::MenuItem("About")) {
					ShowAboutPanel = !ShowAboutPanel;
				}


				ImGui::EndMenu();
			}
		}
		ImGui::PopStyleVar();
		ImGui::EndMainMenuBar();

	}

	void ImGuiLayer::GUIDrawPerformancePanel() {

		ImGui::SetNextWindowSize(ImVec2(250, 400));
		ImGui::Begin("Performance", &ShowPerformancePanel);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));

		int fpscap = App->GetFPSCap();
		ImGui::Text("Fps cap :"); ImGui::SameLine();
		if (ImGui::SliderInt("##fpscap", &fpscap, -1, 100))
			App->SetFPSCap(fpscap);
		ImGui::SameLine();
		HelpMarker("By default is 0, when is no fps cap");

		ImGui::Separator();

		AcumulateLogDT();

		char title[25];

		sprintf_s(title, 25, "Framerate %0.1f", fps_log[fps_log.size() - 1]);
		//ImGui::PlotLines("frame", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(200, 100));
		ImGui::PlotHistogram("##framerate", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(220, 100));

		sprintf_s(title, 25, "Milliseconds %0.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(220, 100));

		ImGui::Separator();

		ImGui::Text("To future implement :");
		ImGui::Text("        Current usage Drawcalls");
		ImGui::Text("        Memory scene using");
		ImGui::Text("        etc...");

		ImGui::PopStyleVar();

		ImGui::End();
	}

	void ImGuiLayer::GUIDrawInspectorMenu(GameObject* CurrentGameObject)
	{
		//ImGui::SetNextWindowSize(ImVec2(500, 400));
		ImGui::Begin("Inspector", &ShowInspectorPanel);

			if (CurrentGameObject != nullptr)
			{
				ImGui::Checkbox(" ", &CurrentGameObject->SetActive()); ImGui::SameLine();
				static char buf1[64];
				strcpy(buf1, CurrentGameObject->GetName().c_str());

				if (ImGui::InputText("###", buf1, 64)) 
					CurrentGameObject->SetName(buf1);
				
				ImGui::Separator();
				GUIDrawTransformPMenu(CurrentGameObject);

				if (CurrentGameObject->GetComponent<MeshComponent>() != nullptr)
					GUIDrawMeshMenu(CurrentGameObject);
				
				//	if (CurrentGameObject->GetComponent<MeshComponent>()->GetTexturesVector().size() > 0)
				if (CurrentGameObject->GetComponent<MeshComponent>() != nullptr) 
						GUIDrawMaterialsMenu(CurrentGameObject);
				
				if (CurrentGameObject->GetComponent<CameraComponent>() != nullptr)
					GUIDrawCameraComponentMenu(CurrentGameObject);

			}
			if (m_CurrentAssetSelected != nullptr&&m_CurrentAssetSelected->GetType() == ItemType::ITEM_TEXTURE_PNG) {
				GUIDrawAssetLabelInspector();
			}

		ImGui::End();

	}

	void ImGuiLayer::GUIDrawTransformPMenu(GameObject* CurrentGameObject)
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			TransformComponent* test = CurrentGameObject->GetComponent<TransformComponent>();
			ObjectPos = test->GetTranslation();
			ObjectRot = test->GetOrientation();
			ObjectScale = test->GetScale();

			static bool toChange;
			ImGui::AlignTextToFramePadding();
			ImGui::Text("Position");
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); if (ImGui::DragFloat("##valueX", &ObjectPos.x, 0.1f))toChange = true; ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); if(ImGui::DragFloat("##valueY", &ObjectPos.y, 0.1f))toChange=true; ImGui::SameLine();
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); if(ImGui::DragFloat("##valueZ", &ObjectPos.z, 0.1f))toChange=true;
			
			if (toChange)
			{
				test->SetPosition(ObjectPos);
				toChange = false;
			}

			ImGui::Text("Rotation");
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); if (ImGui::DragFloat("##value1", &ObjectRot.x, 0.1f))toChange = true; ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); if (ImGui::DragFloat("##value2", &ObjectRot.y, 0.1f))toChange = true; ImGui::SameLine();
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(50); if (ImGui::DragFloat("##value3", &ObjectRot.z, 0.1f))toChange = true;
			
			if (toChange)
			{
				test->SetOrientation(ObjectRot);
				toChange = false;
			}

			ImGui::Text("Scale");
			ImGui::Text("X"); ImGui::SameLine(); ImGui::SetNextItemWidth(50);  if (ImGui::DragFloat("##value4", &ObjectScale.x, 0.1f))toChange=true; ImGui::SameLine();
			ImGui::Text("Y"); ImGui::SameLine(); ImGui::SetNextItemWidth(50);  if (ImGui::DragFloat("##value5", &ObjectScale.y, 0.1f))toChange=true; ImGui::SameLine();
			ImGui::Text("Z"); ImGui::SameLine(); ImGui::SetNextItemWidth(50);  if (ImGui::DragFloat("##value6", &ObjectScale.z, 0.1f))toChange=true;
			
			if (toChange)
			{
				test->SetScale(ObjectScale);
				toChange = false;
			}
		}

		ImGui::Separator();
	}

	void ImGuiLayer::GUIDrawAssetLabelInspector()
	{
		ImGui::BeginChild("test");
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

		static int WinPosition = 0;
		static bool DragWindow = false;

		ImGui::SetCursorPosY(WinPosition);

		bool open = ImGui::CollapsingHeader(m_CurrentAssetSelected->GetDetails().c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		if (ImGui::IsItemClicked()) {
			DragWindow = true;
		}
		if (DragWindow) {
			//if(App->input->GetMouseY()>)
			WinPosition = App->input->GetMouseY() - ImGui::GetWindowPos().y;
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP) {
				DragWindow = false;
			}
		}

		if (open) {

			ImGui::BeginChild("AssetInspectorLabel");

			int AvSizeX = ImGui::GetWindowWidth();
			int AvSizeY = ImGui::GetWindowHeight();

			if (AvSizeX >= m_CurrentAssetSelected->GetResolution().x)
				AvSizeX = m_CurrentAssetSelected->GetResolution().x;
			else if (AvSizeX > AvSizeY)
				AvSizeX = AvSizeY;


			static float aspectRatio = m_CurrentAssetSelected->GetResolution().x / m_CurrentAssetSelected->GetResolution().y;
			ImGui::SameLine(ImGui::GetWindowWidth() / 2 - AvSizeX / 2);

			int CenterY = AvSizeY - (ImGui::GetWindowHeight() / 2 + AvSizeX / 2);
			ImGui::SetCursorPosY(CenterY);

			ImGui::Image(TOTEX (m_CurrentAssetSelected->GetIconTexture()-1), ImVec2(AvSizeX, AvSizeX*aspectRatio),ImVec2(0,1),ImVec2(1,0));

			ImGui::EndChild();
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::EndChild();
	}

	void ImGuiLayer::GUIDrawMeshMenu(GameObject* CurrentGameObject)
	{
		if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text(CurrentGameObject->GetName().c_str());

			ImGui::Checkbox("Draw Normals", &CurrentGameObject->GetComponent<MeshComponent>()->setDebugDraw());
			ImGui::Checkbox("Draw Central Axis", &CurrentGameObject->GetComponent<MeshComponent>()->SetDrawAxis());

			ImGui::Separator();
		}
	}

	void ImGuiLayer::GUIDrawCameraComponentMenu(GameObject* CurrentGameObject)
	{
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			CameraComponent* camera = CurrentGameObject->GetComponent<CameraComponent>();

			static float CameraMoveSpeed = camera->GetCameraMoveSpeed();
			static float CameraScrollSpeed = camera->GetCameraScrollSpeed();
			static float CameraFieldOfView = camera->GetFOV();
			static float CameraNearPlane = camera->GetNearPlane();
			static float CameraFarPlane = camera->GetFarPlane();

			//ImGui::Text("Camera Options");

			//Setters -----------------------------------------------------------------------------------------
			/*ImGui::NewLine();
			ImGui::SameLine(15); ImGui::Text("Camera Move Speed: "); sameLine;
			if (ImGui::SliderFloat("##cameraMoveSpeed", &CameraMoveSpeed, 1.0f, 100.0f, "%.2f", 1.0f))
				camera->SetMoveSpeed(CameraMoveSpeed);

			ImGui::NewLine();
			ImGui::SameLine(15); ImGui::Text("Camera Scroll Speed: "); sameLine;
			if (ImGui::SliderFloat("##cameraScrollSpeed", &CameraScrollSpeed, 1.0f, 100.0f, "%.2f", 1.0f))
				camera->SetScrollSpeed(CameraScrollSpeed);*/

			ImGui::NewLine();
			ImGui::SameLine(15); ImGui::Text("Field of View : "); sameLine;
			if (ImGui::SliderFloat("##cameraFOV", &CameraFieldOfView, MIN_FOV, MAX_FOV, "%.2f", 1.0f))
				camera->SetFOV(CameraFieldOfView);

			ImGui::NewLine();
			if (ImGui::SliderFloat("NearPlane ", &CameraNearPlane, 0.1, 500, "%.2f", 1.0f))
				camera->SetNearPlane(CameraNearPlane);

			ImGui::NewLine();
			ImGui::SetNextItemWidth(100);
			if (ImGui::SliderFloat("FarPlane ", &CameraFarPlane, 0.1, 1000, "%.2f", 1.0f))
				camera->SetFarPlane(CameraFarPlane);

			//Set to default values ------------------------------------------------------------------------------
			if (ImGui::Button("Default"))
			{
				//CameraMoveSpeed = 5.0;
				//CameraScrollSpeed = 20.0;
				CameraFieldOfView = 60.0f;
				CameraNearPlane = 1.0f;
				CameraFarPlane = 100.0f;
				//App->engineCamera->SetMoveSpeed(CameraMoveSpeed);
				//App->engineCamera->SetScrollSpeed(CameraScrollSpeed);
				camera->SetFOV(CameraFieldOfView);
				camera->SetNearPlane(CameraNearPlane);
				camera->SetFarPlane(CameraFarPlane);

			}
		}
	}

	void ImGuiLayer::GUIDrawMaterialsMenu(GameObject* CurrentGameObject)
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

			Texture* Diffuse = nullptr;
			MaterialComponent* Cn_Material = CurrentGameObject->GetComponent<MaterialComponent>();

			if (Cn_Material != nullptr)
			{
				for (auto& tv : Cn_Material->GetTextures())
					if (tv.first == TextureType::DIFFUSE)
						Diffuse = (tv.second);
			}

			if (Diffuse != nullptr)
				ImGui::ImageButton((void*)Diffuse->GetTextureID(), ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials);
			else
				ImGui::ImageButton(NULL, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials);


			if (ImGui::BeginDragDropTarget())
			{
				//ImGui::GetID("Scene");
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Window"))
				{
					int payload_n = *(const int*)payload->Data;

					if (m_CurrentAssetSelected->GetType() == ItemType::ITEM_TEXTURE_DDS || m_CurrentAssetSelected->GetType() == ItemType::ITEM_TEXTURE_TGA
						|| m_CurrentAssetSelected->GetType() == ItemType::ITEM_TEXTURE_JPEG || m_CurrentAssetSelected->GetType() == ItemType::ITEM_TEXTURE_PNG)
					{
						AssetItems* AssetData = (AssetItems*)payload->Data;
						CurrentGameObject->GetComponent<MaterialComponent>()->SetTexture(AssetData->GetTexture(), TextureType::DIFFUSE);
					}
				}

				ImGui::EndDragDropTarget();
			}


			ImGui::SameLine();
			ImGui::AlignTextToFramePadding();
			ImGui::Text("\n   Ambient/Albedo"); ImGui::SameLine();
			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
			ImVec2 FramePadding(100.0f, 3.0f);
			//ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 30));
			static int  test = ImGui::GetCursorPosY();

			ImGui::SetCursorPosY(test+15);

			glm::vec4 col = CurrentGameObject->GetComponent<MaterialComponent>()->GetColor();
			color = ImVec4(col.r, col.g, col.b, col.a);

			if (ImGui::ColorEdit4(" \n MyColor##3", (float*)&color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | misc_flags))
			{
				CurrentGameObject->GetComponent<MaterialComponent>()->SetColor(glm::vec4(color.x, color.y, color.z, color.w));
			}
			//ImGui::PopStyleVar();


			ImGui::ImageButton(NULL, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials); ImGui::SameLine();
			//ImGui::AlignTextToFramePadding();
			ImGui::Text("\n   Specular"); ImGui::SameLine();
			static float SpecIntensity = 1.0f;
			static int  test2 = ImGui::GetCursorPosY();
			ImGui::SetCursorPosY(test2 + 13);

			ImGui::PushItemWidth(70); ImGui::SliderFloat("##", &SpecIntensity, 0.0f, 1.0f);

			//if (ImGui::ImageButton(NULL, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials)) {
			//	ImGui::OpenPopup("Context");
			//}
			//ImGui::SameLine();
			//ImGui::Text("\n   Metallic");

			//ImGui::ImageButton(NULL, ImVec2(60, 60), ImVec2(0, 0), ImVec2(1, 1), FramePaddingMaterials); ImGui::SameLine();
			//ImGui::Text("\n   Roughtness");
			//if (ImGui::Button("Hello")) {
			//	ImGui::OpenPopup("Context");
			//}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::Separator();
		}
	}


	void ImGuiLayer::GUIDrawHierarchyPanel()
	{

		ImGui::Begin("Hierarchy", &ShowHierarchyMenu, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar()) {

				if (ImGui::BeginMenu("Create")) {

					if (ImGui::MenuItem("Empty Object"))
					{
						PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::EMPTY, "Empty", { 1,1,1 });
						App->scene->m_GameObjects.push_back(ret);
					}
					if (ImGui::BeginMenu("3D Object"))
					{
						PrimitivesMenu();
						ImGui::EndMenu();
					}

					if (ImGui::MenuItem("Camera"))
					{
						PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::CUBE, "Camera", { 0.5f, 0.5f, 0.8f });
						ret->GetComponent<TransformComponent>()->SetPosition({ 0, 3, 5 });
						ret->GetComponent<MaterialComponent>()->SetColor({ 0.6f, 0.6f, 0.6f, 1.0f });

						CameraComponent* cameraComp = (CameraComponent*)(ret->CreateComponent(ComponentType::CAMERA));

						ret->m_Components.push_back(cameraComp);
						App->scene->m_GameObjects.push_back(ret);
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
			ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.392f, 0.369f, 0.376f, 0.70f));

			for (auto&go : App->scene->m_GameObjects)
			{
				std::string GameObject_Name = go->GetName();
				std::string ID = std::to_string(go->GetGOID());
				ImGuiTreeNodeFlags Treenode_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

				if (go->isActive() == false) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1.0f));
				}

				if (nodeHirearchySelected == go->GetGOID())
				{
					Treenode_flags |= ImGuiTreeNodeFlags_Selected;
				}

				if (go->GetCountChilds() <= 0) {
					Treenode_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
					ImGui::TreeNodeEx((void*)(intptr_t)go->GetGOID(), Treenode_flags, "%s", GameObject_Name.c_str());
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
						GetGameObject(go);
					}
					if (ImGui::BeginDragDropTarget())
					{
						setParentGameObject(go);
					}
					if (ImGui::IsItemClicked()) {
						CurrentGameObject = go;
						nodeHirearchySelected = go->GetGOID();
					}
					if (ImGui::IsItemClicked(1))
					{
						ImGui::OpenPopup(ID.c_str());
					}

					RightOptions(go);

				}
				else {

					bool open = ImGui::TreeNodeEx((void*)(intptr_t)go->GetGOID(), Treenode_flags, "%s", GameObject_Name.c_str());
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
						GetGameObject(go);
					}
					if (ImGui::IsItemClicked()) {
						CurrentGameObject = go;
						nodeHirearchySelected = go->GetGOID();
					}
					if (ImGui::IsItemClicked(1))
					{
						ImGui::OpenPopup(ID.c_str());
					}

					RightOptions(go);
					if (ImGui::BeginDragDropTarget())
					{
						setParentGameObject(go);
					}
					if (open) {
						HierarchyIterator(*go);
						ImGui::TreePop();
					}
				}
				if (go->isActive() == false) {
					ImGui::PopStyleColor();
				}

			}

			ImGui::PopStyleColor(); //OJU POSIBLE CRASH
			float Vec2Ytest = ImGui::GetCursorPosY() + 5.0f;
			ImGui::InvisibleButton("##invisibleButton", ImVec2(ImGui::GetWindowWidth()-30, ImGui::GetWindowHeight() - Vec2Ytest));
			if (ImGui::BeginDragDropTarget()) {
				if(Draging!=nullptr)
					if(Draging->GetParentGameObject()!=nullptr)
						BreakParentGameObject(Draging);
			}
			if (ImGui::IsWindowHovered() && ImGui::GetMousePos().y > Vec2Ytest)
				if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN) {
					CurrentGameObject = nullptr;
					nodeHirearchySelected = 0;
				}
		}
		ImGui::End();
	}


	void ImGuiLayer::GUIDrawAssetPanel()
	{
		ImGui::Begin("Project", &ShowAssetMenu, ImGuiWindowFlags_MenuBar );
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

			ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;

			ImGui::BeginChild("left panel", ImVec2(150, 0),true,window_flags);

			ImGuiTreeNodeFlags Treenode_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			bool open = ImGui::TreeNodeEx(App->filesystem->GetLabelAssetRoot().c_str(),Treenode_flags);
			if (ImGui::IsItemClicked())
				m_CurrentDir = AssetDirectories;

			if (open) {
				AssetImguiIterator(*AssetDirectories);
				ImGui::TreePop();
			}

			ImGui::EndChild();

			const char* SceneLabel = "Scenes";


			ImGui::SameLine();

			// right
			ImGui::BeginGroup();

			int testa = ImGui::GetWindowWidth();
			ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us

			std::string LabelFolder= m_CurrentDir->m_Directories.filename().string();
			ImGui::Text("%s", LabelFolder.c_str());



			static char buf1[64] = "Asset Browser";
			ImGui::SetNextItemWidth(ImGui::CalcTextSize(buf1).x + 25);

			if (ImGui::InputText("###", buf1, 64, ImGuiInputTextFlags_CharsNoBlank)){
				static Directories* tempDir = new Directories();
				if (m_CurrentDir != tempDir) {
					LastDir = m_CurrentDir;
				}
				m_CurrentDir = tempDir;
				App->filesystem->SearchFile(tempDir, buf1);
			}

			std::string a = buf1;
			if (a != "Asset Browser") {
				ImGui::SameLine();
				if (ImGui::Button("Reset")) {
					sprintf_s(buf1, "%s", "Asset Browser");
					m_CurrentDir = LastDir;
				}
			}

			if (m_CurrentDir != AssetDirectories) {
				ImGui::SameLine(ImGui::GetWindowWidth() - 30);
				if (ImGui::ImageButton("", ImVec2(20, 20), ImVec2(0, 0), ImVec2(0, 0), 2))
					m_CurrentDir = m_CurrentDir->GetParentDirectory();
			}
	/*		if(ImGui::Button("CreateDirectory")){
				std::string Tempcreate = m_CurrentDir->m_LabelDirectories;
				Tempcreate += "/Hello";d
				App->filesystem->CreateNewDirectory(m_CurrentDir,"Hello");
			} ImGui::SameLine();

			if (ImGui::Button("DeleteDirectory")) {
				std::string Tempcreate = m_CurrentDir->m_LabelDirectories;
				Tempcreate += "/Hello";
				App->filesystem->DeleteDirectory(Tempcreate.c_str());
			}*/

			ImGui::Separator();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			int spaceCounter = 180;
			for (auto& a : m_CurrentDir->m_Container) {
				a->DrawIcons();
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {

					ImGui::SetDragDropPayload("Window", a, sizeof(AssetItems));
					ImGui::Image((void*)(a->GetIconTexture() - 1), ImVec2(50, 50), ImVec2(0, 1), ImVec2(1, 0));
					ImGui::EndDragDropSource();

				}
				if (a->GetType() == ItemType::ITEM_FOLDER&&ImGui::IsItemClicked(0))
					m_CurrentDir = a->folderDirectory;

				spaceCounter += a->GetElementSize();

				if (spaceCounter < ImGui::GetWindowWidth()) {
					ImGui::SameLine();
				}
				else
					spaceCounter = 180;
				//ImGui::Button(a.m_Elements.c_str());
			}



			ImGui::PopStyleColor();

			ImGui::EndChild();
			ImGui::EndGroup();

		}



		ImGui::End();
	}

	void ImGuiLayer::GUIDrawNodeEditorPanel()
	{
		ImGui::PushStyleColor(ImGuiCol_TitleBg | ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2);
		if (ImGui::Begin("Node Editor", &ShowNodeEditorPanel,ImGuiWindowFlags_NoDocking)) {
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

		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
	}

	void ImGuiLayer::GUIDrawConfigurationPanel() {

		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::PushStyleColor(ImGuiCol_TitleBg | ImGuiCol_TitleBgActive, ImVec4(0.392f, 0.369f, 0.376f, 1.00f));
		ImGui::Begin("Configuration", &ShowConfigurationPanel, ImGuiWindowFlags_NoDocking);
		ImGui::PopStyleColor();


		ImGui::BeginGroup();
		static int selected=-1;
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.392f, 0.369f, 0.376f, 1.00f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
		ImGui::BeginChild("left pane", ImVec2(150, 0), true);


			if (ImGui::Selectable("Application", selected==0)) {
				currentMenu = ConfigMenus::Application;
				selected = 0;
			}
			else if (ImGui::Selectable("Window", selected==1)) {
				currentMenu = ConfigMenus::Window;
				selected = 1;
			}
			else if (ImGui::Selectable("Hardware", selected == 2)) {
				currentMenu = ConfigMenus::Hardware;
				selected = 2;
			}
			else if (ImGui::Selectable("Software", selected == 3)) {
				currentMenu = ConfigMenus::Software;
				selected = 3;
			}
			else if (ImGui::Selectable("Renderer", selected == 4)){
				currentMenu = ConfigMenus::Renderer;
				selected = 4;
			}
			else if (ImGui::Selectable("Viewport", selected == 5)) {
				currentMenu = ConfigMenus::Viewport;
				selected = 5;
			}
			else if (ImGui::Selectable("Input", selected == 6)) {
				currentMenu = ConfigMenus::Input;
				selected = 6;
			}
			else if (ImGui::Selectable("Audio", selected == 7)) {
				currentMenu = ConfigMenus::Audio;
				selected = 7;
			}
			else if (ImGui::Selectable("Texture", selected == 8)) {
				currentMenu = ConfigMenus::Texture;
				selected = 8;
			}


			ImGui::PopStyleVar();
			ImGui::PopStyleColor();


		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginGroup();
			ImGui::BeginChild("Menus");
				switch (currentMenu)
				{
				case Cronos::ConfigMenus::Application:
					GUIDrawConfigApplicationMenu();
					break;
				case Cronos::ConfigMenus::Window:
					GUIDrawConfigWindowMenu();
					break;
				case Cronos::ConfigMenus::Hardware:
					GUIDrawConfigHardwareMenu();
					break;
				case Cronos::ConfigMenus::Software:
					GUIDrawConfigSoftwareMenu();
					break;
				case Cronos::ConfigMenus::Renderer:
					GUIDrawConfigRendererMenu();
					break;
				case Cronos::ConfigMenus::Viewport:
					GUIDrawConfigViewPortMenu();
					break;
				case Cronos::ConfigMenus::Input:
					GUIDrawConfigInputMenu();
					break;
				case Cronos::ConfigMenus::Audio:
					GUIDrawConfigAudioMenu();
					break;
				case Cronos::ConfigMenus::Texture:
					GUIDrawConfigTexturesMenu();
					break;
				default:
					break;
				}
				if (ImGui::Button("Save Changes"))
					App->SaveEngineData();
			ImGui::EndChild();

		ImGui::EndGroup();

		ImGui::EndGroup();


		ImGui::End();

	}

	void ImGuiLayer::GUIDrawConfigApplicationMenu(){

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
		ImVec4 Color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
		ImGui::Text("Application");
		ImGui::Separator();
		ImGui::Text ("App Name : "); ImGui::SameLine();

		//setting app name
		static char buf1[64];
		strcpy(buf1, App->GetAppTitle().c_str());
		if (ImGui::InputText("###Name", buf1, 50, ImGuiInputTextFlags_CharsNoBlank)) {
			App->SetAppTitle(buf1);
		}sameLine;
		//setting Version
		static char buf3[10];
		strcpy(buf3, App->GetAppVersion().c_str());
		if (ImGui::InputText("###version", buf3, 5, ImGuiInputTextFlags_CharsNoBlank)) {
			App->SetAppVersion(buf3);
		}

		ImGui::TextColored(Color, (App->GetAppTitle() + " " + App->GetAppVersion()).c_str());
		ImGui::Text("Organization: "); ImGui::SameLine();
		static char buf2[90];
		strcpy(buf2, App->GetAppOrganization().c_str());
		if (ImGui::InputText("###Organization", buf2, 90, ImGuiInputTextFlags_CharsNoBlank)) {
			App->SetAppOrganization(buf2);
		}
		ImGui::Text("Authors: "); ImGui::SameLine();

		static char buf4[90];
		strcpy(buf4, App->GetAppAuthors().c_str());
		if (ImGui::InputText("###Authors", buf4, 90, ImGuiInputTextFlags_CharsNoBlank)) {
			App->SetAppAuthors(buf4);
		}
		ImGui::Separator();
		ImGui::Text("");
		ImGui::Text("Performance here"); ImGui::SameLine();
		if (ImGui::Button("Performance"))
			ShowPerformancePanel = !ShowPerformancePanel;
		ImGui::PopStyleVar();
	}
	void ImGuiLayer::GUIDrawConfigWindowMenu() {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
		static int Height = (int)App->window->m_Data.Height;
		static int Width = (int)App->window->m_Data.Width;
		ImGui::Text("Window");
		ImGui::Separator();
		static bool TempWindowActive; //Only To show, it's temporary
		ImGui::Checkbox("Active", &TempWindowActive);
		ImGui::Text("Icon: "); ImGui::SameLine(); ImGui::Image("", ImVec2(40, 40));

		static float brightnesTest=SDL_GetWindowBrightness(App->window->window); //temporary
		if (ImGui::SliderFloat("Brightness", &brightnesTest, 0.2f, 2.0f, "%.01f")) {
			SDL_SetWindowBrightness(App->window->window, brightnesTest);
		}

		ImGui::Text("Height"); ImGui::SameLine();
		if (ImGui::SliderInt("##hidelabel", &Height, 100, 1080,"%d")) {
			SDL_SetWindowSize(App->window->window, Width, Height);
		}
		ImGui::Text("Width"); ImGui::SameLine();
		if (ImGui::SliderInt("##hidelabel2", &Width, 100, 1920)) {
			SDL_SetWindowSize(App->window->window, Width, Height);
		}

		static Uint32 flagsFullscreen=NULL;
		static Uint32 flagsFullWindowed = NULL;

		static bool resizable = App->window->m_Data.WindowResizable;
		static bool borderless = false;
		if(ImGui::CheckboxFlags("Fullscreen", &(unsigned int )flagsFullscreen,SDL_WINDOW_FULLSCREEN)){
			SDL_SetWindowFullscreen(App->window->window, flagsFullscreen);
		}ImGui::SameLine();
		if (ImGui::Checkbox("Borderless", &borderless)) {
			//SDL_SetWindowFullscreen(App->window->window, flags);
			SDL_SetWindowBordered(App->window->window, (SDL_bool)!borderless);
		}
		if (ImGui::CheckboxFlags("Full Windowed", &(unsigned int)flagsFullWindowed,SDL_WINDOW_FULLSCREEN_DESKTOP)) {
			SDL_SetWindowFullscreen(App->window->window, flagsFullWindowed);
		}ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable)) {
			App->window->m_Data.WindowResizable = resizable;
		}ImGui::SameLine(); HelpMarker("Restart to apply");

		ImGui::PopStyleVar();

	}

	void ImGuiLayer::GUIDrawConfigHardwareMenu() {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 15));
		ImVec4 Color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
		ImGui::Text("HARDWARE");
		ImGui::Separator();
		ImGui::Separator();

		ImGui::Text("MEMORY");
		ImGui::Separator();

		ImGui::Text("Total System RAM: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetRAMSizeFromSDL()) + " GB").c_str()));
		ImGui::Text("Total Physical Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetPhysicalMemory()) + " GB").c_str()));
		ImGui::Text("Available Physical Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetFreePhysicalMemory()) + " GB").c_str()));
		ImGui::Text("Used Physical Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetUsedPhysicalMemory()) + " GB").c_str()));
		ImGui::Text("Percentage of Memory Load: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetPercentageOfMemoryLoad()) + " " + "%%").c_str()));
		ImGui::Text("");
		ImGui::Text("Physical Memory Used by Process: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetPhysMemoryUsedByProcess()) + " MB").c_str()));
		ImGui::Text("Virtual Memory Used by Process: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetVirtualMemoryUsedByProcess()) + " MB").c_str()));

		ImGui::Text(" MMGR Memory Statistics"); ImGui::SameLine(); ImGui::Separator();
		ImGui::Text("Total Reported Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_TotalReportedMemory()) + " Bytes").c_str()));
		ImGui::Text("Total Actual/Real Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_TotalActualMemory()) + " Bytes").c_str()));
		ImGui::Text("Peak Reported Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_PeakReportedMemory()) + " Bytes").c_str()));
		ImGui::Text("Peak Actual/Real Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_PeakActualMemory()) + " Bytes").c_str()));
		ImGui::Text("Accumulated Reported Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_AccumulatedReportedMemory())).c_str()));
		ImGui::Text("Accumulated Actual/Real Memory: "); ImGui::SameLine(); ImGui::TextColored(Color, ((std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_AccumulatedActualMemory())).c_str()));

		ImGui::Text(" MMGR Allocated Unit Count Statistics"); ImGui::SameLine(); ImGui::Separator();
		ImGui::Text("Total Allocated Unit Count: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_TotalAllocUnitCount()).c_str()));
		ImGui::Text("Peak Allocated Unit Count: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_PeakAllocUnitCount()).c_str()));
		ImGui::Text("Accumulated Allocated Unit Count: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(HardwareInfo.GetMemoryHardwareInfo().GetMemStatsFromMMGR_AccumulatedAllocUnitCount()).c_str()));

		ImGui::Separator();
		ImGui::Separator();
		ImGui::Text("PROCESSOR");
		ImGui::Separator();

		ImGui::Text("CPU Brand: "); ImGui::SameLine(); ImGui::TextColored(Color, (HardwareInfo.GetCPUHardwareInfo().GetCPUBrand().c_str()));
		ImGui::Text("CPU Vendor: "); ImGui::SameLine(); ImGui::TextColored(Color, (HardwareInfo.GetCPUHardwareInfo().GetCPUVendor().c_str()));
		ImGui::Text("CPU Arhitecture: "); ImGui::SameLine(); ImGui::TextColored(Color, (HardwareInfo.GetCPUHardwareInfo().GetCPUArchitecture().c_str()));
		ImGui::SameLine(); ImGui::Text("    CPU Cores: "); ImGui::SameLine(); ImGui::TextColored(Color, std::to_string(HardwareInfo.GetCPUHardwareInfo().GetCPUCores()).c_str());
		ImGui::SameLine(); ImGui::Text("    CPU Processors: "); ImGui::SameLine(); ImGui::TextColored(Color, HardwareInfo.GetCPUHardwareInfo().GetNumberOfProcessors().c_str());
		ImGui::Text("");
		ImGui::Text("CPU Revision: "); ImGui::SameLine(); ImGui::TextColored(Color, HardwareInfo.GetCPUHardwareInfo().GetProcessorRevision().c_str());
		ImGui::Text("CPU Line L1 Cache Size: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(HardwareInfo.GetCPUHardwareInfo().GetCPUCacheLine1Size()) + " Bytes").c_str());
		ImGui::Text("");
		ImGui::Text("CPU Instructions Set (CAPS): "); ImGui::SameLine(); ImGui::TextColored(Color, HardwareInfo.GetCPUHardwareInfo().GetCPUInstructionSet().c_str());

		ImGui::Separator();
		ImGui::Separator();
		ImGui::Text("GRAPHICS CARD");
		ImGui::Separator();

		ImGui::Text("GPU Benchmark: "); ImGui::SameLine(); ImGui::TextColored(Color, (const char*)(HardwareInfo.GetGPUHardwareInfo().GetGPUBenchmark()));
		ImGui::Text("GPU Brand: "); ImGui::SameLine(); ImGui::TextColored(Color, HardwareInfo.GetGPUHardwareInfo().GetGPUInfo_GPUDet().m_GPUBrand.c_str());
		ImGui::Text("GPU Model: "); ImGui::SameLine(); ImGui::TextColored(Color, (const char*)(HardwareInfo.GetGPUHardwareInfo().GetGPUModel()));
		ImGui::Text("GPU Vendor: "); ImGui::SameLine(); ImGui::TextColored(Color, std::to_string(HardwareInfo.GetGPUHardwareInfo().GetGPUInfo_GPUDet().m_GPUVendor).c_str());
		ImGui::SameLine(); ImGui::Text("   GPU ID: "); ImGui::SameLine(); ImGui::TextColored(Color, std::to_string(HardwareInfo.GetGPUHardwareInfo().GetGPUInfo_GPUDet().m_GPUID).c_str());

		ImGui::Text("GPU VRAM"); ImGui::SameLine(); ImGui::Separator();
		ImGui::Text("Total VRAM: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(HardwareInfo.GetGPUHardwareInfo().GetGPUInfo_GPUDet().mPI_GPUDet_TotalVRAM_MB) + " GB").c_str());
		ImGui::Text("Actual/Real VRAM: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(HardwareInfo.GetGPUHardwareInfo().GetGPUInfo_GPUDet().mPI_GPUDet_CurrentVRAM_MB) + " GB").c_str());
		ImGui::Text("Reserved VRAM: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(HardwareInfo.GetGPUHardwareInfo().GetGPUInfo_GPUDet().mPI_GPUDet_VRAMReserved_MB) + " GB").c_str());
		ImGui::Text("Used VRAM: "); ImGui::SameLine(); ImGui::TextColored(Color, (std::to_string(HardwareInfo.GetGPUHardwareInfo().GetGPUInfo_GPUDet().mPI_GPUDet_VRAMUsage_MB) + " GB").c_str());

		if (ImGui::Button("Recalculate Parametters"))
			HardwareInfo.RecalculateParameters();

		ImGui::PopStyleVar();
	};

	void ImGuiLayer::GUIDrawConfigSoftwareMenu() {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
		ImVec4 Color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
		ImGui::Text("Software");
		ImGui::Separator();
		ImGui::Separator();

		ImGui::Text("Compilation Date: "); ImGui::SameLine(); ImGui::TextColored(Color, SoftwareInfo.GetSoftwareInfo().GetCompilationDate().c_str());
		ImGui::SameLine(); ImGui::Text("   and Time: "); ImGui::SameLine(); ImGui::TextColored(Color, SoftwareInfo.GetSoftwareInfo().GetCompilationTime().c_str());
		ImGui::Text(SoftwareInfo.GetSoftwareInfo().MultithreadedSpecified().c_str());

		ImGui::Separator();
		ImGui::Text("OS Windows Version: "); ImGui::SameLine(); ImGui::TextColored(Color, SoftwareInfo.GetSoftwareInfo().GetWindowsVersion().c_str());
		ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(Color, SoftwareInfo.GetSoftwareInfo().GetSDLVersion().c_str());
		ImGui::Text("OpenGL Version: "); ImGui::SameLine(); ImGui::TextColored(Color, (const char*)(SoftwareInfo.GetSoftwareInfo().GetOGLVersion()));
		ImGui::SameLine(); ImGui::Text("   OpenGL Shading Version: "); ImGui::SameLine(); ImGui::TextColored(Color, (const char*)(SoftwareInfo.GetSoftwareInfo().GetOGLShadingVersion()));

		ImGui::Text(" C++ Programming Language"); ImGui::SameLine(); ImGui::Separator();
		ImGui::Text("C++ Version Implemented by Compiler: "); ImGui::SameLine(); ImGui::TextColored(Color, SoftwareInfo.GetSoftwareInfo().GetCppVersionImplementedByCompiler().c_str());
		ImGui::SameLine(); ImGui::Text(" ("); ImGui::SameLine(); ImGui::TextColored(Color, SoftwareInfo.GetSoftwareInfo().GetCPPNumericalVersion().c_str()); ImGui::SameLine(); ImGui::Text(")");

		ImGui::Text("C++ Used Version: "); ImGui::SameLine(); ImGui::TextColored(Color, (SoftwareInfo.GetSoftwareInfo().GetCppCompilerVersion()).c_str());
		ImGui::Text("Visual Studio Compiler Version: "); ImGui::SameLine(); ImGui::TextColored(Color, SoftwareInfo.GetSoftwareInfo().GetVSCompilerVersion().c_str());


		ImGui::PopStyleVar();

	};

	void ImGuiLayer::GUIDrawConfigRendererMenu() {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
		ImGui::Text("Renderer");
		ImGui::Separator();
		static bool Tempvsync = false;
		ImGui::Text("VSync "); ImGui::SameLine(); ImGui::Checkbox("##vsync",&Tempvsync);
		ImGui::PopStyleVar();

	};
	void ImGuiLayer::GUIDrawConfigViewPortMenu() {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));

		static float CameraMoveSpeed = App->engineCamera->GetCameraMoveSpeed();
		static float CameraScrollSpeed = App->engineCamera->GetCameraScrollSpeed();
		static float CameraFieldOfView = App->engineCamera->GetFOV();
		static float CameraNearPlane = App->engineCamera->GetNearPlane();
		static float CameraFarPlane = App->engineCamera->GetFarPlane();

		ImGui::Text("Viewport");
		ImGui::Separator();
		ImGui::Text("Viewport Camera Options");
		ImGui::BeginChild("Camera Options");

		//Setters -----------------------------------------------------------------------------------------
		ImGui::SameLine(15); ImGui::Text("Camera Move Speed: "); sameLine;
		if (ImGui::SliderFloat("##cameraMoveSpeed", &CameraMoveSpeed, 1.0f, 100.0f, "%.2f", 1.0f))
			App->engineCamera->SetMoveSpeed(CameraMoveSpeed);

		ImGui::NewLine();
		ImGui::SameLine(15); ImGui::Text("Camera Scroll Speed: "); sameLine;
		if (ImGui::SliderFloat("##cameraScrollSpeed", &CameraScrollSpeed, 1.0f, 100.0f, "%.2f", 1.0f))
			App->engineCamera->SetScrollSpeed(CameraScrollSpeed);

		ImGui::NewLine();
		ImGui::SameLine(15); ImGui::Text("Field of View : "); sameLine;
		if (ImGui::SliderFloat("##cameraFOV", &CameraFieldOfView, MIN_FOV, MAX_FOV, "%.2f", 1.0f))
			App->engineCamera->SetFOV(CameraFieldOfView);

		ImGui::NewLine();
		ImGui::SameLine(15); ImGui::Text("Viewing Frustum: ");

		ImGui::NewLine();
		ImGui::SameLine(30);
		ImGui::SetNextItemWidth(100);

		if (ImGui::SliderFloat("NearPlane ", &CameraNearPlane, 0.1, 500, "%.2f", 1.0f))
			App->engineCamera->SetNearPlane(CameraNearPlane);

		sameLine;
		ImGui::SetNextItemWidth(100);

		if (ImGui::SliderFloat("FarPlane ", &CameraFarPlane, 0.1, 1000, "%.2f", 1.0f))
			App->engineCamera->SetFarPlane(CameraFarPlane);

		//Set to default values ------------------------------------------------------------------------------
		if (ImGui::Button("Default"))
		{
			//CameraMoveSpeed = 5.0;
			//CameraScrollSpeed = 20.0;
			CameraFieldOfView = 60;
			CameraNearPlane = 1.0;
			CameraFarPlane = 100.0;
			//App->engineCamera->SetMoveSpeed(CameraMoveSpeed);
			//App->engineCamera->SetScrollSpeed(CameraScrollSpeed);
			App->engineCamera->SetFOV(CameraFieldOfView);
			App->engineCamera->SetNearPlane(CameraNearPlane);
			App->engineCamera->SetFarPlane(CameraFarPlane);

		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	void ImGuiLayer::GUIDrawConfigInputMenu() {

		static char mousepos[50];
		static char mouseMotion[50];
		static char WheelMotion[30];

		sprintf_s(mousepos,50, "Mouse Position x: %i y: %i", App->input->GetMouseX(),App->input->GetMouseY());
		sprintf_s(mouseMotion, 50, "Mouse Motion x: %i y: %i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
		sprintf_s(WheelMotion, 30, "Wheel Motion : %i", App->input->GetMouseZ());

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
		ImGui::Text("Input");
		ImGui::Separator();
		ImGui::Text(mousepos);
		ImGui::Text(mouseMotion);
		ImGui::Text(WheelMotion);
		ImGui::Separator();

		ImGui::BeginChild("Inputs");
			ImGui::TextUnformatted(LogInputs.begin());
			ImGui::SetScrollHere(1.0f);
		ImGui::EndChild();

		ImGui::PopStyleVar();
	};

	void ImGuiLayer::GUIDrawConfigAudioMenu() {
		ImGui::Text("Audio");
		ImGui::Separator();
	};
	void ImGuiLayer::GUIDrawConfigTexturesMenu() {
		ImGui::Text("Textures");
		ImGui::Separator();
	};

	void ImGuiLayer::GUIDrawSupportExitOptions() {

		ImGui::OpenPopup("##menuQuit");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(9, 13));
		if (ImGui::BeginPopupModal("##menuQuit",NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you sure you want to quit?");
			ImGui::Text("Have you saved ?");
			ImGui::Separator();

			//static bool dont_ask_me_next_time = false;
			//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
			//ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
			//ImGui::PopStyleVar();

			if (ImGui::Button("Yes", ImVec2(100, 0))) { ImGui::CloseCurrentPopup(); current_status = UPDATE_STOP; }
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(100, 0))) { ImGui::CloseCurrentPopup(); App->input->updateQuit(false); }
			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}

	}

	void ImGuiLayer::GetInput(uint key, uint state,bool mouse) {

		if (currentMenu == ConfigMenus::Input) {
			static std::stringstream temp;
			static std::string states[] = { "KEY_IDLE","KEY_DOWN","KEY_REPEAT","KEY_UP " };
			if (!mouse)
				temp << "Key: " << key << "- KeyState : " << states[state] << "\n";
			else {
				switch (key)
				{
				case 1:
					temp << "Left click -" << "State : " << states[state] << "\n";
					break;
				case 2:
					temp << "Middle click -" << "State : " << states[state] << "\n";
					break;
				case 3:
					temp << "Right click -" << "State : " << states[state] << "\n";
					break;
				}
			}
			bool a = true;

			LogInputs.appendf(temp.str().c_str());
		}

	}
	void ImGuiLayer::AddLog(std::string log)
	{
		log += "\n\n";
		LogBuffer.appendf(log.c_str());

	}

	void ImGuiLayer::GUIDrawAboutPanel()
	{
		ImGui::PushStyleColor(ImGuiCol_TitleBg | ImGuiCol_TitleBgActive, ImVec4(0.392f, 0.369f, 0.376f, 1.00f));
		ImGui::SetNextWindowSize(ImVec2(430, 600),ImGuiCond_Appearing);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
		ImGui::Begin("About", &ShowAboutPanel);

			if (ImGui::Button("Cronos")) { App->RequestBrowser("https://github.com/lucho1/CronosEngine"); } ImGui::SameLine(); ImGui::Text(" v0.1");
			ImGui::Text("3D Game Engine based on OpenGL made for the Degree in Videogames Design");
			ImGui::Text("and Development of Universitat Politecnica de Catalunya for 3rd course Engines subject.");
			ImGui::Text("By"); ImGui::SameLine(); if (ImGui::Button("Lucho Suaya")) { App->RequestBrowser("https://github.com/lucho1"); }
			ImGui::SameLine(); ImGui::Text("&"); ImGui::SameLine(); if (ImGui::Button("Roger Leon")) { App->RequestBrowser("https://github.com/rleonborras"); }

		ImGui::Separator();

			ImGui::Text("3rd Party Libraries Used:");
			ImGui::BulletText("SDL 2.0.6");
			ImGui::BulletText("SDL mixer 2.0.0");
			ImGui::BulletText("Glad");
			ImGui::BulletText("ImGui 1.73");
			ImGui::BulletText("OpenGl 4.3");

		ImGui::Separator();
		ImGui::BeginChild("LicenseChild");
			ImGui::Text("License: ");
			ImGui::TextUnformatted(LicenseString.c_str());
		ImGui::EndChild();

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void ImGuiLayer::GUIDrawConsolePanel()
	{

		ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 15));
		ImGui::Begin("Console", &ShowConsolePanel);

		if (ImGui::Button("Clear")) {
			LogBuffer.clear();
		}

		ImGui::Separator();

		ImGui::BeginChild("Console Loging");
		ImGui::SameLine(10);
		if (!LogBuffer.empty()) {
			ImGui::TextUnformatted(LogBuffer.c_str());
			if(!ImGui::GetScrollMaxY())
				ImGui::SetScrollHere(1.0f);
		}

		ImGui::EndChild();
		ImGui::End();
		ImGui::PopStyleVar();

	}

	void ImGuiLayer::AcumulateLogDT()
	{
		static uint count = 0;
		if (count == 100) {

			for (uint i = 0; i < 99; i++) {
				ms_log[i] = ms_log[i + 1];
				fps_log[i] = fps_log[i + 1];
			}
		}
		else
			count++;

		fps_log[count - 1] = App->GetFramesInLastSecond();
		ms_log[count - 1] = App->GetLastFrameMS();
	}

	void PrimitivesMenu() {

		if (ImGui::MenuItem("Cube"))
		{
			PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::CUBE, "Cube", { 1,1,1 });
			App->scene->m_GameObjects.push_back(ret);
		}
		if (ImGui::MenuItem("Sphere"))
		{
			PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::SPHERE, "Sphere");
			App->scene->m_GameObjects.push_back(ret);
		}
		if (ImGui::MenuItem("Cone")) {
			PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::CLOSED_CONE, "Cone");
			App->scene->m_GameObjects.push_back(ret);
		}
		if (ImGui::MenuItem("Torus")) {
			PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::TORUS, "Torus");
			App->scene->m_GameObjects.push_back(ret);
		}
		if (ImGui::MenuItem("Cylinder")) {
			PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::CLOSED_CYLINDER, "Cylinder");
			App->scene->m_GameObjects.push_back(ret);
		}
		if (ImGui::MenuItem("Plane")) {
			PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::PLANE, "Plane");
			App->scene->m_GameObjects.push_back(ret);
		}
		if (ImGui::BeginMenu("Empty Primitives"))
		{
			if (ImGui::MenuItem("Empty Cylinder")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::EMPTY_CYLINDER, "Empty Cylinder");
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Empty Cone")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::EMPTY_CONE, "Empty Cone");
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Semi Sphere")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::SEMI_SPHERE, "Semi Sphere");
				App->scene->m_GameObjects.push_back(ret);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Complex")) {

			if (ImGui::MenuItem("Tetrahedron")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::TETRAHEDRON, "Tetrahedron");
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Octahedron")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::OCTAHEDRON, "Octahedron");
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("DodeCahedron")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::DODECAHEDRON, "Dodecahedron");
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Icosahedron")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::ICOSAHEDRON, "Icosahedron");
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Trefoil Knot")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::TREFOIL_KNOT, "Trefoil Knot");
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Klein Bottle")) {
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::KLEIN_BOTTLE, "Klein Bottle");
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Disk"))
			{
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::EMPTY, "Disk", { 1,1,1 });
				ret->CreateDisk();
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Rock"))
			{
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::EMPTY, "Rock", { 1,1,1 });
				ret->CreateRock();
				App->scene->m_GameObjects.push_back(ret);
			}
			if (ImGui::MenuItem("Sub-Sphere"))
			{
				PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::EMPTY, "Sub-Sphere", { 1,1,1 });
				ret->CreateSubdividedSphere();
				App->scene->m_GameObjects.push_back(ret);
			}
			ImGui::EndMenu();
		}
	}

}
