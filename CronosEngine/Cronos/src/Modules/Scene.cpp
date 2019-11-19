#include "Providers/cnpch.h"
#include "Scene.h"

#include "Application.h"

//#include "Renderer/Model.h"
//#include "Renderer/CronosPrimitive.h"
#include <glm/gtx/transform.hpp>

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

		m_FloorPlane = Plane(0.0f, 1.0f, 0.0f, 0.0f); //Express the normal (0 centered)
		m_FloorPlane.axis = true; //Enable axis render

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

			void main()
			{
				//vec4 t1 = texture2D(u_DiffuseTexture, v_TexCoords);
				//vec4 t2 = texture2D(u_SpecularTexture, v_TexCoords);
				//vec4 texColor = t1*t2;
				vec4 texColor = (texture(u_DiffuseTexture, v_TexCoords)) * vec4(1.0,1.0,1.0,1.0);
				
				//texColor = mix(texColor, texture(u_NormalMap, v_TexCoords), 0.0);
				//texColor = mix(texColor, texture(u_HeightMap, v_TexCoords), 0.0);
				
				color = texColor;
				//color = mix(texture2D(u_DiffuseTexture, v_TexCoords), texture2D(u_SpecularTexture, v_TexCoords), 0.0);
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
		BasicTestShader->Bind();
		BasicTestShader->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrixMAT4());
		BasicTestShader->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrixMAT4());
		BasicTestShader->SetUniformMat4f("u_Model", glm::mat4(1.0f));
		BasicTestShader->Unbind();

		//House Model Load
		//if(!m_HouseModel->HasMeta())
		m_HouseModel = m_CNAssimp_Importer.LoadModel(std::string("res/models/bakerhouse/BakerHouse.fbx"));
		GameObject* testing = App->filesystem->Load(m_HouseModel->GetMetaPath());

		////App->filesystem->Load(m_HouseModel->GetMetaPath());
		m_GameObjects.push_back(testing);
		return ret;
	}

	// Load assets
	bool Scene::OnCleanUp()
	{
		LOG("Unloading Intro scene");
		for (auto element : m_GameObjects)
			element->CleanUp();

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
		// "Floor" Plane
		glColor3f(White.r, White.g, White.b);
		glBindTextures(0, 31, 0);
		m_FloorPlane.Render();

		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else if (App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe)
		{
			glLineWidth(0.5f);
			glColor3f(White.r, White.g, White.b);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

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
