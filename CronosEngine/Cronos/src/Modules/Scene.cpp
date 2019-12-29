#include "Providers/cnpch.h"
#include "Scene.h"

#include "Application.h"

#include "Filesystem.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/GLRenderer3D.h"
#include "EngineCamera.h"
#include "Input.h"
#include "TextureManager.h"

#include "SDLWindow.h"

#include "GameObject/Components/TransformComponent.h"
#include "GameObject/Components/CameraComponent.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	Scene::Scene(Application* app, bool start_enabled) : Module(app, "Module Scene", start_enabled), m_SceneName("NewScene")
	{}

	Scene::~Scene()
	{}

	// Load assets
	bool Scene::OnStart()
	{
		LOG("Loading Scene");
		bool ret = true;
		App->renderer3D->SetOpenGLSettings();

		//Water Simulation --------------------------------------------------------------------------------
		//Wave Object
		m_Wave = m_CNAssimp_Importer.LoadModel("res/models/waterPlane/FinalWaterPlane.FBX");
		m_Wave->GetComponent<TransformComponent>()->SetPosition({ 0.0f, 2.0f, 0.0f });
		m_Wave->GetComponent<TransformComponent>()->SetOrientation(glm::vec3(-90.f, 0.0f, 0.0f));

		//Water Shader &
		m_WaterShader = new Shader("res/shaders/WaterShader.glsl");

		//Water Material
		m_WaveMaterial = new Material();
		m_WaveMaterial->SetName("Water Wave Material");
		m_WaveMaterial->SetShader(*m_WaterShader);

		//Water Textures & Color
		m_WaveTexture = App->textureManager->CreateTexture("res/models/waterPlane/textures/water_tex_used.png", TextureType::DIFFUSE);
		m_WaveSpecText = App->textureManager->CreateTexture("res/models/waterPlane/specwater.jpg", TextureType::SPECULAR);
		m_WaveMaterial->SetTexture(m_WaveTexture, TextureType::DIFFUSE);
		m_WaveMaterial->SetTexture(m_WaveSpecText, TextureType::SPECULAR);
		m_WaveMaterial->SetColor(glm::vec4(1.0f));

		(*m_Wave->m_Childs.begin())->GetComponent<MaterialComponent>()->SetMaterial(*m_WaveMaterial);

		//Water timer for movement calc
		m_WaveTimer.Start();
		WaveColor = { 1.0f, 1.0f, 1.0f, 0.8f };
		// ------------------------------------------------------------------------------------------------

		m_StreetModel = m_CNAssimp_Importer.LoadModel(std::string("res/models/street/stre.FBX"));
		m_GameObjects.push_back(m_StreetModel);

		ToCopy = nullptr;
		return ret;
	}

	// CleanUp
	bool Scene::OnCleanUp()
	{
		//m_Wave->CleanUp();
		//m_WaveTexture->~Texture();
		//m_WaveSpecText->~Texture();

		LOG("Unloading Intro scene");


		App->renderer3D->m_CameraList.clear();

		App->renderer3D->ClearLightList();


		for (auto element : m_GameObjects)
		{
			element->CleanUp();
			RELEASE(element);
		}

		m_GameObjects.clear();


		//std::list<Texture*>::iterator it = m_TexturesLoaded.begin();
		//while (it != m_TexturesLoaded.end() && *it)
		//{
		//	RELEASE(*it);
		//	it = m_TexturesLoaded.erase(it);
		//}

		//m_TexturesLoaded.clear();

		return true;
	}

	// PreUpdate
	update_status Scene::OnPreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	// Update: draw background
	update_status Scene::OnUpdate(float dt)
	{

		//------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------- WAVE UPDATE -------------------------------------------------------------------------------------

		if (App->EditorGUI->ShowWaterPannel) {
			GameObject* WaveMesh = (*m_Wave->m_Childs.begin());
			m_WaterShader->Bind();

			// Wave Calculations ----------------
			float maxT = 180.0f;
			m_WaterShader->SetUniform1f("u_Time", m_WaveTimer.ReadSec());
			m_WaterShader->SetUniform1f("u_MaxTime", WaveMaxTime);
			m_WaterShader->SetUniform1f("u_Amplitude", WaveAmplitude);
			m_WaterShader->SetUniform1f("u_WaveLength", WaveLenght);
			m_WaterShader->SetUniform1f("u_Velocity", WaveVelocity);
			m_WaterShader->SetUniform1f("u_FOAMVelocity", FoamVelocity);
			m_WaterShader->SetUniform1f("u_ColorGradingOffset", WaveColorGrading); //Fragment Shader
			m_WaterShader->SetUniformVec2f("u_FOAMDirection", glm::vec2(FoamDirectionX,FoamDirectionY));
			m_WaterShader->SetUniform1f("u_WaveMovementMultiplicator", WaveMovementMultuplier);

			if (m_WaveTimer.ReadSec() >= maxT)
				m_WaveTimer.Start();

			// Shader Stuff ---------------------
			m_WaterShader->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrix());
			m_WaterShader->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrix());
			m_WaterShader->SetUniformMat4f("u_Model", WaveMesh->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix());

			MaterialComponent* material = WaveMesh->GetComponent<MaterialComponent>();
			VertexArray* VAO = WaveMesh->GetComponent<MeshComponent>()->GetVAO();

			//Binding ----------------------------
			if (material != nullptr)
				material->Bind();
			VAO->Bind();
			material->SetColor(WaveColor);

			//Drawing ----------------------------
			glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

			//Unbinding --------------------------
			if (material != nullptr)
				material->Unbind();
			VAO->UnBind();
			m_WaterShader->Unbind();
		}
		//------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		
		//Game Objects update
		for (auto element : m_GameObjects)
			element->Update(dt);

		//Copy & Paste
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
			if (App->EditorGUI->GetCurrentGameObject() != nullptr) {
				ToCopy = App->EditorGUI->GetCurrentGameObject();
			}
		}

		//Axis Gizmo modification
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && App->input->GetMouseButton(1) != KEY_REPEAT) {
			guizmo_operation = ImGuizmo::TRANSLATE;
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) {
			guizmo_operation = ImGuizmo::ROTATE;
		}
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) {
			guizmo_operation = ImGuizmo::SCALE;
		}

		//if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) {
		//	if (App->EditorGUI->GetCurrentGameObject() != nullptr && ToCopy != nullptr) {

		//		GameObject* NewGO = new GameObject(*App->EditorGUI->GetCurrentGameObject());
		//		//NewGO->SetNewID();
		//		if (App->EditorGUI->GetCurrentGameObject()->GetParentGameObject() == nullptr) {
		//			NewGO->SetParent(App->EditorGUI->GetCurrentGameObject());
		//			App->EditorGUI->GetCurrentGameObject()->m_Childs.push_back(NewGO);
		//		}
		//		else {
		//			NewGO->SetParent(App->EditorGUI->GetCurrentGameObject()->GetParentGameObject());
		//			App->EditorGUI->GetCurrentGameObject()->m_Childs.push_back(NewGO);
		//		}
		//		App->filesystem->SaveOwnFormat(App->EditorGUI->GetCurrentGameObject());
		//		//ToCopy = nullptr;
		//	}
		//}

		return UPDATE_CONTINUE;
	}

	// PostUpdate
	update_status Scene::OnPostUpdate(float dt)
	{
		if (mustCleanScene) {
			if(App->EditorGUI->GetCurrentGameObject()!=nullptr)
				App->EditorGUI->CancelGameObject();
			if(ToCopy!=nullptr)
				ToCopy = nullptr;
			OnCleanUp();
			mustCleanScene = false;
		}
		if (mustSave) {
			SaveScene(m_SceneName.c_str());
			mustSave = false;
		}
		if (mustLoad) {
			LoadScene(m_SceneName.c_str());
			mustLoad = false;
		}
		return UPDATE_CONTINUE;
	}

	//----------------------------------------------------------------------------
	GameObject* Scene::CreateModel(const char* path)
	{
		return m_CNAssimp_Importer.LoadModel(path);
	}

	//----------------------------------------------------------------------------
	bool Scene::SaveScene(const char * SceneName)
	{
		std::string Path = App->filesystem->GetScenePath();
		Path += SceneName;
		Path += ".scene";

		json sceneFile;
		sceneFile["Name"] = SceneName;
		sceneFile["Path"] = Path;
		sceneFile["TotalofRootGO"] = m_GameObjects.size();
		int index=0;
		for (auto& Go : m_GameObjects) {
			App->filesystem->SaveOwnFormat(Go);
			sceneFile["ParentGOID"][index] = Go->GetGOID();
			index++;
		}

		std::ofstream OutputFile_Stream{ Path,std::ofstream::out };

		OutputFile_Stream << std::setw(2) << sceneFile;

		OutputFile_Stream.close();

		return false;
	}

	bool Scene::LoadScene(const char* SceneName)
	{
		std::string Path = App->filesystem->GetScenePath();
		Path += SceneName;
		Path += ".scene";
		bool exists = std::filesystem::exists(Path);

		if (exists)
		{
			App->renderer3D->ResetTree();

			App->EditorGUI->CancelGameObject();
			ToCopy = nullptr;
			OnCleanUp();

			std::ifstream file{ Path.c_str() };
			if (file.is_open())
			{
				json LoadSceneFile=json::parse(file);
				uint numofRootGo = LoadSceneFile["TotalofRootGO"].get<uint>();

				for (int i = 0; i < numofRootGo; i++)
				{
					int id = LoadSceneFile["ParentGOID"][i].get<int>();
					GameObject* ret = App->filesystem->Load(id);
					m_GameObjects.push_back(ret);
				}
			}

			return true;
		}

		LOG("Couldn't Found any Scene with that path! Scene not loaded", Path.c_str());
		return false;
	}

	void Scene::DrawGuizmo(Camera* camera, GameObject* go)
	{
		TransformComponent* transform = go->GetComponent<TransformComponent>();
		glm::mat4 model = transform->GetLocalTranformationMatrix();		

		ImGuizmo::BeginFrame();
		ImGuizmo::Enable(true);
		static float x = 10.0f, y = -255.0f;
		ImGuizmo::SetRect(x, y, (float)App->window->GetWidth(), (float)App->window->GetHeight());

		ImGuizmo::SetDrawlist();
		ImGuizmo::Manipulate(glm::value_ptr(camera->GetViewMatrix()), glm::value_ptr(camera->GetProjectionMatrix()), guizmo_operation, guizmo_mode, glm::value_ptr(model));

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 scale, translation, rot;
			glm::quat q;
			glm::decompose(model, scale, q, translation, glm::vec3(), glm::vec4());
			rot = glm::degrees(glm::eulerAngles(q));			

			if (glm::abs(glm::length(rot)) < 0.0001f)
				rot = transform->GetOrientation();

			transform->SetPosition(translation);
			transform->Rotate(rot);
			transform->SetScale(scale);
		}
	}
}
