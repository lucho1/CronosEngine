#include "Providers/cnpch.h"
#include "MeshComponent.h"
#include "TransformComponent.h"

#include "Application.h"

namespace Cronos {

	MeshComponent::MeshComponent(GameObject* attachedGO) :
		Component(ComponentType::MESH , attachedGO)
	{
	}

	MeshComponent::~MeshComponent()
	{
		for (uint i = 0; i < m_TexturesVector.size(); i++)
		{
			RELEASE(m_TexturesVector[i]);
			m_TexturesVector.erase(m_TexturesVector.begin() + i);
		}
		for (uint i = 0; i < m_VertexVector.size(); i++)
			m_VertexVector.erase(m_VertexVector.begin() + i);
		for (uint i = 0; i < m_IndicesVector.size(); i++)
			m_IndicesVector.erase(m_IndicesVector.begin() + i);

		m_TexturesVector.clear();
		m_IndicesVector.clear();
		m_VertexVector.clear();

		RELEASE(m_MeshVAO);
		RELEASE(m_MeshIBO);
		RELEASE(m_MeshVBO);
	}

	void MeshComponent::SetupMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices, std::vector<Texture*> textures)
	{
		CRONOS_ASSERT(vertices.size() != 0, "THE MESH IS NOT WELL CREATED! YOU NEED TO PROPERLY SETUP THE VERTEX VECTOR! \n Try Calling function()");
		CRONOS_ASSERT(indices.size() != 0, "THE MESH IS NOT WELL CREATED! YOU NEED TO PROPERLY SETUP THE INDICES VECTOR! \n Try Calling function()");
		CRONOS_WARN(m_MeshVAO == nullptr, "Warning! Mesh VAO being created is not nullptr!");
		CRONOS_WARN(m_MeshVBO == nullptr, "Warning! Mesh VBO being created is not nullptr!");
		CRONOS_WARN(m_MeshIBO == nullptr, "Warning! Mesh IBO being created is not nullptr!");

		m_VertexVector = vertices;
		m_IndicesVector = indices;

		if (textures.size() > 0)
			m_TexturesVector = textures;


		m_MeshVAO = new VertexArray();
		m_MeshVBO = new VertexBuffer(&m_VertexVector[0], m_VertexVector.size() * sizeof(CronosVertex));

		m_MeshVBO->SetLayout({
			{Cronos::VertexDataType::VEC3F, "a_Position"},
			{Cronos::VertexDataType::VEC3F, "a_Normal"},
			{Cronos::VertexDataType::VEC2F, "a_TexCoords"}
			});
		m_MeshVAO->AddVertexBuffer(*m_MeshVBO);

		m_MeshIBO = new IndexBuffer(&m_IndicesVector[0], m_IndicesVector.size());
		m_MeshVAO->AddIndexBuffer(*m_MeshIBO);
	}

	void MeshComponent::SetTextures(std::vector<Texture*> newTexture, TextureType textureType)
	{
		for (uint i = 0; i < m_TexturesVector.size() && m_TexturesVector[i] != nullptr; i++)
		{
			if (m_TexturesVector[i]->GetTextureType() == textureType)
			{
				RELEASE(m_TexturesVector[i]);
				m_TexturesVector.erase(m_TexturesVector.begin() + i);
			}
		}

		m_TexturesVector.insert(m_TexturesVector.end(), newTexture.begin(), newTexture.end());
	}


	void MeshComponent::Update(float dt)
	{
		Draw(GetParent()->GetComponent<MaterialComponent>(), true);
	}

	void MeshComponent::Draw(MaterialComponent* material, bool bindShader)
	{
		if (!isEnabled())
			return;

		if (material != nullptr)
			material->Bind(bindShader);

		m_MeshVAO->Bind();
		glDrawElements(GL_TRIANGLES, m_MeshVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);

		if (material != nullptr)
			material->Unbind();

		m_MeshVAO->UnBind();

	//	DrawCentralAxis();
		if (m_DebugDraw)
		{
			DrawVerticesNormals();
			DrawPlanesNormals();
		}
	}

	void MeshComponent::DrawCentralAxis()
	{
		auto comp = GetParent()->GetComponent<TransformComponent>();
		if (comp != nullptr)
		{
			float linelength = 1.0f;
			glm::vec3 axis = comp->GetCentralAxis();
			glLineWidth(5.0f);
			glBegin(GL_LINES);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
				glVertex3f(axis.x, axis.y, axis.z);
				glVertex3f(axis.x + linelength, axis.y, axis.z);
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
				glVertex3f(axis.x, axis.y, axis.z);
				glVertex3f(axis.x, axis.y + linelength, axis.z);
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
				glVertex3f(axis.x, axis.y, axis.z);
				glVertex3f(axis.x, axis.y, axis.z + linelength);
				glColor4f(White.r, White.g, White.b, White.a);
			glEnd();
		}
	}

	void MeshComponent::DrawVerticesNormals()
	{
		glLineWidth(2.0f);
		float linelength = 0.2f;
		glColor4f(0.1f, 0.5f, 0.8f, 1.0f);
		std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
		for (; item != m_VertexVector.end(); item++)
		{
			glm::vec3 pos = (*item).Position;
			glm::vec3 norm = (*item).Position + (*item).Normal * linelength;

			glBegin(GL_LINES);
			glVertex3f(pos.x, pos.y, pos.z);
			glVertex3f(norm.x, norm.y, norm.z);
			glEnd();
		}
	}

	void MeshComponent::DrawPlanesNormals()
	{
		glLineWidth(2.0f);
		float linelength = 0.5f;
		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		for (uint i = 0; i < m_IndicesVector.size() - 2; i += 3)
		{
			glm::vec3 p1 = m_VertexVector[m_IndicesVector[i]].Position;
			glm::vec3 p2 = m_VertexVector[m_IndicesVector[i + 1]].Position;
			glm::vec3 p3 = m_VertexVector[m_IndicesVector[i + 2]].Position;

			glm::vec3 PlaneNormal = glm::cross(p2 - p1, p3 - p1);
			PlaneNormal = glm::normalize(PlaneNormal);
			PlaneNormal *= linelength;

			glm::vec3 TriCenter = { 0, 0, 0 };
			TriCenter.x = (p1.x + p2.x + p3.x) / 3;
			TriCenter.y = (p1.y + p2.y + p3.y) / 3;
			TriCenter.z = (p1.z + p2.z + p3.z) / 3;

			glBegin(GL_LINES);
			glVertex3f(TriCenter.x, TriCenter.y, TriCenter.z);
			glVertex3f(TriCenter.x + PlaneNormal.x, TriCenter.y + PlaneNormal.y, TriCenter.z + PlaneNormal.z);
			glEnd();
		}
	}

	void MeshComponent::CalculateNormals(std::vector<glm::vec3>& normals, std::vector<glm::vec3>& positions)
	{
		//Clear Vectors passed
		if (normals.size() > 0 || positions.size() > 0)
		{
			positions.clear();
			normals.clear();
		}

		//Calculate normals and add them to the vectors passed
		for (uint i = 0; i < m_IndicesVector.size(); i += 3)
		{
			glm::vec3 p1 = m_VertexVector[m_IndicesVector[i]].Position;
			glm::vec3 p2 = m_VertexVector[m_IndicesVector[i + 1]].Position;
			glm::vec3 p3 = m_VertexVector[m_IndicesVector[i + 2]].Position;

			glm::vec3 PlaneNormal = glm::cross(p2 - p1, p3 - p1);
			glm::normalize(PlaneNormal);

			glm::vec3 TriCenter = { 0, 0, 0 };
			TriCenter.x = (p1.x + p2.x + p3.x) / 3;
			TriCenter.y = (p1.y + p2.y + p3.y) / 3;
			TriCenter.z = (p1.z + p2.z + p3.z) / 3;

			positions.push_back(TriCenter);
			normals.push_back(PlaneNormal);
		}
	}
}
