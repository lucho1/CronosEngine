#include "Providers/cnpch.h"
#include "Scene.h"

#include "Application.h"

//#include "Renderer/Model.h"
//#include "Renderer/CronosPrimitive.h"

#include <glm/gtx/transform.hpp>

#include "mmgr/mmgr.h"

namespace Cronos {

	//static CronosModel* vmodelxd;
	static GameObject* vmodelxd;
	//static CronosModel* NanoSuitModel;
	static PrimitiveGameObject* vCubePrimitivexd;

	/*
	AssimpCronosTranslator m_ACT(this);
		m_ACT.LoadModel(filepath);
	*/

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

		//vmodelxd = new CronosModel("res/BakerHouse.fbx"); //warrior   BakerHouse
		//vCubePrimitivexd = new CronosPrimitive(PrimitiveType::CUBE, { 1, 1, 1 });
	//	NanoSuitModel = new CronosModel("res/nanosuit/nanosuit.obj");

		GameObject* Test = new GameObject("Test", App->m_RandomNumGenerator.GetIntRN(), "");
		GameObject* TestChild = new GameObject("TestChild", App->m_RandomNumGenerator.GetIntRN(), "");


		vmodelxd = m_CNAssimp_Importer.LoadModel(std::string("res/BakerHouse.fbx"));
		m_GameObjects.push_back(vmodelxd);
		vCubePrimitivexd = new PrimitiveGameObject(PrimitiveType::CUBE, { 1, 1, 1 }, "PRGO");
		m_GameObjects.push_back(vCubePrimitivexd);
		
		Test->m_Childs.push_back(TestChild);
		m_GameObjects.push_back(Test);

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
				vec4 texColor = mix(texture(u_DiffuseTexture, v_TexCoords), texture(u_SpecularTexture, v_TexCoords), 0.0);
				color = texColor;
			}
		)";

		BasicTestShader = new Shader(vertexShader, fragmentShader);
		BasicTestShader->Bind();
		BasicTestShader->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrixMAT4());
		BasicTestShader->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrixMAT4());
		BasicTestShader->SetUniformMat4f("u_Model", modelMatDef);
		//BasicTestShader->Unbind();

		return ret;
	}

	// Load assets
	bool Scene::OnCleanUp()
	{
		LOG("Unloading Intro scene");
		for (auto element : m_GameObjects)
			element->CleanUp();

		m_GameObjects.clear();
		return true;
	}

	// Update: draw background
	update_status Scene::OnUpdate(float dt)
	{
		// "Floor" Plane
		m_FloorPlane.Render();
		glColor3f(White.r, White.g, White.b);

		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else if (App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe)
		{
			glLineWidth(0.5f);
			glColor3f(White.r, White.g, White.b);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		static float move = 0.0f;
		static float angle = 0.0f;
		static float scale = 1.0f;
		static glm::mat4 translation = glm::mat4(1.0f);


		/*if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
			vmodelxd->MoveModel(glm::vec3(1, 0, 1), 0);

		if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
		{
			vmodelxd->ScaleModel(glm::vec3(scale));
			scale -= 0.1f;
		}
		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
		{
			vmodelxd->RotateModel(angle, glm::vec3(0, 1, 0));
			angle += 10.0f;
		}*/

	//	bool wireDrawing = (App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe ? true : false);
	//	if (wireDrawing)
	//		vmodelxd->Draw(BasicTestShader, false);
	//	else
	//		vmodelxd->Draw(BasicTestShader, true);
		

		//BasicTestShader->Bind();
		vmodelxd->Update(dt);
		vCubePrimitivexd->Update(dt);
		//std::list<GameObject*>::iterator listItem = vmodelxd->m_Childs.begin();
		//for (; listItem != vmodelxd->m_Childs.end(); listItem++)
		//{
		//	(*listItem)->Update();
		//}
		


	//	//NanoSuitModel->Draw(BasicTestShader, true);
		//vmodelxd->DrawModelAxis();
		////vmodelxd->DrawPlanesNormals();
		//vmodelxd->DrawVerticesNormals();

		glColor3f(White.r, White.g, White.b);
		//vCubePrimitivexd->Draw(BasicTestShader, false);

		return UPDATE_CONTINUE;
	}

	// PreUpdate
	update_status Scene::OnPreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	// UPostUpdate
	update_status Scene::OnPostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

}
