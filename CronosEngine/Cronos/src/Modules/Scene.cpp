#include "Providers/cnpch.h"

#include "Application.h"
#include "Scene.h"

#include "Renderer/Buffers.h"
#include "Renderer/Model.h"
#include "Renderer/CronosPrimitive.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	static CronosMesh* vmeshxd; //To test
	static CronosModel* vmodelxd;
	static CronosPrimitive* vCubePrimitivexd;

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
			-2.0f,	-2.0f,	-2.0f, //1
			 2.0f,	-2.0f,	-2.0f, //2
			 2.0f,	 2.0f,	-2.0f, //3
			-2.0f,	 2.0f,	-2.0f, //4
			-2.0f,	 2.0f,	 2.0f, //5
			 2.0f,	 2.0f,	 2.0f, //6
			 2.0f,	-2.0f,	 2.0f, //7
			-2.0f,	-2.0f,	 2.0f  //8
		};	// x	  y		  z
		uint cbeIndices[6 * 6] = {
			0, 1, 2, 2, 3, 0, //F1
			0, 1, 6, 6, 7, 0, //F2
			6, 5, 4, 4, 7, 6, //F3
			6, 5, 2, 2, 1, 6, //F4
			2, 5, 4, 4, 3, 2, //F5
			4, 3, 0, 0, 7, 4  //F6
		};

		std::vector<CronosVertex>VertexVec;
		std::vector<CronosTexture>TextureVec;
		CronosVertex defV = { glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec2(1.0, 1.0) };
		CronosTexture defT = { 0, "TEXTURENONE" };

		for (int i = 0; i < 8; i++)
		{
			TextureVec.push_back(defT);
			VertexVec.push_back(defV);
		}

		uint iter = 0;
		for (int i = 0; i < 8; i++)
		{
			glm::vec3 pos = glm::vec3(cbeVertices[0 + iter], cbeVertices[1 + iter], cbeVertices[2 + iter]);
			VertexVec[i].Position = pos;
			iter += 3;
		}
		
		std::vector<uint> indvec;
		indvec.assign(cbeIndices, cbeIndices + (6*6));
		vmeshxd = new CronosMesh(VertexVec, indvec, TextureVec);
		vmodelxd = new CronosModel("res/BakerHouse.fbx"); //warrior   BakerHouse
		vCubePrimitivexd = new CronosPrimitive(PrimitiveType::TETRAHEDRON);

		//TETRAHEDRON, OCTAHEDRON, DODECAHEDRON, ICOSAHEDRON
	//CYLINDER, CONE, SPHERE, SEMI_SPHERE, PLANE, KLEIN_BOTTLE

		//vmodelxd->ScaleModel(glm::vec3(1, 1, 1), 0.1f);

		//uint va;
		//glCreateVertexArrays(1, &va);
		//glBindVertexArray(va);
		//
		//uint vb;
		//glCreateBuffers(1, &vb);
		//glBindBuffer(GL_ARRAY_BUFFER, vb);
		//glBufferData(GL_ARRAY_BUFFER, 3*8* sizeof(float), cbeVertices, GL_STATIC_DRAW);
		//
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
		//
		//uint ib;
		//glCreateBuffers(1, &ib);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 6 * sizeof(uint), cbeIndices, GL_STATIC_DRAW);
		//
		//glBindVertexArray(va);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

		//VAO = new VertexArray();
		//
		//VertexBuffer* VBO = new VertexBuffer(cbeVertices, sizeof(cbeVertices));
		//VBO->SetLayout({ {Cronos::VertexDataType::VEC3F, "a_Position"} });
		//VAO->AddVertexBuffer(*VBO);
		////
		//IndexBuffer* IBO = new IndexBuffer(cbeIndices, sizeof(cbeIndices));
		//VAO->AddIndexBuffer(*IBO);

		//glBindBuffer(GL_ARRAY_BUFFER, m_ID);
		return ret;
	}

	// Load assets
	bool Scene::OnCleanUp()
	{
		LOG("Unloading Intro scene");
		//VAO->~VertexArray();
		delete vmeshxd;
		if (vmeshxd != nullptr) vmeshxd = nullptr;
		vmodelxd->~CronosModel();
		delete vmodelxd;
		if (vmodelxd != nullptr)
			vmodelxd = nullptr;
		vCubePrimitivexd->~CronosPrimitive();
		if (vCubePrimitivexd != nullptr)
			vCubePrimitivexd = nullptr;

		return true;
	}

	// Update: draw background
	update_status Scene::OnUpdate(float dt)
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// "Floor" Plane
		m_FloorPlane.Render();
		glColor3f(White.r, White.g, White.b);
		//glDrawArrays(GL_TRIANGLES, 0, 8);
		//VAO->Bind();
		//Cube cbe = Cube(5, 5, 5);
		//cbe.Render();
		//vmeshxd->Draw();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//vmodelxd->Draw();
		//vmodelxd->DrawTextureCoordinates();
		//vmodelxd->DrawVerticesNormals();
		//vmodelxd->DrawPlanesNormals();

		vCubePrimitivexd->Draw();

		if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
			vCubePrimitivexd->ScaleModel(0.9f);

		if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
			vCubePrimitivexd->MoveModel(glm::vec3(2, 0, 0));

		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN)
			vCubePrimitivexd->RotateModel(45.0, glm::vec3(0, 1, 0));

		
		glm::vec3 axis_vec = vCubePrimitivexd->GetModelAxis();
		glLineWidth(2.0f);
		glBegin(GL_LINES);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glVertex3f(axis_vec.x, axis_vec.y, axis_vec.z);
			glVertex3f(axis_vec.x + 2, axis_vec.y, axis_vec.z);
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertex3f(axis_vec.x, axis_vec.y, axis_vec.z);
			glVertex3f(axis_vec.x, axis_vec.y + 2, axis_vec.z);
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertex3f(axis_vec.x, axis_vec.y, axis_vec.z);
			glVertex3f(axis_vec.x, axis_vec.y, axis_vec.z + 2);
		glEnd();

		

		//glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, nullptr);
		//REMEMBER THAT CULL FACE IS ACTIVE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//glDrawElements(GL_TRIANGLES, VAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr/* VAO->GetIndexBuffer()*/);

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
