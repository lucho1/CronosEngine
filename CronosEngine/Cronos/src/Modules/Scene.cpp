#include "Providers/cnpch.h"
#include "Scene.h"

#include "Application.h"

#include "GameObject/Components/TransformComponent.h"
#include "GameObject/Components/CameraComponent.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	Scene::Scene(Application* app, bool start_enabled) : Module(app, "Module Scene", start_enabled)
	{}

	Scene::~Scene()
	{}

	// Load assets
	bool Scene::OnStart()
	{
		LOG("Loading Scene");
		bool ret = true;
		App->renderer3D->SetOpenGLSettings();

		//Shaders
		/*std::string vertexShader = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Normal;
			layout(location = 2) in vec2 a_TexCoords;

			uniform mat4 u_View;
			uniform mat4 u_Proj;
			uniform mat4 u_Model;

			out vec2 v_TexCoords;

			void main()
			{
				gl_Position = u_Proj * u_View * u_Model * vec4(a_Position, 1.0);
				v_TexCoords = a_TexCoords;
			}
		)";

		std::string fragmentShader = R"(
			#version 330 core

			out vec4 color;
			in vec2 v_TexCoords;

			uniform sampler2D u_AmbientTexture;
			uniform sampler2D u_DiffuseTexture;
			uniform sampler2D u_SpecularTexture;
			uniform sampler2D u_NormalMap;
			uniform sampler2D u_HeightMap;
			uniform sampler2D u_LightMap;

			uniform vec4 u_AmbientColor = vec4(1.0,1.0,1.0,1.0);

			uniform int u_TextureEmpty = 1;

			uniform vec2 u_CamPlanes; //x for near plane, y for far plane
			uniform int u_drawZBuffer = 0;

			float LinearizeZ(float depth)
			{
				float z = depth*2.0 - 1.0;
				return (2.0*u_CamPlanes.x*u_CamPlanes.y)/(u_CamPlanes.y + u_CamPlanes.x - z*(u_CamPlanes.y - u_CamPlanes.x));
			}

			void main()
			{
				vec4 texColor = vec4(0.8, 0.8, 0.8, 1.0);
				if(u_TextureEmpty == 0)
				{
					texColor = (texture(u_DiffuseTexture, v_TexCoords)) * u_AmbientColor;
					color = texColor;
				}
				else
				{
					color = u_AmbientColor;
				}

				if(u_drawZBuffer == 1)
				{
					float depth = (LinearizeZ(gl_FragCoord.z)/u_CamPlanes.y);
					color = vec4(vec3(depth), 1.0);
				}
			}
		)";*/

		//vec4 texColor = mix(texture(u_DiffuseTexture, v_TexCoords), texture(u_SpecularTexture, v_TexCoords), 0.0);
		//vec3 diffuse = vec3(texture(u_DiffuseTexture, v_TexCoords));
		//vec4 texColor = mix(texture2D(u_DiffuseTexture, v_TexCoords), texture2D(u_SpecularTexture, v_TexCoords), 0.0);

		/*std::string fragmentShader = R"(
			#version 330 core

			out vec4 color;
			in vec2 v_TexCoords;

			uniform sampler2D u_AmbientTexture;
			uniform sampler2D u_DiffuseTexture;
			uniform sampler2D u_SpecularTexture;
			uniform sampler2D u_NormalMap;
			uniform sampler2D u_HeightMap;
			uniform sampler2D u_LightMap;

			void main()
			{
				vec4 texColor = mix(texture(u_DiffuseTexture, v_TexCoords), texture(u_SpecularTexture, v_TexCoords), 0.0);
				color = texColor;
			}
		)";*/

		m_SceneName = "NewScene";
		//BasicTestShader = new Shader(vertexShader, fragmentShader);
		BasicTestShader = new Shader("res/shaders/basic.glsl");
		m_WaterShader = new Shader("res/shaders/WaterShader.glsl");
		m_WaveTexture = App->textureManager->CreateTexture("res/models/waterPlane/water1.jpg", TextureType::DIFFUSE);
		m_WaveTimer.Start();

		//House Model Load
		//if(!m_HouseModel->HasMeta())
		//House Model Load & Floor Plane primitive
		//int id = m_HouseModel->GetGOID();
		//GameObject* testing = App->filesystem->Load(m_StreetModel->GetGOID());

		//res/models/waterPlane/waterPlaneOBJ.obj
		//res/models/waterPlane/waterPlane.FBX
		m_Wave = m_CNAssimp_Importer.LoadModel("res/models/waterPlane/waterPlaneOBJ.obj");
		m_Wave->GetComponent<TransformComponent>()->SetPosition({ 0.0f, 2.0f, 0.0f });

		m_StreetModel = m_CNAssimp_Importer.LoadModel(std::string("res/models/street/stre.FBX"));
		m_GameObjects.push_back(m_StreetModel);

		////App->filesystem->Load(m_HouseModel->GetMetaPath());
		//m_GameObjects.push_back(testing);
		ToCopy = nullptr;
		return ret;
	}

	// CleanUp
	bool Scene::OnCleanUp()
	{

		m_Wave->CleanUp();
		m_WaveTexture->~Texture();

		LOG("Unloading Intro scene");
		for (auto element : m_GameObjects)
		{
			element->CleanUp();
			RELEASE(element);
		}

		m_GameObjects.clear();

		std::list<Texture*>::iterator it = m_TexturesLoaded.begin();
		while (it != m_TexturesLoaded.end())
		{
			RELEASE(*it);
			it = m_TexturesLoaded.erase(it);
		}
		m_TexturesLoaded.clear();
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

	/*	GameObject* WaveMesh = (*m_Wave->m_Childs.begin());
		m_WaterShader->Bind();

		// Wave Calculations ----------------
		float maxT = 15.0f;
		m_WaterShader->SetUniform1f("u_Time", m_WaveTimer.ReadSec());
		m_WaterShader->SetUniform1f("u_MaxTime", maxT);
		m_WaterShader->SetUniform1f("u_Amplitude", 2.0f);
		m_WaterShader->SetUniform1f("u_WaveLength", 20.0f);
		m_WaterShader->SetUniform1f("u_ColorGradingOffset", 0.1f); //Fragment Shader

		if (m_WaveTimer.ReadSec() >= maxT)
			m_WaveTimer.Start();

		// Shader Stuff ---------------------
		m_WaterShader->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrix());
		m_WaterShader->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrix());
		m_WaterShader->SetUniformMat4f("u_Model", WaveMesh->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix());
		m_WaterShader->SetUniform1i("u_WaterTexture", m_WaveTexture->GetTextureID());

		MaterialComponent* material = WaveMesh->GetComponent<MaterialComponent>();
		VertexArray* VAO = WaveMesh->GetComponent<MeshComponent>()->GetVAO();

		//Binding ----------------------------
		if (material != nullptr)
			material->Bind(true);
		VAO->Bind();
		material->SetColor({ 1.0f, 1.0f, 1.0f, 0.8f });
		m_WaveTexture->Bind();

		//Drawing ----------------------------
		glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

		//Unbinding --------------------------
		if (material != nullptr)
			material->Unbind();
		VAO->UnBind();
		m_WaveTexture->Unbind();
		m_WaterShader->Unbind();

		//------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		*/

		//Game Objects update
		for (auto element : m_GameObjects)
			element->Update(dt);

		//Creating Camera
		if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN && App->EditorGUI->GetCurrentGameObject() != nullptr && App->EditorGUI->GetCurrentGameObject()->GetComponent<CameraComponent>() != nullptr)
			App->renderer3D->SetRenderingCamera(*App->EditorGUI->GetCurrentGameObject()->GetComponent<CameraComponent>()->GetCamera());
		else if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
			App->renderer3D->SetRenderingCamera(*App->engineCamera->GetCamera());

		//Copy & Paste
		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) {
			if (App->EditorGUI->GetCurrentGameObject() != nullptr) {
				ToCopy = App->EditorGUI->GetCurrentGameObject();
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN) {
			if (App->EditorGUI->GetCurrentGameObject() != nullptr && ToCopy != nullptr) {

				GameObject* NewGO = App->filesystem->Load(ToCopy->GetGOID());


				NewGO->SetNewID();
				if (App->EditorGUI->GetCurrentGameObject()->GetParentGameObject() == nullptr) {
					NewGO->SetParent(App->EditorGUI->GetCurrentGameObject());
					App->EditorGUI->GetCurrentGameObject()->m_Childs.push_back(NewGO);
				}
				else {
					NewGO->SetParent(App->EditorGUI->GetCurrentGameObject()->GetParentGameObject());
					App->EditorGUI->GetCurrentGameObject()->m_Childs.push_back(NewGO);
				}
				App->filesystem->SaveOwnFormat(App->EditorGUI->GetCurrentGameObject());
				//ToCopy = nullptr;
			}
		}

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
}
