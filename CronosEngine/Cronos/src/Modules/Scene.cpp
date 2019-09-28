#include "cnpch.h"
#include "Scene.h"
#include "Application.h"

#include "Renderer/Buffers.h"

namespace Cronos {

	Scene::Scene(Application* app, bool start_enabled) : Module(app, "Module Scene", start_enabled)
	{
	}

	Scene::~Scene()
	{}

	// Load assets
	bool Scene::OnStart()
	{
		LOG("Loading Intro assets");
		bool ret = true;

		m_FloorPlane = Plane(0.0f, 1.0f, 0.0f, 0.0f); //Express the normal (0 centered)
		m_FloorPlane.axis = true; //Enable axis render

		App->engineCamera->Move(vec3(1.0f, 1.0f, 0.0f)); //Camera begins one unit up in Y and one unit to the right
		App->engineCamera->LookAt(vec3(0.0f, 0.0f, 0.0f)); //To look at center


		//---------------- TEST ----------------//		
		float cbeVertices[3 * 8] = {
			-10.0f,	-2.0f,	-10.0f, //1
			 10.0f,	-2.0f,	-10.0f, //2
			 10.0f,	 2.0f,	-10.0f, //3
			-10.0f,	 2.0f,	-10.0f, //4
			-10.0f,	 2.0f,	 10.0f, //5
			 10.0f,	 2.0f,	 10.0f, //6
			 10.0f,	-2.0f,	 10.0f, //7
			-10.0f,	-2.0f,	 10.0f  //8
		};	// x	  y		  z	
		uint cbeIndices[6 * 6] = {
			0, 1, 2, 2, 3, 0, //F1
			0, 1, 6, 6, 7, 0, //F2
			6, 5, 4, 4, 7, 6, //F3
			6, 5, 2, 2, 1, 6, //F4
			2, 5, 4, 4, 3, 2, //F5
			4, 3, 0, 0, 7, 4  //F6
		};

		VAO = new VertexArray();

		VertexBuffer* VBO = new VertexBuffer(cbeVertices, sizeof(cbeVertices));
		VBO->SetLayout({ {Cronos::ShaderDataType::VEC3F, "a_Position"} });
		VAO->AddVertexBuffer(*VBO);

		IndexBuffer* IBO = new IndexBuffer(cbeIndices, sizeof(cbeIndices));
		VAO->AddIndexBuffer(*IBO);
		

		return ret;
	}

	// Load assets
	bool Scene::OnCleanUp()
	{
		LOG("Unloading Intro scene");
		delete VAO;
		return true;
	}

	// Update: draw background
	update_status Scene::OnUpdate(float dt)
	{
		

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// "Floor" Plane 
		m_FloorPlane.Render();
		VAO->Bind();
		//uint xd = VAO->GetIndexBuffer()->GetCount();
		uint xd = VAO->m_IBuffer->GetCount();
		//IndexBuffer tuputamadre = VAO->m_IBuffer;

		glDrawElements(GL_TRIANGLES, xd, GL_UNSIGNED_INT, &(VAO->m_IBuffer));

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