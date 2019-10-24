#include "Providers/cnpch.h"

#include "Application.h"
#include "Scene.h"

#include "Renderer/Buffers.h"
#include "Renderer/Model.h"
#include "Renderer/CronosPrimitive.h"
#include "TextureManager.h"

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
		App->renderer3D->SetOpenGLSettings();

		LOG("Loading Intro assets");
		bool ret = true;

		m_FloorPlane = Plane(0.0f, 1.0f, 0.0f, 0.0f); //Express the normal (0 centered)
		m_FloorPlane.axis = true; //Enable axis render


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
		vCubePrimitivexd = new CronosPrimitive(PrimitiveType::CLOSED_CONE, { 1, 1, 1 });




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

			uniform sampler2D u_Texture;

			void main()
			{
				vec4 texColor = texture(u_Texture, v_TexCoords) * vec4(1.0,1.0,1.0,1.0);;
				color = texColor;
			}
		)";

		default_tex = App->textureManager->CreateTexture("res/Baker_house.png");
		BasicTestShader = new Shader(vertexShader, fragmentShader);
		BasicTestShader->Bind();

		mat4x4 modelDef = mat4x4(); 
		BasicTestShader->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrixMAT4());
		BasicTestShader->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrixMAT4());
		BasicTestShader->SetUniformMat4f("u_Model", modelDef);

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
		//delete vmeshxd;
		//if (vmeshxd != nullptr) vmeshxd = nullptr;
		//vmodelxd->~CronosModel();
		////delete vmodelxd;
		////if (vmodelxd != nullptr)
		////	vmodelxd = nullptr;
		//vCubePrimitivexd->~CronosPrimitive();
		//if (vCubePrimitivexd != nullptr)
		//	vCubePrimitivexd = nullptr;

		return true;
	}

	// Update: draw background
	update_status Scene::OnUpdate(float dt)
	{
		// "Floor" Plane
		m_FloorPlane.Render();
		glColor3f(White.r, White.g, White.b);
		//glDrawArrays(GL_TRIANGLES, 0, 8);
		//VAO->Bind();
		//Cube cbe = Cube(5, 5, 5);
		//cbe.Render();
		//vmeshxd->Draw();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (App->EditorGUI->GetCurrentShading() == ShadingMode::Shaded)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else if (App->EditorGUI->GetCurrentShading() == ShadingMode::Wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}



		if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN && vmodelxd != nullptr)
			vmodelxd->MoveModel(glm::vec3(1, 0, 0), 1);
		if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN && vCubePrimitivexd != nullptr)
			vmodelxd->ScaleModel(glm::vec3(0.9f, 0.9f, 0.9f));
		if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN && vCubePrimitivexd != nullptr)
			vmodelxd->RotateModel(45.0, glm::vec3(0, 1, 0));


		BasicTestShader->Bind();
		default_tex->Bind(default_tex->GetTextureID());
		
		//glActiveTexture(GL_TEXTURE0 + default_tex);
		BasicTestShader->SetUniform1i("u_Texture", default_tex->GetTextureID());
	//	glBindTexture(GL_TEXTURE_2D, default_tex);
		
		mat4x4 modelDef = mat4x4();
		BasicTestShader->SetUniformMat4f("u_Proj", App->engineCamera->GetProjectionMatrixMAT4());
		BasicTestShader->SetUniformMat4f("u_View", App->engineCamera->GetViewMatrixMAT4());
		BasicTestShader->SetUniformMat4f("u_Model", modelDef);

		//glEnable(GL_DEPTH_TEST);
		vmodelxd->Draw();

		glBindTexture(GL_TEXTURE_2D, 0);
		BasicTestShader->Unbind();

		
		//vmodelxd->DrawTextureCoordinates();
		//vmodelxd->DrawVerticesNormals();
		//vmodelxd->DrawPlanesNormals();
		

		/*
		m_Texture->Bind();

		//Set the model matrix to modify (live) the texture we have
		glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), m_translationA);
		glm::mat4 MVP_mat = m_projMat * m_viewMat * modelMat;

		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", MVP_mat);
		renderer.Draw(*m_VAO, *m_IBO, *m_Shader);

		//2nd Draw
		modelMat = glm::translate(glm::mat4(1.0f), m_translationB);
		MVP_mat = m_projMat * m_viewMat * modelMat;

		m_Shader->Bind();
		m_Shader->SetUniformMat4f("u_MVP", MVP_mat);
		renderer.Draw(*m_VAO, *m_IBO, *m_Shader);
		*/



		//if(vCubePrimitivexd != nullptr)
		//	vCubePrimitivexd->Draw();

		//if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN && vCubePrimitivexd != nullptr)
		//	vCubePrimitivexd->ScaleModel(glm::vec3(0.9f, 0.9f, 0.9f));

		//if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN && vCubePrimitivexd != nullptr)
		//	vCubePrimitivexd->MoveModel(glm::vec3(1, 0, 0), 1);

		//if (App->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN && vCubePrimitivexd != nullptr)
		//	vCubePrimitivexd->RotateModel(45.0, glm::vec3(0, 1, 0));

		//if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN && vCubePrimitivexd != nullptr)
		//	vCubePrimitivexd->CreateDisk();

		//if (vCubePrimitivexd != nullptr) {
		//	glm::vec3 axis_vec = vCubePrimitivexd->GetModelAxis();
		//	glLineWidth(2.0f);
		//	glBegin(GL_LINES);
		//		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		//		glVertex3f(axis_vec.x, axis_vec.y, axis_vec.z);
		//		glVertex3f(axis_vec.x + 2, axis_vec.y, axis_vec.z);
		//		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		//		glVertex3f(axis_vec.x, axis_vec.y, axis_vec.z);
		//		glVertex3f(axis_vec.x, axis_vec.y + 2, axis_vec.z);
		//		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		//		glVertex3f(axis_vec.x, axis_vec.y, axis_vec.z);
		//		glVertex3f(axis_vec.x, axis_vec.y, axis_vec.z + 2);
		//	glEnd();
		//}


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
