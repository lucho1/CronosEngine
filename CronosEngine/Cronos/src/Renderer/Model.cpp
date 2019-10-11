#include "Providers/cnpch.h"

#include "Model.h"

#include "mmgr/mmgr.h"


namespace Cronos {

	Mesh::Mesh()
	{
	}

	Mesh::Mesh(std::vector<Vertex>vertices, std::vector<uint>indices)
	{
		m_VertexVector = vertices;
		m_IndicesVector = indices;

		SetupMesh();
	}

	Mesh::~Mesh()
	{
		m_MeshVAO->~VertexArray();

		//if (m_MeshVAO != nullptr)
		//	delete m_MeshVAO;
		//if (m_MeshIBO != nullptr)
		//	delete m_MeshIBO;
		//if (m_MeshVBO != nullptr)
		//	delete m_MeshVBO;
	}

	void Mesh::Draw()
	{
		m_MeshVAO->Bind();
		glDrawElements(GL_TRIANGLES, m_MeshVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void Mesh::SetupMesh()
	{
		m_MeshVAO = new VertexArray();

		Vertex* VBasArray = &m_VertexVector[0];
		m_MeshVBO = new VertexBuffer(VBasArray, m_VertexVector.size());

		m_MeshVBO->SetLayout({
			{Cronos::VertexDataType::VEC3F, "a_Position"},
			{Cronos::VertexDataType::VEC3F, "a_Normal"},
			{Cronos::VertexDataType::VEC2F, "a_TexCoords"}
		});

		m_MeshVAO->AddVertexBuffer(*m_MeshVBO);

		m_MeshIBO = new IndexBuffer(&m_IndicesVector[0], m_IndicesVector.size());
		m_MeshVAO->AddIndexBuffer(*m_MeshIBO);
		/*
		glm::vec3 Position; vec3 float
		glm::vec3 Normal; vec3 float
		glm::vec2 TexCoords; vec2 float
		*/
	}

}