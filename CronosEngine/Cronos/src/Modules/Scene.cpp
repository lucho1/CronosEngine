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
				}
				
				color = texColor;
				if(u_drawZBuffer == 1)
				{
					float depth = (LinearizeZ(gl_FragCoord.z)/u_CamPlanes.y);
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


		AABB OT_Test_AABB = AABB(glm::vec3(-50.0f), glm::vec3(50.0f));
		OT_Test = CnOctree(OT_Test_AABB, 2);

		return ret;
	}

	// Load assets
	bool Scene::OnCleanUp()
	{
		OT_Test.CleanUp();

		LOG("Unloading Intro scene");
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
		//"Floor" Plane
		App->renderer3D->DrawFloorPlane(true);

		//Wireframe Mode (or not)
		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else if (App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe)
		{
			glLineWidth(0.5f);
			glColor3f(White.r, White.g, White.b);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		//Shader bind & uniforms send
		//BasicTestShader->Bind();
		//App->scene->BasicTestShader->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrix());
		//App->scene->BasicTestShader->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrix());

		/*if (changeZBufferDrawing)
		{
			changeZBufferDrawing = false;
			drawZBuffer = !drawZBuffer;

			if (drawZBuffer)
				BasicTestShader->SetUniform1i("u_drawZBuffer", 1);
			else
				BasicTestShader->SetUniform1i("u_drawZBuffer", 0);

		}
		if (drawZBuffer)
			BasicTestShader->SetUniformVec2f("u_CamPlanes", glm::vec2(App->engineCamera->GetNearPlane(), App->engineCamera->GetFarPlane()));
*/
		//BasicTestShader->Unbind();


		//Game Objects update
		for (auto element : m_GameObjects)
			element->Update(dt);

		OT_Test.Draw();
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			for (uint i = 0; i < m_GameObjects.size(); i++)
			{
				OT_Test.Insert(m_GameObjects[i]);

				std::list<GameObject*>::iterator it = m_GameObjects[i]->m_Childs.begin();
				for (; it != m_GameObjects[i]->m_Childs.end(); it++)
					OT_Test.Insert(*it);
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN && OT_Test.IsSplitted())
		{
			AABB OT_Test_AABB = OT_Test.GetCubicSpace();
			OT_Test.CleanUp();
			OT_Test = CnOctree(OT_Test_AABB, 2);
		}

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
