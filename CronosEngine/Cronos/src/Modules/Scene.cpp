#include "Providers/cnpch.h"
#include "Scene.h"

#include "Application.h"

//#include "Renderer/Model.h"
//#include "Renderer/CronosPrimitive.h"
#include "GameObject/Components/TransformComponent.h"

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
		std::string vertexShader = R"(
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
	
			uniform int u_TextureEmpty = 1;

			float nearP = 1.0;
			float farP = 10.0;
			uniform int u_drawZBuffer = 0;
			
			float LinearizeZ(float depth)
			{
				float z = depth*2.0 - 1.0;
				float ret = (2.0*nearP*farP)/(farP + nearP - z*(farP - nearP));
				return ret;
			}

			void main()
			{
				vec4 texColor = vec4(0.8, 0.8, 0.8, 1.0);
				if(u_TextureEmpty == 0)
				{
					texColor = (texture(u_DiffuseTexture, v_TexCoords)) * vec4(1.0,1.0,1.0,1.0);
				}
				
				color = texColor;
				if(u_drawZBuffer == 1)
				{
					float depth = (LinearizeZ(gl_FragCoord.z)/farP);
					color = vec4(vec3(depth), 1.0);
				}
			}
		)";

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

		BasicTestShader = new Shader(vertexShader, fragmentShader);

		//House Model Load & Floor Plane primitive
		m_HouseModel = m_CNAssimp_Importer.LoadModel(std::string("res/models/bakerhouse/BakerHouse.fbx"));
		m_GameObjects.push_back(m_HouseModel);
		m_SceneFloorPlane = new PrimitiveGameObject(PrimitiveType::PLANE, "FloorPrimitive", { 20.0f, 20.0f, 1.0f }, glm::vec3(0.0f), 0.6, 5, 5);
		m_SceneFloorPlane->GetComponent<TransformComponent>()->SetOrientation(glm::vec3(-90, 0, 0));
		
		return ret;
	}

	// Load assets
	bool Scene::OnCleanUp()
	{
		LOG("Unloading Intro scene");

		m_SceneFloorPlane->CleanUp();
		RELEASE(m_SceneFloorPlane);
		for (auto element : m_GameObjects)
		{
			element->CleanUp();
			RELEASE(element);
		}

		m_GameObjects.clear();
		RELEASE(BasicTestShader);
		
		std::list<Texture*>::iterator it = m_TexturesLoaded.begin();
		while (it != m_TexturesLoaded.end())
		{
			RELEASE(*it);
			it = m_TexturesLoaded.erase(it);
		}
		m_TexturesLoaded.clear();

		return true;
	}

	// Update: draw background
	update_status Scene::OnUpdate(float dt)
	{
		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else if (App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe)
		{
			glLineWidth(0.5f);
			glColor3f(White.r, White.g, White.b);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		//glEnable(GL_DEPTH_TEST);
		//glDepthFunc(GL_ALWAYS);

		BasicTestShader->Bind();
		BasicTestShader->SetUniformMat4f("u_View", App->engineCamera->m_ViewMatrix);
		BasicTestShader->SetUniformMat4f("u_Proj", App->engineCamera->m_ProjectionMatrix);
		BasicTestShader->SetUniform1i("u_drawZBuffer", 1);
		BasicTestShader->Unbind();

		//"Floor" Plane
		//TODO: This might conflict if Cull face isn't active
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_SceneFloorPlane->Update(dt);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);

		for (auto element : m_GameObjects)
			element->Update(dt);

		return UPDATE_CONTINUE;
	}

	GameObject* Scene::CreateModel(const char* path)
	{
		return m_CNAssimp_Importer.LoadModel(path);
	}

	// PreUpdate
	update_status Scene::OnPreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	// PostUpdate
	update_status Scene::OnPostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

}
